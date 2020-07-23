/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FABRIC CELL SNAKE TEST H
 */
 
#ifndef _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_H_
#define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_H_





#include <soc/error.h>
#include <soc/types.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>











int
  soc_fe3200_cell_snake_test_prepare(
    int unit, 
    uint32 flags);
    
int
  soc_fe3200_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_fabric_cell_snake_test_results_t* results);

#endif
