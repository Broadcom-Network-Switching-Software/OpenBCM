/*! \file bcma_bcmbdcmd_cmicd_insert.h
 *
 * CLI 'insert' command for CMICd
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBDCMD_CMICD_INSERT_H
#define BCMA_BCMBDCMD_CMICD_INSERT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMBDCMD_CMICD_INSERT_DESC \
    "Insert chip hash table entry."

/*! Syntax for CLI command. */
#define BCMA_BCMBDCMD_CMICD_INSERT_SYNOP \
    "<expr> <value> ... | <field>=<value> ... | all=<value>"

/*! Help for CLI command. */
#define BCMA_BCMBDCMD_CMICD_INSERT_HELP \
    "Insert (or replace) chip hash table entry according to entry key.\n" \
    "Entry may be inserted either by entering raw 32-bit word values or\n" \
    "by specifying each field separately."

/*! Examples for CLI command. */
#define BCMA_BCMBDCMD_CMICD_INSERT_EXAMPLES \
    "L2Xm L2:VLAN_ID=2 L2:MAC_ADDR=0x010203040506 VALID=1"

/*!
 * \brief CLI 'insert' command implementation for CMICd.
 *
 * \param [in] cli CLI object.
 * \param [in] args Argument list.
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmbdcmd_cmicd_insert(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMBDCMD_CMICD_INSERT_H */
