/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Failover initializers
 */

#include <sal/core/libc.h>
#include <bcm/failover.h>

#ifdef INCLUDE_L3

/* 
 * Function:
 *      bcm_failover_element_t_init
 * Purpose:
 *      Initialize Failover element struct
 * Parameters: 
 *      failover - Pointer to the struct to be init'ed
 */

void
bcm_failover_element_t_init(bcm_failover_element_t *failover)
{   
    if (failover != NULL) {
        sal_memset(failover, 0, sizeof(*failover));
        failover->failover_id = BCM_FAILOVER_INVALID;
        failover->intf = BCM_IF_INVALID;
        failover->port = BCM_GPORT_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_failover_multi_level_t_init
 * Purpose:
 *      Initialize multi level Failover struct
 * Parameters:
 *      multi_level_failover - Pointer to the struct to be init'ed
 */
void
bcm_failover_multi_level_t_init(
    bcm_failover_multi_level_t *multi_level_failover)
{
    if (multi_level_failover != NULL) {
        sal_memset(multi_level_failover, 0, sizeof(*multi_level_failover));
    }
    return;
}

#else
typedef int _bcm_failover_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

/*
 * Function:
 *      bcm_failover_ring_t_init
 * Purpose:
 *      Initialize Failover ring struct
 * Parameters:
 *      failover - Pointer to the struct to be init'ed
 */

void
bcm_failover_ring_t_init(bcm_failover_ring_t *failover_ring)
{
    if (failover_ring != NULL) {
        sal_memset(failover_ring, 0, sizeof(*failover_ring));
        failover_ring->port0 = BCM_GPORT_INVALID;
        failover_ring->port1 = BCM_GPORT_INVALID;
    }
    return;
}
