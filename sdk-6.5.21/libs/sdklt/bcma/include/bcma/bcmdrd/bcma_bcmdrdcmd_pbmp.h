/*! \file bcma_bcmdrdcmd_pbmp.h
 *
 * CLI 'pbmp' command for converting a port list into a port bitmap.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDRDCMD_PBMP_H
#define BCMA_BCMDRDCMD_PBMP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMDRDCMD_PBMP_DESC \
    "Convert a port list string into a port bitmap."

/*! Syntax for CLI command. */
#define BCMA_BCMDRDCMD_PBMP_SYNOP \
    "<port_list>"

/*! Help for CLI command. */
#define BCMA_BCMDRDCMD_PBMP_HELP \
    "The port list string may contain commas to separate port numbers and\n" \
    "hyphens to indicate ranges of ports.\n" \
    "The output port bitmap will also be saved in the local environment\n" \
    "variable \"$0\", which can be useful for scripting."

/*! Examples for CLI command. */
#define BCMA_BCMDRDCMD_PBMP_EXAMPLES \
    "1,5,9,12-16"

/*!
 * \brief CLI 'pbmp' command handler.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmdrdcmd_pbmp(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMDRDCMD_PBMP_H */
