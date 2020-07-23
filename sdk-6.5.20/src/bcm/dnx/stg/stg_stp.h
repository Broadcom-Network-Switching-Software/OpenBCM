/*! \file stg_stp.h
 *
 *  Internal DNX STG APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#ifndef STG_STP_H_INCLUDED
/* { */
#define STG_STP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */

/*
 * }
 */
/*
 * MACROs
 * {
 */

/*
 * DECLARATIONs
 * {
 */

/**
 * \brief - Check if the stg is in range and if it is allocated.
 *
 * \param [in] unit - unit ID
 * \param [in] stg_id - Given stg_id
 * \param [in] verify_alloc - None-zero: check if the stg_id was allocated already.
 *   
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *   'verify_alloc' should be TRUE if dispatching configurations to the stg.
 *
 * \see
 *   dnx_algo_res_is_allocated.
 *   dnx_stg_general_info_get
 */
shr_error_e dnx_stg_id_verify(
    int unit,
    bcm_stg_t stg_id,
    int verify_alloc);

/**
 * \brief - Get stg general information from sw table. 
 *
 * \param [in] unit - Unit ID
 * \param [out] stg_min - Pointer for the minimum STG ID in the system.
 * \param [out] stg_max - Pointer for the maximum STG ID in the system.
 * \param [out] stg_defl - Pointer for the default STG ID in the system.
 * \param [out] stg_count - Pointer for number of STG IDs allocated.
 *
 * \remark
 *  There must be at least one valid pointer in the inputs.
 *  A null pointer indicates no request for its value.
 *
 * \see
 *   * dnx_stg_general_info_set.
 */
shr_error_e dnx_stg_general_info_get(
    int unit,
    bcm_stg_t * stg_min,
    bcm_stg_t * stg_max,
    bcm_stg_t * stg_defl,
    int *stg_count);

/*
 * }
 */

#endif /* STG_STP_H_INCLUDED */
