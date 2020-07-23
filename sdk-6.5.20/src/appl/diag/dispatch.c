/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dispatch.c
 * Purpose:     Dispatch related diagnostic commands
 */

#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/custom.h>

#ifdef  BCM_ASYNC_SUPPORT
#include <bcm/async.h>
#include <bcm_int/async_server.h>
#endif  /* BCM_ASYNC_SUPPORT */

#ifdef  BCM_RPC_SUPPORT
#include <appl/cpudb/cpudb.h>
#include <bcm_int/rpc/rpc.h>
#include <bcm_int/rpc/rlink.h>
#if defined(BROADCOM_DEBUG)
extern void bcm_rpc_dump(void);
#endif  /* BROADCOM_DEBUG */
#endif  /* BCM_RPC_SUPPORT */

#include <bcm_int/control.h>
#ifdef  INCLUDE_LIB_STKTASK
#include <appl/stktask/attach.h>
#endif  /* INCLUDE_LIB_STKTASK */

#include <appl/diag/diag.h>

#ifdef  BCM_ASYNC_SUPPORT
char cmd_async_usage[] = 
    "async start: start the async api task\n"
    "async stop: stop the async api task\n"
    "async test: submit an async bcm call (vlan create)\n"
;

void
cmd_async_cb(void *cookie, int ret)
{
    int  n;

    n = PTR_TO_INT(cookie);

    sal_printf("async callback cookie %d ret %d (%s)\n",
               n, ret, bcm_errmsg(ret));
}

static int cmd_async_count;

cmd_result_t
cmd_async(int unit, args_t *args)
{
    char  *cmd, *subcmd;
    int   rv;

    cmd = ARG_CMD(args);
    subcmd = ARG_GET(args);

    if (subcmd == NULL) {
        subcmd = "show";
    }
    if (sal_strcasecmp(subcmd, "show") == 0) {
        /* hmmm.  Nothing to do here */
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "start") == 0) {
        rv = bcm_async_start();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "stop") == 0) {
        rv = bcm_async_stop();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "test") == 0) {
        ++ cmd_async_count;
        sal_printf("[bcm vlan create 0, %d]\n", cmd_async_count);
        bcm_vlan_create_async(0, cmd_async_count,
                              cmd_async_cb, INT_TO_PTR(cmd_async_count));
        return CMD_OK;
    }
    return CMD_USAGE;
}
#endif  /* BCM_ASYNC_SUPPORT */

#ifdef  BCM_RPC_SUPPORT
char cmd_rpc_usage[] = 
    "rpc start: start the BCM API RPC server task\n"
    "rpc stop: stop the BCM API RPC server task\n"
    "rpc nexthop: next rpc start will use nexthop transport\n"
    "rpc nonexthop: next rpc start will use cpu2cpu transport\n"
#if defined(BROADCOM_DEBUG)
    "rpc show: display internal rpc information\n"
#endif  /* BROADCOM_DEBUG */
;

extern int _rpc_nexthop;
extern int _rlink_nexthop;

cmd_result_t
cmd_rpc(int unit, args_t *args)
{
    char  *cmd, *subcmd;
    int   rv;

    cmd = ARG_CMD(args);
    subcmd = ARG_GET(args);

    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "nexthop") == 0) {
        _rpc_nexthop = 1;
        _rlink_nexthop = 1;
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "nonexthop") == 0) {
        _rpc_nexthop = 0;
        _rlink_nexthop = 0;
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "start") == 0) {
        rv = bcm_rpc_start();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: rpc_start: %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_rlink_start();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: rlink_start: %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }
    if (sal_strcasecmp(subcmd, "stop") == 0) {
        rv = bcm_rpc_stop();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: rpc_stop: %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_rlink_stop();
        if (rv < 0) {
            sal_printf("ERROR: %s %s: rlink_stop: %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

#if defined(BROADCOM_DEBUG)
    if (sal_strcasecmp(subcmd, "show") == 0) {
        bcm_rpc_dump();
        return CMD_OK;
    }
#endif  /* BROADCOM_DEBUG */

    return CMD_USAGE;
}
#endif  /* BCM_RPC_SUPPORT */

void
cmd_dispatch_link_notify(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    if (info->linkstatus == BCM_PORT_LINK_STATUS_UP) {
        sal_printf(" port %d.%d link up %dMb/s %s\n",
                   unit, port, info->speed, info->duplex ? "FD" : "HD");
    } else {
        sal_printf(" port %d.%d link down\n", unit, port);
    }
}

#ifdef  INCLUDE_LIB_STKTASK
void
cmd_dispatch_attach_notify(int unit, int attached,
                           cpudb_entry_t *cpuent, int cpuinit)
{
    sal_printf(" unit %d %s\n",
               unit, attached ? "attached" : "detached");
}
#endif  /* INCLUDE_LIB_STKTASK */

void
cmd_dispatch_l2_notify(int unit, bcm_l2_addr_t *l2addr, int insert, void *cookie)
{
    sal_printf(" unit %d l2 %s %x:%x:%x:%x:%x:%x modport %d.%d vlan %d\n",
               unit, insert ? "insert" : "delete",
               l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
               l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
               l2addr->modid, l2addr->port,
               l2addr->vid);
}

int
custom_unit_vlan_vector_get(int unit, int len, uint32 *args, int *actual)
{
    int rv = BCM_E_NONE;
    int i, vid, idx, maxidx, count = 0;
    bcm_vlan_data_t *list = NULL;
   
    rv = bcm_vlan_list(unit, &list, &count);
    if (rv < 0) {
        return rv;
    }

    sal_memset(args, 0, sizeof(args[0])*len);    
    args[0] = count; /* store the number of active VLANS in word0 */
    maxidx = 0;
    for (i = 0; i < count; i++) {
        vid = list[i].vlan_tag;
        idx = vid/32 + 1;
        if (idx < len) {
            args[idx] |=  1 << (vid % 32);
            if (idx > maxidx) {
                maxidx = idx;
            }
        }
    }
    *actual = maxidx;
    rv = bcm_vlan_list_destroy(unit, list, count);
    return rv;
}

/*
 * Generic Custom handler
 *
 * We now have 3 examples using this Custom handler
 *
 * CUSTOM_HANDLER_VLAN: Gets configured vlans across the system
 *                     
 * CUSTOM_HANDLER_STAT: Get all of a port's stats in one call.
 * 
 * CUSTOM_HANDLER_STAT_SYNC: Sync stats counters and then get them
 *
 * This is the custom handler that runs on the slave devices.
 * See companion calling routine custom_unit_vlan_vector_get/
 * custom_stat_get() that runs on the master.
 */
int
custom_handler(int unit, bcm_port_t port, int setget, int type, int len,
               uint32 *args, int *actual, void *user_data)
{
    int     rv, argi, s;
    uint64  val;

    if (setget != BCM_CUSTOM_GET) {
        return BCM_E_PARAM;
    }

    switch (type) {
    case CUSTOM_HANDLER_VLAN_GET: /* gets vlan vector for a unit */
        rv = custom_unit_vlan_vector_get(unit, len, args, actual);
        break;
        
    case CUSTOM_HANDLER_STAT_SYNC_GET:
        /* sync stats counters, then get them */
        rv = bcm_stat_sync(unit);
        if (BCM_FAILURE(rv)) {
            break;
        }
        /* FALLTHROUGH */
    case CUSTOM_HANDLER_STAT_GET:    /* just get all stats */
        rv = BCM_E_NONE;
        argi = 0;
        for (s = 0; s < snmpValCount && argi < len; s++) {
            COMPILER_64_ZERO(val);
            rv = bcm_stat_get(unit, port, s, &val);
            if (BCM_FAILURE(rv) && rv != BCM_E_PARAM) {
                /* only break if the error is serious */
                break;
            }
            args[argi++] = COMPILER_64_HI(val);
            args[argi++] = COMPILER_64_LO(val);
        }
        *actual = argi;
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }
    return rv;
}

int
custom_stat_get(int unit, bcm_port_t port,
                int dosync, uint32 num_stats, uint64 stats[])
{
    int     rv, argi, actual, op;
    uint32  *args = NULL;

    if(num_stats == 0 || num_stats > snmpValCount){
        rv = BCM_E_PARAM;
        return rv;
    }

    args = sal_alloc(num_stats * sizeof(uint32) * 2, "custom_stat_get args Mem");
    if (args == NULL) {
        return BCM_E_MEMORY;
    }

    op = dosync ? CUSTOM_HANDLER_STAT_SYNC_GET : CUSTOM_HANDLER_STAT_GET;
    rv = bcm_custom_port_get(unit, port, op, num_stats*2, args, &actual);
    if (rv >= 0) {
        argi = 0;
        for (argi = 0; argi < num_stats; argi++) {
            COMPILER_64_SET(stats[argi], args[argi*2], args[argi*2+1]);
        }
    }
    sal_free(args);
    return rv;
}

char cmd_dispatch_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
    "dispatch <option>\n"
#else
    "dispatch show: list attached bcm units\n"
    "dispatch attach unit type unit: attach bcm unit\n"
    "dispatch detach unit: detach bcm unit\n"
    "dispatch find type unit [subtype]: find matching bcm unit\n"
    "dispatch vcreate unit vid: bcm_vlan_create(unit, vid)\n"
    "dispatch vdestroy unit vid: bcm_vlan_destroy(unit, vid)\n"
    "dispatch vtime count unit vid: time count calls to vcreate\n"
    "dispatch linkreg unit: register for link changes\n"
    "dispatch linkunreg unit: unregister for link changes\n"
    "dispatch l2reg unit: register for l2 changes\n"
    "dispatch l2unreg unit: unregister for l2 changes\n"
    "dispatch attreg: register for bcm dispatch attaches\n"
    "dispatch attunreg unit: unregister for bcm dispatch attaches\n"
    "dispatch cparse key: parse a cpudb key\n"
    "dispatch init_check: check if device was initialized\n"
#endif
    ;

cmd_result_t
cmd_dispatch(int unit, args_t *args)
{
    char  *cmd, *subcmd;
    char  *s, *dtype, *dsub;
    int   u, du, rv, vid;
#ifdef BCM_RPC_SUPPORT
    cpudb_key_t key;
    char keybuf[CPUDB_KEY_STRING_LEN];
#endif /* BCM_RPC_SUPPORT */

    cmd = ARG_CMD(args);
    subcmd = ARG_GET(args);

    if (subcmd == NULL) {
        subcmd = "show";
    }
    if (sal_strcasecmp(subcmd, "show") == 0) {
        for (u = 0; u < BCM_CONTROL_MAX; u++) {
            if (!BCM_UNIT_VALID(u)) {
                continue;
            }
            sal_printf("unit %d: dispatch %s, %s unit %d chip %x.%x.%x "
                       "caps %#x\n",
                       u,
                       BCM_TYPE_NAME(u),
                       BCM_CONTROL(u)->subtype ? BCM_CONTROL(u)->subtype : "-",
                       BCM_CONTROL(u)->unit,
                       BCM_CONTROL(u)->chip_vendor,
                       BCM_CONTROL(u)->chip_device,
                       BCM_CONTROL(u)->chip_revision,
                       BCM_CONTROL(u)->capability);
        }
        return CMD_OK;
    }

    /* dispatch attach N type N */
    if (sal_strcasecmp(subcmd, "attach") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        if (u < -1 || u >= BCM_CONTROL_MAX) {
            sal_printf("ERROR: %s %s: unit %d out of range\n", cmd, subcmd, u);
            return CMD_FAIL;
        }
        dtype = ARG_GET(args);
        if (dtype == NULL) {
            sal_printf("ERROR: %s %s: dispatch type missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: dispatch unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        du = parse_integer(s);
        dsub = ARG_GET(args);
#ifdef BCM_RPC_SUPPORT
        /* As a convenience to the user, treat client type subtype
           strings as cpudb keys, and put into a standard format */
        if (sal_strcasecmp(dtype, "client") == 0) {

            rv = cpudb_key_parse(dsub, &key);
            if (BCM_SUCCESS(rv)) {
                /* put in standard format */
                rv = cpudb_key_format(key, keybuf, sizeof(keybuf));
                if (BCM_SUCCESS(rv)) {
                    dsub = keybuf;
                } else {
                    sal_printf("ERROR: %s %s: could not format key %s: %s\n",
                               cmd, subcmd, dsub, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            } else {
                sal_printf("ERROR: %s %s: could not parse key %s: %s\n",
                           cmd, subcmd, dsub, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
#endif
        
        rv = bcm_attach(u, dtype, dsub, du);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        if (u < 0) {
            sal_printf("[unit %d attached]\n", rv);
        }
        return CMD_OK;
    }

    /* dispatch detach N */
    if (sal_strcasecmp(subcmd, "detach") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_detach(u);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    /* dispatch init_check N*/
    if (sal_strcasecmp(subcmd, "init_check") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_init_check(u);
        return CMD_OK;
    }

    /* dispatch find type remunit subtype */
    if (sal_strcasecmp(subcmd, "find") == 0) {
        dtype = ARG_GET(args);
        if (dtype == NULL) {
            sal_printf("ERROR: %s %s: dispatch type missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: dispatch unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        dsub = ARG_GET(args);

        rv = bcm_find(dtype, dsub, u);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        sal_printf("[unit %d found]\n", rv);
        return CMD_OK;
    }

    /* dispatch vcreate N N */
    if (sal_strcasecmp(subcmd, "vcreate") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: vlanid missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        vid = parse_integer(s);
        rv = bcm_vlan_create(u, vid);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    /* dispatch vdestroy N N */
    if (sal_strcasecmp(subcmd, "vdestroy") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: vlanid missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        vid = parse_integer(s);
        rv = bcm_vlan_destroy(u, vid);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    /* dispatch vtime N N N */
    if (sal_strcasecmp(subcmd, "vtime") == 0) {
        int i, count;
        uint32 btime, etime;
        
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: count missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        count = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: vlanid missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        vid = parse_integer(s);
        btime = sal_time_usecs();
        for (i = 0; i < count; i++) {
            (void)bcm_vlan_create(u, vid);
        }
        etime = sal_time_usecs();
        sal_printf("%d api calls in %u usec (%d usec/call)\n",
                   count, etime-btime, (etime-btime)/count);
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "linkreg") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_linkscan_register(u, cmd_dispatch_link_notify);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "linkunreg") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_linkscan_unregister(u, cmd_dispatch_link_notify);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "l2reg") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_l2_addr_register(u, cmd_dispatch_l2_notify, NULL);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "l2unreg") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_l2_addr_unregister(u, cmd_dispatch_l2_notify, NULL);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

#ifdef  INCLUDE_LIB_STKTASK
    if (sal_strcasecmp(subcmd, "attreg") == 0) {
        rv = bcm_stack_attach_register(cmd_dispatch_attach_notify);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "attunreg") == 0) {
        rv = bcm_stack_attach_unregister(cmd_dispatch_attach_notify);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }
#endif  /* INCLUDE_LIB_STKTASK */

#ifdef  INCLUDE_LIB_CPUDB
    if (sal_strcasecmp(subcmd, "cparse") == 0) {
        char         buf[CPUDB_KEY_STRING_LEN];
        cpudb_key_t  key;

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: key missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        rv = cpudb_key_parse(s, &key);
        if (rv < 0) {
            sal_printf("ERROR: %s %s (cpudb_key_parse): %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = cpudb_key_format(key, buf, CPUDB_KEY_STRING_LEN);
        if (rv < 0) {
            sal_printf("ERROR: %s %s (cpudb_key_format): %s\n",
                       cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        sal_printf("key %x:%x:%x:%x:%x:%x --> %s (hash %d)\n",
                   key.key[0], key.key[1], key.key[2],
                   key.key[3], key.key[4], key.key[5],
                   buf, CPUDB_KEY_HASH(key));
        return CMD_OK;
    }
#endif  /* INCLUDE_LIB_CPUDB */

    /* doesn't really belong here... */
    /* maxframe unit port [size] */
    if (sal_strcasecmp(subcmd, "maxframe") == 0) {
        bcm_port_t p;
        int size;
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            rv = bcm_port_frame_max_get(u, p, &size);
            if (rv < 0) {
                sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
                return CMD_FAIL;
            }
            sal_printf("%d\n", size);
        } else {
            size = parse_integer(s);
            rv = bcm_port_frame_max_set(u, p, size);
            if (rv < 0) {
                sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        return CMD_OK;
    }

    /* doesn't really belong here... */
    /* jam unit port [enable] */
    if (sal_strcasecmp(subcmd, "jam") == 0) {
        bcm_port_t p;
        int enable;
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            rv = bcm_port_jam_get(u, p, &enable);
            if (rv < 0) {
                sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
                return CMD_FAIL;
            }
            sal_printf("jamming %s\n", enable ? "enabled" : "disabled");
        } else {
            enable = parse_integer(s);
            rv = bcm_port_jam_set(u, p, enable);
            if (rv < 0) {
                sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        return CMD_OK;
    }

    /* dispatch custom_reg <unit> */
    if (sal_strcasecmp(subcmd, "custom_reg") == 0) {
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        rv = bcm_custom_register(u, custom_handler, NULL);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    /* dispatch custom_stat <unit> <port> */
    if (sal_strcasecmp(subcmd, "custom_stat") == 0) {
        int     p, si;
        const static char *sname[] = BCM_STAT_NAME_INITIALIZER;
        uint64  stats[snmpValCount];

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);

        rv = custom_stat_get(u, p, 0, snmpValCount, stats);
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        for (si = 0; si < snmpValCount; si++) {
            if (COMPILER_64_IS_ZERO(stats[si])) {
                continue;
            }
            if (COMPILER_64_HI(stats[si]) == 0) {
                cli_out("\t%18u\t%s (stat %d)\n",
                        COMPILER_64_LO(stats[si]), sname[si], si);
            } else {
                cli_out("\t0x%08x%08x\t%s (stat %d)\n",
                        COMPILER_64_HI(stats[si]),
                        COMPILER_64_LO(stats[si]),
                        sname[si], si);
            }

        }
        return CMD_OK;
    }

    /* dispatch custom_stattime <count> <unit> <port> */
    if (sal_strcasecmp(subcmd, "custom_stattime") == 0) {
        int i, count, p;
        uint32 btime, etime;
        uint64 stats[snmpValCount];

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: count missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        count = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);
        btime = sal_time_usecs();
        for (i = 0; i < count; i++) {
            (void)custom_stat_get(u, p, 0, snmpValCount, stats);
        }
        etime = sal_time_usecs();
        sal_printf("%d api calls in %u usec (%d usec/call)\n",
                   count, etime-btime, (etime-btime)/count);
        return CMD_OK;
    }

    /* dispatch stattime <count> <unit> <port> */
    if (sal_strcasecmp(subcmd, "stattime") == 0) {
        int i, count, p, si;
        uint32 btime, etime;
        uint64 stats[snmpValCount];

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: count missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        count = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);
        btime = sal_time_usecs();
        for (i = 0; i < count; i++) {
            for (si = 0; si < snmpValCount; si++) {
                (void)bcm_stat_get(u, p, si, &stats[si]);
            }
        }
        etime = sal_time_usecs();
        sal_printf("%d api calls in %u usec (%d usec/call)\n",
                   count, etime-btime, (etime-btime)/count);
        return CMD_OK;
    }

    /* dispatch pause_addr <unit> <port> [<mac>] */
    if (sal_strcasecmp(subcmd, "pause_addr") == 0) {
        int p;
        bcm_mac_t mac;
        char macstr[SAL_MACADDR_STR_LEN];

        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: unit missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        u = parse_integer(s);
        s = ARG_GET(args);
        if (s == NULL) {
            sal_printf("ERROR: %s %s: port missing\n", cmd, subcmd);
            return CMD_FAIL;
        }
        p = parse_integer(s);

        s = ARG_GET(args);
        if (s == NULL) {
            rv = bcm_port_pause_addr_get(u, p, mac);
            if (rv >= 0) {
                format_macaddr(macstr, mac);
                sal_printf("port %d.%d: pause mac addr %s\n", u, p, macstr);
            }
        } else {
            if (parse_macaddr(s, mac) < 0) {
                sal_printf("ERROR: %s %s: illegal mac-addr\n", cmd, subcmd);
                return CMD_FAIL;
            }
            rv = bcm_port_pause_addr_set(u, p, mac);
        }
        if (rv < 0) {
            sal_printf("ERROR: %s %s: %s\n", cmd, subcmd, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}
