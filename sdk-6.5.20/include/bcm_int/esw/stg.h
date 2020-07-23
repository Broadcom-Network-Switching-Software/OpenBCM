/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains STG definitions internal to the BCM library.
 */

#ifndef _BCM_INT_STG_H
#define _BCM_INT_STG_H

extern int _bcm_stg_stp_translate(int unit, int bcm_state, int *pvp_state);
extern int _bcm_stg_pvp_translate(int unit, int pvp_state, int *bcm_state);
extern int _bcm_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid);
extern int bcm_esw_stg_detach(int unit);

#define BCM_LYNX_VLANS_PER_MSTP_ENTRY   16

/* MSTP mask support */
#define _BCM_STG_STP_TO_MSTP_MASK_ENABLE(stp_state) \
        (((stp_state) != BCM_STG_STP_DISABLE) && \
        ((stp_state) != BCM_STG_STP_BLOCK))

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_stg_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_stg_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif	/* !_BCM_INT_STG_H */
