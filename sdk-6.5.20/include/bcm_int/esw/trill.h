/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains TRILL definitions internal to the BCM library.
 */

#include <bcm/types.h>
#include <bcm/trill.h>

#ifndef _BCM_INT_TRILL_H
#define _BCM_INT_TRILL_H

#ifdef INCLUDE_L3

#define BCM_MAX_NUM_TRILL_TREES 16

#define BCM_TRILL_ACCESS_PORT 0x01
#define BCM_TRILL_NETWORK_PORT 0x02
#define _BCM_TRILL_PORT_MULTIPATH                (1 << 30)   /* Associated Multipath Next-hop */

typedef struct _bcm_td_trill_multicast_count_s {
    bcm_multicast_t  l3mc_group;     /* TRILL Domain Multicast Group */
    uint16  network_port_count; /* TRILL Port count */
} _bcm_td_trill_multicast_count_t;

/*
 * Software book keeping for Trill  related information
 */
typedef struct _bcm_td_trill_bookkeeping_s {
    int         initialized;        /* Set to TRUE when Trill module initialized */
    sal_mutex_t    trill_mutex;    /* Protection mutex. */
    bcm_trill_name_t    rootBridge[BCM_MAX_NUM_TRILL_TREES]; /* Root Bridge Store */
    bcm_trill_name_t    *rBridge;    /* RBridges Store */
    _bcm_td_trill_multicast_count_t      *multicast_count; /* Multicast Store */
} _bcm_td_trill_bookkeeping_t;

extern _bcm_td_trill_bookkeeping_t *_bcm_td_trill_bk_info[BCM_MAX_NUM_UNITS];

#define     L2_TRILL_MEM_CHUNKS_DEFAULT   100

typedef struct _bcm_td_trill_multicast_entry_traverse_s {
    bcm_trill_multicast_entry_traverse_cb          user_cb;    /* User callback function */
    void                        *user_data; /* Data provided by the user, cookie */
}_bcm_td_trill_multicast_entry_traverse_t;

/* Generic memory allocation routine. */
#define BCM_TD_TRILL_ALLOC(_ptr_,_size_,_descr_)                      \
            do {                                                     \
                if ((NULL == (_ptr_))) {                             \
                   _ptr_ = sal_alloc((_size_),(_descr_));            \
                }                                                    \
                if((_ptr_) != NULL) {                                \
                    sal_memset((_ptr_), 0, (_size_));                \
                }                                                    \
            } while (0)

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_trill_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_td_trill_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
extern int _bcm_esw_trill_trunk_member_add(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array) ;
extern int _bcm_esw_trill_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
        int trunk_member_count, bcm_port_t *trunk_member_array);
void bcm_td_trill_tree_profile_get (int unit, bcm_trill_name_t trill_name, 
                                    uint8 *trill_tree_profile_idx);
void bcm_td_trill_root_name_get (int unit, uint8 trill_tree_profile_idx, bcm_trill_name_t *trill_name);

#if (defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT))
extern int bcm_td_trill_tpid_add(int unit, bcm_gport_t port, int tpid_idx);
extern int bcm_td_trill_tpid_set(int unit, bcm_gport_t port, int tpid_idx);
extern int bcm_td_trill_tpid_get(int unit, bcm_gport_t port, int *tpid_idx);
extern int bcm_td_trill_tpid_delete(int unit, bcm_gport_t port, int tpid_idx);
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
extern int _bcm_tr3_trill_transit_entry_set(int unit,
                            bcm_trill_port_t *trill_port,int nh_ecmp_index);
extern int _bcm_tr3_trill_transit_entry_reset(int unit, int vp,
                                              int nh_ecmp_idx, int ecmp);
extern int _bcm_tr3_trill_learn_entry_set(int unit,
                                    bcm_trill_port_t *trill_port, int vp);
extern int _bcm_tr3_trill_learn_entry_reset(int unit, int vp);
extern int _bcm_tr3_trill_decap_entry_set(int unit,
                                    bcm_trill_port_t *trill_port);
extern int bcm_tr3_trill_multicast_transit_entry_get(int unit, 
                      bcm_trill_name_t root_name, bcm_multicast_t *group);
extern int _bcm_tr3_trill_decap_entry_reset(int unit,  int vp);
extern int bcm_tr3_trill_multicast_transit_entry_set(int unit, uint32 flags,
                                    bcm_trill_name_t root_name, int mc_index, uint8 trill_tree_id);
extern int bcm_tr3_trill_multicast_transit_entry_reset(int unit,
                                    bcm_trill_name_t root_name);
extern int bcm_tr3_trill_multicast_transit_entry_reset_all (int unit,
                                    uint8 tree_id);
extern int bcm_tr3_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name);
extern int bcm_tr3_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name);
extern int  bcm_tr3_trill_l2_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int  bcm_tr3_trill_l2_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int bcm_tr3_trill_vlan_multicast_entry_modify (int unit, bcm_vlan_t vlan, int set_bit);
extern int  bcm_tr3_trill_l2_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id);
extern int bcm_tr3_trill_l2_multicast_entry_delete_all (int unit, uint8 tree_id);
extern int bcm_tr3_trill_l2_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group);
extern int bcm_tr3_trill_l2_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int bcm_tr3_trill_l2_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int bcm_tr3_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);

#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
extern int bcm_td2_trill_l2_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int bcm_td2_trill_vlan_multicast_entry_modify (int unit, bcm_vlan_t vlan, int set_bit);
extern int bcm_td2_trill_l2_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int bcm_td2_trill_l2_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id);
extern int bcm_td2_trill_l2_multicast_entry_delete_all (int unit, uint8 tree_id);
extern int bcm_td2_trill_l2_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group);
extern int bcm_td2_trill_l2_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int bcm_td2_trill_l2_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);

#endif /* BCM_TRIDENT2_SUPPORT */

#endif	/* INCLUDE_L3 */
#endif	/* !_BCM_INT_TRILL_H */
