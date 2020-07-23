/*! \file bcma_bcmbdcmd_cmicx_lookup.h
 *
 * CLI 'lookup' command for CMICx
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICX_LOOKUP_H
#define BCMA_BCMBDCMD_CMICX_LOOKUP_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LOOKUP_DESC \
    "Lookup chip hash table entry."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LOOKUP_SYNOP \
    "[raw] [cf] <expr> <value> ... | <field>=<value> ... | all=<value>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LOOKUP_HELP \
    "Lookup chip hash table entry according to entry key.\n" \
    "Entry may be deleted either by entering raw 32-bit word values or\n" \
    "by specifying each field separately.\n" \
    "The raw option suppresses the decoding of individual memory fields.\n" \
    "The cf option selects a compact-field output format, which is useful\n" \
    "for slow terminals and high-volume output."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICX_LOOKUP_EXAMPLES \
    "L2Xm L2:VLAN_ID=2 L2:MAC_ADDR=0x010203040506"

/*!
 * \brief CLI 'lookup' command implementation for CMICx.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicx_lookup(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICX_LOOKUP_H */
