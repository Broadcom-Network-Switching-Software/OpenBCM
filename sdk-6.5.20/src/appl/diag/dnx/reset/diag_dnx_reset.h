/*
 * ! \file diag_dnx_reset.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_RESET_H_INCLUDED
#define DIAG_DNX_RESET_H_INCLUDED

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
extern sh_sand_man_t sh_dnx_reset_man;
extern sh_sand_option_t sh_dnx_reset_options[];

/*************
 * FUNCTIONS *
 */

/**
 * \brief - Device reset command implementation
 * \param [in] unit - Unit #
 * \param [in] args - Command line arguments, may be NULL if called from outside the shell
 * \param [in] sand_control - Shell command control structure
 * \return
 *      Standard error handling.
 */
shr_error_e sh_dnx_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_RESET_H_INCLUDED */
