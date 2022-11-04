/** \file bcm_int/dnx/tsn/tsn.h
 * 
 *  
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_TSN_INCLUDED_
#define _DNX_TSN_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/tsn.h>
#include <soc/sand/shrextend/shrextend_debug.h>

/**
 * \brief
 *  TSN init function.
 *  Used for configurations for TSN features, required at init
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e dnx_tsn_init(
    int unit);

/**
 * \brief - Init TSN pid manager thread
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_thread_init(
    int unit);

/**
 * \brief - Deinit TSN pid manager thread
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_thread_deinit(
    int unit);

/**
 * \brief - Handle an event of TAS-ToD Update:
 *          1. Deactivate all TAF,TAS,CQF gates.
 *          2. Restart TSN counter
 *          3. Calculate and configure new base time for all TAF,TAS,CQF gates.
 *          4. Activate all TAF,TAS,CQF gates.
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_tas_tod_update_handle(
    int unit);

/**
 * \brief - TAS ToD update interrupt handler
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   See shr_error_e
 * \see
 *   * None
 */
shr_error_e dnx_tsn_tas_tod_update_interrupt_handler(
    int unit);

#endif /* _DNX_TSN_INCLUDED_ */
