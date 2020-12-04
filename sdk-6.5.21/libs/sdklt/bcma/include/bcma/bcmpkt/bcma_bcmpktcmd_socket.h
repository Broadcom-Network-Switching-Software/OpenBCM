/*! \file bcma_bcmpktcmd_socket.h
 *
 * CLI command related to SOCKET control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPKTCMD_SOCKET_H
#define BCMA_BCMPKTCMD_SOCKET_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPKTCMD_SOCKET_DESC \
    "Manage SOCKET."

/*! Syntax for CLI command. */
#define BCMA_BCMPKTCMD_SOCKET_SYNOP \
    "[<resource>] <action> [options]"

/*! Help for CLI command. */
#define BCMA_BCMPKTCMD_SOCKET_HELP \
    "This command is used to create and manage SOCKET. A SOCKET performs packet\n" \
    "transmiting and receiving for a network interface between Linux\n" \
    "kernel and SDK API.\n" \
    "    ATtach [options] - Attach a SOCKET driver.\n" \
    "        Type=<value>  - SOCKET driver type.\n" \
    "    DeTach - Detach SOCKET driver.\n" \
    "    Create  [<id>] - Create SOCKET onto a network interface.\n" \
    "    Destroy [<id>] - Destroy SOCKET from a network interface.\n" \
    "    Info    [<id>] - Show whether SOCKET exists on the netif_id.\n" \
    "    Stats - SOCKET Statistics operations.\n" \
    "        Show  [<id>]  - Show a SOCKET's statistics.\n" \
    "        Clear [<id>]  - Clear a SOCKET's statistics."

/*! Examples for CLI command. */
#define BCMA_BCMPKTCMD_SOCKET_EXAMPLES \
    "create 1\n" \
    "c 1"

/*!
 * \brief SOCKET command in CLI.
 *
 * \param[in] cli CLI object
 * \param[in] args CLI arguments list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpktcmd_socket(bcma_cli_t *cli, bcma_cli_args_t *args);

#endif /* BCMA_BCMPKTCMD_SOCKET_H */
