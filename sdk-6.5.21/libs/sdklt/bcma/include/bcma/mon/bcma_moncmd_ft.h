/*! \file bcma_moncmd_ft.h
 *
 * CLI 'ft' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_MONCMD_FT_H
#define BCMA_MONCMD_FT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_MONCMD_FT_DESC  "FlowTracker debug command."

/*! Syntax for CLI command. */
#define BCMA_MONCMD_FT_SYNOP \
    "sw dump \n"             \
    "stats learn\n"          \
    "stats export\n"         \
    "stats age\n"


/*! Help for CLI command. */
#define BCMA_MONCMD_FT_HELP \
    "Dump FlowTracker module information."

/*! Examples for CLI command. */
#define BCMA_MONCMD_FT_EXAMPLES BCMA_MONCMD_FT_SYNOP

/*!
 * \brief CLI 'ft' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_moncmd_ft(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_MONCMD_FT_H */
