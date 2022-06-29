/** \file diag_dnx_field_presel.h
 *
 * Diagnostics procedures, for DNX, for 'presel' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_PRESEL_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_PRESEL_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include "diag_dnx_field_utils.h"
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
 *   This function prepares a string of the presel data.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage of the presel
 * \param [in] presel_id - Id of the presel to print.
 * \param [in] delineator_string - String to place between qualifiers.
 * \param [out] presel_quals_string_out - String of the presel info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e diag_dnx_field_presel_string(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_field_presel_t presel_id,
    const char *delineator_string,
    char presel_quals_string_out[DIAG_DNX_FIELD_UTILS_INPUT_STR_SIZE]);

/**
 * \brief
 *   List of shell options for 'presel' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of preselectors.
 */
extern sh_sand_cmd_t Sh_dnx_field_presel_cmds[];

#endif /* DIAG_DNX_PRESEL_H_INCLUDED */
