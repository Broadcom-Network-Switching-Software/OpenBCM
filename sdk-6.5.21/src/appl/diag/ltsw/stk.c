/*! \file stk.c
 *
 * LTSW Stack CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* SDK6 API header file */
#include <bcm/vlan.h>
#include <bcm/stack.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>

/* Mixed header file */
#include <bcm_int/ltsw/stack.h>

/* SDKLT header file */

#define INT_TO_PTR(x)           ((void *)((sal_vaddr_t)(x)))

static void
_report_flags(int unit, int port, uint32 flags) {
    if (port > 0) {
        cli_out("Stack flags for unit %d, port %d: (0x%x)\n", unit,
                port, flags);
    } else {
        cli_out("Stack flags for unit %d: (0x%x)\n", unit, flags);
    }
    if (flags & (BCM_STK_CAPABLE | BCM_STK_ENABLE | BCM_STK_INACTIVE |
                 BCM_STK_SIMPLEX | BCM_STK_DUPLEX)) {
        cli_out("    %s%s%s%s%s\n",
                flags & BCM_STK_CAPABLE  ? "capable " : "",
                flags & BCM_STK_ENABLE   ? "enable " : "",
                flags & BCM_STK_INACTIVE ? "inactive " : "",
                flags & BCM_STK_SIMPLEX  ? "simplex " : "",
                flags & BCM_STK_DUPLEX   ? "duplex " : "");
    }
    if (flags & (BCM_STK_NONE | BCM_STK_SL | BCM_STK_HG |
                 BCM_STK_INTERNAL | BCM_STK_NO_LINK)) {
        cli_out("    %s%s%s%s%s\n",
                flags & BCM_STK_NONE  ? "none " : "",
                flags & BCM_STK_SL   ? "sl " : "",
                flags & BCM_STK_HG ? "hg " : "",
                flags & BCM_STK_INTERNAL ? "internal " : "",
                flags & BCM_STK_NO_LINK ? "no_link " : "");
    }
}

cmd_result_t
cmd_ltsw_stk_port_set(int unit, args_t *args)
{
    parse_table_t pt;
    int rv;
    int stk_unit = unit;
    int stk_port = -1; /* This is intentionally a port */
    int capable = 0,
        enable = 1,
        inactive = 0,
        simplex = 0,
        internal = 0,
        duplex = 0;
    uint32 flags = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT,
                    INT_TO_PTR(unit), &stk_unit, 0);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &stk_port, 0);
    parse_table_add(&pt, "Capable", PQ_DFL|PQ_BOOL, 0, &capable, 0);
    parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, INT_TO_PTR(1), &enable, 0);
    parse_table_add(&pt, "Inactive", PQ_DFL|PQ_BOOL, 0, &inactive, 0);
    parse_table_add(&pt, "Simplex", PQ_DFL|PQ_BOOL, 0, &simplex, 0);
    parse_table_add(&pt, "Duplex", PQ_DFL|PQ_BOOL, 0, &duplex, 0);
    parse_table_add(&pt, "Internal", PQ_DFL|PQ_BOOL, 0, &internal, 0);
    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);
    if (stk_port < 0) {
        cli_out("ERROR: Must specify port number > 0\n");
        return CMD_USAGE;
    }

    flags |= capable ?   BCM_STK_CAPABLE   : 0;
    flags |= enable ?    BCM_STK_ENABLE    : 0;
    flags |= inactive ?  BCM_STK_INACTIVE  : 0;
    flags |= simplex ?   BCM_STK_SIMPLEX   : 0;
    flags |= duplex ?    BCM_STK_DUPLEX    : 0;
    flags |= internal ?  BCM_STK_INTERNAL  : 0;

    rv = bcm_stk_port_set(stk_unit, stk_port, flags);
    if (rv < 0) {
        cli_out("ERROR: stk_port_set unit %d port %d flags 0x%x: %s\n",
                stk_unit, stk_port, flags, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_stk_port_get(int unit, args_t *args)
{
    parse_table_t pt;
    int rv;
    int stk_unit = unit;
    int stk_port = -1; /* This is intentionally a port */
    uint32 flags = 0;
    bcm_pbmp_t stk_pbmp, inactive_pbmp;
    int i, start_unit, end_unit;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT,
                    INT_TO_PTR(unit), &stk_unit, 0);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &stk_port, 0);
    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (stk_port >= 0) {
        if (stk_unit < 0) {
            cli_out("ERROR: Can't specify port with unit < 0\n");
            return CMD_FAIL;
        }

        rv = bcm_stk_port_get(stk_unit, stk_port, &flags);
        if (rv < 0) {
            cli_out("ERROR: stk_port_get unit %d port %d flags 0x%x: %s\n",
                    stk_unit, stk_port, flags, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        _report_flags(stk_unit, stk_port, flags);
        return CMD_OK;
    }

    if (unit >= 0) {
        start_unit = unit;
        end_unit = unit + 1;
    } else {
        start_unit = 0;
        end_unit = soc_ndev;
    }

    for (i = start_unit; i < end_unit; i++) {
        rv = bcmi_ltsw_stk_pbmp_get(i, &stk_pbmp, &inactive_pbmp, NULL);
        if (rv < 0) {
            cli_out("ERROR: stk_pbmp_get unit %d: %s\n", i, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        /* coverity[overrun-local] */
        BCM_PBMP_ITER(stk_pbmp, stk_port) {
            rv = bcm_stk_port_get(i, stk_port, &flags);
            if (rv < 0) {
                cli_out("ERROR: stk_port_get unit %d port %d flags 0x%x: %s\n",
                        i, stk_port, flags, bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _report_flags(i, stk_port, flags);
        }
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_stkmode(int unit, args_t *a)
{
    parse_table_t   pt;
    int             rv, modcount;
    int             modid, modportclear;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {  /* show current state */
        rv = bcm_stk_modid_get(unit, &modid);
        if (rv < 0) {
            cli_out("%s: ERROR: bcm_stk_modid_get: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (bcm_stk_modid_count(unit, &modcount) < 0) {
            modcount = 1;
        }

        if (modcount != 1) {
            cli_out("%s: unit %d: module id %d (uses %d module ids)\n",
                    ARG_CMD(a), unit, modid, modcount);
        } else {
            cli_out("%s: unit %d: module id %d\n",
                    ARG_CMD(a), unit, modid);
        }

        return CMD_OK;
    }

    modid = -1;
    modportclear = -1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Modid", PQ_INT|PQ_DFL, 0, &modid, NULL);
    parse_table_add(&pt, "ModPortClear", PQ_BOOL|PQ_DFL,
                    0, &modportclear, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (modid >= 0) {
        rv = bcm_stk_modid_set(unit, modid);
        if (rv < 0) {
            cli_out("%s: ERROR: bcm_stk_modid_set: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    if (modportclear > 0) {
        rv = bcm_stk_modport_clear_all(unit);
        if (rv < 0) {
            cli_out("%s: ERROR: bcm_stk_modport_clear_all: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

