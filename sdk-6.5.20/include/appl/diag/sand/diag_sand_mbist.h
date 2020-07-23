/**
 * \file diag_sand_mbist.h
 *
 * Misc. utilities for shell commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_MBIST_H_INCLUDED
#define DIAG_SAND_MBIST_H_INCLUDED

#include <soc/sand/sand_mbist.h>

extern const char cmd_sand_mbist_usage[];
extern const char cmd_sand_mbist_desc[];

extern sh_sand_man_t sh_sand_mbist_man;
extern sh_sand_cmd_t sh_sand_mbist_cmds[];

cmd_result_t cmd_sand_mbist(
    int unit,
    args_t * a);

typedef struct diag_sand_mbist_info_s
{
    sand_mbist_device_t mbist_device;
    sand_mbist_script_t *script;
    uint8 *file_buf;
    uint8 skip_errors;
} diag_sand_mbist_info_t;
int diag_sand_mbist_file_load(
    int unit,
    char *fname);
int diag_sand_mbist_run(
    const int unit);
int diag_sand_mbist_write_command(
    int unit,
    uint32 write_command);
int diag_sand_mbist_stop_on_error(
    int unit,
    int stop_on_error);

#endif /* */
