/*! \file trunk.c
 *
 * LTSW Trunk CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/debug.h>

cmd_result_t
if_ltsw_trunk(int unit, args_t *a)
{
    char *subcmd;
    bcm_trunk_t tid = -1;
    bcm_pbmp_t pbmp;
    int r;
    int psc;
    bcm_pbmp_t arg_pbmp;
    uint32 arg_rtag = 0;
    uint32 arg_tid = 0;
    parse_table_t pt;
    cmd_result_t retCode;
    bcm_trunk_info_t t_add_info;
    int i, j;
    bcm_module_t modid;


    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (sal_strcasecmp(subcmd, "init") == 0) {
        r = bcm_trunk_init(unit);
        if (r < 0) {
            cli_out("%s: trunk init failed: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "deinit") == 0) {
        r = bcm_trunk_detach(unit);
        if (r < 0) {
            cli_out("%s: trunk deinit failed: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    /* trunk show command */
    if (sal_strcasecmp(subcmd, "show") == 0) {
        return _bcm_diag_trunk_show(unit, a);
    }

    r = bcm_stk_my_modid_get(unit, &modid);
    if ((r < 0) && (r != BCM_E_UNAVAIL)) {
        cli_out("BCMX: Could not get mod id %d: %s\n", r, bcm_errmsg(r));
        return r;
    }

    /* If unavailable, indicate unknown */
    if (r < 0) {
        modid = -1;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        bcm_trunk_chip_info_t   ti;
        bcm_module_t tm;
        bcm_port_t   tp;
        bcm_trunk_member_t *member_array = NULL;

        if ((r = bcm_trunk_chip_info_get(unit, &ti)) < 0) {
            cli_out("%s: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id",   PQ_DFL|PQ_INT,  0, &arg_tid,  NULL);
        parse_table_add(&pt, "Rtag", PQ_DFL|PQ_INT,  0, &arg_rtag, NULL);
        parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, 0, &arg_pbmp, NULL);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        tid = arg_tid;
        psc = arg_rtag;
        pbmp = arg_pbmp;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                                 BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }
        j = 0;
        /* coverity[overrun-local] */
        BCM_PBMP_ITER(pbmp, i) {
            tm = modid < 0 ? 0 : modid;
            tp = i;

            r = bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET, tm, tp,
                                   &tm, &tp);
            if ((r < 0) && (r != BCM_E_UNAVAIL)) {
                sal_free(member_array);
                return CMD_FAIL;
            }

            if (j >= BCM_TRUNK_MAX_PORTCNT) {
                sal_free(member_array);
                return CMD_FAIL;
            }
            bcm_trunk_member_t_init(&member_array[j]);
            if (BCM_GPORT_IS_SET(tp)) {
                member_array[j].gport = tp;
            } else {
                BCM_GPORT_MODPORT_SET(member_array[j].gport, tm, tp);
            }
            j += 1;
        }
        bcm_trunk_info_t_init(&t_add_info);
        t_add_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
        t_add_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
        t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
        t_add_info.psc = psc;

        bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
        r = bcm_trunk_set(unit, tid, &t_add_info, j, member_array);

        if (r < 0) {
            bcm_trunk_destroy(unit, tid);
        }
        sal_free(member_array);
    } else if (sal_strcasecmp(subcmd, "remove") == 0) {
        bcm_module_t tm;
        bcm_port_t   tp;
        bcm_trunk_member_t *member_array = NULL;
        int member_count;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0,
                        &arg_tid, NULL);
        parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
                        &arg_pbmp, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        pbmp = arg_pbmp;
        tid = arg_tid;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                                 BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }
        r = bcm_trunk_get(unit, tid, &t_add_info, BCM_TRUNK_MAX_PORTCNT,
                          member_array, &member_count);

        if (r >= 0) {
            BCM_PBMP_CLEAR(pbmp);
            for (i = 0; i < member_count; i++) {
                if (BCM_GPORT_IS_DEVPORT(member_array[i].gport)) {
                    tm = modid < 0 ? 0 : modid;
                    tp = BCM_GPORT_DEVPORT_PORT_GET(member_array[i].gport);
                } else if (BCM_GPORT_IS_MODPORT(member_array[i].gport)) {
                    tm = BCM_GPORT_MODPORT_MODID_GET(member_array[i].gport);
                    tp = BCM_GPORT_MODPORT_PORT_GET(member_array[i].gport);
                } else {
                    sal_free(member_array);
                    return CMD_FAIL;
                }
                BCM_PBMP_PORT_ADD(pbmp, tp);
            }
            BCM_PBMP_REMOVE(pbmp, arg_pbmp);

            j = 0;
            BCM_PBMP_ITER(pbmp, i) {
                tm = modid < 0 ? 0 : modid;
                tp = i;

                r = bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET, tm, tp,
                                       &tm, &tp);
                if ((r < 0) && (r != BCM_E_UNAVAIL)) {
                    sal_free(member_array);
                    return CMD_FAIL;
                }

                bcm_trunk_member_t_init(&member_array[j]);
                if (BCM_GPORT_IS_SET(tp)) {
                    member_array[j].gport = tp;
                } else {
                    BCM_GPORT_MODPORT_SET(member_array[j].gport, tm, tp);
                }
                j += 1;
            }

            if (j) {
                r = bcm_trunk_set(unit, tid, &t_add_info, j, member_array);
            } else {
                r = bcm_trunk_destroy(unit, tid);
            }
        }

        sal_free(member_array);
    } else if (sal_strcasecmp(subcmd, "psc") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0, &arg_tid, NULL);
        parse_table_add(&pt, "Rtag", PQ_DFL|PQ_INT, 0, &arg_rtag, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        tid = arg_tid;
        psc = arg_rtag;

        r = bcm_trunk_psc_set(unit, tid, psc);
    } else {
        return CMD_USAGE;
    }

    if (r < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}
