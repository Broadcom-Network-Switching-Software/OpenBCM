/*! \file shell.c
 *
 * LTSW basic shell command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <shared/shr_bprof.h>
#include <bcm/init.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <appl/diag/sysconf_ltsw.h>

cmd_result_t
cmd_ltsw_init(int unit, args_t *arg)
{
    cmd_result_t rv = CMD_OK;
    char *c;
    uint32 flags = 0;
    int bcm_rv, module_num, found;
    SHR_BPROF_STATS_DECL;

    c = ARG_GET(arg);

    if (!c || !sal_strcasecmp(c, "soc")) {
        SHR_BPROF_STATS_TIME(SHR_BPROF_SOC_INIT) {
            /* Prevent hardware access during chip reset */
            system_shutdown(unit, 0);

            if (ltsw_pw_running(unit)) {
                (void) ltsw_pw_clean(unit);
            }

            if (sysconf_ltsw_dev_init(unit) < 0) {
                cli_out("%s: Unable to initialize device: %d\n",
                        ARG_CMD(arg), unit);
                return CMD_FAIL;
            }

            sysconf_ltsw_dev_env_set(unit);
        }
    } else if (!sal_strcasecmp(c, "bcm")) {
        SHR_BPROF_STATS_TIME(SHR_BPROF_BCM_INIT) {
            if ((bcm_rv = bcm_init(unit)) < 0) {
                cli_out("%s: Unable to initialize BCM driver on unit %d: %s\n",
                        ARG_CMD(arg), unit, bcm_errmsg(bcm_rv));
                rv = CMD_FAIL;
            }
        }
    } else if (!sal_strcasecmp(c, "all")) {
        /* Prevent hardware access during chip reset */
        system_shutdown(unit, 0);
        if ((bcm_rv = system_init(unit)) < 0) {
            cli_out("%s: Unable to reset and initialize BCM driver: %s\n",
                    ARG_CMD(arg), bcm_errmsg(bcm_rv));
            rv = CMD_FAIL;
        }
    } else { /* look for selective init */
        if (!sysconf_ltsw_dev_inited(unit)) {
            cli_out("%s: Device %d was not initialized!"
                    " Please try \"init soc\" first.\n",
                    ARG_CMD(arg), unit);
            return CMD_FAIL;
        }

        if (bcm_attach_check(unit) == BCM_E_UNIT) {
            if ((bcm_rv = bcm_attach(unit, NULL, NULL, unit)) < 0) {
                return CMD_FAIL;
            }
        }

        do {
            found = 0;
            for (module_num = 0; module_num < BCM_MODULE__COUNT; module_num++) {
                if (!sal_strcasecmp(bcm_module_name(unit, module_num), c)) {
                    found = 1;
                    flags = (uint32) module_num;
                    if ((bcm_rv = bcm_init_selective(unit, flags)) < 0) {
                        cli_out("%s: Unable to initialize %s (flags 0x%x): %s\n",
                                ARG_CMD(arg), c, flags, bcm_errmsg(bcm_rv));
                        rv = CMD_FAIL;
                     }
                    break;
                }
            }
            if (!found) {
                cli_out("Unknown module: %s\n", c);
                rv = CMD_USAGE;
                break;
            }
        } while ((c = ARG_GET(arg)) != NULL);
    }

#ifdef BCM_BPROF_STATS
    shr_bprof_stats_time_end();
#endif

    return rv;
}
