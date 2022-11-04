/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXC_SW_STATE_PRINT_H
#define _DNXC_SW_STATE_PRINT_H

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <sal/core/sync.h>
#include <bcm/types.h>
#include <soc/types.h>

void dnxc_sw_state_to_string_uint8_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_int16_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_uint16_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_uint32_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_uint64_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_int_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_shr_bitdcl_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_mutex_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_sem_cb(
    int unit,
    const void *data,
    char **return_string);

void dnxc_sw_state_to_string_char_cb(
    int unit,
    const void *data,
    char **return_string);

#endif
#endif
