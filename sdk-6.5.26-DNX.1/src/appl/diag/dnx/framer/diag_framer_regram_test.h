/**
 * \file diag_framer_regram_test.h  Contains all of the framer regmem test declarations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_FRAMER_REGMEM_TEST_H_INCLUDED
#define DIAG_FRAMER_REGMEM_TEST_H_INCLUDED

#ifndef NO_FRAMER_LIB_BUILD
/****************************
 * FlexE core include file *
 ****************************/
#include <appl/diag/sand/diag_sand_framework.h>

/*************
 * GLOBALS   *
 *************/

/*************
 * DEFINES   *
 *************/
#define DIAG_FRAMER_TEST_FLAG_SET           (0x1 << 0)
#define DIAG_FRAMER_TEST_FLAG_SET_PATTEN    (0x1 << 1)
#define DIAG_FRAMER_TEST_FLAG_REG           (0x1 << 2)
#define DIAG_FRAMER_TEST_FLAG_MEM           (0x1 << 3)
#define DIAG_FRAMER_TEST_FLAG_SKIP          (0x1 << 4)
#define DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT   (0x1 << 5)

/*************
 * TYPE DEFS *
 *************/

/*************
 * FUNCTIONS *
 *************/
shr_error_e diag_framer_test_reg_default(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control);

shr_error_e diag_framer_test_ram_default(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control);

shr_error_e diag_framer_test_list_module(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control);

#endif /* NO_FRAMER_LIB_BUILD */
#endif /* DIAG_FRAMER_REGMEM_TEST_H_INCLUDED */
