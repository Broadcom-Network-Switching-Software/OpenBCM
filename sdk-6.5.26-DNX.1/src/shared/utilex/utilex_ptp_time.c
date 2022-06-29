/** \file utilex_ptp_time.c
 *
 * Provide a utility to help work with PTP time (bcm_ptp_timestamp_t)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#include <soc/drv.h>

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
utilex_ptp_time_to_nanoseconds_convert(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *nanoseconds)
{
    uint64 calc;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, ptp_time->seconds);
    COMPILER_64_UMUL_32(calc, 1000000000);
    COMPILER_64_ADD_32(calc, ptp_time->nanoseconds);
    COMPILER_64_COPY(*nanoseconds, calc);

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_ptp_time_from_nanoseconds_convert(
    int unit,
    uint64 nanoseconds,
    bcm_ptp_timestamp_t * ptp_time)
{
    uint64 calc;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_COPY(calc, nanoseconds);
    COMPILER_64_UDIV_32_WITH_REMAINDER(calc, 1000000000, ptp_time->nanoseconds);
    COMPILER_64_COPY(ptp_time->seconds, calc);

    SHR_FUNC_EXIT;
}
