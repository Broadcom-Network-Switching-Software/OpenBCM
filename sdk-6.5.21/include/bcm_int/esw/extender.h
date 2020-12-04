/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Extender definitions internal to the BCM library.
 */

#ifndef _BCM_INT_EXTENDER_H
#define _BCM_INT_EXTENDER_H

#if defined(INCLUDE_L3)

extern int _bcm_esw_extender_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_extender_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_extender_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);
extern int _bcm_esw_extender_port_match_add(
               int unit, bcm_gport_t port, uint16 extended_port_vid,
               bcm_vlan_t vlan, int vp);
extern int _bcm_esw_extender_port_match_delete(
               int unit, bcm_gport_t port, uint16 extended_port_vid,
               bcm_vlan_t vlan, int vp);
extern int bcm_tr3_extender_port_learn_set(
               int unit, bcm_gport_t extender_port_id, uint32 flags);
extern int bcm_tr3_extender_port_learn_get(
               int unit, bcm_gport_t extender_port_id, uint32 *flags);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_extender_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_extender_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */

#endif	/* !_BCM_INT_EXTENDER_H */
