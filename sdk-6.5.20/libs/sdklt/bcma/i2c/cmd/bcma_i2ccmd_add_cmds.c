/*! \file bcma_i2ccmd_add_cmds.c
 *
 * Add CLI commands for i2c operation support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/i2c/bcma_i2ccmd.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2ccmd_op.h>
#include <bcma/i2c/bcma_i2ccmd_bb.h>

static bcma_cli_command_t shcmd_i2c = {
    .name = "i2c",
    .func = bcma_i2ccmd_i2c,
    .desc = BCMA_I2CCMD_I2C_DESC,
    .synop = BCMA_I2CCMD_I2C_SYNOP,
    .help = { BCMA_I2CCMD_I2C_HELP },
    .examples = BCMA_I2CCMD_I2C_EXAMPLES
};

static bcma_cli_command_t shcmd_bb = {
    .name = "bb",
    .func = bcma_i2ccmd_bb,
    .desc = BCMA_I2CCMD_BB_DESC,
    .synop = BCMA_I2CCMD_BB_SYNOP,
    .help = { BCMA_I2CCMD_BB_HELP },
    .examples = BCMA_I2CCMD_BB_EXAMPLES
};

int
bcma_i2ccmd_add_cmds(bcma_cli_t *cli)
{
    bcma_cli_add_command(cli, &shcmd_i2c, 0);
    bcma_cli_add_command(cli, &shcmd_bb, 0);

    return 0;
}

