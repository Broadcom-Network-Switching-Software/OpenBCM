/*! \file bcma_clicmd_delay.h
 *
 * CLI 'delay' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLICMD_DELAY_H
#define BCMA_CLICMD_DELAY_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_CLICMD_DELAY_DESC \
    "Put CLI task in a busy-wait loop for some amount of time."

/*! Syntax for CLI command. */
#define BCMA_CLICMD_DELAY_SYNOP \
    "<usec>"

/*! Help for CLI command. */
#define BCMA_CLICMD_DELAY_HELP \
    "Cause the CLI task to busy-wait for a specified number of\n" \
    "microseconds. The actual amount of delay is only approximate.\n"

/*!
 * \brief CLI 'delay' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_clicmd_delay(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_CLICMD_DELAY_H */
