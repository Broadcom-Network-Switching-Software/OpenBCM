/*! \file diag_dnx_time_analyzer.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_TIME_ANALYZER_H_INCLUDED
#define DIAG_DNX_TIME_ANALYZER_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_time_analyzer_man;
extern sh_sand_cmd_t sh_dnx_time_analyzer_cmds[];
extern sh_sand_man_t dnx_time_analyzer_start_man;
extern sh_sand_man_t dnx_time_analyzer_stop_man;
extern sh_sand_man_t dnx_time_analyzer_remove_man;
extern sh_sand_man_t dnx_time_analyzer_add_man;
extern sh_sand_man_t dnx_time_analyzer_module_list_man;
extern sh_sand_option_t sh_dnx_time_analyzer_start_stop_remove_options[];
extern sh_sand_option_t sh_dnx_time_analyzer_add_options[];

/*************
 * FUNCTIONS *
 */

#endif /* DIAG_DNX_TIME_ANALYZER_H_INCLUDED */
