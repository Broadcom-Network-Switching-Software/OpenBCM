/** \file diag_dnx_ecgm.h
 * 
 * DNX TM egress congestion diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_ECGM_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ECGM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * manuals extern
 * {
 */
extern sh_sand_man_t sh_dnx_tm_egress_congestion_man;
/**
 * }
 */

/**
 * options extern
 * {
 */
extern sh_sand_option_t sh_dnx_tm_egress_congestion_options[];
/**
 * }
 */

/**
 * command declaration
 * {
 */
/**
* \brief -  The TM EGress CoNGeSTion shell command. Can be run in the BCM_USER with "tm eg cngst [ALL(default)/CURrent/MAX]".
* 			More in depth explanation is described in the manual "sh_dnx_tm_egress_congestion_man".
*
* \param [in] unit - Chip unit id.
* \param [in] args - Pointer standard bcm shell argument structure, used by parse.h MACROS and routines.
* \param [in] sand_control - Control structure for DNX shell framework.
*
* \return
*   shr_error_e
*
* \remarks
*	The logic of printing current tables and max tables is the same, therefore by using function pointers - a single static function can be utilized to print both tables.
*	Each prt_per_X_resources_table() is called once in sh_dnx_tm_egress_congestion_current_option() with dnx_ecgm_dbal_X_current_get() function pointer and
*  	once in sh_dnx_tm_egress_congestion_max_option() with dnx_ecgm_dbal_X_max_get() function pointer.
*/
shr_error_e sh_dnx_tm_egress_congestion_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
/**
 * }
 */

#endif /** DIAG_DNX_ECGM_H_INCLUDED */
