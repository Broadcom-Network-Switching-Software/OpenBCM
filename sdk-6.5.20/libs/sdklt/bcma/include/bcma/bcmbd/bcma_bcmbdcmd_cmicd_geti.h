/*! \file bcma_bcmbdcmd_cmicd_geti.h
 *
 * CLI 'geti' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICD_GETI_H
#define BCMA_BCMBDCMD_CMICD_GETI_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICD_GETI_DESC \
    "Get chip register/memory contents (raw)."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICD_GETI_SYNOP \
    "<access-type> <address> [<size>]"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICD_GETI_HELP \
    "Read raw chip register or memory contents. The <access-type>\n" \
    "must be one of cmic, reg, mem or miim. The optional size\n" \
    "parameter is the number of 32-bit words to read."

/*!
 * \brief CLI 'geti' command implementation for CMICd.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicd_geti(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICD_GETI_H */
