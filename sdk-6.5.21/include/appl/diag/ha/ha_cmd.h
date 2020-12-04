/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ha_cmd.h
 * Purpose: For HA commands.
 */

#ifndef _DIAG_HA_HA_CMD_H
#define _DIAG_HA_HA_CMD_H

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>


extern cmd_result_t cmd_ha(int, args_t *);
extern char cmd_ha_usage[];

#endif /* _DIAG_HA_HA_CMD_H */
