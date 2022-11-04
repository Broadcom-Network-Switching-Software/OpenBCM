/** \file diag_dnx_field_ace.h
 *
 * Diagnostics procedures, for DNX, for 'ACE' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#ifndef DIAG_DNX_ACE_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ACE_H_INCLUDED
/*
 * Include files.
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/**
 * \brief
 *   List of shell options for 'ace' shell commands (list, info)
 */
extern sh_sand_cmd_t Sh_dnx_field_ace_cmds[];

/**
 * \brief
 *   Conditional check for ACE diag support
 * \param [in] unit - unit ID 
 * \param [in] test_list - test list 
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error detected. See \ref shr_error_e
 */
shr_error_e diag_dnx_field_ace_is_available(
    int unit,
    rhlist_t * test_list);

#endif /* DIAG_DNX_ACE_H_INCLUDED */
