/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_TOPO_H__
#define __BCM_TOPO_H__

#include <bcm/port.h>

/* Topology mapping lookup function prototype. */
typedef int (*bcm_topo_map_f)(
    int unit, 
    int dest_modid, 
    bcm_port_t *exit_port);

/* Set or Get Topology mapping lookup function. */
extern int bcm_topo_map_get(
    bcm_topo_map_f *_map);

/* Set or Get Topology mapping lookup function. */
extern int bcm_topo_map_set(
    bcm_topo_map_f _map);

#ifndef BCM_HIDE_DISPATCHABLE

/* Lookup a path to a destination module. */
extern int bcm_topo_port_get(
    int unit, 
    int dest_modid, 
    bcm_port_t *exit_port);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_TOPO_H__ */
