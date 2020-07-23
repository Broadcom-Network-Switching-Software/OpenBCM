/** \file diag_dnx_ingress_rate.h
 * 
 * DNX TM ingress rate diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_INGRESS_RATE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_INGRESS_RATE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

extern sh_sand_enum_t tm_queue_id_table[];

/**
 * \brief - calculate and display ingress cgm counter rate
 */
shr_error_e sh_dnx_ingress_cgm_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_ingress_cgm_rate_options[];
extern sh_sand_man_t sh_dnx_ingress_cgm_rate_man;

/**
 * \brief - calculate and display ingress ipt counter rate
 */
shr_error_e sh_dnx_ingress_ipt_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnx_ingress_ipt_rate_man;

/**
 * \brief - calculate and display ingress sqm counter rate
 */
shr_error_e sh_dnx_ingress_sqm_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnx_ingress_sqm_rate_man;

/**
 * \brief - calculate and display ingress ire counter rate
 */
shr_error_e sh_dnx_ingress_ire_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnx_ingress_ire_rate_man;

#endif /** DIAG_DNX_INGRESS_RATE_H_INCLUDED */
