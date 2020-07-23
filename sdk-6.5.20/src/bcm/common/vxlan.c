/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * VXLAN initializers
 */

#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <bcm/vxlan.h>

/* 
 * Function:
 *      bcm_vxlan_port_t_init
 * Purpose:
 *      Initialize VXLAN  port struct
 * Parameters: 
 *      vxlan_port - Pointer to the struct to be init'ed
 */
void
bcm_vxlan_port_t_init(bcm_vxlan_port_t *vxlan_port)
{   
    if (vxlan_port != NULL) {
        sal_memset(vxlan_port, 0, sizeof(*vxlan_port));
        vxlan_port->vnid = 0xffffffff;
    }
    return;
}

/* 
 * Function:
 *      bcm_vxlan_vpn_config_t_init
 * Purpose:
 *      Initialize VXLAN  vpn config struct
 * Parameters: 
 *      vxlan_vpn - Pointer to the struct to be init'ed
 */
void
bcm_vxlan_vpn_config_t_init(bcm_vxlan_vpn_config_t *vxlan_vpn)
{
    if (vxlan_vpn != NULL) {
        sal_memset(vxlan_vpn, 0, sizeof(*vxlan_vpn));
    }
    return;
}

/* 
 * Function:
 *      bcm_vxlan_network_domain_config_t_init
 * Purpose:
 *      Initialize VXLAN  network domain config struct
 * Parameters: 
 *      config - Pointer to the struct to be init'ed
 */
void
bcm_vxlan_network_domain_config_t_init(bcm_vxlan_network_domain_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
    return;
}


#else
typedef int _bcm_vxlan_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */


