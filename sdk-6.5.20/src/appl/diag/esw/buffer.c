/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI Buffer command
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif /*BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif /*BCM_TRIUMPH3_SUPPORT */

char cmd_buffer_usage[] =
    "  buffer <object> [<attribute>=<value>] ...\n"
    "  buffer check\n";

cmd_result_t
cmd_buffer(int unit, args_t *arg)
{
    char *subcmd;
    int rv = 0;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "check")) {
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = soc_tr3_mmu_config_init(unit, TRUE);
        }
#endif

#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TRIDENT(unit)) {
            rv = soc_trident_mmu_config_init(unit, TRUE);
        }
#endif
    } else {
        return CMD_USAGE;
    }

    if (rv < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

#endif /* BCM_TRIDENT_SUPPORT */
