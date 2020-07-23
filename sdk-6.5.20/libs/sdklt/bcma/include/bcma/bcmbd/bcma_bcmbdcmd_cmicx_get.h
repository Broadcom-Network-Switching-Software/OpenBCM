/*! \file bcma_bcmbdcmd_cmicx_get.h
 *
 * CLI 'get' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_GET_H
#define BCMA_BCMBDCMD_CMICX_GET_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_GET_DESC \
    "Get chip register/memory contents."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_GET_SYNOP \
    "<expr> [nz] [raw] [cf] [flags=<symflags>] [<field-filters>]"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_GET_HELP \
    "<field-filters> := [fn=<field-names>] [ff=<field-flags>]\n" \
    "\n" \
    "Read and decode chip register or memory contents, where <expr> is a\n" \
    "register/memory expression in one of the following forms:\n\n" \
    "name\n" \
    "name.port\n" \
    "name.block\n" \
    "name.block.first-port,last-port\n" \
    "name[index]\n" \
    "name[index].block\n" \
    "name[index].block.first-port,last-port\n" \
    "name{pipe}\n" \
    "name{pipe}.block\n" \
    "name{pipe}.block.first-port,last-port\n" \
    "name{pipe}[index]\n" \
    "name{pipe}[index].block\n" \
    "name{pipe}[index].block.first-port,last-port\n"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_GET_HELP_2 \
    "If the nz option is specified, then only registers/memories with\n" \
    "non-zero contents will be displayed. If the diff option is specified,\n" \
    "then only registers/memories with non-default contents will be\n" \
    "displayed. The raw option suppresses the decoding of individual\n" \
    "register/memory fields. The cf option selects a compact-field output\n" \
    "format, which is useful for slow terminals and high-volume output. The\n" \
    "flags option will filter output based on one or more of the following\n" \
    "flags: register, port, counter, memory, r64, big-endian, soft-port,\n" \
    "memmapped.\n" \
    "The fn option suppresses the fields not specified in the field-names\n" \
    "list. And the ff option will filter output based on one or more of the\n" \
    "following field flags: counter, key, and mask.\n" \
    "Use comma-separation to specify multiple flags and fields."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_GET_EXAMPLES \
    "GMACC1r.0\n" \
    "GMACC1r.gport0.1,4\n" \
    "LWMCOSCELLSETLIMITr[1].mmu0.1,2\n" \
    "MMU_CCP_RESEQ_MEMm{3}[0]\n" \
    "nz flags=counter\n" \
    "MMU_CCP_RESEQ_MEMm[0] fn=PG,SHARED"

/*!
 * \brief CLI 'get' command implementation for CMICx.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_get(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_GET_H */
