/*! \file bcma_bcmptmcmd_sbr.h
 *
 * CLI 'sbr' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPTMCMD_SBR_H
#define BCMA_BCMPTMCMD_SBR_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPTMCMD_SBR_DESC  "Execute a SBR debug command."

/*! Syntax for CLI command. */
#define BCMA_BCMPTMCMD_SBR_SYNOP "stats show | show_ha"

/*! Help for CLI command. */
#define BCMA_BCMPTMCMD_SBR_HELP \
    "stats show all statistics related to SBR usage from current DB or HA DB."

/*! Examples for CLI command. */
#define BCMA_BCMPTMCMD_SBR_EXAMPLES \
    "stats show - show usage statistics from current DB.\n" \
    "stats show_ha - show usage statistics from HA DB.\n"

/*!
 * \brief CLI 'sbr' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmptmcmd_sbr(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPTMCMD_SBR_H */
