/** \file utilex_ptp_time.h
 * 
 * Provide a utility to help work with PTP time (bcm_ptp_timestamp_t)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_TIME_PTP_TIME_H_INCLUDED
#define UTILEX_TIME_PTP_TIME_H_INCLUDED
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
 * \brief - Convert PTP time format to nanoseconds
 *  nanoseconds = (ptp_time->seconds * 1000000000) + ptp_time->nanoseconds
 *
 * \param [in] unit -  Unit-ID
 * \param [in] ptp_time - PTP time
 * \param [out] nanoseconds - PTP converted into nanoseconds
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e utilex_ptp_time_to_nanoseconds_convert(
    int unit,
    bcm_ptp_timestamp_t * ptp_time,
    uint64 *nanoseconds);

/**
 * \brief - Convert nanoseconds to PTP time format
 *  ptp_time->seconds =  nanoseconds / 1000000000
 *  ptp_time->nanoseconds = nanoseconds % 1000000000
 *
 * \param [in] unit -  Unit-ID
 * \param [in] nanoseconds - time in nanoseconds
 * \param [out] ptp_time - PTP time converted from nanoseconds

 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e utilex_ptp_time_from_nanoseconds_convert(
    int unit,
    uint64 nanoseconds,
    bcm_ptp_timestamp_t * ptp_time);

/*
 * }
 */

#endif /* UTILEX_TIME_PTP_TIME_H_INCLUDED */
