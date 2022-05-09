/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    diag_dnx_cmdlist.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */

#ifndef _INCLUDE_DNX_CMDLIST_H
#define _INCLUDE_DNX_CMDLIST_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/sand/shrextend/shrextend_error.h>

extern int bcm_dnx_cmd_cnt;
extern cmd_t bcm_dnx_cmd_list[];
extern sh_sand_enum_t cmd_dnx_sh_valid_ports_table[];

shr_error_e cmd_dnx_sh_valid_ports_init(
    int unit);

shr_error_e cmd_dnx_sh_init(
    int unit);

shr_error_e cmd_dnx_sh_deinit(
    int unit);

shr_error_e cmd_dnx_sh_calibrate(
    int unit);

#endif /* _INCLUDE_DNX_CMDLIST_H */
