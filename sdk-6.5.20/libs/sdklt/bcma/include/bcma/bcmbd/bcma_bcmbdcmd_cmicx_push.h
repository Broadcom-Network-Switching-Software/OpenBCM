/*! \file bcma_bcmbdcmd_cmicx_push.h
 *
 * CLI 'push' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_PUSH_H
#define BCMA_BCMBDCMD_CMICX_PUSH_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_PUSH_DESC \
    "Push an entry into a FIFO."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_PUSH_SYNOP \
    "[raw] <expr> <value> ... | <field>=<value> ... | all=<value>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_PUSH_HELP \
    "Push an entry into a device FIFO.\n" \

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_PUSH_EXAMPLES \
    "MOS_OSH0_TX_FIFOm[0] 0x34343434 0x12121212"

/*!
 * \brief CLI 'push' command implementation for CMICx.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_push(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_PUSH_H */
