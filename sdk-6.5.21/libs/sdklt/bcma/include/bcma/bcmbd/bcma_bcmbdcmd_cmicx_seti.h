/*! \file bcma_bcmbdcmd_cmicx_seti.h
 *
 * CLI 'seti' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_SETI_H
#define BCMA_BCMBDCMD_CMICX_SETI_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SETI_DESC \
    "Modify chip register/memory contents (raw)."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SETI_SYNOP \
    "<access-type> <addr> <value>[:<value2>[:<value3> ... ]] [<size>]"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SETI_HELP \
    "Modify raw chip register or memory contents. The <access-type>\n" \
    "must be one of cmic, reg, mem or miim. The optional size\n" \
    "parameter is the number of 32-bit words to write."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SETI_EXAMPLES \
    "mem 0x01900002 0x4\n" \
    "mem 0x06700000 0x112233:0x44550000:0x8 3"

/*!
 * \brief CLI 'seti' command implementation for CMICx.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_seti(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_SETI_H */
