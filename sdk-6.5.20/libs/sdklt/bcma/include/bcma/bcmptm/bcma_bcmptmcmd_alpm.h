/*! \file bcma_bcmptmcmd_alpm.h
 *
 * CLI 'alpm' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPTMCMD_ALPM_H
#define BCMA_BCMPTMCMD_ALPM_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPTMCMD_ALPM_DESC  "Execute a ALPM debug command."

/*! Syntax for CLI command. */
#define BCMA_BCMPTMCMD_ALPM_SYNOP "show [db=0|1|2|3] [OutputFile=<file>] [Verbose=0|1] [Brief=0|1]\n"\
                                  "recreate\n"\
                                  "dump [db=0|1|2|3] [Index=<l1_index>] [Levels=1|2|3]\n"\
                                  "sanity [lt=<table name>]\n"

/*! Help for CLI command. */
#define BCMA_BCMPTMCMD_ALPM_HELP \
    "alpm show\n" \
    "   If the 'db' parameter is not set, default db=0 is used.\n"\
    "   If the 'Verbose' parameter is not set, default verbose=0 is used.\n"\
    "   If the 'Brief' parameter is not set, default brief=0 is used.\n"\
    "   If the 'OutputFile' parameter is set, the output\n" \
    "   will be logged to the specified file name.\n" \
    "\n"\
    "alpm recreate\n"\
    "   Deinit and recreate RM-ALPM states based on PT-cache.\n"\
    "   It is expected after recreate, the show result is same as\n"\
    "   the one prior to recreate. Only exceptition is number of reads.\n" \
    "   After recreate done, time elapsed for this\n"\
    "   operation will be shown. Use this tool only when RM-ALPM is\n"\
    "   in idle, i.e., no commits to ALPM LTs in parallel.\n"\
    "\n"\
    "dump:\n"\
    "   Dump ALPM DB table from top to bottom\n"\
    "   If 'db' is not set, default db=0 is used.\n"\
    "   If 'Index' is not set, it dumps all index.\n"\
    "   If 'Levels' is not set, default levels=1.\n"\
    "\n"\
    "alpm sanity\n"\
    "   Verify all existing entries can be correctly located with \n"\
    "   each's pivot (and pre-pivot if any). This tool is not aimed \n"\
    "   to verifying everything, just focus on the trie. It does \n"\
    "   cross check between various tries: the level-1 pivot trie, \n"\
    "   level-2 pivot trie, level-3 pivot trie, the bucket trie holding\n"\
    "   the pivot, and the bucket trie holding the route entry.\n"\

/*! Examples for CLI command. */
#define BCMA_BCMPTMCMD_ALPM_EXAMPLES \
    "show\n" \
    "show v=1\n" \
    "show v=1 db=0\n" \
    "show v=1 db=0 b=1\n" \
    "show v=1 of=show.log\n" \
    "recreate\n" \
    "dump\n" \
    "dump db=0 index=0x400 levels=1\n" \
    "dump db=2 levels=3\n" \
    "sanity\n" \
    "sanity lt=L3_IPV4_UC_ROUTE\n" \

/*!
 * \brief CLI 'alpm' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmptmcmd_alpm(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPTMCMD_ALPM_H */
