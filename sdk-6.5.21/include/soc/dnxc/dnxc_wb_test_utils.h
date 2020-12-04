/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _TEST_DNXC_WB_H_
#define _TEST_DNXC_WB_H_

#define DNXC_WARM_BOOT_API_TEST_MODE_NONE                          (0)
#define DNXC_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API               (1)

#define DNXC_WARMBOOT_API_FUNCTION_NAME_MAX_LEN 100

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#define BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)\
do {\
    dnxc_wb_test_mode_skip_wb_sequence(unit);\
    }while (0)
#else
#define BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit)
#endif

typedef int (
    *dnxc_wb_test_callback_func_t) (
    int unit,
    void *userdata);

typedef struct
{
    dnxc_wb_test_callback_func_t dnxc_wb_test_callback_func;
    void *dnxc_wb_test_callback_data;
} dnxc_wb_test_callback_t;

#ifdef BCM_WARM_BOOT_API_TEST

void dnxc_wb_api_test_filter_set(
    int unit,
    uint8 low_high,
    uint32 value);

void dnxc_wb_test_mode_set(
    int unit,
    int mode);

void dnxc_wb_test_mode_get(
    int unit,
    int *mode);

void dnxc_wb_no_wb_test_set(
    int unit,
    int wb_flag);

void dnxc_wb_no_wb_test_get(
    int unit,
    int *override_counter);

void dnxc_wb_deinit_init_during_wb_test_set(
    int unit,
    int wb_flag);

int dnxc_wb_deinit_init_during_wb_test_get(
    int unit);

int dnxc_wb_test_mode_skip_wb_sequence(
    int unit);

int dnxc_wb_api_test_counter_increase(
    int unit);
int dnxc_wb_api_test_counter_decrease(
    int unit);

int dnxc_warm_boot_api_test_reset(
    int unit);

void dnxc_warmboot_test_tr_141_command_set(
    int unit,
    int (*function_ptr) (int unit));

void dnxc_wb_test_callback_register(
    int unit,
    uint32 flags,
    dnxc_wb_test_callback_func_t callback,
    void *userdata);

void dnxc_wb_test_callback_unregister(
    int unit);

int dnxc_wb_all_reset_test_utils_preconditions(
    int unit);
#endif

#endif
