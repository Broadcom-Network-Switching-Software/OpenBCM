/*! \file diag_dnxc.h
 * Purpose: Extern declarations for diag commands from dnxc folder
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNXC_H_INCLUDED
#define DIAG_DNXC_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <appl/diag/sand/diag_sand_framework.h>
/*************
 *  DEFINES  *
 */

#define DIAG_DNXC_LANE_TO_SERDES_NIF_SIDE     0
#define DIAG_DNXC_LANE_TO_SERDES_FABRIC_SIDE  BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnxc_fabric_man;
extern sh_sand_cmd_t sh_dnxc_fabric_cmds[];

extern sh_sand_man_t sh_dnxc_clear_man;
extern sh_sand_cmd_t sh_dnxc_clear_cmds[];

extern sh_sand_man_t sh_dnxc_show_man;
extern sh_sand_cmd_t sh_dnxc_show_cmds[];

extern sh_sand_man_t sh_dnxc_data_man;
extern sh_sand_cmd_t sh_dnxc_data_cmds[];

extern sh_sand_man_t sh_dnxc_appl_man;
extern sh_sand_cmd_t sh_dnxc_appl_cmds[];

extern sh_sand_man_t sh_dnxc_diag_man;
extern sh_sand_cmd_t sh_dnxc_diag_cmds[];

extern sh_sand_man_t sh_dnxc_avs_man;
extern sh_sand_cmd_t sh_dnxc_avs_cmds[];

extern sh_sand_man_t sh_dnxc_intr_man;
extern sh_sand_cmd_t sh_dnxc_intr_cmds[];

extern sh_sand_man_t sh_dnxc_init_dnx_man;
extern sh_sand_option_t sh_dnxc_init_dnx_options[];

#ifdef BCM_DNXF_SUPPORT
extern sh_sand_man_t sh_dnxf_linkscan_man;
#endif
#ifdef BCM_DNX_SUPPORT
extern sh_sand_man_t sh_dnx_linkscan_man;
#endif
extern sh_sand_option_t dnxc_linkscan_options[];

extern sh_sand_man_t sh_dnxc_port_man;
extern sh_sand_cmd_t sh_dnxc_port_cmds[];

extern sh_sand_man_t sh_dnxc_phy_man;
extern sh_sand_cmd_t sh_dnxc_phy_cmds[];

extern sh_sand_man_t sh_dnxc_mta_man;
extern sh_sand_cmd_t sh_dnxc_mta_cmds[];

extern sh_sand_man_t sh_dnxc_warmboot_man;
extern sh_sand_cmd_t sh_dnxc_warmboot_cmds[];

extern sh_sand_man_t sh_dnxc_port_status_man;
extern sh_sand_option_t sh_dnxc_port_status_options[];

extern sh_sand_man_t sh_dnxc_port_loopback_man;
extern sh_sand_option_t sh_dnxc_port_loopback_options[];

extern sh_sand_man_t sh_dnxc_port_enable_man;
extern sh_sand_option_t sh_dnxc_port_enable_options[];

extern sh_sand_man_t sh_dnxc_port_pm_man;
extern sh_sand_cmd_t sh_dnxc_port_pm_cmds[];

extern sh_sand_man_t sh_dnxc_soc_man;

extern sh_sand_man_t sh_dnxc_pbmp_man;
extern sh_sand_option_t dnxc_pbmp_options[];

#ifdef BCM_DNXF_SUPPORT
extern sh_sand_man_t sh_dnxf_counter_man;
#endif
#ifdef BCM_DNX_SUPPORT
extern sh_sand_man_t sh_dnx_counter_man;
#endif
extern sh_sand_option_t dnxc_counter_options[];

extern sh_sand_option_t dnxc_mta_options[];
extern sh_sand_option_t dnxc_warmboot_options[];

/*************
 * FUNCTIONS *
 */
extern shr_error_e cmd_dnxc_linkscan(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e cmd_dnxc_soc(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e cmd_dnxc_pbmp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e cmd_dnxc_lane_map_dump(
    int unit,
    int flags,
    int print_lane,
    sh_sand_control_t * sand_control);

/**
 * \brief - Dump the port status
 */
extern shr_error_e cmd_dnxc_port_status(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e cmd_dnxc_counter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e sh_dnxc_init_dnx_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern shr_error_e cmd_dnxc_gport(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Multi thread analyzer ops
 */
extern shr_error_e cmd_dnxc_mta(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Warmboot ops
 */
extern shr_error_e cmd_dnxc_warmboot(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Set port loopback
 */
extern shr_error_e cmd_dnxc_port_loopback(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - CMD to enable/disable port
 */
shr_error_e cmd_dnxc_port_enable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - Deinit diag counter values database
 */
shr_error_e diag_dnxc_diag_counter_values_database_deinit(
    int unit);

#endif /* DIAG_DNX_DIAG_H_INCLUDED */
