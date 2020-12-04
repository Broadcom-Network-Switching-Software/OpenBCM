/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef FCMAP_CMN_H
#define FCMAP_CMN_H

#if defined(INCLUDE_FCMAP)

#include <bcm/fcmap.h>

extern int 
_bcm_common_fcmap_init(int unit);

extern int 
bcm_common_fcmap_port_traverse(int unit, 
                             bcm_fcmap_port_traverse_cb callback, 
                             void *user_data);
extern int
bcm_common_fcmap_port_config_set(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg);

extern int
bcm_common_fcmap_port_config_get(int unit, bcm_port_t port, 
                               bcm_fcmap_port_config_t *cfg);

extern int
bcm_common_fcmap_port_speed_set(int unit, bcm_port_t port, 
                               bcm_fcmap_port_speed_t speed);

extern int
bcm_common_fcmap_port_enable(int unit, bcm_port_t port);


extern int
bcm_common_fcmap_port_shutdown(int unit, bcm_port_t port);


extern int
bcm_common_fcmap_port_link_reset(int unit, bcm_port_t port);

extern int
bcm_common_fcmap_vlan_map_add(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan);

extern int
bcm_common_fcmap_vlan_map_get(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan);

extern int
bcm_common_fcmap_vlan_map_delete(int unit, bcm_port_t port, 
                               bcm_fcmap_vlan_vsan_map_t *vlan);


extern int 
bcm_common_fcmap_stat_clear(int unit, bcm_port_t port);

extern int 
bcm_common_fcmap_stat_get(int unit, bcm_port_t port, 
                        bcm_fcmap_stat_t stat, 
                        uint64 *val);

extern int 
bcm_common_fcmap_stat_get32(int unit, bcm_port_t port, 
                          bcm_fcmap_stat_t stat,
                          uint32 *val);


extern int 
bcm_common_fcmap_event_register(int unit, bcm_fcmap_event_cb cb, void *user_data);

extern int 
bcm_common_fcmap_event_unregister(int unit, bcm_fcmap_event_cb cb);

extern int
bcm_common_fcmap_event_enable_set(int unit,
                                bcm_fcmap_event_t event, int enable);

extern int
bcm_common_fcmap_event_enable_get(int unit,
                                bcm_fcmap_event_t event, int *enable);

/*
 * Debug/Print config set/get.
 */
extern int bcm_common_fcmap_config_print(uint32 level);

extern int
bcm_common_fcmap_linkfault_trigger_rc_get(int unit, bcm_port_t port, 
                                          bcm_fcmap_lf_tr_t *lf_trigger, bcm_fcmap_lf_rc_t *lf_rc);

extern int
bcm_common_fcmap_diag_get(int unit, bcm_port_t port, 
                          bcm_fcmap_diag_code_t *diag);

/*
 * BFCMAP Port Speed Ability Set
 * Sets discrete port speeds for AN or single forced port speed
 */

extern int bcm_common_fcmap_port_ability_advert_set(int unit, bcm_port_t port, 
                                                    bcm_fcmap_port_ability_t *ability_mask);

/*
 * BFCMAP Port Speed Ability Get
 * Retrieves supported port speeds 
 */

extern int bcm_common_fcmap_port_ability_advert_get(int unit, bcm_port_t port, 
                                                    bcm_fcmap_port_ability_t *ability_mask);

#endif /* defined(INCLUDE_FCMAP) */

#endif /* FCMAP_CMN_H */

