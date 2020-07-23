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
    "start|stop|wake [<timeout-secs>]"

/*! Help for CLI command. */
#define BCMA_SALCMD_THREAD_HELP \
    "Test thread API.\n\n" \
    "An optional timeout value may be specified for the start command\n" \
    "to make the thread wake up at regular intervals.\n\n" \
    "If the timeout value is zero, the thread will exit immediately.\n\n" \
    "If the no timeout is speficied or the timeout value is negative,\n" \
    "then the thread will wait indefinitely for 'wake' or 'stop'.\n\n" \
    "A timeout value may also be specified for the stop command to\n" \
    "determine the time to wait for the thread to terminate.\n\n" \
    "The status command may show any of the following letters:\n" \
    "  A - Thread is active.\n" \
    "  S - Thread received a stop request.\n" \
    "  W - Thread is waiting for a wake event.\n" \
    "  D - Thread is done.\n" \
    "  E - Thread error."

/*! Examples for CLI command. */
#define BCMA_SALCMD_THREAD_EXAMPLES \
    "start\n" \
    "wake\n" \
    "stop\n" \
    "start 5\n" \
    "status"

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
