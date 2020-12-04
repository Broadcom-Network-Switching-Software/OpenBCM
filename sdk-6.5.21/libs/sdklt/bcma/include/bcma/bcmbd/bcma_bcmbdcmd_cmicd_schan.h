/*! \file bcma_bcmbdcmd_cmicd_schan.h
 *
 * BCMBD CLI 'schan' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICD_SCHAN_H
#define BCMA_BCMBDCMD_CMICD_SCHAN_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICD_SCHAN_DESC  "Perform raw S-channel operation."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICD_SCHAN_SYNOP "<DW0> [... <DWn>]"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICD_SCHAN_HELP \
    "Sends the specified raw data as an S-channel message, waits for a\n" \
    "response, then displays the S-channel message buffer (response)."

/*!
 * \brief CLI 'schan' command implementation for CMICd.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicd_schan(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICD_SCHAN_H */
