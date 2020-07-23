/**
 * \file diag_dnx_pp_load_balancing.h
 *
 * Diagnostics definitions, for DNX, for 'switch/loiad_balancing' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_PP_LOAD_BALANCING_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PP_LOAD_BALANCING_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * Options
 * {
 */
/**
 * \brief
 *   Keyword for the load balancing info to be displayed.
 *   Currently, only one level is allowed: "all"
 */
#define DNX_DIAG_PP_LB_OPTION_DISPLAY_LEVEL           "level"
/*
 * }
 */

extern shr_error_e sh_dnx_pp_lb_vis_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *   List of shell options for 'load_balancing' shell commands (display, ...)
 * \remark
 *   * For now, just passive display.
 */
extern sh_sand_cmd_t sh_dnx_diag_pp_lb_info_cmds[];

extern sh_sand_man_t sh_dnx_diag_pp_lb_vis_man;
extern sh_sand_man_t sh_dnx_diag_pp_lb_info_man;

/*
 * }
 */
/*
 * }
 */
#endif /* DIAG_DNX_ACTION_H_INCLUDED */
