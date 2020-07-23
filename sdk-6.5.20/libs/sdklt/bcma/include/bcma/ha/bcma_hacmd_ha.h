/*! \file bcma_hacmd_ha.h
 *
 * CLI 'ha' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_HACMD_HA_H
#define BCMA_HACMD_HA_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_HACMD_HA_DESC  "HA memory usage and associate information."

/*! Syntax for CLI command. */
#define BCMA_HACMD_HA_SYNOP \
    "alloc status [*|<component> ...]\n" \
    "file info"

/*! Help for CLI command. */
#define BCMA_HACMD_HA_HELP \
    "Provide the HA memory used in current device.\n" \
    "The detailed HA memory used in a component will be shown\n" \
    "if the component name or ID is specified. Use \"*\" to show\n" \
    "all detailed HA memory information used in current device."

/*!
 * \brief CLI 'ha' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_hacmd_ha(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_HACMD_HA_H */
