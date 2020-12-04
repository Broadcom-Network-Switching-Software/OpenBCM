/** \file src/bcm/dnx/multicast/multicast_pp.h
 * Internal DNX MIRROR PROFILE APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_MULTICAST_PP_INCLUDED__

#define _DNX_MULTICAST_PP_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <include/bcm_int/dnx/field/field_map.h>
/*
 * }
 */

/**
 * The ACE ID of the ACE formats used for PPMC.
 */
#define DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_ID (1)
#define DNX_MULTICAST_PPMC_DOUBLE_OUTLIF_ACE_ID (2)
#define DNX_MULTICAST_PPMC_TRIPLE_OUTLIF_ACE_ID (3)
/**
 * The names of the ACE formats used for PPMC.
 */
#define DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_FORMAT_NAME ("PPMC_SINGLE_OUTLIF")
#define DNX_MULTICAST_PPMC_DOUBLE_OUTLIF_ACE_FORMAT_NAME ("PPMC_DOUBLE_OUTLIF")
#define DNX_MULTICAST_PPMC_TRIPLE_OUTLIF_ACE_FORMAT_NAME ("PPMC_TRIPLE_OUTLIF")

/**
 * The IDs of the user defined action of the ACE formats used for PPMC.
 */
#define DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID (1)
#define DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_ID (2)
#define DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_ID (3)
/**
 * The names of the user defined action of the ACE formats used for PPMC.
 */
#define DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_NAME ("UDA_PPMC_OUTLIF_0")
#define DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_NAME ("UDA_PPMC_OUTLIF_1")
#define DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_NAME ("UDA_PPMC_OUTLIF_2")

/** 
 * The maximum number of outlifs in the PPMC ACE group, and also the number of ACE formats (ACE IDs).
 */
#define DNX_MULTICAST_PPMC_MAX_OUTLIFS (3)
/**
 * The value to update the outlif_valid signal that corresponds to the outlif that we update.
 */
#define DNX_MULTICAST_PPMC_OUTLIF_VALID_VALUE (1)

/**
 * \brief
 *   Initialize dnx multicast PP
 * \param [in] unit - Unit ID
 * \see
 *   * dnx_multicast_init
 */
shr_error_e dnx_multicast_pp_init(
    int unit);

/**
 *\brief
 *  When working on Jericho 1 mode an invalid CUD is represented by -1 (18 bits) and Jericho 2 consider an invalid CUD as 0.
 *  To solve this issue the PPMC table is used to switch JR1 invalid CUD with the JR2 invalid CUD value.
 *  This function add the PPMC entry that replace the invalid CID values
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 */
shr_error_e dnx_multicast_pp_jr1_smae_interface_init(
    int unit);

#endif/*_DNX_MULTICAST_PP_INCLUDED__*/
