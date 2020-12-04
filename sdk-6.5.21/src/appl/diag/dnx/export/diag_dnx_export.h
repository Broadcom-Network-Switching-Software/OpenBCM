
/*! \file diag_dnx_export.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_EXPORT_H_INCLUDED
#define DIAG_DNX_EXPORT_H_INCLUDED

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
extern sh_sand_man_t dnx_export_script_man;
extern sh_sand_option_t dnx_export_script_options[];

/*************
 * FUNCTIONS *
 */
shr_error_e cmd_dnx_export_script(
    int unit,
    args_t * args,
    sh_sand_control_t *sand_control);


#endif /*  DIAG_DNX_EXPORT_H_INCLUDED */
