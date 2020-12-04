/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag SDKLT LT shell command
 */

#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/ltsw/cmdlist.h>
#include <bcm_int/ltsw/dev.h>

#include <bcma/sys/bcma_sys_conf_hybrid_sdk.h>


char cmd_bcmlt_shell_usage[] =
    "  bsh [-c <command>]\n";

cmd_result_t
cmd_bcmlt_shell(int unit, args_t *args)
{
    int dunit;
    const char *opt;
    dunit = bcmi_ltsw_dev_dunit(unit);
    opt = ARG_GET(args);
    if (opt && sal_strcmp(opt, "-c") == 0) {
        /* Execute debug command */
        return bcma_sys_conf_hybrid_sdk_bsh_process(dunit, ARG_GET(args));
    } else {
        bcma_sys_conf_hybrid_sdk_bsh(dunit);
    }

    return CMD_OK;
}

char cmd_sdklt_debug_shell_usage[] =
    "  dsh [-c <command>]\n";

cmd_result_t
cmd_sdklt_debug_shell(int unit, args_t *args)
{
    int dunit;
    const char *opt;
    dunit = bcmi_ltsw_dev_dunit(unit);
    opt = ARG_GET(args);
    if (opt && sal_strcmp(opt, "-c") == 0) {
        /* Execute debug command */
        return bcma_sys_conf_hybrid_sdk_dsh_process(dunit, ARG_GET(args));
    } else {
        bcma_sys_conf_hybrid_sdk_dsh(dunit);
    }

    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */
