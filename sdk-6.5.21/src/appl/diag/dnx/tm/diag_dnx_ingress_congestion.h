/** \file diag_dnx_ingress_congestion.h
 * 
 * DNX TM ingress congestion diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_INGRESS_CONGESTION_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_INGRESS_CONGESTION_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

extern sh_sand_option_t sh_dnx_ingress_congestion_options[];
extern sh_sand_man_t sh_dnx_ingress_congestion_man;

/**
 * \brief - dump ingress congestion statistics
 */
shr_error_e sh_dnx_ingress_congestion_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_ingress_congestion_rate_class_options[];
extern sh_sand_man_t sh_dnx_ingress_congestion_rate_class_man;

/**
 * \brief - dump ingress congestion statistics
 */
shr_error_e sh_dnx_ingress_congestion_rate_class_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e sh_dnx_tm_voq_rate_class_info_cmd(
    int unit,
    int rate_class,
    sh_sand_control_t * sand_control);

const char *sh_dnx_tm_ingress_congestion_resource_str(
    dnx_ingress_congestion_resource_type_e resource);

extern sh_sand_cmd_t sh_dnx_tm_vsq_cmds[];
extern sh_sand_man_t sh_dnx_tm_vsq_man;

/**
 * \brief - queue count command info
 */
shr_error_e sh_dnx_tm_queue_count_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_tm_queue_count_options[];
extern sh_sand_man_t sh_dnx_tm_queue_count_man;

/**
 * \brief - queue vsq non empty
 */
shr_error_e sh_dnx_tm_queue_non_empty_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_tm_queue_non_empty_options[];
extern sh_sand_man_t sh_dnx_tm_queue_non_empty_man;

#endif /** DIAG_DNX_INGRESS_CONGESTION_H_INCLUDED */
