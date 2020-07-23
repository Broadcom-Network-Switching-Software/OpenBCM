/*! \file bcma_bcmbdcmd_cmicx_pid.c
 *
 * BCMBD CLI 'pid' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

#include <bcma/bcmbd/bcma_bcmbdcmd_cmicx_pid.h>

int
bcma_bcmbdcmd_cmicx_pid(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_bcmbd_id_t sid;
    int unit;
    const char *arg;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return bcma_cli_parse_error("identifier", arg);
    }

    if (bcma_bcmbd_parse_id(arg, &sid, 0) < 0) {
        return bcma_cli_parse_error("identifier", arg);
    }

    if (sid.addr.valid) {
        cli_out("Address: '%s' : 0x%"PRIx32" : %d : %d\n",
                sid.addr.name, sid.addr.name32,
                sid.addr.start, sid.addr.end);
    }
    else {
        cli_out("Address: (invalid)\n");
    }

    if (sid.block.valid) {
        cli_out("Block:   '%s' : 0x%"PRIx32" : %d : %d\n",
                sid.block.name, sid.block.name32,
                sid.block.start, sid.block.end);
    }
    else {
        cli_out("Block:   (invalid)\n");
    }

    if (sid.pipe.valid) {
        cli_out("Pipe:    '%s' : 0x%"PRIx32" : %d : %d\n",
                sid.pipe.name, sid.pipe.name32,
                sid.pipe.start, sid.pipe.end);
    }
    else {
        cli_out("Pipe:    (invalid)\n");
    }

    if (sid.port.valid) {
        cli_out("Port:    '%s' : 0x%"PRIx32" : %d : %d\n",
                sid.port.name, sid.port.name32,
                sid.port.start, sid.port.end);
    }
    else {
        cli_out("Port:    (invalid)\n");
    }

    return 0;
}
