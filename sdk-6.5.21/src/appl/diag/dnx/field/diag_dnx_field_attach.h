/** \file diag_dnx_field_attach.h
 *
 * Diagnostics procedures, for DNX, for 'attach' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_ATTACH_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_ATTACH_H_INCLUDED
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
 *   List of shell options for 'attach' shell commands (info)
 * \remark
 *   * Displays context attach information about a given FG.
 */
extern sh_sand_cmd_t Sh_dnx_field_attach_cmds[];

#endif /* DIAG_DNX_ATTACH_H_INCLUDED */
