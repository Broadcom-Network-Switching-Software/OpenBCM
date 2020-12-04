/*! \file bcm_int/dnx/srv6/srv6.h
 *
 * Internal DNX SRv6 APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dbal/dbal.h>

#ifndef _SRV6_API_INCLUDED__
/*
 * {
 */
#define _SRV6_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */

/*
 * }
 */

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
 * }
 */

/*
 * INERNAL TYPES
 * {
 */
/*
 * }
 */

/*
 * APIS
 * {
 */
/**
 * \brief 
 *      Initialize DNX SRv6 module 
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e -\n
 *     See error code description.
 * \see
 *   shr_error_e
 */
shr_error_e dnx_srv6_init(
    int unit);

/**
 * \brief - API sets the SRv6 Virtual Register and SW States for:
 *          - Set to Egress PSP mode - '1', or USP - '0'.
 *            Default after BCM Init is USP.
 *          - Set Encapsulation Reduced mode - '1', or Normal - '0'.
 *            Default after BCM Init is Normal.
 *
 *   \param [in] unit - unit ID
 *   \param [in] type - switch control type:
 *                        bcmSwitchSRV6EgressPSPEnable - use to set to Egress PSP mode
 *                        bcmSwitchSRV6ReducedModeEnable use to set to Reduced encapsulation mode
 *    \param [in] value - the value to set above modes - 1 or 0.
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
shr_error_e dnx_switch_srv6_modes_set(
    int unit,
    bcm_switch_control_t type,
    int value);

/**
 * \brief - API gets the SRv6 Virtual Register and SW States for:
 *          - Get the Egress PSP mode - '1', or USP - '0'.
 *          - Get Encapsulation Reduced mode - '1', or Normal - '0'.
 *
 *   \param [in] unit - unit ID
 *   \param [in] type - switch control type:
 *                        bcmSwitchSRV6EgressPSPEnable - use to set to Egress PSP mode
 *                        bcmSwitchSRV6ReducedModeEnable use to set to Reduced encapsulation mode
 *    \param [in] value - the value of the Egress or encapsulation mode as set by type
 *
 * \return
 *   \retval shr_error_e - Negative in case of an error, zero in case all ok.
 */
shr_error_e dnx_switch_srv6_modes_get(
    int unit,
    bcm_switch_control_t type,
    int *value);

/*
 * } 
 */
#endif/*_SRv6_API_INCLUDED__*/
