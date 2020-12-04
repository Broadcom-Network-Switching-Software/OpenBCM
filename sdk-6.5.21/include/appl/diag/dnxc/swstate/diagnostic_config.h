/** \file diagnostic_config.h
 *
 * sw state commandline configurations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DIAGNOSTIC_CONFIG_H
#define _DIAGNOSTIC_CONFIG_H
#include <soc/dnxc/swstate/sw_state_defs.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
extern sh_sand_cmd_t dnxc_swstate_config_cmds[];
extern sh_sand_man_t dnxc_swstate_config_man;
extern const char cmd_dnxc_swstate_config_desc[];
extern shr_error_e sh_dnxc_swstate_config_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_option_t dnxc_swstate_config_dump_options[];
extern sh_sand_man_t dnxc_swstate_config_dump_man;
#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* _DIAGNOSTIC_CONFIG_H */
