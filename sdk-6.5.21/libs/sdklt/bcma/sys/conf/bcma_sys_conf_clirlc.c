/*! \file bcma_sys_conf_clirlc.c
 *
 * Initialization of SDK CLI command completion support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/clirlc/bcma_clirlc.h>

#include <bcma/sys/bcma_sys_conf.h>

#include <bcma/bcmlt/bcma_bcmltcmd_complete.h>

static bcma_clirlc_cmd_t compcmds[] = {
    { "lt", bcma_bcmltcmd_lt_complete },
    { "pt", bcma_bcmltcmd_pt_complete }
};

int
bcma_sys_conf_clirlc_init(void)
{
    int idx;

    if (bcma_clirlc_init() < 0) {
        return -1;
    }

    for (idx = 0; idx < COUNTOF(compcmds); idx++) {
        bcma_clirlc_add_cmd(&compcmds[idx]);
    }

    return 0;
}
