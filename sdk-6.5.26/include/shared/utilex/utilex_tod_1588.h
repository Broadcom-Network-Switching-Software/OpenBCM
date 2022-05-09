/** \file utilex_tod_1588.h
 * 
 * Provide a utility to help work with TOD( time of day) in 1588 V1 format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_TIME_TOD_1588_H_INCLUDED
#define UTILEX_TIME_TOD_1588_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/time.h>

/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
 * \brief -
 * Add/subtract to the size of TOD 1588.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] subtract -  If TURE, subtract instead of add.
 * \param [in] tod_1588_in -  Time of day to add nanoseconds_to_add to.
 * \param [in] nanoseconds_to_add -  Amount of nanoseconds to add to tod_1588.
 * \param [out] tod_1588_out -  Time of day after summation.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_tod_1588_add_nanoseconds(
    int unit,
    int subtract,
    uint64 tod_1588_in,
    uint64 nanoseconds_to_add,
    uint64 *tod_1588_out);

/**
 * \brief -
 * Find if the later time a time period is before an earlier time.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] tod_1588_earlier -  The beginning of the time period.
 * \param [in] tod_1588_later -  The end of the time period.
 * \param [out] time_diff_nanosecond -  Time period in nanoseconds.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_tod_1588_diff_nanoseconds(
    int unit,
    uint64 tod_1588_earlier,
    uint64 tod_1588_later,
    uint64 *time_diff_nanosecond);

/*
 * }
 */

#endif /* UTILEX_TIME_TOD_1588_H_INCLUDED */
