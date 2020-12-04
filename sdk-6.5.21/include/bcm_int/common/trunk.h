/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains trunk definitions internal to the BCM library.
 */

#ifndef _BCM_COMMON_TRUNK_H
#define _BCM_COMMON_TRUNK_H

#include <bcm/trunk.h>

typedef struct bcm_trunk_add_info_s {
    uint32 flags;                       /* BCM_TRUNK_FLAG_xxx. */
    int num_ports;                      /* Number of ports in the trunk group. */
    int psc;                            /* Port selection criteria. */
    int ipmc_psc;                       /* Port selection criteria for software
                                           IPMC trunk resolution. */
    int dlf_index;                      /* DLF/broadcast port for trunk group. */
    int mc_index;                       /* Multicast port for trunk group. */
    int ipmc_index;                     /* IPMC port for trunk group. */
    uint32 member_flags[BCM_TRUNK_MAX_PORTCNT]; /* BCM_TRUNK_MEMBER_xxx */
    bcm_port_t tp[BCM_TRUNK_MAX_PORTCNT]; /* Ports in trunk. */
    bcm_module_t tm[BCM_TRUNK_MAX_PORTCNT]; /* Modules per port. */
    uint32 dynamic_size;                /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    uint32 dynamic_age;                 /* Inactivity duration, in microseconds. */
    uint32 dynamic_load_exponent;       /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    uint32 dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
} bcm_trunk_add_info_t;

extern void bcm_trunk_add_info_t_init(bcm_trunk_add_info_t *add_info);

#endif	/* !_BCM_COMMON_TRUNK_H */
