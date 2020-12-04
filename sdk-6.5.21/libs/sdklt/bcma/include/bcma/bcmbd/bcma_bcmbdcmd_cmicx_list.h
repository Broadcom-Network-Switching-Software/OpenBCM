/*! \file bcma_bcmbdcmd_cmicx_list.h
 *
 * CLI 'list' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_LIST_H
#define BCMA_BCMBDCMD_CMICX_LIST_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LIST_DESC \
    "List symbols for a chip."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LIST_SYNOP \
    "[raw] [cf] <symbol-string>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LIST_HELP \
    "List all symbols or symbols partially matching optional\n" \
    "<symbol-string> argument. If raw is specified, only the name\n" \
    "of matching symbols will be displayed. The cf option selects\n" \
    "a compact-field output format, which is useful for slow terminals\n" \
    "and high-volume output."

/*!
 * \brief CLI 'list' command implementation for CMICx.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_list(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_LIST_H */
