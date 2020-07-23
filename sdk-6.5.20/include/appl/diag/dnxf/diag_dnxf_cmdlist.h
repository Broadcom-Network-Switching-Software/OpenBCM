/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    cmdlist.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */

#ifndef DNXF_CMDLIST_H_INCLUDED
#define DNXF_CMDLIST_H_INCLUDED

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/shrextend/shrextend_error.h>

extern int bcm_dnxf_cmd_cnt;
extern cmd_t bcm_dnxf_cmd_list[];

shr_error_e
cmd_dnxf_sh_init(
    int unit);

shr_error_e
cmd_dnxf_sh_deinit(
    int unit);

#endif /* DNXF_CMDLIST_H_INCLUDED */
