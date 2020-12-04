/*
 * ! \file diag_swstate_tests.h Purpose: shell registers commands for sw state tests 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNXF_FABRIC_H_INCLUDED
#define DIAG_DNXF_FABRIC_H_INCLUDED

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

/*
* { Local Structures
*/

/* 
* } Local Structures
*/

/* 
* { Functions
*/
/**
 * DNXF specific command for printing FE queues status
 * 
 * @author db889754 (2/2/2018)
 * 
 * @param unit 
 * @param args
 * @param sand_control 
 * 
 * @return shr_error_e 
 */
shr_error_e cmd_dnxf_fabric_queues(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_option_t dnxf_fabric_queues_options[];
extern sh_sand_man_t dnxf_fabric_queues_man;
/**
 * DNXF specific command for printing fabric CGM thresholds
 *  
 * 
 * \param unit
 * \param args
 * \param sand_control
 * 
 * \return shr_error_e
 */
shr_error_e cmd_dnxf_fabric_thresholds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_option_t dnxf_fabric_thresholds_options[];
extern sh_sand_man_t dnxf_fabric_thresholds_man;
/**
 * DNXF specific command for printing fabric counters in as a 
 * graphic 
 * 
 * @author db889754 (2/2/2018)
 * 
 * @param unit 
 * @param sand_control 
 * 
 * @return shr_error_e 
 */
shr_error_e diag_dnxf_counters_graphical_print(
    int unit,
    sh_sand_control_t * sand_control);
/**
 * DNXF specific command for printing fabric traffic profile
 * 
 * @author db889754 (2/2/2018)
 * 
 * @param unit 
 * @param args 
 * @param sand_control 
 * 
 * @return shr_error_e 
 */
shr_error_e cmd_dnxf_fabric_traffic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_option_t dnxf_fabric_traffic_options[];
extern sh_sand_man_t dnxf_fabric_traffic_man;

/* 
* } Local Functions
*/
#endif /* DIAG_DNXF_FABRIC_H_INCLUDED */
