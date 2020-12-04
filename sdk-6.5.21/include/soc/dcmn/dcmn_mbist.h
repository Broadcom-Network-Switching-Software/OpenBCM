/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _SOC_DCMN_MBIST_H
#define _SOC_DCMN_MBIST_H

#include <soc/sand/sand_mbist.h>

#define DCMN_MBIST_COMMAND_WAIT    SAND_MBIST_COMMAND_WAIT
#define DCMN_MBIST_COMMAND_WRITE   SAND_MBIST_COMMAND_WRITE
#define DCMN_MBIST_COMMAND_READ    SAND_MBIST_COMMAND_READ
#define DCMN_MBIST_COMMAND_COMMENT SAND_MBIST_COMMAND_COMMENT

#define DCMN_MBIST_COMMAND_MASK    SAND_MBIST_COMMAND_MASK
#define DCMN_MBIST_COMMAND_INVMASK SAND_MBIST_COMMAND_INVMASK

#define DCMN_MBIST_TEST_LONG_WAIT_VALUE SAND_MBIST_TEST_LONG_WAIT_VALUE 
#define DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC 

#define DCMN_MBIST_UINT32(value) SAND_MBIST_UINT32(value)

#define DCMN_MBIST_READ(mask, expected_value, error_line_offset) SAND_MBIST_READ(mask, expected_value, error_line_offset)
#define DCMN_MBIST_WRITE(value) SAND_MBIST_WRITE(value)
#define DCMN_MBIST_WAIT(time) SAND_MBIST_WAIT(time)
#define DCMN_MBIST_COMMENT SAND_MBIST_COMMENT
#define DCMN_MBIST_COMMENT_TEXT(comment) SAND_MBIST_COMMENT_TEXT(comment)


typedef sand_cpu2tap_script_t dcmn_mbist_script_t;


typedef sand_cpu2tap_dynamic_t dcmn_mbist_dynamic_t;


typedef sand_cpu2tap_device_t dcmn_mbist_device_t;



uint32 soc_dcmn_mbist_init_legacy(
    const int unit, 
    const dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);


uint32 soc_dcmn_mbist_deinit(
                             const int unit, 
                             const dcmn_mbist_device_t *mbist_device, 
                             const dcmn_mbist_dynamic_t *dynamic
                             );

uint32 soc_dcmn_run_mbist_script(
    const int unit, 
    const dcmn_mbist_script_t *script,  
    const dcmn_mbist_device_t *mbist_device, 
    dcmn_mbist_dynamic_t *dynamic);




#define DCMN_MBIST_TEST_SCRIPT(func, mbist_device) SAND_MBIST_TEST_SCRIPT(func, mbist_device)


#define DCMN_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, mbist_device) \
    SAND_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, mbist_device)


#endif 

