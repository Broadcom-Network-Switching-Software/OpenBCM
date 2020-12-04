/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Implements power test APIs that will dispatch to per project power test APIs
 * Projects supported:
 * TRIDENT3
 * MAVERICK2
 * TOMAHAWK3
 */


#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <bcm/port.h>
#include <bcm/vlan.h>

#include "testlist.h"

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT)

/*
 * Function:
 *      power_test_init
 * Purpose:
 *      Test init function. Parse CLI params and do necessary init.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
power_test_init(int unit, args_t *a, void **pa)
{
    int rv = BCM_E_NONE;
    return rv;
}


/*
 * Function:
 *      power_test
 * Purpose:
 *      Set up ports/streams and send packets.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
power_test(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;
    return rv;
}


/*
 * Function:
 *      power_test_cleanup
 * Purpose:
 *      Do test end checks and free all allocated memory.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
int
power_test_cleanup(int unit, void *pa)
{
    int rv = BCM_E_NONE;

    return rv;
}

#endif
