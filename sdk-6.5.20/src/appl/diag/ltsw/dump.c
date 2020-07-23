/*! \file dump.c
 *
 * LTSW dump CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/debug.h>
#include <soc/ltsw/config.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/pktio.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/ipmc.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/flexstate.h>

static cmd_result_t
do_dump_sw(int unit, args_t *a)
{
    char *c;
    int  dump_all = FALSE;

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(c, "all")) {
        dump_all = TRUE;
    }

    if (dump_all || !sal_strcasecmp(c, "config")) {
        soc_ltsw_config_sw_dump(bcmi_ltsw_dev_dunit(unit));
    }

    if (dump_all || !sal_strcasecmp(c, "cosq")) {
        bcmi_ltsw_cosq_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "dlb")) {
        bcmi_ltsw_dlb_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "flow")) {
        bcmi_ltsw_flow_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "l2")) {
        bcmi_ltsw_l2_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "l2_if")) {
        bcmi_ltsw_l2_if_dump_sw(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "mirror")) {
        bcmi_ltsw_mirror_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "port")) {
        bcmi_ltsw_port_dump_sw(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "profile")) {
        bcmi_ltsw_profile_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "sbr")) {
        bcmi_ltsw_sbr_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "stg")) {
        bcmi_ltsw_stg_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "trunk")) {
        bcmi_ltsw_trunk_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "vlan")) {
        bcmi_ltsw_vlan_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "qos")) {
        bcmi_ltsw_qos_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "mpls")) {
        bcmi_ltsw_mpls_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "pktio")) {
        bcmi_ltsw_pktio_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "multicast")) {
        bcmi_ltsw_multicast_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "ipmc")) {
        bcmi_ltsw_ipmc_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "flexctr")) {
        bcmi_ltsw_flexctr_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "flexstate")) {
        bcmi_ltsw_flexstate_sw_dump(unit);
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_dump(int unit, args_t *a)
{
    char *arg1;
    volatile int rv = CMD_FAIL;
    parse_table_t pt;
    volatile char *fname = "";
    int append = FALSE;
    volatile int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf ctrl_c;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        goto done;
    }

    if (parse_arg_eq(a, &pt) < 0) {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0) {
        /*
         * Catch control-C in case if using file output option.
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c)) {
            rv = CMD_INTR;
            goto done;
        }
#endif

        sh_push_ctrl_c(&ctrl_c);

        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled()) {
            cli_out("%s: Can't dump to file while logging is enabled\n",
                    ARG_CMD(a));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *)fname, append) < 0) {
            cli_out("%s: Could not start log file\n", ARG_CMD(a));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);

        console_disabled = 1;
    }

    if ((arg1 = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* SW data structures dump */
    if (!sal_strcasecmp(arg1, "sw")) {
        rv = do_dump_sw(unit, a);
        goto done;
    }

    cli_out("Unknown option to dump (use 'help dump' for more info)\n");

    rv = CMD_FAIL;

done:
    if (fname[0] != 0) {
        bslfile_close();
    }

    if (console_disabled && console_was_on) {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c) {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

