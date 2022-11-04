/*
 * ! \file bcm_int/dnx/ipmc/ipmc.h Internal DNX IPMC APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _IPMC_API_INCLUDED__
/*
 * {
 */
#define _IPMC_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * The IN-LIF mask size of 22bits.
 */
#define IPMC_GLOBAL_IN_LIF_MASK_SIZE 22
/** The size of the VSI used for L2 tables */
#define IPMC_BRIDGE_VSI_SIZE 17
/**
 * The value of the first two bytes of the IPv6 multicast address
 */
#define IPMC_MC_IPV6_ADDRESS_PREFIX 0xFF
/**
 * The number of bytes in an IPv6 address
 */
#define NOF_IPV6_ADDR_BYTES  16
/**
 * The IN-LIF (in_lif) can be fully mask (don't care about it value) or not masked at all (nothing in between).
 */
#define IPMC_GET_IN_LIF_MASK(in_lif) (in_lif == 0 ? 0 : SAL_UPTO_BIT(IPMC_GLOBAL_IN_LIF_MASK_SIZE))
/** Get the mask for the VSI field based on the VSI value */
#define IPMC_GET_VSI_MASK(vsi) (vsi == 0 ? 0 : SAL_UPTO_BIT(IPMC_BRIDGE_VSI_SIZE))
/*
 * Checks whether the mask of the IPv6 address is empty (all 1s) - if the last byte is 0xFF,
 * then we can assume that all bytes in the mask are equal to 0xFF.
 */
#define IPMC_IS_IPV6_NO_MASKING(mask) (mask[NOF_IPV6_ADDR_BYTES-1] == 0xFF)
/*
 * Checks whether the IPv6 addess (an array of 16 uint8) is fully masked.
 * We can assume that if the first byte is 0, then all the others are 0.
 */
#define IPMC_IS_IPV6_FULL_MASKING(mask) (mask[0] == 0)
/*
 * Indicate whether the IPv6 address is multicast - If its first byte is 0xFF
 */
#define IPMC_IS_IPV6_ADDRESS_MULTICAST(addr, mask) ((addr[0] & mask[0]) == IPMC_MC_IPV6_ADDRESS_PREFIX)

/*
 * }
 */

/*
 * ENUMS
 * {
 */

/**
 * A list of available modes for IPMC configuration entries.
 */
typedef enum dnx_ipmc_config_compression_mode_e
{
    IPMC_CONFIG_CMPRS_SIP,
    IPMC_CONFIG_CMPRS_SIP_INTF,
    IPMC_CONFIG_CMPRS_VSI_GROUP,
    IPMC_CONFIG_CMPRS_VRF_GROUP,
    IPMC_CONFIG_CMPRS_NOF_MODES
} dnx_ipmc_config_compression_mode_e;

/*
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
 * \brief
 *   Determine whether the IPv6 address is a default one or not. If
 *   the IPv6 address is 0:0:0:0:0:0:0:0 then it is default.
 * \param [in] address - The IPv6 address that needs to be checked
 *                       if it is all zeros.
 * \param [in] mask - The mask of the IPv6 address.
 *   A raised bit in the mask indicates that a bit from the IP address is shown.
 * \return
 *   \retval Zero if the IPv6 address is not default
 *   \retval One if the IPv6 address is default.
 * \see
 *  * dnx_ipmc_verify
 */
int dnx_ipmc_ipv6_default_address(
    bcm_ip6_t address,
    bcm_ip6_t mask);
/*
 * }
 */

#endif/*_IPMC_API_INCLUDED__*/
