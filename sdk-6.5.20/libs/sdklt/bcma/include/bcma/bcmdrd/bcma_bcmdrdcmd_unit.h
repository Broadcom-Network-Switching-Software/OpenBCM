/*! \file bcma_bcmdrdcmd_unit.h
 *
 * CLI 'unit' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDRDCMD_UNIT_H
#define BCMA_BCMDRDCMD_UNIT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMDRDCMD_UNIT_DESC \
    "List device-specific information."

/*! Syntax for CLI command. */
#define BCMA_BCMDRDCMD_UNIT_SYNOP \
    "[*] [all] [id] [resource] [device] [chip] [block] [stat]"

/*! Help for CLI command. */
#define BCMA_BCMDRDCMD_UNIT_HELP \
    "By default the device ID information is shown for the current unit.\n" \
    "Use 'all' to show all available information.\n" \
    "Specifying '*' will iterate over all attached units.\n" \
    "The list of all attached units will be saved in the local environment\n" \
    "variable \"$0\", which can be useful for scripting."

/*! Examples for CLI command. */
#define BCMA_BCMDRDCMD_UNIT_EXAMPLES \
    "* id res"

/*!
 * \brief CLI 'unit' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmdrdcmd_unit(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMDRDCMD_UNIT_H */
