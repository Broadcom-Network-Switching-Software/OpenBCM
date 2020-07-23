/**
 * \file diag_sand_system.h
 *
 * Framework utilities, structures and definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SAND_SYSTEM_H_INCLUDED
#define SAND_SYSTEM_H_INCLUDED

#include <appl/diag/sand/diag_sand_prt.h>

typedef struct
{
    sh_sand_control_t *sand_control;
    prt_control_t *prt_ctr;
    int failed;
    int succeeded;
    int skipped;
} shell_flex_t;

#endif /* SAND_SYSTEM_H_INCLUDED */
