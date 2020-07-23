/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * WLAN initializers
 */
#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/port.h>
#include <bcm/error.h> 
#include <bcm/wlan.h>

/* 
 * Function:
 *      bcm_wlan_client_t_init
 * Purpose:
 *      Initialize the WLAN client struct
 * Parameters: 
 *      wlan_vpn_config - Pointer to the struct to be init'ed
 */
void
bcm_wlan_client_t_init(bcm_wlan_client_t *wlan_client)
{
    if (wlan_client != NULL) {
        sal_memset(wlan_client, 0, sizeof(*wlan_client));
    }
    return;
}

/* 
 * Function:
 *      bcm_wlan_port_t_init
 * Purpose:
 *      Initialize the WLAN port struct
 * Parameters: 
 *      port_info - Pointer to the struct to be init'ed
 */
void
bcm_wlan_port_t_init(bcm_wlan_port_t *port_info)
{   
    if (port_info != NULL) {
        sal_memset(port_info, 0, sizeof(*port_info));
    }
    return;
}

#else
typedef int _bcm_wlan_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

