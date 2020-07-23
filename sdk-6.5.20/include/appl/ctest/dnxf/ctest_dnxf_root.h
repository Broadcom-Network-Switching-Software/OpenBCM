/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file ctest_dnxf_root.h
 *
 * Inlcude file for globals of root ctests for DNXF devices
 */

#ifndef _INCLUDE_CTEST_DNXF_ROOT_H
#define _INCLUDE_CTEST_DNXF_ROOT_H

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (RAMON) family only!"
#endif

#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnxf_ctest_man;
extern sh_sand_cmd_t sh_dnxf_ctest_commands[];

#endif /* _INCLUDE_CTEST_DNX_ROOT_H */
