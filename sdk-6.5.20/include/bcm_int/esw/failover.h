/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Failover module definitions internal to the BCM library.
 */
#ifndef _BCM_INT_ESW_FAILOVER_H_
#define _BCM_INT_ESW_FAILOVER_H_

#if defined(INCLUDE_L3)

#define _BCM_FAILOVER_1_PLUS_PROTECTION  0x1
#define _BCM_FAILOVER_DEFAULT_MODE  0x0
#define _BCM_FAILOVER_1_1_MC_PROTECTION_MODE  0x2
#define _BCM_FAILOVER_INGRESS  0x4
#define _BCM_FAILOVER_MULTI_LEVEL  0x8
#define _BCM_FAILOVER_ING_MC_PROTECTION_MODE  0x10


#define FAILOVER_TYPE_SHIFT 24
#define FAILOVER_TYPE_MASK  0xff000000 
#define FAILOVER_ID_MASK    0x00ffffff 

#define _BCM_ENCAP_TYPE_IN_FAILOVER_ID(failover_id, type) \
    (failover_id = ((type << FAILOVER_TYPE_SHIFT) | (failover_id)))  

#define _BCM_GET_FAILOVER_ID(failover_id) \
    failover_id = failover_id &  FAILOVER_ID_MASK  

#define _BCM_GET_FAILOVER_TYPE(failover_id, type) \
    type = ((failover_id & FAILOVER_TYPE_MASK) >> FAILOVER_TYPE_SHIFT)  

#define _BCM_FAILOVER_IS_MULTI_LEVEL(failover_id) \
    (((failover_id & FAILOVER_TYPE_MASK) >> FAILOVER_TYPE_SHIFT) == \
         _BCM_FAILOVER_MULTI_LEVEL)

#define _FAILOVER_FIXED_NH_OFFSET_ON 0x80000000
#define _BCM_FAILOVER_ID_HAS_FIXED_NH_OFFSET(failover_id) \
    ((failover_id & _FAILOVER_FIXED_NH_OFFSET_ON) == _FAILOVER_FIXED_NH_OFFSET_ON)

/*
 * Software book keeping for Failover related information
 */
typedef struct _bcm_failover_bookkeeping_s {
    int         initialized;        /* Set to TRUE when Failover module initialized */
    int         prot_offset;        /* TH3 fixed protection offset (0 when disable) */
    SHR_BITDCL  *prot_group_bitmap;       /* INITIAL_PROT_GROUP usage bitmap */
    SHR_BITDCL  *prot_nhi_bitmap;       /* INITIAL_PROT_NHI usage bitmap */
    SHR_BITDCL  *mmu_prot_group_bitmap;       /* MMU_PROT_GROUP usage bitmap */
    SHR_BITDCL  *egress_prot_group_bitmap;       /* EGRESS_PROT_GROUP usage bitmap */
    SHR_BITDCL  *ingress_prot_group_bitmap; /* RX_PROT_GROUP usage bitmap */
    SHR_BITDCL  *multi_level_failover_bitmap; /* multi level failover bitmap */
    int         *init_prot_to_multi_failover_id; /* multi failover ids are only in s/w
                                                  * therefore we need to keep track of
                                                  * them to return ids back to user 
                                                  * in case of get and traverse */
    sal_mutex_t    failover_mutex;			  /* Protection mutex. */
} _bcm_failover_bookkeeping_t;

#define FAILOVER_INFO(_unit_)   (&_bcm_failover_bk_info[_unit_])

#define BCM_MULTI_LEVEL_FAILOVER_BITMAP(_u_)       (FAILOVER_INFO(_u_)->multi_level_failover_bitmap)
#define BCM_MULTI_LEVEL_FAILOVER_ID_MAP(_u_)       (FAILOVER_INFO(_u_)->init_prot_to_multi_failover_id)
#define BCM_FAILOVER_PROT_GROUP_MAP(_u_)           (FAILOVER_INFO(_u_)->prot_group_bitmap)
#define BCM_FAILOVER_INGRESS_PROT_GROUP_MAP(_u_)   (FAILOVER_INFO(_u_)->ingress_prot_group_bitmap)

/*
 * Multi level protection status bits.
 */
#define LVL_1_PROT_ENABLE    6
#define LVL_1_PROT_MODE      5
#define LVL_2_PROT_ENABLE    4
#define LVL_2_PROT_MODE      3
#define LVL_2_1_REP_ENABLE   2
#define LVL_2_0_REP_ENABLE   1
#define LVL_1_REP_ENABLE     0

/*
 * Software structure for bookkeeping multi level failovers.
 */
typedef struct bcmi_failover_multi_level_info_s {
    bcm_failover_t parent_failover_id;
    uint32 parent_failover_type;
    bcm_failover_t child_group_1;
    bcm_failover_t child_group_2;
}bcmi_failover_multi_level_info_t;

/*
 * INITIAL_PROT_GROUP  usage bitmap operations
 */
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_GROUP_MAP_IS_VALID(_u_) \
        (FAILOVER_INFO((_u_))->prot_group_bitmap != NULL)     

#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->mmu_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->mmu_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_MMU_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->mmu_prot_group_bitmap, (_map_))


/*
 * EGR_TX_PROT_GROUP  usage bitmap operations
 */
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->egress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->egress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_EGRESS_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->egress_prot_group_bitmap, (_map_))

/*
 * RX_PROT_GROUP usage bitmap operations
 */
#define _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->ingress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->ingress_prot_group_bitmap, (_map_))
#define _BCM_FAILOVER_INGRESS_PROT_GROUP_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->ingress_prot_group_bitmap, (_map_))

/*
 * INITIAL_PROT_NHI  usage bitmap operations
 */
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->prot_nhi_bitmap, (_map_))
#define _BCM_FAILOVER_PROT_NHI_MAP_IS_VALID(_u_) \
        (FAILOVER_INFO((_u_))->prot_nhi_bitmap != NULL)        

/*
 * RX_PROT_GROUP usage bitmap operations
 */
#define _BCM_FAILOVER_MULTI_LEVEL_MAP_USED_GET(_u_, _map_) \
        SHR_BITGET(FAILOVER_INFO(_u_)->multi_level_failover_bitmap, (_map_))
#define _BCM_FAILOVER_MULTI_LEVEL_MAP_USED_SET(_u_, _map_) \
        SHR_BITSET(FAILOVER_INFO((_u_))->multi_level_failover_bitmap, (_map_))
#define _BCM_FAILOVER_MULTI_LEVEL_MAP_USED_CLR(_u_, _map_) \
        SHR_BITCLR(FAILOVER_INFO((_u_))->multi_level_failover_bitmap, (_map_))

/*
 * multi level failover id to nh_id get/set macros.
 */
#define _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_SET(_u_, nh_id, failover_id) \
        ((FAILOVER_INFO(_u_)->init_prot_to_multi_failover_id[nh_id]) = failover_id)
#define _BCM_FAILOVER_MULTI_LEVEL_FAILOVER_ID_GET(_u_, nh_id, failover_id) \
        (failover_id = (FAILOVER_INFO(_u_)->init_prot_to_multi_failover_id[nh_id]))

extern _bcm_failover_bookkeeping_t  _bcm_failover_bk_info[BCM_MAX_NUM_UNITS]; 

extern int _bcm_esw_failover_id_check(int unit, bcm_failover_t failover_id);
extern int _bcm_esw_failover_egr_check(int unit, bcm_l3_egress_t  *egr);
extern int _bcm_esw_failover_prot_nhi_create(int unit, uint32 flags, int nh_index, int prot_nh_index,
                             bcm_multicast_t mc_group, bcm_failover_t failover_id);
extern int _bcm_esw_failover_prot_nhi_cleanup(int unit, int nh_index);
extern int _bcm_esw_failover_prot_nhi_get (int unit, int nh_index,
         bcm_failover_t *failover_id, int *prot_nh_index, int *multicast_group);
extern int _bcm_esw_failover_mpls_check(int unit, bcm_mpls_port_t  *mpls_port);
extern int _bcm_esw_failover_prot_nhi_update (int unit, int old_nh_index, int new_nh_index);
extern int _bcm_esw_failover_extender_check(int unit, bcm_extender_port_t *extender_port);
extern int _bcm_esw_failover_ecmp_prot_nhi_create(int unit, int ecmp, int index, 
                    int nh_index, bcm_failover_t failover_id, int prot_nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_cleanup(int unit, int ecmp, int index, int nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_get(int unit, int ecmp, int index, 
                    int nh_index, bcm_failover_t *failover_id, int *prot_nh_index);
extern int _bcm_esw_failover_ecmp_prot_nhi_update(int unit, int ecmp, int index, 
                    int old_nh_index, int new_nh_index);


extern void bcmi_esw_failover_dependent_free_resources(int unit);
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_tr2_failover_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcm_esw_failover_sync(int unit);

#endif
#endif /* INCLUDE_L3 */

#endif /* !_BCM_INT_ESW_FAILOVER_H_ */
