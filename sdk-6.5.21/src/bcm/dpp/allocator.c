/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Global resource allocator
 */

#include <bcm_int/dpp/allocator.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/appl/io.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <shared/idxres_afl.h>
#include <shared/idxres_fl.h>
#include <shared/hash_tbl.h>









/* A common debug message prefix */

/*
 *   Function
 *      _dpp_resource_init
 *   Purpose
 *      Initialize the shr resource manager for all HW resources
 *      for the unit
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE all resources are successfully initialized
 *       BCM_E_* as appropriate otherwise
 *   Notes
 *       Returns error is any of the resources cannot be initialized
 *       Does not clean up partial initialisation.
 */

int
_dpp_resource_init(int unit)
{
    int                         status = BCM_E_NONE;

    return status;
}

/*
 *   Function
 *      _dpp_alloc_wellknown_resources
 *   Purpose
 *      Allocate well-known resouces per unit
 *      and save the same in the soc structure
 *      for sharing between modules.
 *
 *   Parameters
 *      (IN) unit   : unit number of the device
 *   Returns
 *       BCM_E_NONE - All required resources are allocated
 *       BCM_E_*    - failure
 *   Notes
 */

int
_dpp_alloc_wellknown_resources(int unit)
{

    return BCM_E_NONE;
}
