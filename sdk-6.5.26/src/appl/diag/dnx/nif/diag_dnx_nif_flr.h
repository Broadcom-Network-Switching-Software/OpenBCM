/*! \file diag_dnx_nif_flr.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_NIF_FLR_H_INCLUDED
#define DIAG_DNX_NIF_FLR_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

extern sh_sand_man_t sh_dnx_nif_flr_man;
extern sh_sand_option_t sh_dnx_nif_flr_options[];

/*************
 * FUNCTIONS *
 *************/

/**
 * \brief - Dump the NIF mapped lane info
 */
shr_error_e sh_dnx_nif_flr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_NIF_FLR_H_INCLUDED */
