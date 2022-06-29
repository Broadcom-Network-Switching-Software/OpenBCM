/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF STAT H
 */

#ifndef _SOC_DNXF_STAT_H_
#define _SOC_DNXF_STAT_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <bcm/stat.h>

#define SOC_DNXF_MAX_NOF_COUNTERS_PER_STAT_TYPE (3)
#define _SOC_DNXF_FABRIC_STAT_PIPE_NO_VERIFY   (-1)

#define SOC_DNXF_STAT_COUNTER_MAPPING_SET(_dest, _ctr_type) \
{ \
    if (*array_size <= max_array_size) \
    { \
        _dest = _ctr_type; \
    } \
    else \
    { \
        *array_size = 0; \
        SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received"); \
    } \
}

shr_error_e soc_dnxf_stat_get(
    int unit,
    bcm_port_t port,
    uint64 *value,
    int *counters,
    int arr_size);

shr_error_e soc_dnxf_stat_rate_get(
    int unit,
    bcm_port_t port,
    uint64 *rate,
    int *counters,
    int arr_size);

shr_error_e soc_dnxf_fabric_stat_verify(
    int unit,
    int link,
    int pipe);

#endif
