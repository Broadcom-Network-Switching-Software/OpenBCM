/**
 * \file diag_dnx_framer.h Contains all of the framer diag&test declarations for external usage
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_FRAMER_H_INCLUDED
#define DIAG_DNX_FRAMER_H_INCLUDED

/****************************
 * FlexE core include file *
 ****************************/
#include <appl/diag/sand/diag_sand_framework.h>

/*************
 * GLOBALS   *
 *************/
extern sh_sand_man_t sh_dnx_diag_framer_man;
extern sh_sand_cmd_t sh_dnx_diag_framer_cmds[];

/*************
 * DEFINES   *
 *************/

/*************
 * TYPE DEFS *
 *************/

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief - CB for shell commands infra to check if framer is available
 *
 * \param [in] unit - unit number
 * \param [in] test_list - test_list
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_cmd_is_framer_available(
    int unit,
    rhlist_t * test_list);

#endif /* DIAG_DNX_FRAMER_H_INCLUDED */
