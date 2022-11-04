/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * RAMON FABRIC CELL SNAKE TEST H
 */

#ifndef _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_H_
#define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

#include <soc/error.h>
#include <soc/types.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>

int soc_ramon_cell_snake_test_prepare(
    int unit,
    uint32 flags,
    soc_dnxf_fabric_cell_snake_test_results_t * results);

int soc_ramon_cell_snake_test_run(
    int unit,
    uint32 flags,
    soc_dnxf_fabric_cell_snake_test_results_t * results);

#endif
