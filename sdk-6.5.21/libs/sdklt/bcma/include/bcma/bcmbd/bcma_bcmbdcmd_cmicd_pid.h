/*! \file bcma_bcmbdcmd_cmicd_pid.h
 *
 * BCMBD CLI 'pid' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICD_PID_H
#define BCMA_BCMBDCMD_CMICD_PID_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICD_PID_DESC  "Show how input identifier is parsed."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICD_PID_SYNOP "<identifier>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICD_PID_HELP \
    "This command prints out how an input identifier is parsed.\n" \
    "Used mainly for debugging.\n\n" \
    "If you aren't getting the right information based on the input\n" \
    "identifier, you can use this command to see if it got parsed\n" \
    "incorrectly."

/*!
 * \brief CLI 'pid' command implementation for CMICd.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicd_pid(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICD_PID_H */
