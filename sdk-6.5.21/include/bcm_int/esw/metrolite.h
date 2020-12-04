/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        metrolite.h
 * Purpose:     Function declarations for metrolite bcm functions
 */

#ifndef _BCM_INT_METROLITE_H_
#define _BCM_INT_METROLITE_H_
#if defined(BCM_METROLITE_SUPPORT)
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm/qos.h>
#include <bcm/failover.h>
#include <bcm/sat.h>
#if defined(INCLUDE_L3)

#endif
extern int _bcm_ml_port_lanes_set_post_operation(int unit, bcm_port_t port);
extern int _bcm_ml_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_ml_port_lanes_get(int unit, bcm_port_t port, int *value);

extern int _bcm_ml_port_sw_info_display(int unit, bcm_port_t port);
extern int _bcm_ml_port_hw_info_display(int unit, bcm_port_t port);

extern int bcm_ml_oam_opcodes_count_profile_create( int unit, uint8 *lm_count_profile);

extern int bcm_ml_oam_opcodes_count_profile_set(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap); 

extern int bcm_ml_oam_opcodes_count_profile_get(
        int unit, 
        uint8 lm_count_profile, 
        uint8 count_enable, 
        bcm_oam_opcodes_t *opcodes_bitmap); 

extern int bcm_ml_oam_opcodes_count_profile_delete(
        int unit, 
        uint8 lm_count_profile);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP /* BCM_WARM_BOOT_SUPPORT_SW_DUMP*/

#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_WARM_BOOT_SUPPORT /* BCM_WARM_BOOT_SUPPORT */

#endif/* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_METROLITE_SUPPORT */
#endif  /* !_BCM_INT_METROLITE_H_ */

