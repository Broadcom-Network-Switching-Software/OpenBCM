/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * TRILL initializers
 */

#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <bcm/trill.h>
#include <bcm/stat.h>

/* 
 * Function:
 *      bcm_trill_port_t_init
 * Purpose:
 *      Initialize TRILL  port struct
 * Parameters: 
 *      trill_port - Pointer to the struct to be init'ed
 */
void
bcm_trill_port_t_init(bcm_trill_port_t *trill_port)
{   
    if (trill_port != NULL) {
        sal_memset(trill_port, 0, sizeof(*trill_port));
        trill_port->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        trill_port->outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    }
    return;
}

void bcm_trill_multicast_entry_t_init(
    bcm_trill_multicast_entry_t *trill_multicast_entry)
{
    if (trill_multicast_entry != NULL) {
        sal_memset(trill_multicast_entry, 0, sizeof(*trill_multicast_entry));
    }
    return;
}
/* Initialize the TRILL VPN config structure. */
void bcm_trill_vpn_config_t_init(
    bcm_trill_vpn_config_t *trill_vpn_config)
{
    if (trill_vpn_config != NULL) {
        sal_memset(trill_vpn_config, 0, sizeof(*trill_vpn_config));
    }
    return;

}
#else
typedef int _bcm_trill_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
