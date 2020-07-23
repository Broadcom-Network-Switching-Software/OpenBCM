/*
 * ! \file diag_sand_dsig.h
 *
 * Purpose: Interface to debug_signal database
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DIAG_SAND_DSIG_H
#define __DIAG_SAND_DSIG_H

#include <appl/diag/shell.h>

#define SIGNALS_PRINT_DETAIL    0x01
#define SIGNALS_PRINT_VALUE     0x02
#define SIGNALS_PRINT_DEVICE    0x08
#define SIGNALS_PRINT_CORE      0x10
#define SIGNALS_PRINT_HW        0x20
#define SIGNALS_PRINT_ASIC      0x40
#define SIGNALS_PRINT_STATUS    0x80

typedef enum
{
    SIG_COMMAND_DEBUG,
    SIG_COMMAND_INTERNAL,
    SIG_COMMAND_STAGE,
    SIG_COMMAND_PARAM,
    SIG_COMMAND_STRUCT,
    SIG_COMMAND_DEBUG_SINGLE
} SIG_COMMAND;

cmd_result_t cmd_sand_dsig_show(
    int unit,
    args_t * a);

#endif /* __DIAG_SAND_DSIG_H */
