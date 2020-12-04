/*
 * ! \file bcm_int/dnx/ipmc/ipmc.h Internal DNX IPMC APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _IPMC_API_INCLUDED__
/*
 * {
 */
#define _IPMC_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * The IN-LIF mask size of 22bits.
 */
#define IPMC_GLOBAL_IN_LIF_MASK_SIZE 22
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
#endif/*_IPMC_API_INCLUDED__*/
