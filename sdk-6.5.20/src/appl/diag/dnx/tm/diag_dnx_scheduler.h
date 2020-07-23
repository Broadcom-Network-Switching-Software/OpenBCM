/** \file diag_dnx_scheduler.h
 *
 * DNX TM scheduler diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_SCHEDULER_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_SCHEDULER_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#define DIAG_DNX_SCH_GPORT_MODE_STR_LENGTH                  15
#define DIAG_DNX_SCH_GROUP_STR_LENGTH                       4
#define DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW   2
#define DIAG_DNX_SCH_PORT_SCHEME_TCG_TO_TC_STR_LENGTH       30

/* ENUMS
 * {
 */

typedef enum
{
    /*
     *  undefined value
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_INVALID = -1,
    /*
     *  Uni Cast VOQ
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_UCAST_GROUP = 0,
    /*
     *  Multi Cast VOQ
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_MCAST_GROUP = 1,
    /*
     *  HR scheduler element
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_HR = 2,
    /*
     *  FQ scheduler element
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_FQ = 3,
    /*
     *  CL scheduler element
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_CL = 4,
    /*
     *  VOQ connector
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_VOQ_CONN = 5,
    /*
     *  Must be the last value
     */
    DIAG_DNX_SCH_GPORT_TYPE_STRING_LAST
} diag_dnx_sch_gport_type_string_e;

/* ENUMS
 * }
 */
/**
 * \brief - e2e scheduler connection info
 */
shr_error_e sh_dnx_scheduler_connection_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_connection_options[];
extern sh_sand_man_t sh_dnx_scheduler_connection_man;

/**
 * \brief - gport command info
 */
shr_error_e sh_dnx_scheduler_gport_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_gport_options[];
extern sh_sand_man_t sh_dnx_scheduler_gport_man;

/**
 * \brief - print_flow_and_up diagnostic
 */
shr_error_e sh_dnx_scheduler_print_flow_and_up_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_print_flow_and_up_options[];
extern sh_sand_man_t sh_dnx_scheduler_print_flow_and_up_man;

/**
 * \brief - scheduler port scheme diag
 */
shr_error_e sh_dnx_scheduler_port_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_port_info_options[];
extern sh_sand_man_t sh_dnx_scheduler_port_info_man;

/**
 * \brief - Calculate and display scheduler port rate.
 */
shr_error_e sh_dnx_scheduler_port_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_port_rate_options[];
extern sh_sand_man_t sh_dnx_scheduler_port_rate_man;

shr_error_e sh_dnx_scheduler_flow_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_scheduler_flow_rate_options[];
extern sh_sand_man_t sh_dnx_scheduler_flow_rate_man;

/**
 * \brief - Config FSM counter
 */
shr_error_e sh_dnx_scheduler_fsm_config_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnx_scheduler_fsm_config_man;
extern sh_sand_option_t sh_dnx_scheduler_fsm_config_options[];

/**
 * \brief - Read FSM counter
 */
shr_error_e
sh_dnx_scheduler_fsm_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);
extern sh_sand_man_t sh_dnx_scheduler_fsm_read_man;

#endif /** DIAG_DNX_SCHEDULER_H_INCLUDED */
