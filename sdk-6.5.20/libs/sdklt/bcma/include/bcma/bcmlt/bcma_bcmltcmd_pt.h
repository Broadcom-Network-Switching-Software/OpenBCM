/*! \file bcma_bcmltcmd_pt.h
 *
 * CLI command related to physical table access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_PT_H
#define BCMA_BCMLTCMD_PT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_PT_DESC \
    "Physical table access."

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_PT_SYNOP \
    "list [<opt>] [<pt-match-string> ...]\n" \
    "dump [<opt>] [<pt-match-string> ...]\n" \
    "reset <pt-match-string> [<pt-match-string> ...]\n" \
    "[<cmd-params>] <table-op> [<table-op> ...]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_PT_HELP \
    "<table-op> := <pt-name> [<attrlist>] <entry-op> [<entry-op> ...]\n" \
    "<entry-op> := <op> [<opt>] [<field>=<val> ...]\n" \
    "\n" \
    "The <table-op> performs physical table operation(s) through an entry\n" \
    "in the given physical table <pt-name>.\n" \
    "\n" \
    "<cmd-params> := <param>=<val> [<param>=<val> ...] | =\n" \
    "  The <cmd-params> is used to configure the commit parameters for\n" \
    "  the physical table operations. Default is to commit the physical\n"\
    "  table operations in synchronous entry mode.\n" \
    "Current available parameters are:\n" \
    "  TRans=0|1: Commit in entry or transaction mode.\n" \
    "  TransType=Atomic|Batch: Transaction type.\n" \
    "                          Valid in transaction mode.\n" \
    "  Async=0|1: Commit in synchronous mode or asynchronous mode.\n" \
    "  PRIority=Normal|High: Priority level used in asynchronous mode.\n" \
    "  NoTiFy=Commit|Hw|All|None: Notification option in asynchronous mode.\n" \
    "  ScalarDisplayMode=Hex|Dec|HexDec: Display mode for scalar field.\n" \
    "  ArrayDisplayMode=Hex|Dec|HexDec: Display mode for array field.\n" \
    "  ArrayDelimiter=Comma|Newline|Space: Display style for arrays.\n" \
    "  FieldDelimiter=Comma|Newline|Space: Display style for fields.\n" \
    "  Verbose=0|1: Whether to run the command in verbose mode.\n" \
    "  ContCmdMode=0|1: Valid in transaction mode only and default value is\n" \
    "                   always 0. Identify multiple transaction commands in\n" \
    "                   continuous command mode that starts from 1 and ends\n" \
    "                   with 0.\n" \
    "  DEFault=0|1: Whether to save the current parameters to default.\n" \
    "The current command parameters can be shown and modified by using\n" \
    "the '=' syntax as well.\n" \
    "\n" \
    "<attrlist>: attr=<attr>[,<attr>...]\n" \
    "  Currently supported <attr> is CACHE_BYPASS.\n" \
    "\n" \
    "The valid <op>s in table <entry-op> are: set, get and modify.\n" \
    "\n" \
    "<opt>: A dash followed by one or more letters.\n" \
    "pt list\n" \
    "  -l: Use long output format when listing table information.\n" \
    "  -b: Use brief output format when listing table information.\n" \
    "pt dump\n" \
    "  -n: Show all fields of the entries with non-default value.\n" \
    "  -d: Only show the fields with non-default contents.\n" \
    "pt <table-op>\n" \
    "  -n: Applicable to get and lookup operation to show all fields of the\n" \
    "      entry if it is different from default value.\n" \
    "  -d: Applicable to get and lookup operation to only show the fields\n" \
    "      with non-default contents."

/*! Examples for CLI command. */
#define BCMA_BCMLTCMD_PT_EXAMPLES \
    "list -b\n" \
    "list MMU\n" \
    "list -l ^MMU\n" \
    "dump VRFm\n" \
    "reset VRFm\n" \
    "VRFm set FLEX_CTR_BASE_COUNTER_IDX=0x123 BCMLT_PT_INDEX=1\n" \
    "VRFm attr=CACHE_BYPASS get BCMLT_PT_INDEX=1\n" \
    "L3_TUNNEL_KEY_BUFFERm get BCMLT_PT_INSTANCE=1 BCMLT_PT_INDEX=0\n" \
    "XLPORT_CONFIGr get BCMLT_PT_PORT=257 BCMLT_PT_INDEX=0\n" \
    "XLPORT_MODE_REGr get BCMLT_PT_PORT=0"

/*!
 * \brief Physical table access command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_pt(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMLTCMD_PT_H */
