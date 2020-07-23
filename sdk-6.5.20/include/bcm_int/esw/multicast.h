/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains multicast definitions internal to the BCM library.
 */

#ifndef _BCM_INT_ESW_MULTICAST_H_
#define _BCM_INT_ESW_MULTICAST_H_


#define _BCM_VIRTUAL_TYPES_MASK                                 \
        (BCM_MULTICAST_TYPE_VPLS | BCM_MULTICAST_TYPE_MIM |      \
         BCM_MULTICAST_TYPE_SUBPORT | BCM_MULTICAST_TYPE_TRILL | \
         BCM_MULTICAST_TYPE_WLAN | BCM_MULTICAST_TYPE_VLAN | \
         BCM_MULTICAST_TYPE_NIV)

#define BCM_MULTICAST_PORT_MAX  4096

extern int _bcm_esw_multicast_flags_to_group_type(uint32 flags);
extern uint32 _bcm_esw_multicast_group_type_to_flags(int group_type);

extern int _bcm_tr_multicast_ipmc_group_type_set(int unit,
                                                 bcm_multicast_t group);
extern int _bcm_tr_multicast_ipmc_group_type_get(int unit, uint32 ipmc_id,
                                                 bcm_multicast_t *group);

extern int _bcm_esw_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                                     int nh_index, int is_l3);
extern int _bcm_esw_ipmc_egress_intf_delete(int unit, int ipmc_id,
                                           bcm_port_t port, int if_max,
                                           int nh_index, int is_l3);
extern int _bcm_esw_multicast_ipmc_read(int unit, int ipmc_id, 
                                        bcm_pbmp_t *l2_pbmp, bcm_pbmp_t *l3_pbmp);
extern int _bcm_esw_multicast_ipmc_write(int unit, int ipmc_id, 
                                         bcm_pbmp_t l2_pbmp, bcm_pbmp_t l3_pbmp, 
                                         int valid);
extern int bcmi_multicast_multi_count_set(int unit, bcm_multicast_t group,
                                          int count);
extern int bcmi_multicast_multi_count_get(int unit, bcm_multicast_t group,
                                          int *count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_multicast_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_multicast_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif	/* !_BCM_INT_ESW_MULTICAST_H_ */
