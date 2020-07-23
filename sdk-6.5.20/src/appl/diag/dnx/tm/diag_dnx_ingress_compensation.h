/** \file diag_dnx_ingress_compensation.h
 * 
 * DNX TM ingress compensation diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

extern sh_sand_option_t sh_dnx_ingress_compensation_options[];
extern sh_sand_man_t sh_dnx_ingress_compensation_man;

/**
 * \brief - print ingress compensation table with basic
 *        information according to what user provided
 *
 *Compensation Template is general:
 *Header-Truncate
 *IRPP-delta
 *VOQ credit class
 *In-PP-Port
 *Header Append
 *
 *Sch : all, In-PP-Port is disabled on init and only In-PP-Port user can select to enable/disable
 *CRPS-IRPP: IRPP-delta, In-PP-Port are enabled, Header-Truncate is disabled on init - user can select to enable/disable
 *CRPS-ITM:  IRPP-delta, In-PP-Port are enabled, Header-Truncate is enabled on init - user can select to disable/enable
 *STIF:      IRPP-delta, In-PP-Port are enabled, Header-Truncate is disabled - user can select to enable/disable
 *
 * \param [in] unit - unit id
 * \param [in] args - args
 * \param [in] sand_control  -  sand_control
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnx_ingress_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED */
