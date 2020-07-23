/*! \file bcma_bcmbdcmd_tsfifo.h
 *
 * BCMBD CLI 'tsfifo' command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_TSFIFO_H
#define BCMA_BCMBDCMD_TSFIFO_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_TSFIFO_DESC  "TSFIFO command."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_TSFIFO_SYNOP \
    "CAPture start [LogFile=<file>]\n" \
    "CAPture stop \n" \
    "CAPture dump [LogFile=<file>] [Raw=0|1]"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_TSFIFO_HELP \
    "CAPture start\n"\
    "    Start capturing events timestamps from TS FIFO and\n"\
    "    store them in a log file.\n" \
    "    If 'LogFile' parameter is not specified, the name\n"\
    "    'tsfifo.cap' will be used.\n\n"\
    "CAPture stop\n"\
    "    Stop capturing events. \n\n"\
    "CAPture dump\n"\
    "    Dump captured log file in human-readable format.\n"\
    "    If 'Raw' parameter is specified to 1, the dump will\n"\
    "    be in comma-separated csv format."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_TSFIFO_EXAMPLES \
    "CAP start LogFile=tsfifo.cap\n" \
    "CAP start\n" \
    "CAP stop\n" \
    "CAP dump LogFile=tsfifo.cap\n" \
    "CAP dump" \


/*!
 * \brief CLI 'tsfifo' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_tsfifo(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_TSFIFO_H */
