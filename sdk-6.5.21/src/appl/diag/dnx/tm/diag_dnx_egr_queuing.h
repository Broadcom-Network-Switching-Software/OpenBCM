/** \file diag_dnx_egr_queuing.h
 * 
 * DNX TM ingress reassembly diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_EGR_QUEUING_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_EGR_QUEUING_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Macro indicating the value, on the various menus, which indicates
 * that option is to NOT be included.
 */
#define EGQ_NOT_INCLUDED_VAL            -1
/**
 * data structure for egq coutner rate information
 */
typedef struct
{
    char name[DNX_EGQ_COUNTER_NAME_LEN];
    dnx_egq_counter_type_t type;
    int count_ovf;
    uint64 rate;
} dnx_egq_counter_rate_info_t;

/**
 * specifies the egress epni rate scheme
 */
typedef enum
{
    DNX_EGR_EPNI_RATE_SCHEME_TOTAL,
    DNX_EGR_EPNI_RATE_SCHEME_IF,
    DNX_EGR_EPNI_RATE_SCHEME_PORT,
    DNX_EGR_EPNI_RATE_SCHEME_QP,
    DNX_EGR_EPNI_RATE_SCHEME_MIRROR
} dnx_egr_epni_rate_scheme_e;
/*
 * Prototypes and 'extern's for options and man
 * {
 */
extern sh_sand_option_t sh_dnx_egq_shaping_options[];
extern sh_sand_man_t sh_dnx_egq_shaping_man;
/**
 * \brief
 *   Dump egq shaping information per port. If only one
 *   port is specified then display, graphically, corresponding
 *   port scheduler.
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   * shr_error_e
 *   * sh_dnx_egq_shaping_man
 *   * sh_dnx_egq_shaping_options
 */
shr_error_e sh_dnx_egq_shaping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egq_compensation_options[];
extern sh_sand_man_t sh_dnx_egq_compensation_man;

/**
 * \brief
 *   If rquired, set a new 'compensation' value for specified logical ports.
 *   Otherwise, just get all 'compensation' values for specified ports.
 *   In any case, dump egq compensation information per port
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   * shr_error_e
 *   * sh_dnx_egq_compensation_man
 *   * sh_dnx_egq_compensation_options
 */
shr_error_e sh_dnx_egq_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);


extern sh_sand_option_t sh_dnx_egr_rqp_rate_options[];
extern sh_sand_man_t sh_dnx_egr_rqp_rate_man;

/**
 * \brief - display egress rqp coutner rate
 */
shr_error_e sh_dnx_egr_rqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_pqp_rate_options[];
extern sh_sand_man_t sh_dnx_egr_pqp_rate_man;

/**
 * \brief - display egress pqp coutner rate
 */
shr_error_e sh_dnx_egr_pqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_epep_rate_options[];
extern sh_sand_man_t sh_dnx_egr_epep_rate_man;

/**
 * \brief - display egress epep coutner rate
 */
shr_error_e sh_dnx_egr_epep_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_epni_rate_options[];
extern sh_sand_man_t sh_dnx_egr_epni_rate_man;

/**
 * \brief - calculate and display egress epni according to the scheme
 */
shr_error_e sh_dnx_egr_epni_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_queuing_rate_options[];
extern sh_sand_man_t sh_dnx_egr_queuing_rate_man;

/**
 * \brief
 *   calculate and display egress queuing couters related to 'rate'
 *
 * \param [in] unit -
 *   HW identifier of unit.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   * shr_error_e
 *   * sh_dnx_egr_queuing_rate_man
 *   * sh_dnx_egr_queuing_rate_options
 */
shr_error_e sh_dnx_egr_queuing_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_interface_rate_options[];
extern sh_sand_man_t sh_dnx_egr_interface_rate_man;

/**
 * \brief - display egress interface coutner rate per EGQ IF. the rate is calculated per user-defined interval
 */
shr_error_e sh_dnx_egr_interface_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */
#endif /** DIAG_DNX_EGR_QUEUING_H_INCLUDED */
