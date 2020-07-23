/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains NIV definitions internal to the BCM library.
 */

#ifndef _BCM_INT_NIV_H
#define _BCM_INT_NIV_H

#if defined(INCLUDE_L3)

extern int _bcm_esw_niv_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_niv_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp);
extern int _bcm_esw_niv_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_niv_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_niv_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

int _bcm_niv_check_init(int unit);

#define BCM_NIV_GET_ONE_EGR_OBJECT  -1

#endif /* INCLUDE_L3 */

#endif	/* !_BCM_INT_NIV_H */
