/*! \file bcm_int/dnx/srv6/srv6.h
 *
 * Internal DNX SRv6 APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>

#ifndef _SRV6_API_INCLUDED__
/*
 * {
 */
#define _SRV6_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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
 * \brief - SRv6 GSID/USID NOF allowed Prefixes
 */
#define SRV6_GSID_PREFIX_NOF                        (2)
#define SRV6_USID_PREFIX_NOF                        (2)
/**
 * \brief - SRv6 SID Size in bytes
 */
#define SRV6_SID_SIZE_IN_BYTES                      (16)

/**
 * \brief - SRvH Extension length bytes unit
 */
#define SRV6_HEADER_LENGTH_UNITS_IN_BYTES            (8)

/*
 * Translate nof_sids to Header-Extension-Length
 */
#define SRV6_NOF_SIDS_TO_HEADER_LENGTH(_nof_sids_)         (_nof_sids_*(SRV6_SID_SIZE_IN_BYTES/SRV6_HEADER_LENGTH_UNITS_IN_BYTES))

/*
 * Translate nof_sids to Header-Extension-Length
 */
#define SRV6_HEADER_LENGTH_TO_NOF_SIDS(_header_length_)    ((_header_length_)/(SRV6_SID_SIZE_IN_BYTES/SRV6_HEADER_LENGTH_UNITS_IN_BYTES))

/*
 * \brief - SRv6 GSID/USID allowed Prefix sizes
 */
extern const int srv6_gsid_prefix_sizes[];
extern const int srv6_usid_prefix_sizes[];
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
 *                        bcmSwitchSrv6GsidPrefixBits - use to set GSID to Prefix of 64b mode
 *                        bcmSwitchSrv6UsidPrefixBits - use to set Usid to Prefix of 48b mode
 *   \param [in] value - the value to set above modes - 1 or 0.
 *
 * \return
 *      shr_error_e
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
 *                        bcmSwitchSRV6EgressPSPEnable - use to get to Egress PSP mode
 *                        bcmSwitchSRV6ReducedModeEnable use to get to Reduced encapsulation mode
 *                        bcmSwitchSrv6GsidPrefixBits - use to get GSID to Prefix of 64b mode
 *                        bcmSwitchSrv6UsidPrefixBits - use to get Usid to Prefix of 48b mode
 *   \param [in] value - the value of the Egress or encapsulation mode as set by type
 *
 * \return
 *      shr_error_e
 */
shr_error_e dnx_switch_srv6_modes_get(
    int unit,
    bcm_switch_control_t type,
    int *value);

/**
 * \brief - API configure the LPM result that will be identified as an SRv6 USP trap in PMF
 *
 *   \param [in] unit - unit ID
 *   \param [in] value - The destination value.
 *                       It is a gport that can be encoded as a FEC, MC_ID, or no encoding (used as RAW)
 *
 * \return
 *      shr_error_e
 */
shr_error_e dnx_switch_srv6_lpm_destination_for_srv6_trap_set(
    int unit,
    int value);

/**
 * \brief - API retrive the LPM result that will be identified as an SRv6 USP trap in PMF
 *
 *   \param [in] unit - unit ID
 *   \param [out] value - The destination value.
 *                        It is a gport that can be encoded as a FEC, MC_ID, or no encoding (used as RAW)
 *
 * \return
 *      shr_error_e
 */
shr_error_e dnx_switch_srv6_lpm_destination_for_srv6_trap_get(
    int unit,
    int *value);

/*
 * } 
 */
#endif/*_SRv6_API_INCLUDED__*/
