/*! \file bcma_bcmbdcmd_cmicx_pop.h
 *
 * CLI 'pop' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_POP_H
#define BCMA_BCMBDCMD_CMICX_POP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_POP_DESC \
    "Pop an entry from a FIFO."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_POP_SYNOP \
    "[raw] [cf] <expr>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_POP_HELP \
    "Pop an entry from a device FIFO.\n" \
    "The raw option suppresses the decoding of individual memory fields.\n" \
    "The cf option selects a compact-field output format, which is useful\n" \
    "for slow terminals and high-volume output."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_POP_EXAMPLES \
    "ING_SER_FIFOm{0}"

/*!
 * \brief CLI 'pop' command implementation for CMICx.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_pop(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_POP_H */
