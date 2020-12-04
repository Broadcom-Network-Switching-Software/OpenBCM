/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains FCOE definitions internal to the BCM library.
 */

#ifndef _BCM_INT_FCOE_H
#define _BCM_INT_FCOE_H

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/fcoe.h>

extern int bcm_esw_fcoe_init_check(int unit);

/* Get flex counter for FCOE VSAN ID */                          
extern int 
_bcm_esw_fcoe_vsan_stat_counter_get (int                  unit, 
                                     int                  sync_mode, 
                                     bcm_fcoe_vsan_id_t   vsan,
                                     bcm_fcoe_vsan_stat_t stat, 
                                     uint32               num_entries, 
                                     uint32               *counter_indexes, 
                                     bcm_stat_value_t     *counter_values);

/* Get flex counter for FCOE Route */                                
extern int 
_bcm_esw_fcoe_route_stat_counter_get (int                   unit, 
                                      int                   sync_mode, 
                                      bcm_fcoe_route_t      *route,
                                      bcm_fcoe_route_stat_t stat, 
                                      uint32                num_entries, 
                                      uint32                *counter_indexes, 
                                      bcm_stat_value_t      *counter_values);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_fcoe_sync(int unit);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_fcoe_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif	/* !_BCM_INT_FCOE_H */
