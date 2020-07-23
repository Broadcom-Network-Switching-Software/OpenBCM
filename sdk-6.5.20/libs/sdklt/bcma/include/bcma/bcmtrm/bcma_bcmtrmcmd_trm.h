/*! \file bcma_bcmtrmcmd_trm.h
 *
 * CLI 'trm' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMTRMCMD_TRM_H
#define BCMA_BCMTRMCMD_TRM_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMTRMCMD_TRM_DESC  "Display TRM statistics."

/*! Syntax for CLI command. */
#define BCMA_BCMTRMCMD_TRM_SYNOP "stats"

/*! Help for CLI command. */
#define BCMA_BCMTRMCMD_TRM_HELP \
    "Provide in-use, available and high-water marks for both outstanding\n" \
    "transactions and entries on a system basis."

/*!
 * \brief CLI 'trm' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmtrmcmd_trm(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMTRMCMD_TRM_H */
