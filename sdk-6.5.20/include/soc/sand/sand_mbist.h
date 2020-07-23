/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _SOC_SAND_MBIST_H
#define _SOC_SAND_MBIST_H

#include <sal/types.h>
#include <sal/core/time.h>
#include <soc/register.h>
#include <soc/shared/sat.h>
#include <shared/shrextend/shrextend_debug.h>

#define SAND_MBIST_COMMAND_WAIT    0
#define SAND_MBIST_COMMAND_WRITE   0x40
#define SAND_MBIST_COMMAND_READ    0x80
#define SAND_MBIST_COMMAND_COMMENT 0xc0

#define SAND_MBIST_COMMAND_MASK    0xc0
#define SAND_MBIST_COMMAND_INVMASK 0x3f
#define SAND_MBIST_RUN_FULL        0x1000
#define SAND_MBIST_TEST_LONG_WAIT_VALUE 0x3fffffff 
#define SAND_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC 0x80000000 

#define SAND_MBIST_UINT32(value) (value)>>24, ((value)>>16) & 0xff, ((value)>>8) & 0xff, (value) & 0xff

#define SAND_MBIST_RETURN_VALUE(mask, expected_value, error_line_offset) SAND_MBIST_COMMAND_READ | (((error_line_offset)>>8) & \
  SAND_MBIST_COMMAND_INVMASK), (error_line_offset) & 0xff, SAND_MBIST_UINT32(mask), SAND_MBIST_UINT32(expected_value | ~mask)
#define SAND_MBIST_READ(mask, expected_value, error_line_offset) SAND_MBIST_COMMAND_READ | (((error_line_offset)>>8) & \
  SAND_MBIST_COMMAND_INVMASK), (error_line_offset) & 0xff, SAND_MBIST_UINT32(mask), SAND_MBIST_UINT32(expected_value)
#define SAND_MBIST_WRITE(value) SAND_MBIST_COMMAND_WRITE, SAND_MBIST_UINT32(value)
#define SAND_MBIST_WAIT(time) SAND_MBIST_COMMAND_WAIT | (((time)>>24) & SAND_MBIST_COMMAND_INVMASK), ((time)>>16) & 0xff, ((time)>>8) & 0xff, (time) & 0xff
#define SAND_MBIST_COMMENT SAND_MBIST_COMMAND_COMMENT
#if defined(COMPILER_STRING_CONST_LIMIT) && !defined(INCLUDE_MBIST_COMMENTS)
#define SAND_MBIST_COMMENT_TEXT(comment) ""
#else
#define SAND_MBIST_COMMENT_TEXT(comment) comment
#endif


typedef struct sand_cpu2tap_script_s {
    const uint8       *commands;
    const char        **comments;
    uint32      nof_commands;
    uint32      nof_comments;
    char        *script_name;
    uint32      sleep_after_write;
} sand_cpu2tap_script_t;
typedef sand_cpu2tap_script_t sand_mbist_script_t;


typedef struct sand_cpu2tap_dynamic_s {
    uint32      nof_errors;
    uint8       skip_errors;
    uint8       log_level; 
    sal_usecs_t measured_time;
    uint32      ser_test_delay; 
} sand_cpu2tap_dynamic_t;
typedef sand_cpu2tap_dynamic_t sand_mbist_dynamic_t;


typedef struct sand_cpu2tap_device_s {
    uint32      sleep_divisor;
    soc_reg_t   reg_tap_command;
    soc_reg_t   reg_tap_data_in;
    soc_reg_t   reg_tap_data_out;
} sand_cpu2tap_device_t;
typedef sand_cpu2tap_device_t sand_mbist_device_t;


uint32 soc_sand_mbist_init_legacy(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_mbist_dynamic_t *dynamic);

uint32 soc_sand_mbist_init_new(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic);

uint32 soc_sand_cpu2tap_init_reset_enable(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic);

uint32 soc_sand_cpu2tap_init_no_sys_reset_enable(
    const int unit, 
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic);


uint32 soc_sand_mbist_deinit(
                             const int unit, 
                             const sand_cpu2tap_device_t *cpu2tap_device, 
                             const sand_cpu2tap_dynamic_t *dynamic
                             );


uint32 soc_sand_mbist_deinit_new(
                             const int unit,
                             const sand_cpu2tap_device_t *cpu2tap_device, 
                             const sand_cpu2tap_dynamic_t *dynamic
                             );


uint32 soc_sand_cpu2tap_deinit_mbist(
                             const int unit,
                             const sand_cpu2tap_device_t *cpu2tap_device, 
                             const sand_cpu2tap_dynamic_t *dynamic
                             );

uint32 soc_sand_cpu2tap_deinit_no_sys_enable(
                             const int unit,
                             const sand_cpu2tap_device_t *cpu2tap_device, 
                             const sand_cpu2tap_dynamic_t *dynamic
                             );


uint32 soc_sand_mbist_deinit_legacy(
                             const int unit, 
                             const sand_cpu2tap_device_t *cpu2tap_device, 
                             const sand_cpu2tap_dynamic_t *dynamic
                             );

uint32 soc_sand_run_mbist_script(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic);
uint32 soc_sand_run_cpu2tap_script(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic,
    uint32 *output_values); 
uint32 soc_sand_run_cpu2tap_script_no_stop(
    const int unit, 
    const sand_cpu2tap_script_t *script,  
    const sand_cpu2tap_device_t *cpu2tap_device, 
    sand_cpu2tap_dynamic_t *dynamic,
    uint32 *output_values); 
#ifdef BCM_DNX_SUPPORT
shr_error_e sand_init_cpu2tap(
    const  int unit);
shr_error_e sand_deinit_cpu2tap(
    const int unit);
#endif



#define SAND_SHR_CPU2TAP_SCRIPT(func, cpu2tap_device, output_values) \
    SHR_IF_ERR_EXIT(soc_sand_run_cpu2tap_script(unit, & func ## _script, &(cpu2tap_device), &dynamic, (output_values)));


#define SAND_MBIST_TEST_SCRIPT(func, cpu2tap_device) \
    SOCDNX_IF_ERR_EXIT(soc_sand_run_cpu2tap_script(unit, & func ## _script, &cpu2tap_device, &dynamic, NULL));

#define SAND_SHR_MBIST_TEST_SCRIPT(func, cpu2tap_device) \
    SHR_IF_ERR_EXIT(soc_sand_run_cpu2tap_script(unit, & func ## _script, &cpu2tap_device, &dynamic, NULL));


#define SAND_MBIST_TEST_SCRIPT_DO_NOT_FAIL(func, cpu2tap_device) \
    soc_sand_run_cpu2tap_script(unit, & func ## _script, &cpu2tap_device, &dynamic, NULL);

#define SAND_MBIST_TEST_SCRIPT_DO_NOT_FAIL_DO_NOT_STOP(func, cpu2tap_device) \
    soc_sand_run_cpu2tap_script_no_stop(unit, & func ## _script, &cpu2tap_device, &dynamic, NULL);

#define SAND_MBIST_TEST_SCRIPT_DO_NOT_STOP(func, cpu2tap_device) \
    SHR_IF_ERR_EXIT(soc_sand_run_cpu2tap_script_no_stop(unit, & func ## _script, &cpu2tap_device, &dynamic, NULL));


#endif 

