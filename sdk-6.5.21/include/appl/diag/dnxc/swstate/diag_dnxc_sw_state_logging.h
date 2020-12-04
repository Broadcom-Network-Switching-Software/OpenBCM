/** \file diag_dnxc_sw_state_logging.h
 *
 * sw state logging.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DIAG_DNXC_SW_STATE_LOGGING_H
#define _DIAG_DNXC_SW_STATE_LOGGING_H
#include <soc/dnxc/swstate/sw_state_defs.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_cmd_t dnxc_swstate_logging_cmds[];

extern sh_sand_man_t dnxc_swstate_logging_man;

extern const char cmd_dnxc_swstate_logging_desc[];

extern shr_error_e sh_dnxc_swstate_logging_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e sh_dnxc_swstate_logging_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e sh_dnxc_swstate_logging_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t dnxc_swstate_log_options[];

extern sh_sand_option_t dnxc_swstate_log_add_remove_options[];

extern sh_sand_man_t dnxc_swstate_log_start_man;

extern sh_sand_man_t dnxc_swstate_log_stop_man;

extern sh_sand_man_t dnxc_swstate_log_add_man;

extern sh_sand_man_t dnxc_swstate_log_remove_man;

extern sh_sand_man_t dnxc_swstate_log_print_man;

#endif /* DNX_SW_STATE_DIAGNOSTIC */

#endif /* _DIAG_DNXC_SW_STATE_LOGGING_H */
