/*! \file bcma_moncmd_hchk.h
 *
 * CLI 'hchk' command for debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_MONCMD_HCHK_H
#define BCMA_MONCMD_HCHK_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_MONCMD_HCHK_DESC  "Check health of device."

/*! Help for CLI command. */
#define BCMA_MONCMD_HCHK_HELP \
    "Check the current state of device i.e.\n" \
    "    - All traffic has cleared out cleanly from datapath. \n" \
    "    - No fatal interrupts set. \n" \
    "    - Any credits/pointers used are returned properly.\n" \
    "    - Could be run before starting any new traffic in the system."

/*!
 * \brief CLI 'hchk' command implementation.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
int
bcma_moncmd_hchk(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_MONCMD_HCHK_H */
