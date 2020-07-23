/*! \file bcma_bcmltcmd_lt.h
 *
 * CLI command related to logical table access.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_LT_H
#define BCMA_BCMLTCMD_LT_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_LT_DESC \
    "Logical table access."

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_LT_SYNOP \
    "list [<opt>] [<lt-match-string> ...]\n" \
    "traverse [<attrlist>] [<opt>] [<lt-match-string> ...]\n" \
    "[<cmd-params>] <table-op> [<table-op> ...]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_LT_HELP \
    "<table-op> := <lt-name> [<attrlist>] [<env-var-list>]\n" \
    "                        <entry-op> [<entry-op> ...]\n" \
    "<entry-op> := <op> [<opt>] [<field>=<val> ...]\n\n" \
    "The <table-op> performs logical table operation(s) through an entry\n" \
    "in the given logical table <lt-name>.\n" \
    "The asterisk (*) can be used to set the specified field to\n" \
    "its default value.\n" \
    "\n" \
    "<cmd-params> := <param>=<val> [<param>=<val> ...] | =\n" \
    "  The <cmd-params> is used to configure the commit parameters for\n" \
    "  the logical table operations. Default is to commit the logical\n" \
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
    "  Currently the supported <attr>s are CACHE_BYPASS, FILTER_DFLT,\n" \
    "  LPM_LOOKUP, TRVS_SNAPSHOT and TRVS_DONE.\n" \
    "\n" \
    "<env-var-list>: EnvVar=<str>[,<str>...]\n" \
    "  Applicable to LOOKUP, index-with-allocate INSERT operation or\n" \
    "  TRAVERSE operation with -l option (LOOKUP while traversing) to set\n" \
    "  result for the fields name containing <str> to CLI local variables.\n" \
    "  Use '*' to set all fields for LOOKUP operation and key fields for\n" \
    "  index-with-allocate INSERT operation.\n" \
    "  For array field, each element n will be expanded to variable name\n" \
    "  <field_name>__n associate with the element value. Token matching for\n" \
    "  specific index m or indices range m to n of array field can be\n" \
    "  specified as <str>[m] or <str>[m-n].\n" \
    "  For TRAVERSE operation with no options specified, each key field\n" \
    "  value of the traversed entry will be concatenated with commas and\n" \
    "  set to CLI local variables.\n" \
    "  The variables will be automatically cleared at the next LT operation.\n" \
    "\n" \
    "The valid <op>s in table <entry-op> are:\n" \
    "  insert, delete, update, lookup and traverse.\n" \
    "Field qualifier is supported for the traverse opcode to filter\n" \
    "unwanted entries from the traverse result. The supported field\n" \
    "qualifiers are ==, !=, >=, <=, > and <.\n" \
    "\n" \
    "<opt>: A dash followed by one or more letters.\n" \
    "lt <table-op>\n" \
    "  -a: Auto-fill default values for any unspecified key fields.\n" \
    "      Applicable to insert/update/lookup/delete operations.\n" \
    "  -i: Automatically attempt an insert operation if an update\n" \
    "      operation returns entry-not-found.\n" \
    "  -f: Force all unspecified fields to reset to their default values.\n" \
    "      Applicable to update operation.\n" \
    "  -l: Applicable to traverse operation to perform lookup in traverse.\n" \
    "  -u: Applicable to traverse operation to perform update in traverse.\n" \
    "  -d: Applicable to traverse operation to perform delete in traverse.\n" \
    "lt list\n" \
    "  -l: Use long output format when listing table information.\n" \
    "  -b: Use brief output format when listing table information.\n" \
    "  -d: Display table documentation.\n" \
    "  -m: Display physical table mapping information.\n" \
    "lt traverse\n" \
    "  -l: List all entry fields instead of key fields only.\n" \
    "      LTs with no entries will be silently ignored.\n" \
    "  -v: List the traverse result in verbose mode. All matching LTs\n" \
    "      will be displayed regardless of the number of entries.\n" \
    "\n" \
    "The \'lt list\' and \'lt traverse\' commands can operate on multiple\n" \
    "LTs depending on the specified lt-match-string(s)."

/*! Examples for CLI command. */
#define BCMA_BCMLTCMD_LT_EXAMPLES \
    "list -b\n" \
    "list -l ^TABLE\n" \
    "list -m TABLE\n" \
    "list -d CTR_MAC\n" \
    "list -ld @CTR_MAC\n" \
    "CTR_CONTROL update COLLECTION_ENABLE=1 INTERVAL=2000000\\\n" \
    "PORT_ID[1-3]=1,PORT_ID[5]=1,PORT_ID[7]=1\n" \
    "CTR_MAC insert PORT_ID=2 TX_PKT=100 RX_PKT=90\n" \
    "CTR_MAC lookup PORT_ID=2\n" \
    "CTR_MAC attr=CACHE_BYPASS lookup PORT_ID=2\n" \
    "CTR_MAC update -i PORT_ID=3 TX_PKT=200\n" \
    "CTR_MAC traverse\n" \
    "CTR_MAC traverse -u TX_PKT=0 RX_PKT=0\n" \
    "CTR_MAC traverse -l OPERATIONAL_STATE==VALID TX_BYTES>0\n" \
    "traverse -v CTR_MAC\n" \
    "CTR_CONTROL update INTERVAL=* PORT_ID[5]=0\n" \
    "CTR_CONTROL EnvVar=MULTIPLIER,PORT_ID[0],id[3-5] lookup"

/*!
 * \brief Logical table access command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_lt(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMLTCMD_LT_H */
