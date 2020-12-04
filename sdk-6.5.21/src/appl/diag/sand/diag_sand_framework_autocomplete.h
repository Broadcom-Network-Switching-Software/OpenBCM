/**
 * \file diag_sand_framework_autocomplete.h
 *
 * Framework utilities, structures and definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_SAND_FRAMEWORK_AUTOCOMPLETE_H_INCLUDED
#define DIAG_SAND_FRAMEWORK_AUTOCOMPLETE_H_INCLUDED

#include "diag_sand_framework_internal.h"

#ifdef INCLUDE_AUTOCOMPLETE
#include <sal/appl/editline/autocomplete.h>

/**
* \brief Routine serves to add autocomplete support to access read cmd
* \par DIRECT INPUT:
*     \param [in] unit - unit id
*/
shr_error_e sh_sand_access_read_autocomplete_init(
    int unit);

#if defined(DNX_SW_STATE_LOGGING)
/**
* \brief Routine serves to add autocomplete support to swstate commands.
* \par DIRECT INPUT:
*     \param [in] unit - unit id
*/
void sh_sand_sw_state_cmd_autocomplete_init(
    int unit);
#endif /* DNX_SW_STATE_LOGGING */

#endif /* INCLUDE_AUTOCOMPLETE */

/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
void sh_sand_autocomplete_dummy_empty_function_to_make_compiler_happy(
    int unit);

#endif /* DIAG_SAND_FRAMEWORK_AUTOCOMPLETE_H_INCLUDED */
