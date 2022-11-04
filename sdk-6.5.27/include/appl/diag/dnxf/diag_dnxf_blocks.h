/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNXF_BLOCKS_H_INCLUDED
#define DIAG_DNXF_BLOCKS_H_INCLUDED

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnxf_blocks_man;
extern sh_sand_cmd_t sh_dnxf_blocks_cmds[];

extern sh_sand_man_t sh_dnxf_device_reset_man;
extern sh_sand_option_t sh_dnxf_device_reset_options[];
shr_error_e sh_dnxf_device_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
#endif /* DIAG_DNXF_BLOCKS_H_INCLUDED */
