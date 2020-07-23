/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF FABRIC CELL SNAKE TEST H
 */
 
#ifndef _SOC_DNXF_FABRIC_CELL_SNAKE_TEST_H_
#define _SOC_DNXF_FABRIC_CELL_SNAKE_TEST_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <bcm/types.h>




#define SOC_DNXF_ENABLE_MAC_LOOPBACK         (0x1)
#define SOC_DNXF_ENABLE_PHY_LOOPBACK         (0x0)
 
#define SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK    (0x2)
#define SOC_DNXF_DONT_TOUCH_MAC_INTERRUPTS   (0x4)
#define SOC_DNXF_ENABLE_ASYNC_FIFO_LOOPBACK  (0x8)


#define SOC_DNXF_SNAKE_INFINITE_RUN 0x1
#define SOC_DNXF_SNAKE_STOP_TEST 0x2 


#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG 0x1
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET 0x2
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION 0x4
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION 0x8
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND 0x10
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND 0x20
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND 0x40
#define SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND 0x80



typedef struct soc_dnxf_fabric_cell_snake_test_results_s
{
    int                    test_failed;
    uint32                 interrupts_status[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32                 failure_stage_flags;
} soc_dnxf_fabric_cell_snake_test_results_t;




int
  soc_dnxf_cell_snake_test_prepare(
    int unit, 
    uint32 flags);
    
int
  soc_dnxf_cell_snake_test_run(
    int unit, 
    uint32 flags,
    soc_dnxf_fabric_cell_snake_test_results_t* results);




#endif 
