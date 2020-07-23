
/*! \file diag_dnx_sec.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_SEC_H_INCLUDED
#define DIAG_DNX_SEC_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_sec_man;
extern sh_sand_cmd_t sh_dnx_sec_cmds[];

/*************
 * FUNCTIONS *
 */
/**
 * \brief - CB for shell commands infra to check if flexe is available
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
shr_error_e sh_cmd_is_sec_available(
    int unit,
    rhlist_t * test_list);

#endif /* DIAG_DNX_SEC_H_INCLUDED */
