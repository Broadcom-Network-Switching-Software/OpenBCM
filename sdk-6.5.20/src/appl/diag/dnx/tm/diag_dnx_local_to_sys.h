/** \file diag_dnx_ingress_congestion.h
 * 
 * DNX TM ingress congestion diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_LOCAL_TO_SYS_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_LOCAL_TO_SYS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <appl/diag/sand/diag_sand_framework.h>

shr_error_e sh_dnx_ingress_local_to_sys_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnx_ingress_local_to_sys_man;

#endif /** DIAG_DNX_LOCAL_TO_SYS_H_INCLUDED */
