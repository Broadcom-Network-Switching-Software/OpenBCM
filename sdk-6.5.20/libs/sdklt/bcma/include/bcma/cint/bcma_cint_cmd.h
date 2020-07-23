/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bcma_cint_cmd.h
 * Purpose:     Interfaces to diag shell CINT functions
 *
 * Note - this file needs to be includable by cint_yy.h, so cannot include any
 * SDK interfaces directly.
 */

#ifndef   BCMA_CINT_CMD_H
#define   BCMA_CINT_CMD_H

#include <bcma/cli/bcma_cli.h>
extern int cmd_cint_initialize(void);
extern void cmd_cint_fatal_error(char *msg);
extern int bcma_cintcmd_add_cint_cmd(bcma_cli_t *cli);

#endif /* BCMA_CINT_CMD_H */
