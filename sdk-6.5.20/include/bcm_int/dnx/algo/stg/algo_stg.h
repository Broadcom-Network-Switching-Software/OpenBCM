/**
 * \file algo_stg.h
 *
 *  Internal DNX STG Managment APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifndef ALGO_STG_H_INCLUDED
/*
 * {
 */
#define ALGO_STG_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_stg_access.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * }
 */

/*
 * Defines
 * {
 */
#define DNX_ALGO_RES_TYPE_STG_ID                 "STG_ID"

/**
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * }
 */

/**
 * \brief
 *  Initialize STG resources.
 *
 * \param [in] unit - Unit-ID
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e

 * \remark
 *  The resource of STG IDs is initialized in the range of {1, nof_topology_ids}
 *  which is the range of STG IDs BCM APIs are using.
 *  Topology IDs in hardware is encoded with STG_ID - 1.
 *
 * \see
 *  dnx_algo_res_create
 */
shr_error_e dnx_algo_stg_init(
    int unit);

/*
 * }
 */
#endif /* ALGO_STG_H_INCLUDED */
