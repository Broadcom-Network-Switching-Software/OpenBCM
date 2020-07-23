/*! \file bcma_bcmptmcmd_ptm.h
 *
 * CLI 'ptm' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPTMCMD_PTM_H
#define BCMA_BCMPTMCMD_PTM_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPTMCMD_PTM_DESC  "Execute a PTM debug command."

/*! Syntax for CLI command. */
#define BCMA_BCMPTMCMD_PTM_SYNOP "wal_stats show | enable | disable | clear"

/*! Help for CLI command. */
#define BCMA_BCMPTMCMD_PTM_HELP \
    "wal_stats command can enable, disable, show, clear statistics " \
    "related to WAL buffer usage."

/*! Examples for CLI command. */
#define BCMA_BCMPTMCMD_PTM_EXAMPLES \
    "wal_stats enable\n" \
    "wal_stats show\n" \
    "wal_stats disable\n" \
    "wal_stats clear"

/*!
 * \brief CLI 'ptm' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmptmcmd_ptm(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPTMCMD_PTM_H */
