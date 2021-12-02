/** \file diag_dnx_egr_mirror_rcy.h
 *
 * diagnostics for egress mirror recycle
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_PORT_MGMT_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PORT_MGMT_H_INCLUDED

 /*
  * LOCAL DIAG PACK:
  * {
  */
extern sh_sand_option_t sh_dnx_egr_mirror_rcy_options[];
extern sh_sand_man_t sh_dnx_egr_mirror_rcy_man;

/*
 * }
 */
#endif /** DIAG_DNX_PORT_MGMT_H_INCLUDED*/

/**
 * \brief -prints a mapping between forward ports to mirror recycle ports
 *
 * \param [in] unit - Unit #
 * \param [in] logical_ports - bitmap of required logical ports
 * \param [in] sand_control - pointer to framework control structure
 */
shr_error_e sh_dnx_tm_egr_mirror_rcy(
    int unit,
    bcm_pbmp_t logical_ports,
    sh_sand_control_t * sand_control);

/**
 * \brief - Parse args and call to sh_dnx_tm_egr_mirror_rcy() with the requested data params
 *
 * \param [in] unit - Unit #
 * \param [in] args - Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control - pointer to framework control structure
 */
shr_error_e sh_dnx_tm_egr_mirror_rcy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
