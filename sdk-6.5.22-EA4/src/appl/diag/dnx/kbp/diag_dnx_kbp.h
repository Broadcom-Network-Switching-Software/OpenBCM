/** \file diag_dnx_kbp.h
 *
 * Diagnostics procedures, for DNX KBP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_KBP_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_KBP_H_INCLUDED
/*
 * Include files.
 * {
 */
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
 *   List of shell options for 'kbp' shell diagnostic commands
 */
extern sh_sand_cmd_t sh_dnx_kbp_cmds[];
extern sh_sand_man_t sh_dnx_kbp_man;

/** CMD API exposed for usage in kbp_sdk_ver.c */
shr_error_e dnx_kbp_diag_sdk_ver_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_KBP_H_INCLUDED */
