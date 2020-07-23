/*! \file bcma_salcmd_date.h
 *
 * CLI 'date' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_DATE_H
#define BCMA_SALCMD_DATE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SALCMD_DATE_DESC \
    "Show current time and date."

/*! Syntax for CLI command. */
#define BCMA_SALCMD_DATE_SYNOP \
    ""

/*! Help for CLI command. */
#define BCMA_SALCMD_DATE_HELP \
    "Show current system time and date."

/*!
 * \brief CLI 'date' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_date(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_SALCMD_DATE_H */
