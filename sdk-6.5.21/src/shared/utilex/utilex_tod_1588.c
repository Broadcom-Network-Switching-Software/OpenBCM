/** \file utilex_tod_1588.c
 *
 * Provide a utility to help work with TOD( time of day) in 1588 V1 format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>

#include <shared/utilex/utilex_tod_1588.h>
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
utilex_tod_1588_add_nanoseconds(
    int unit,
    int subtract,
    uint64 tod_1588_in,
    uint64 nanoseconds_to_add,
    uint64 *tod_1588_out)
{
    uint32 seconds_in;
    uint32 nanoseconds_in;
    uint32 seconds_out;
    uint32 nanoseconds_out;
    uint64 seconds_to_add_uint64;
    uint64 nanoseconds_to_add_cyc_billion_uint64;
    uint64 seconds_to_add_in_nanoseconds_uint64;
    uint32 seconds_to_add_uint32;
    uint32 nanoseconds_to_add_cyc_billion_uint32;
    uint32 billion_uint32 = 1000000000;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tod_1588_out, _SHR_E_PARAM, "tod_1588_out");

    /*
     * We assume TOD 1588 is in V1 format, meaning the 32 LSB are nanoseconds, cyclical by 1 billion,
     * and the 32 MSB are seconds.
     */

    seconds_in = COMPILER_64_HI((tod_1588_in));
    nanoseconds_in = COMPILER_64_LO((tod_1588_in));

    if (nanoseconds_in >= billion_uint32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported format. Expected nanoseconds (%u) to be smaller then 1 billion. "
                     "second %u.\n", nanoseconds_in, seconds_in);
    }

    COMPILER_64_COPY(seconds_to_add_uint64, nanoseconds_to_add);
    COMPILER_64_UDIV_32(seconds_to_add_uint64, billion_uint32);

    COMPILER_64_COPY(seconds_to_add_in_nanoseconds_uint64, seconds_to_add_uint64);
    COMPILER_64_UMUL_32(seconds_to_add_in_nanoseconds_uint64, billion_uint32);

    if (COMPILER_64_GT(seconds_to_add_in_nanoseconds_uint64, nanoseconds_to_add))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected rounding error.\n");
    }

    COMPILER_64_COPY(nanoseconds_to_add_cyc_billion_uint64, nanoseconds_to_add);
    COMPILER_64_SUB_64(nanoseconds_to_add_cyc_billion_uint64, seconds_to_add_in_nanoseconds_uint64);

    if (COMPILER_64_HI(seconds_to_add_uint64) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Value to add too large (MSB %u,LSB %u).\n",
                     COMPILER_64_HI(nanoseconds_to_add), COMPILER_64_LO(nanoseconds_to_add));
    }
    if (COMPILER_64_HI(nanoseconds_to_add_cyc_billion_uint64) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illogical value reached.\n");
    }

    seconds_to_add_uint32 = COMPILER_64_LO(seconds_to_add_uint64);
    nanoseconds_to_add_cyc_billion_uint32 = COMPILER_64_LO(nanoseconds_to_add_cyc_billion_uint64);

    seconds_out = (subtract) ? (seconds_in - seconds_to_add_uint32) : (seconds_in + seconds_to_add_uint32);
    if ((subtract && nanoseconds_to_add_cyc_billion_uint32 > nanoseconds_in) ||
        (!subtract && nanoseconds_to_add_cyc_billion_uint32 + nanoseconds_in >= billion_uint32))
    {
        seconds_out = (subtract) ? (seconds_out - 1) : (seconds_out + 1);
        nanoseconds_out = (subtract) ? (nanoseconds_to_add_cyc_billion_uint32 - nanoseconds_in) :
            (nanoseconds_to_add_cyc_billion_uint32 + nanoseconds_in - billion_uint32);
    }
    else
    {
        nanoseconds_out = (subtract) ? (nanoseconds_in - nanoseconds_to_add_cyc_billion_uint32) :
            (nanoseconds_in + nanoseconds_to_add_cyc_billion_uint32);
    }

    COMPILER_64_SET((*tod_1588_out), seconds_out, nanoseconds_out);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_tod_1588_diff_nanoseconds(
    int unit,
    uint64 tod_1588_earlier,
    uint64 tod_1588_later,
    uint64 *time_diff_nanosecond)
{
    uint32 seconds_later;
    uint32 nanoseconds_later;
    uint32 seconds_earlier;
    uint32 nanoseconds_earlier;
    uint32 seconds_diff;
    uint32 nanoseconds_diff;
    uint64 total_diff_in_nanoseconds;
    uint32 billion_uint32 = 1000000000;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * We assume TOD 1588 is in V1 format, meaning the 32 LSB are nanoseconds, cyclical by 1 billion,
     * and the 32 MSB are seconds.
     */

    seconds_earlier = COMPILER_64_HI((tod_1588_earlier));
    nanoseconds_earlier = COMPILER_64_LO((tod_1588_earlier));

    seconds_later = COMPILER_64_HI((tod_1588_later));
    nanoseconds_later = COMPILER_64_LO((tod_1588_later));

    if (nanoseconds_earlier >= billion_uint32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported format. Expected nanoseconds (%u) to be smaller then 1 billion. "
                     "second %u. (earlier time).\n", nanoseconds_earlier, seconds_earlier);
    }
    if (nanoseconds_later >= billion_uint32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported format. Expected nanoseconds (%u) to be smaller then 1 billion. "
                     "second %u. (later time).\n", nanoseconds_later, seconds_later);
    }

    seconds_diff = seconds_later - seconds_earlier;
    if (nanoseconds_later < nanoseconds_earlier)
    {
        seconds_diff--;
        nanoseconds_diff = nanoseconds_earlier - nanoseconds_later;
    }
    else
    {
        nanoseconds_diff = nanoseconds_later - nanoseconds_earlier;
    }

    COMPILER_64_SET(total_diff_in_nanoseconds, 0, seconds_diff);
    COMPILER_64_UMUL_32(total_diff_in_nanoseconds, billion_uint32);
    COMPILER_64_ADD_32(total_diff_in_nanoseconds, nanoseconds_diff);

    COMPILER_64_COPY((*time_diff_nanosecond), total_diff_in_nanoseconds);

exit:
    SHR_FUNC_EXIT;
}
