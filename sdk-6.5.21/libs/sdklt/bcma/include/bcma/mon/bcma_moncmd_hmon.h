/*! \file bcma_moncmd_hmon.h
 *
 * CLI 'hmon' command for debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_MONCMD_HMON_H
#define BCMA_MONCMD_HMON_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_MONCMD_HMON_DESC  "Show system health."

/*! Syntax for CLI command. */
#define BCMA_MONCMD_HMON_SYNOP \
    "TEMPerature"

/*! Help for CLI command. */
#define BCMA_MONCMD_HMON_HELP \
    "temperature \n" \
    "    - show chip temperature of monitored locations."

/*! Examples for CLI command. */
#define BCMA_MONCMD_HMON_EXAMPLES \
    "TEMPerature"

/*!
 * \brief CLI 'hmon' command implementation.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_moncmd_hmon(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_MONCMD_HMON_H */
