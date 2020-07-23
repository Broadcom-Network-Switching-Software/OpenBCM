/**
 * \file ctest_dnxc_infra_init.h
 *
 * Infrastructure init utilities, structures and definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNXC_CTEST_INFRA_INIT_H_INCLUDED
#define DNXC_CTEST_INFRA_INIT_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnxc_infra_init_parallel_test_man;

extern sh_sand_invoke_t sh_dnxc_infra_init_parallel_tests[];

extern sh_sand_option_t sh_dnxc_infra_init_parallel_test_options[];

/**
 * \brief - execute parallel (multi threaded) deinit+init for all active units
 *
 * \param [in] unit - Unit ID
 * \param [in] args -  command args
 * \param [in] sand_control - shell sand control
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnxc_infra_init_parallel_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - diag_dnxc_infra_init_test_options is common for Time test and Error recovery test
 */
extern sh_sand_option_t diag_dnxc_infra_init_test_options[];

extern sh_sand_man_t diag_dnxc_infra_init_error_recovery_test_man;

/**
 * \brief - run error recovery test per each init step
 *
 * \param [in] unit - Unit ID
 * \param [in] args -  command args
 * \param [in] sand_control - shell sand control
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_infra_init_error_recovery_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - create pointer to test list for each init step
 *
 * \param [in] unit - Unit ID
 * \param [out] test_list - pointer to test list for each init step
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_infra_init_error_recovery_tests_creation(
    int unit,
    rhlist_t * test_list);

extern sh_sand_man_t diag_dnxc_infra_init_time_test_man;

/**
 * \brief - Run time test performed by verifying that each step is taking less
 * time than a pre-defined threshold
 *
 * \param [in] unit - Unit ID
 * \param [in] args -  command args
 * \param [in] sand_control - shell sand control
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_infra_init_time_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - create pointer to test list for each init step
 *
 * \param [in] unit - Unit ID
 * \param [out] test_list - pointer to test list for each init step
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_infra_init_time_tests_creation(
    int unit,
    rhlist_t * test_list);

extern sh_sand_cmd_t dnxc_infra_init_test_cmds[];

extern sh_sand_cmd_t dnxc_infra_appl_test_cmds[];

#endif /* DNXC_CTEST_INFRA_INIT_H_INCLUDED */
