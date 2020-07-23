/*! \file bcma_bcmltcmd_ltcapture.h
 *
 * CLI command related to logical table capture.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLTCMD_LTCAPTURE_H
#define BCMA_BCMLTCMD_LTCAPTURE_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMLTCMD_LTCAPTURE_DESC \
    "Capture of logical table operations."

/*! Syntax for CLI command. */
#define BCMA_BCMLTCMD_LTCAPTURE_SYNOP \
    "start [LogFile=<file>] [WriteOnly=0|1]\\\n" \
    "      [VerboseRealTime=0|1] [VerboseRealTimeLogFile=<file>]\n" \
    "stop\n" \
    "status\n" \
    "replay [LogFile=<file>] [Timing=0|1] [Verbose=0|1] [Submit=0|1]\\\n" \
    "       [OutputFile=<file>] [Raw=0|1]"

/*! Help for CLI command. */
#define BCMA_BCMLTCMD_LTCAPTURE_HELP \
    "This command will capture table operations invoked through the BCMLT\n" \
    "API and store them in a log file along with a time stamp.\n" \
    "The captured table operations can then be replayed later on demand.\n" \
    "\n" \
    "If the 'VerboseRealTime' option is set to true, the captured data\n" \
    "will be displayed as CLI commands in real time. To capture the\n" \
    "CLI commands output in a file, use the 'VerboseRealTimeLogFile'\n" \
    "parameter.\n" \
    "In RealTimeVerbose mode the normal capture function can be disabled by\n" \
    "setting the 'LogFile' to \"-\".\n" \
    "\n" \
    "To skip read operations such as 'lookup' and 'traverse' from\n" \
    "the capture, set the 'WriteOnly' option to true.\n" \
    "\n" \
    "By default the replay is running in verbose mode without submitting\n" \
    "the table operations (dry run). The 'Submit' parameter must be\n" \
    "turned on for the replay to take effect.\n" \
    "The replay will be executed as fast as the system permits unless the\n" \
    "'Timing' parameter is set to true, in which case the replay function\n" \
    "will attempt to impose the original timing.\n" \
    "Set 'Raw' parameter to 1 to log the human readable replay to a raw\n" \
    "staged output result. This option will always be true when sumbit=1.\n" \
    "\n" \
    "If a log file name is not specified, the name 'bcmlt.lcap' will be\n" \
    "used. If the 'OutputFile' parameter is set, the replay result\n" \
    "will be logged to the specified file name."

/*!
 * \brief Logical table capture command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmltcmd_ltcapture(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the ltcapture command.
 *
 * This function can be called before the CLI object is about to be destroyed
 * to prevent resource leak when the capture is not stopped before the CLI is
 * terminated.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmltcmd_ltcapture_cleanup(bcma_cli_t *cli);

#endif /* BCMA_BCMLTCMD_LTCAPTURE_H */
