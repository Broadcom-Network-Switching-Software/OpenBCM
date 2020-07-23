/** \file diag_dnx_field_fem.h
 *
 * Diagnostics procedures, for DNX, for 'fem' operations (allocation/add/remove, context attach/detach)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_FEM_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FEM_H_INCLUDED
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
 *   List of shell options for 'fem' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of allocated fems and general fems space
 */
extern sh_sand_cmd_t sh_dnx_field_fem_cmds[];

#endif /* DIAG_DNX_FEM_H_INCLUDED */
