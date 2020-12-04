/*! \file bcma_bcmbdcmd_cmicx_set.h
 *
 * CLI 'set' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_SET_H
#define BCMA_BCMBDCMD_CMICX_SET_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SET_DESC \
    "Modify chip register/memory contents."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SET_SYNOP \
    "[raw] <expr> <value> ... | <field>=<value> ... | all=<value>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SET_HELP \
    "Modify chip register or memory contents. Contents may be modified\n" \
    "either by entering raw 32-bit word values or by changing each\n" \
    "field separately.\n" \
    "If the raw option is specified, the set command will perform\n" \
    "a write-only operation instead of a read-modify-write operation."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_SET_EXAMPLES \
    "GMACC1r.0 RXEN0=1 TXEN0=1\n" \
    "GMACC1r.gport0.[1,2] RXEN0=0b1 TXEN0=0b1\n" \
    "L2Xm[0] MAC_ADDR=0x010203040506\n" \
    "L2Xm[0] all=0\n" \
    "L2Xm[0] 0 0 0\n" \
    "LWMCOSCELLSETLIMITr[1].mmu0.1,2 CELLSETLIMIT=0xa"

/*!
 * \brief CLI 'set' command implementation for CMICx.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_set(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_SET_H */
