/*! \file bcma_bcmltcmd_counter.h
 *
 * CLI command related to show the collected information from logical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_COUNTER_H
#define BCMA_BCMLTCMD_COUNTER_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_COUNTER_DESC \
    "Show/Clear the counter logical table entries."

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_COUNTER_SYNOP \
    "Show [<flags>] [<types>] [<counter> ...] [<port-list>]\n" \
    "Clear [<types>] [<counter> ...] [<port-list>]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_COUNTER_HELP \
    "Convenience command to show and clear the valid entries for\n" \
    "logical tables with counter attribute.\n" \
    "\n" \
    "<flags> := [Changed] [Same] [Zero] [NonZero] [All]\n" \
    "    Changed - show counter LT fields that changed\n" \
    "    Same    - show counter LT fields that did not change\n" \
    "    Zero    - show counter LT fields that are zero\n" \
    "    NonZero - show counter LT fields that are not zero\n" \
    "Notes:\n" \
    "If neither Changed or Same is specified, Changed will be used.\n" \
    "If neither Zero or NonZero is specified, NonZero will be used.\n" \
    "Use All to display all counters regardless of value.\n" \
    "\n" \
    "<types> := [[!]ErrDisc] [[!]TM]\n" \
    "    ErrDisc - handle counter LTs with Error/Discard type\n" \
    "    TM      - handle counter LTs with Traffic Management type\n" \
    "Notes:\n" \
    "Use <types> to filter the LTs from the default counters list when no\n" \
    "<counter> is specified. Exclamation mark (!) can be added before\n" \
    "<types> to filter the inverse type.\n" \
    "\n" \
    "<counter> : Target counter logical table\n" \
    "<port-list> : Logical port numbers which are separated by \',\' or \'-\'"

/*! Examples for CLI command. */
#define BCMA_BCMLTCMD_COUNTER_EXAMPLES \
    "show\n" \
    "show ed !tm\n" \
    "s all tm 1,4-6,10\n" \
    "clear\n" \
    "clear ed\n" \
    "c ctr_mac 1"

/*!
 * \brief Command to operate on counter logical tables in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_counter(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the counter command.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmltcmd_counter_cleanup(bcma_cli_t *cli);

#endif /* BCMA_BCMLTCMD_COUNTER_H */
