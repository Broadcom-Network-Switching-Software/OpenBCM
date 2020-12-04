/*! \file bcma_salcmd_thread.h
 *
 * CLI 'thread' command.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SALCMD_THREAD_H
#define BCMA_SALCMD_THREAD_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_SALCMD_THREAD_DESC \
    "Thread test command."

/*! Syntax for CLI command. */
#define BCMA_SALCMD_THREAD_SYNOP \
    "status|suspend|resume|start|stop|wake [<timeout-secs>]"

/*! Help for CLI command. */
#define BCMA_SALCMD_THREAD_HELP \
    "The status command may show any of the following letters:\n\n" \
    "  A - Thread is active.\n" \
    "  S - Thread received a stop request.\n" \
    "  W - Thread is waiting for a wake event.\n" \
    "  s - Thread is suspended.\n" \
    "  D - Thread is done.\n" \
    "  E - Thread error.\n" \
    "  b - Thread is a background thread.\n\n" \
    "The suspend and resume commands will suspend or resume all\n" \
    "background threads.\n\n" \
    "The start, stop and wake commands will operate on a special test\n" \
    "thread, which is not part of the normal SDK.\n" \
    "An optional timeout value may be specified for the start command\n" \
    "to make the thread wake up at regular intervals.\n" \
    "If the timeout value is zero, the thread will exit immediately.\n" \
    "If no timeout is specified or the timeout value is negative,\n" \
    "then the thread will wait indefinitely for 'wake' or 'stop'.\n" \
    "A timeout value may also be specified for the stop command to\n" \
    "determine the time to wait for the thread to terminate."

/*! Examples for CLI command. */
#define BCMA_SALCMD_THREAD_EXAMPLES \
    "status\n" \
    "start\n" \
    "wake\n" \
    "stop\n" \
    "start 5"

/*!
 * \brief CLI 'thread' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_salcmd_thread(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_SALCMD_THREAD_H */
