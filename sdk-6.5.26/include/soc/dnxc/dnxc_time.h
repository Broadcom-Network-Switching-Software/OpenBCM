/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _SOC_DNXC_TIME_H_
#define _SOC_DNXC_TIME_H_

#include <soc/sand/shrextend/shrextend_debug.h>

typedef enum
{

    DNXC_TIME_UNIT_INVALID = -1,

    DNXC_TIME_UNIT_SEC = 0,

    DNXC_TIME_UNIT_MILISEC = 1,

    DNXC_TIME_UNIT_USEC = 2,

    DNXC_TIME_UNIT_NSEC = 3
} dnxc_time_unit_e;

typedef struct dnxc_time_e
{
    dnxc_time_unit_e time_units;
    uint64 time;
} dnxc_time_t;

shr_error_e dnxc_time_units_convert(
    int unit,
    dnxc_time_t * time_src,
    dnxc_time_unit_e required_time_units,
    dnxc_time_t * time_dest);

shr_error_e dnxc_time_clock_cycles_to_time_common_get(
    int unit,
    uint32 nof_clock_cycles,
    uint32 clock_freq_khz,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

shr_error_e dnxc_time_time_to_clock_cycles_common_get(
    int unit,
    dnxc_time_t * time,
    uint32 clock_freq_khz,
    uint32 *nof_clock_cycles);

shr_error_e dnxc_time_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

shr_error_e dnxc_time_system_ref_clock_cycles_to_time_get(
    int unit,
    uint32 nof_clock_cycles,
    dnxc_time_unit_e required_time_unit,
    dnxc_time_t * time);

shr_error_e dnxc_time_time_to_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles);

shr_error_e dnxc_time_time_to_system_ref_clock_cycles_get(
    int unit,
    dnxc_time_t * time,
    uint32 *nof_clock_cycles);

#include <soc/defs.h>
#endif
