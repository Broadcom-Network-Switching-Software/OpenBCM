/*! \file diag_dnxc_thread.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNXC_THREAD_H_INCLUDED
#define DIAG_DNXC_THREAD_H_INCLUDED

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
extern sh_sand_man_t sh_dnxc_thread_man;
extern sh_sand_man_t sh_dnxc_thread_stop_man;
extern sh_sand_man_t sh_dnxc_thread_wake_man;
extern sh_sand_man_t sh_dnxc_thread_status_man;
extern sh_sand_cmd_t sh_dnxc_thread_cmds[];
extern sh_sand_option_t sh_dnxc_thread_options[];
extern sh_sand_option_t sh_dnxc_thread_wake_options[];

/*************
 * FUNCTIONS *
 */

#endif /**  DIAG_DNXC_THREAD_H_INCLUDED */
