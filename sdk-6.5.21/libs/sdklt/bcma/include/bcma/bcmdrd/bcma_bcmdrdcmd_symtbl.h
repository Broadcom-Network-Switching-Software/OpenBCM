/*! \file bcma_bcmdrdcmd_symtbl.h
 *
 * CLI 'symtbl' command for decode/encode the symbol-specific value format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDRDCMD_SYMTBL_H
#define BCMA_BCMDRDCMD_SYMTBL_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMDRDCMD_SYMTBL_DESC \
    "Transform data based on symbol table information."

/*! Syntax for CLI command. */
#define BCMA_BCMDRDCMD_SYMTBL_SYNOP \
    "Decode <symbol-name> [nz] <raw-data>\n" \
    "Encode <symbol-name> <field>=<value> ..."

/*! Help for CLI command. */
#define BCMA_BCMDRDCMD_SYMTBL_HELP \
    "This command converts a raw hex value into a symbolic representation\n" \
    "and vice versa.\n" \
    "If the nz option is specified, then only symbol fields with non-zero\n" \
    "contents will be displayed.\n" \
    "The raw data could be specified as either a list of 32-bit hex values\n" \
    "in little endian order, or a single value which concatenates the\n" \
    "32-bit hex values together in big endian order.\n"


/*! Examples for CLI command. */
#define BCMA_BCMDRDCMD_SYMTBL_EXAMPLES \
    "decode L3_DEFIP_LEVEL1m nz 0x0001ffff 0xffffe01c 0 0xc\n" \
    "decode L3_DEFIP_LEVEL1m nz 0xc_00000000_ffffe01c_0001ffff\n" \
    "encode L3_DEFIP_LEVEL1m VALID=1" \

/*!
 * \brief CLI 'symtbl' command handler.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmdrdcmd_symtbl(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMDRDCMD_SYMTBL_H */
