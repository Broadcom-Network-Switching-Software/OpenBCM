/**
 * \file diag_sand_framework.h
 *
 * Framework utilities, structures and definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNXC_CTEST_H_INCLUDED
#define DNXC_CTEST_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>

/**
 * \brief Routine serves to add test dynamically
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] test_list list where dynamic tests are supposed to be added
 *     \param [in] test_name test name
 *     \param [in] test_params list of command line options for the test
 *     \param [in] flags marks tests export variety for different conditions
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e sh_sand_test_add(
    int unit,
    rhlist_t * test_list,
    char *test_name,
    char *test_params,
    int flags);

extern sh_sand_cmd_t sh_dnxc_sys_ctest_cmds[];

#endif /* DNXC_CTEST_H_INCLUDED */
