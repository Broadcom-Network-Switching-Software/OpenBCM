/**
 * \file diag_dnx_field_action.h
 *
 * Diagnostics definitions, for DNX, for 'action' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef DIAG_DNX_FIELD_ACTION_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_ACTION_H_INCLUDED
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
 *   List of shell options for 'action' shell commands (display, ...)
 * \remark
 *   * For now, just passive display of allocated field actions and relevant info.
 */
extern sh_sand_cmd_t Sh_dnx_field_action_cmds[];

/*
 * }
 */
#endif /* DIAG_DNX_ACTION_H_INCLUDED */
