/*! \file bcma_bcmbdcmd_ts.h
 *
 * BCMBD CLI 'TS' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_TS_H
#define BCMA_BCMBDCMD_TS_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_TS_DESC  "TimeSync driver control."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_TS_SYNOP \
    "get [inst=<inst>]\n" \
    "set inst=<inst> TimeStampEnable=true|false"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_TS_HELP \
    "Get or set enable status for timestamp counter. If the retrieved\n" \
    "timestamp counter status is enabled, the current timestamp value\n" \
    "in nanoseconds will be displayed as well."

/*!
 * \brief CLI 'TS' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_ts(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_TS_H */
