/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/vswitch.h>
#include <bcm/stg.h>
#include <soc/mem.h>

/* Initialize the bcm_vswitch_cross_connect_t struct. */
void bcm_vswitch_cross_connect_t_init(bcm_vswitch_cross_connect_t *cross_connect){
	cross_connect->encap1 = BCM_FORWARD_ENCAP_ID_INVALID;
	cross_connect->encap2 = BCM_FORWARD_ENCAP_ID_INVALID;
	cross_connect->flags = 0;
	cross_connect->port1 = 0;
	cross_connect->port2 = 0;
}

void bcm_vswitch_flexible_connect_fwd_t_init(
    bcm_vswitch_flexible_connect_fwd_t *vswitch_flexible_connect_fwd) 
{
  vswitch_flexible_connect_fwd->forward_port = BCM_GPORT_INVALID;
  vswitch_flexible_connect_fwd->forward_encap = BCM_FORWARD_ENCAP_ID_INVALID;
}


void bcm_vswitch_flexible_connect_match_t_init(
    bcm_vswitch_flexible_connect_match_t *flexible_connect_match)
{
    if (flexible_connect_match != NULL) {
        sal_memset(flexible_connect_match, 0, sizeof(*flexible_connect_match));
    }
}
