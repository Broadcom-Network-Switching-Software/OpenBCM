/*! \file bcma_bslcmd_debug.h
 *
 * CLI 'debug' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BSLCMD_DEBUG_H
#define BCMA_BSLCMD_DEBUG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BSLCMD_DEBUG_DESC \
    "Enable/Disable debug output."

/*! Syntax for CLI command. */
#define BCMA_BSLCMD_DEBUG_SYNOP \
    "[<var-name> [<value>]]"

/*! Help for CLI command. */
#define BCMA_BSLCMD_DEBUG_HELP \
    "\n" \
    "debug [show] [<layer_list>] [<source_list>] [<severity>|+|-|=]\n" \
    "Options:\n" \
    "show          display current settings\n" \
    "<layer_list>  one or more comma-separated layers\n" \
    "              (if omitted, then all layers)\n" \
    "<source_list> one or more comma-separated sources\n" \
    "              (if omitted, then all sources)\n" \
    "<severity>    Off, Fatal, Error, Warn, Info, Verbose\n" \
    "+             all messages\n" \
    "-             no messages\n" \
    "=             set to defaults\n"

/*! Externded help for CLI command. */
#define BCMA_BSLCMD_DEBUG_HELP_2 \
    "Other usages:\n" \
    "debug sink [<sink_id>] [EnableMIN=<severity>] [EnableMAX=<severity>]\n" \
    "           [Units=<unit-list>] [Ports=<port-list>]\n" \
    "           [eXtra=<param-value>] [PrefixMIN=<severity>]\n" \
    "           [PrefixMAX=<severity>] [PrefixFormat=<str>]\n" \
    "Options:\n" \
    "EnableMIN     minimum severity level for which this sink is enabled\n" \
    "EnableMAX     maximum severity level for which this sink is enabled\n" \
    "Units         unit numbers for which this sink is enabled\n" \
    "Ports         port numbers for which this sink is enabled\n" \
    "eXtra         parameter for which this sink is enabled (-1 to skip)\n" \
    "PrefixMIN     minimum severity level for prefixed log message\n" \
    "PrefixMAX     maximum severity level for prefixed log message\n" \
    "PrefixFormat  prefix format string\n" \
    "    The following format specifiers are supported:\n" \
    "    %F - function name\n" \
    "    %f - file name\n" \
    "    %l - line number\n" \
    "    %u - unit number\n" \
    "    %p - port number\n" \
    "    %x - xtra parameter\n" \
    "    %L - layer of this log message source\n" \
    "    %S - source of this log message\n" \
    "    %s - severity level (string value) of this log message\n" \
    "    %t - time stamp (seconds since system start)\n" \
    "    %T - high resolution timestamp (micro seconds)\n" \
    "    %P - process or thread name\n" \
    "Omitting options will list all installed sinks.\n"


/*!
 * \brief CLI command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bslcmd_debug(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BSLCMD_DEBUG_H */
