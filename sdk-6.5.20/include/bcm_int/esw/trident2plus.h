/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident2plus.h
 * Purpose:     Function declarations for Trident2+ Internal functions.
 */

#ifndef _BCM_INT_TRIDENT2PLUS_H_
#define _BCM_INT_TRIDENT2PLUS_H_

#if defined(BCM_TRIDENT2PLUS_SUPPORT)

#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l3.h>
#include <bcm/failover.h>
#include <bcm/rx.h>
#include <bcm/mpls.h>
#include <bcm/l3.h>
#include <bcm/oam.h>



#define TD2P_AGG_ID_INVALID (-1)
#define TD2P_AGG_ID_HW_INVALID (0x7F)
/* Number of protection switching groups/next hops
 * represented per entry in the PROT_SW tables
 * (INITIAL_PROT_NHI_TABLE_1, TX_INITIAL_PROT_GROUP_TABLE,
 * RX_PROT_GROUP_TABLE, EGR_TX_PROT_GROUP_TABLE and
 * EGR_L3_NEXT_HOP_1)
 */
#define BCM_TD2P_MPLS_PS_NUM_GROUPS_PER_ENTRY 128

#define BCM_TD2P_COE_MODULE_1 1
#define BCM_TD2P_COE_MODULE_2 2
#define BCM_TD2P_COE_MODULE_3 3
#define BCM_TD2P_COE_MODULE_4 4
#define BCM_TD2P_COE_MODULE_5 5

#define BCM_TD2P_ENTROPY_LABEL_ID_MAX          (0xfffff)
#define BCM_TD2P_ENTROPY_LABEL_ID_VALID(_label_)                        \
             (_label_ <= BCM_TD2P_ENTROPY_LABEL_ID_MAX)

#define BCM_TD2P_ENTROPY_LABEL_MASK_MAX        (0xfffff)
#define BCM_TD2P_ENTROPY_LABEL_MASK_VALID(_mask_)                       \
             ((_mask_ > 0) && (_mask_ <= BCM_TD2P_ENTROPY_LABEL_MASK_MAX))

/* Corresponds of maximum logical number for the chip */
#define TD2P_MAX_NUM_PORTS 106

#define BCM_PORT_IS_LOCAL_PHYSICAL(port) \
        (!BCM_GPORT_IS_SET(port) || \
        BCM_GPORT_IS_LOCAL(port) || \
        BCM_GPORT_IS_MODPORT(port))

extern int bcm_td2p_vp_group_init(int unit);
extern int bcm_td2p_vp_group_detach(int unit);
extern int bcm_td2p_vp_vlan_member_set(int unit, bcm_gport_t gport, uint32 flags);
extern int bcm_td2p_vp_vlan_member_get(int unit, bcm_gport_t gport, int ingress, uint32 *flags);
extern int bcm_td2p_vp_group_delete_all(int unit, bcm_vlan_t vlan_vpn, int egress);
extern int bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);

extern int bcm_td2p_egr_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td2p_ing_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_egr_vp_group_unmanaged_get(int unit);
extern int bcm_td2p_vlan_vp_group_set(int unit, int vlan_vpn, int egress,
                                    int vp_group,  int enable);
extern int bcm_td2p_vlan_vp_group_get(int unit, int vlan_vpn, int egress,
                                    int vp_group,  int *enable);
extern int bcm_td2p_vp_group_ut_state_set(int unit, int egr_vlan,
                                    int vp_group, int ut_state);
extern int bcm_td2p_vp_group_ut_state_get(int unit, bcm_port_t gport,
                                        bcm_vlan_t vlan, int *ut_state);
extern int bcm_td2p_vlan_vp_group_get_all(int unit, int vlan, int array_max,
             bcm_gport_t *gport_array, int *flags_array, int *port_cnt);


extern int bcm_td2p_vp_group_port_move(int unit, int gport, bcm_vlan_t vlan_vpn,
                                       int add, int egress, int flags);
extern int bcm_td2p_vp_group_vlan_vpn_stg_set(int unit,
                                        int vlan_vpn, int egress, int stg);
extern int bcm_td2p_vp_group_vlan_vpn_stg_get(int unit,
                                        int vlan_vpn, int egress, int *stg);



#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcmi_xgs5_subport_wb_scache_alloc(int unit, uint16 version,
                                           int *alloc_sz);
extern int bcmi_xgs5_subport_wb_sync(int unit, uint16 version,
                                   uint8 **scache_ptr);
extern int bcmi_xgs5_subport_wb_recover(int unit, uint16 version,
                                      uint8 **scache_ptr);
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td2p_vp_group_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#ifdef INCLUDE_L3
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td2p_ipmc_aggid_info_scache_size_get(int unit, uint32 *size);
extern int bcm_td2p_ipmc_aggid_info_sync(int unit, uint8 **scache_ptr);
extern int bcm_td2p_ipmc_aggid_info_recover(int unit, uint8 **scache_ptr);
#endif

/* IPMC replication per trunk function */
extern int bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member(int unit,
                                                                int repl_group,
                                                                bcm_port_t port,
                                                                int if_count,
                                                                bcm_if_t *if_array,
                                                                int is_l3,
                                                                int check_port,
                                                                bcm_trunk_t tgid);
extern int bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(int unit,
                                                              int repl_group,
                                                              bcm_port_t port,
                                                              bcm_port_t ori_port,
                                                              bcm_trunk_t tgid);
extern int bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode(int unit,
                                                           int repl_group,
                                                           bcm_port_t port,
                                                           int encap_id,
                                                           int is_l3);
extern int bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode(int unit,
                                                           int repl_group,
                                                           bcm_port_t port,
                                                           int if_max,
                                                           int encap_id,
                                                           int is_l3);

extern int bcm_td2p_ipmc_egress_intf_add_for_trunk(int unit, int repl_group,
                                                   bcm_trunk_t tgid,
                                                   int encap_id, int is_l3);
extern int bcm_td2p_ipmc_egress_intf_del_for_trunk(int unit, int repl_group,
                                                   bcm_trunk_t tgid,
                                                   int if_max, int encap_id,
                                                   int is_l3);
extern int bcm_td2p_ipmc_egress_intf_add_trunk_member(int unit, int repl_group,
                                                      bcm_port_t port);
extern int bcm_td2p_ipmc_egress_intf_del_trunk_member(int unit, int repl_group,
                                                      bcm_port_t port);

extern int  bcm_td2p_failover_init(int unit);
extern void bcm_td2p_failover_unlock(int unit);
extern int  bcm_td2p_failover_lock(int unit);
extern int  bcm_td2p_failover_init(int unit);
extern int  bcm_td2p_failover_cleanup(int unit);
extern int  bcm_td2p_failover_create(int unit, uint32 flags,
        bcm_failover_t *failover_id);
extern int  bcm_td2p_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int  bcm_td2p_failover_status_set(int unit,
        bcm_failover_element_t *failover, int value);
extern int  bcm_td2p_failover_status_get(int unit,
        bcm_failover_element_t *failover, int  *value);
extern int  bcm_td2p_failover_prot_nhi_set(int unit, uint32 flags,
        int nh_index, uint32 prot_nh_index, bcm_multicast_t  mc_group,
        bcm_failover_t failover_id);
extern int  bcm_td2p_failover_prot_nhi_get(int unit, int nh_index,
        bcm_failover_t  *failover_id, int  *prot_nh_index,
        bcm_multicast_t  *mc_group);
extern int  bcm_td2p_failover_prot_nhi_cleanup(int unit, int nh_index);
extern int  bcm_td2p_failover_prot_nhi_update(int unit, int old_nh_index,
        int new_nh_index);
extern int _bcm_td2p_failover_ingress_id_validate(int unit,
                                                  bcm_failover_t failover_id);
extern int bcm_td2p_dvp_vlan_xlate_key_set(int unit, 
        bcm_gport_t port_id, int key_type);
extern int bcm_td2p_dvp_vlan_xlate_key_get(int unit, 
        bcm_gport_t port_id, int *key_type);
extern void bcm_td2_vxlan_port_match_count_adjust(int unit, int vp, int step);
extern int bcm_td2p_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap);
extern int bcm_td2p_share_vp_delete(int unit, bcm_vpn_t vpn, int vp);

/* DVP Group ID check*/
extern int bcm_td2p_dvp_group_validate(int unit, int dvp_group_id);

extern int bcm_td2p_mpls_entropy_identifier_add(int unit, uint32 options,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_delete(int unit,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_delete_all(int unit);
extern int bcm_td2p_mpls_entropy_identifier_get(int unit,
        bcm_mpls_entropy_identifier_t *info);
extern int bcm_td2p_mpls_entropy_identifier_traverse(int unit,
        bcm_mpls_entropy_identifier_traverse_cb cb, void *user_data);

extern int bcm_td2p_l3_tables_init(int unit);
extern int bcm_td2p_l3_defip_init(int unit);
extern void bcm_td2p_l3_defip_deinit(int unit);
extern int bcm_td2p_defip_mc_route_check(int unit, _bcm_defip_cfg_t *lpm_cfg);
extern int bcm_td2p_defip_ipmc_count_update(int unit);

extern void bcmi_failover_dependent_free_resources(int unit);

#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)

/* ECMP Single level */
#define BCMI_L3_ECMP_SINGLE_LEVEL         1

/* Overlay validity checks */
#define BCMI_L3_ECMP_IS_MULTI_LEVEL(_u) \
    (((&_bcm_l3_bk_info[unit])->l3_ecmp_levels) > BCMI_L3_ECMP_SINGLE_LEVEL)

#define BCMI_L3_ECMP_OVERLAY_ENTRIES(_u) \
    ((&_bcm_l3_bk_info[_u])->l3_ecmp_overlay_entries)

/* FB6 : Valid sizes of ECMP group for multi-level ECMP */
#define BCMI_L3_ECMP_GROUP_FIRST_LKUP_MEM_SIZE_A 1024
#define BCMI_L3_ECMP_GROUP_FIRST_LKUP_MEM_SIZE_B 2048

#define BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(_u) \
    ((&_bcm_l3_bk_info[_u])->l3_ecmp_group_first_lkup_mem_size == \
                        BCMI_L3_ECMP_GROUP_FIRST_LKUP_MEM_SIZE_B)

#endif

/******************************************************************|
|**************                                   *****************|
|**************           RIOT BLOCK              *****************|
|**************                                   *****************|
 */

/* RIOT related defines */

#ifdef BCM_RIOT_SUPPORT


/* TD2+ : EGR_L3_NEXT_HOP-KEY_TYPEf */
#define BCMI_L3_EGR_NH_MCAST_ENTRY_TYPE  7
#define BCMI_L3_EGR_NH_SD_TAG_ENTRY_TYPE 2


/* L3 interface banks values */
#define BCMI_L3_INTF_ENT_PER_BANK(_u_)   ((SOC_IS_HELIX5(_u_)) ? 1024 : (SOC_IS_HURRICANE4(_u_) ? 256 :2048))
#define BCMI_L3_INTF_BANKS_MAX(_u_)       ((SOC_IS_HELIX5(_u_) || SOC_IS_HURRICANE4(_u_)) ? 8 : 6)

/* L3 next hop banks values */
#define BCMI_L3_NH_ENT_PER_BANK(_u_)     ((SOC_IS_HELIX5(_u_)) ? 2048 : \
                                         ((soc_feature(_u_, soc_feature_8k_l3_nh_per_bank)) ? 8192: 4096))
#define BCMI_L3_NH_BANKS_MAX             (SOC_IS_HURRICANE4(_u_) ? 8 : 12)

/* L3 ECMP banks values */
#define BCMI_L3_ECMP_GROUP_PER_BANK(_u_) ((SOC_IS_HELIX5(_u_)) ? 256 : (SOC_IS_HURRICANE4(_u_) ? 64 : 512))
#define BCMI_L3_ECMP_PER_BANK(_u_)       ((SOC_IS_HELIX5(_u_)) ? 1024 : (SOC_IS_HURRICANE4(_u_) ? 256 :4096))
#define BCMI_L3_ECMP_MAX_BANKS(_u_)      ((SOC_IS_HELIX5(_u_) || SOC_IS_HURRICANE4(_u_)) ? 4 : 8)

/* NH destination encodings, masks and macros */
#define BCMI_L3_NH_DEST_DGLP_SHIFT       15
#define BCMI_L3_NH_DEST_DGLP_MASK        0x7FFF   /* 15 bits value */
#define BCMI_L3_NH_DEST_DGLP_EN_MASK     0x7      /* 3 bits value */
#define BCMI_L3_NH_DEST_DGLP_ENCODING    0x0      /* 15 bits value */
#define BCMI_L3_NH_DEST_DVP_SHIFT        14       /* 14 bits value */
#define BCMI_L3_NH_DEST_DVP_MASK         0x3FFF   /* 14 bits value */
#define BCMI_L3_NH_DEST_DVP_EN_MASK      0xF      /* 4 bits value */
#define BCMI_L3_NH_DEST_DVP_ENCODING     0x3      /* 15 bits value */
#define BCMI_L3_NH_DEST_LAG_SHIFT        14       /* 14 bits value */
#define BCMI_L3_NH_DEST_LAG_MASK         0x3FFF   /* 14 bits value */
#define BCMI_L3_NH_DEST_LAG_EN_MASK      0xF      /* 4 bits value */
#define BCMI_L3_NH_DEST_LAG_ENCODING     0x2      /* 15 bits value */
#define BCMI_L3_NH_DEST_DROP_SHIFT       12       /* 12 bits value */
#define BCMI_L3_NH_DEST_DROP_MASK        0xFFF    /* 12 bits value */
#define BCMI_L3_NH_DEST_DROP_EN_MASK     0x3F     /* 6 bits value */
#define BCMI_L3_NH_DEST_DROP_ENCODING    0x1b     /* 15 bits value */
#define BCMI_L3_NH_DEST_IPMC_SHIFT       14       /* 14 bits value */
#define BCMI_L3_NH_DEST_IPMC_MASK        0x3FFF   /* 14 bits value */
#define BCMI_L3_NH_DEST_IPMC_EN_MASK     0xF      /* 4 bits value */
#define BCMI_L3_NH_DEST_IPMC_ENCODING    0x4      /* 15 bits value */


#define BCMI_L3_NH_DEST_DVP_GET(gport) \
            ((BCMI_L3_NH_DEST_DVP_ENCODING << BCMI_L3_NH_DEST_DVP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DVP_MASK))

#define BCMI_L3_NH_DEST_LAG_GET(gport) \
            ((BCMI_L3_NH_DEST_LAG_ENCODING << BCMI_L3_NH_DEST_LAG_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_LAG_MASK))

#define BCMI_L3_NH_DEST_DGLP_GET(gport) \
            ((BCMI_L3_NH_DEST_DGLP_ENCODING << BCMI_L3_NH_DEST_DGLP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DGLP_MASK))

#define BCMI_L3_NH_DEST_DROP_GET(gport) \
            ((BCMI_L3_NH_DEST_DROP_ENCODING << BCMI_L3_NH_DEST_DROP_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_DROP_MASK))

#define BCMI_L3_NH_DEST_IPMC_GET(gport) \
            ((BCMI_L3_NH_DEST_IPMC_ENCODING << BCMI_L3_NH_DEST_IPMC_SHIFT) | \
            (gport & BCMI_L3_NH_DEST_IPMC_MASK))

#define BCMI_L3_NH_DEST_IS_LAG(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_LAG_SHIFT) & BCMI_L3_NH_DEST_LAG_EN_MASK) == \
            BCMI_L3_NH_DEST_LAG_ENCODING)

#define BCMI_L3_NH_PORT_LAG_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_LAG_MASK))

#define BCMI_L3_NH_DEST_IS_DGLP(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_DGLP_SHIFT) & BCMI_L3_NH_DEST_DGLP_EN_MASK) == \
            BCMI_L3_NH_DEST_DGLP_ENCODING)
#define BCMI_L3_NH_PORT_DGLP_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_DGLP_MASK))

#define BCMI_L3_NH_DEST_IS_DVP(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_DVP_SHIFT) & BCMI_L3_NH_DEST_DVP_EN_MASK) == \
            BCMI_L3_NH_DEST_DVP_ENCODING)

#define BCMI_L3_NH_PORT_DVP_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_DVP_MASK))

#define BCMI_L3_NH_DEST_IS_IPMC(_dest)  \
            ((((_dest) >> BCMI_L3_NH_DEST_IPMC_SHIFT) & BCMI_L3_NH_DEST_IPMC_EN_MASK) == \
            BCMI_L3_NH_DEST_IPMC_ENCODING)

#define BCMI_L3_NH_PORT_IPMC_GET(_dest) \
            (((_dest) & BCMI_L3_NH_DEST_IPMC_MASK))

/* Td2+ : EGR_L3_NEXT_HOP-NEXT_PTR field types */
#define BCMI_L3_NH_EGR_NEXT_PTR_TYPE_DVP    1
#define BCMI_L3_NH_EGR_NEXT_PTR_TYPE_NH     2

/*
 * The below infrastructure is done specially for nh-nh association management.
 * When overlay nh is associated with underlay nh, we will have to keep
 * backward tracing to redraw association if underlay NH is changed.
 */
typedef struct ol_nh_link_s {
    int o_nh_idx;
    struct ol_nh_link_s *next_link;
} ol_nh_link_t;

typedef struct ul_nh_link_s {
    int u_nh_idx;
    ol_nh_link_t *ol_nh_assoc;
    struct ul_nh_link_s *ul_nh_link;
} ul_nh_link_t;

/* TD2P specific routines */
extern int bcmi_l3_riot_bank_sel(int unit);
extern int bcmi_get_port_from_destination(int unit,
               uint32 dest, bcm_l3_egress_t *nh);

/* ol-ul association routines */
extern int bcmi_l3_nh_assoc_ol_ul_link_sw_add(int unit,
               int ol_nh, int ul_nh);
extern int bcmi_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx);
extern int bcmi_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index);
extern int bcmi_l3_nh_assoc_ol_ul_link_replace(int unit,
               int old_ul, int new_ul);
extern int bcmi_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh,
               int ul_nh);
extern int bcmi_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx);
extern int bcmi_l3_nh_dest_set(int unit,
               bcm_l3_egress_t *nh_entry, uint32 *nh_dest);

extern bcm_error_t _bcm_td2p_macda_oui_profile_init(int unit);
extern void _bcm_td2p_macda_oui_profile_deinit(int unit);
extern bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_del(int unit, _bcm_l3_cfg_t *l3cfg,
                                         int macda_oui_profile_index);
extern bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_add(int unit, soc_mem_t mem, _bcm_l3_cfg_t *l3cfg,
                                         uint32 *buf_p, int *macda_oui_profile_index);
extern bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_replace(int unit, soc_mem_t mem,
                                             _bcm_l3_cfg_t *l3cfg, uint32 *bufp);
extern bcm_error_t
_bcm_td2p_l3_macda_oui_profile_entry_parse(int unit, soc_mem_t mem,
                                           _bcm_l3_cfg_t *l3cfg, void *l3x_entry);
extern bcm_error_t
_bcm_td2p_l3_get_macda_oui_profile_index(int unit, soc_mem_t mem_ext,
                                         _bcm_l3_cfg_t *l3cfg,
                                         int *macda_oui_profile_index,
                                         int *ref_count);
extern void bcm_td2p_l3_extended_required_scache_size_get(int unit, int *size);
extern bcm_error_t bcm_td2p_l3_extended_view_sync(int unit, uint8 **scache_ptr);
extern bcm_error_t bcm_td2p_l3_extended_view_reinit(int unit, uint8 **scache_ptr);

/******************************************************************|
|**************                                   *****************|
|**************    RIOT BLOCK - Ends Here         *****************|
|**************                                   *****************|
 */
#endif /* BCM_RIOT_SUPPORT */

/*
 * Failover functions
 */
/******************************************************************|
|**************                                   *****************|
|**************    Multi Level Failover           *****************|
|**************                                   *****************|
 */

int
bcmi_failover_sync(int unit);

int
bcmi_failover_multi_level_prot_nhi_create(int unit, int nh_index);
int
bcmi_failover_multi_level_prot_nhi_set(int unit, int index,
                uint32 prot_nh_index, bcm_multicast_t mc_group,
                bcm_failover_t failover_id);

int
bcmi_failover_multi_level_prot_nhi_cleanup(int unit, int nh_index);
int
bcmi_failover_multi_level_prot_nhi_get(int unit, int nh_index,
                bcm_failover_t *failover_id, int *prot_nh_index,
                bcm_multicast_t *mc_group);
int
bcmi_failover_multi_level_attach(int unit,
        bcm_failover_multi_level_t attach_info);
int
bcmi_failover_sync(int unit);

/*
 * Scale is changed from 16K for apache.
 */
#define BCMI_TX_PROT_GROUP_MASK(u)    \
    ((soc_feature(u, soc_feature_hierarchical_protection)) ? 0X7F : 0x7)

/***************     Multi Level Failover-Till Here    **************/

#endif /* INCLUDE_L3 */

/* VLAN VFI */
extern int bcm_td2p_vlan_vfi_mbrship_port_get(int unit,
                        bcm_vlan_t vid, int egress, pbmp_t *pbmp);
extern int bcm_td2p_vlan_vfi_mbrship_port_set(int unit,
                        bcm_vlan_t vid, int egress, int add, pbmp_t pbmp);
extern int bcm_td2p_vlan_vpn_membership_delete(int unit, bcm_vlan_t vlan_vpn);

extern int bcm_td2p_vp_group_stg_move(int unit, int stg, int port, int stp_state,
                           int egress);
extern int _bcm_vlan_vfi_membership_profile_entry_op(int unit,
                                                     void **entries,
                                                     int entries_per_set,
                                                     int del,
                                                     int egress,
                                                     uint32 *index);

extern int _bcm_vlan_vfi_membership_profile_mem_reference(int unit,
                                                          int index,
                                                          int entries_per_set,
                                                          int egress);
extern int bcm_td2p_vp_group_stp_init(int unit, int stg, int egress, int stp_state);
extern int bcm_td2p_vp_group_stp_state_set(int unit, int stg, int gport,
                                                    int egress, int stp_state);
extern int bcm_td2p_vp_group_stp_state_get(int unit, int stg, int gport, int egress,
                                     int *stp_state);

extern int _bcm_vlan_vfi_mbrship_profile_ref_count(int unit, int index,
                                                   int egress, int *ref_count);

extern int bcmi_td2p_vlan_control_vpn_get(int unit, bcm_vlan_t vid,
               uint32 valid_fields, bcm_vlan_control_vlan_t *control);
extern int bcmi_td2p_vlan_control_vpn_set(int unit, bcm_vlan_t vid,
               uint32 valid_fields, bcm_vlan_control_vlan_t *control);

/* Subport functions */
extern int bcm_td2plus_subport_coe_init(int unit);

extern int bcm_td2p_cosq_ets_mode(int unit);
extern int bcm_td2p_rx_queue_channel_set(int unit,bcm_cos_queue_t queue_id,bcm_rx_chan_t chan_id);

extern int bcm_td2p_oam_init(int unit);
extern int bcm_fp_oam_detach(int unit);
extern int _bcm_td2p_oam_olp_fp_hw_index_get(
            int unit,
            bcm_field_olp_header_type_t olp_hdr_type,
            int *hwindex);
extern int _bcm_td2p_oam_olp_hw_index_olp_type_get(
            int unit,
            int hwindex,
            bcm_field_olp_header_type_t *olp_hdr_type );
extern int bcm_fp_oam_group_create(int unit,
                                      bcm_oam_group_info_t *group_info);
extern int bcm_fp_oam_group_get(int unit, bcm_oam_group_t group,
                         bcm_oam_group_info_t *group_info);
extern int bcm_fp_oam_group_destroy(int unit, bcm_oam_group_t group);
extern int bcm_fp_oam_endpoint_create(int unit, 
                                      bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_fp_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                         bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_fp_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);
extern int bcm_fp_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);
extern int bcm_fp_oam_event_register(int unit,
                                         bcm_oam_event_types_t event_types,
                                         bcm_oam_event_cb cb, void *user_data);
extern int bcm_fp_oam_event_unregister(int unit,
                                           bcm_oam_event_types_t event_types,
                                           bcm_oam_event_cb cb);
extern int bcm_fp_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_fp_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_fp_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_p);
extern int bcm_fp_oam_loss_add(int unit, bcm_oam_loss_t *loss_p);
extern int bcm_fp_oam_loss_get(int unit, bcm_oam_loss_t *loss_p);
extern int bcm_fp_oam_loss_delete(int unit, bcm_oam_loss_t *loss_p);
extern int bcm_fp_oam_delay_add(int unit, bcm_oam_delay_t *delay_p);
extern int bcm_fp_oam_delay_get(int unit, bcm_oam_delay_t *delay_p);
extern int bcm_fp_oam_delay_delete(int unit, bcm_oam_delay_t *delay_p);
extern int _bcm_fp_oam_csf_add(int unit, bcm_oam_csf_t *csf_p);
extern int _bcm_fp_oam_csf_get(int unit, bcm_oam_csf_t *csf_p);
extern int _bcm_fp_oam_csf_delete(int unit, bcm_oam_csf_t *csf_p);
extern void _bcm_fp_oam_sw_dump(int unit);
extern int _bcm_fp_oam_sync(int unit);
extern int bcm_fp_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                           void *user_data);
extern int bcm_fp_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                             bcm_oam_endpoint_traverse_cb cb,
                             void *user_data);
extern int bcm_fp_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group_id);
extern int bcm_fp_oam_group_destroy_all(int unit);
extern int
bcm_fp_oam_pm_profile_create(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_fp_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id);
extern int
bcm_fp_oam_pm_profile_delete_all(int unit);
extern int 
bcm_fp_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id,
                                  bcm_oam_pm_profile_t profile_id);
extern int
bcm_fp_oam_pm_profile_get(int unit, bcm_oam_pm_profile_info_t *profile_info);
extern int
bcm_fp_oam_pm_profile_traverse(int unit,
                    bcm_oam_pm_profile_traverse_cb cb, void *user_data);
extern int bcm_fp_oam_pm_stats_get(int unit,
                            bcm_oam_endpoint_t endpoint_id, 
                            bcm_oam_pm_stats_t *stats_ptr);
extern int bcm_fp_oam_pm_event_register(int unit,
                                    bcm_oam_event_types_t event_types,
                                    bcm_oam_pm_event_cb cb, void *user_data);
extern int bcm_fp_oam_pm_event_unregister(int unit,
                                     bcm_oam_event_types_t event_types,
                                     bcm_oam_pm_event_cb cb);
extern int
bcm_fp_oam_pm_raw_data_read_done(int unit, bcm_oam_event_types_t event_types,
                                  uint32 read_index);
extern int
bcm_fp_oam_pm_profile_attach_get(int unit, bcm_oam_endpoint_t endpoint_id,
                                 bcm_oam_pm_profile_t *profile_id);
extern int
bcm_fp_oam_pm_profile_attach(int unit, bcm_oam_endpoint_t endpoint_id,
        bcm_oam_pm_profile_t profile_id);

extern int 
bcm_fp_oam_trunk_ports_add (int unit, bcm_gport_t trunk_gport,
                            int max_ports, bcm_gport_t *port_arr);
extern int
bcm_fp_oam_trunk_ports_delete (int unit, bcm_gport_t trunk_gport,
                               int max_ports, bcm_gport_t *port_arr);
extern int
bcm_fp_oam_trunk_ports_get (int unit, bcm_gport_t trunk_gport,
                            int max_ports, bcm_gport_t *port_arr,
                            int *port_count);

extern int bcm_td2p_port_coe_e2ecc(int unit, bcm_port_t port,
        bcm_port_congestion_config_t *config);
extern int bcm_td2p_cosq_subport_flow_control_set(int unit, bcm_gport_t subport, 
                                                  bcm_gport_t sched_port);
extern int bcm_td2p_cosq_subport_flow_control_get(int unit, bcm_gport_t subport, 
                                                  bcm_gport_t * sched_port);

/*
 * Port Functions
 */
extern int
_bcm_td2p_port_control_egress_class_select_set(int unit, bcm_port_t port, uint32 value);

extern int
_bcm_td2p_port_control_egress_class_select_get(int unit, bcm_port_t port, uint32 *value);

extern int
_bcm_td2p_port_control_higig_class_select_set(int unit, bcm_port_t port, uint32 value);

extern int
_bcm_td2p_port_control_higig_class_select_get(int unit, bcm_port_t port, uint32 *value);

extern int
bcmi_td2p_port_fn_drv_init(int unit);
extern int
bcmi_td2p_port_lanes_get(int unit, bcm_port_t port, int *lanes);
extern int
bcmi_td2p_port_lanes_set(int unit, bcm_port_t port, int lanes);

/*  Flexport port attach/detach functions  */
extern int
bcm_esw_td2p_flexport_port_attach(int unit, bcm_port_t port);

extern int
bcm_esw_td2p_flexport_port_detach(int unit, bcm_port_t port);

extern  int
_bcm_td2p_port_stack_port_attach(int unit, bcm_port_t port);

extern  int
_bcm_td2p_port_stack_port_detach(int unit, bcm_port_t port);

extern int
_bcm_td2p_vfi_membership_port_attach(int unit, bcm_port_t port);

extern int
_bcm_td2p_vfi_membership_port_detach(int unit, bcm_port_t port);

/* IPMC multicast enhancement */
extern int bcm_td2p_aggid_info_detach(int unit);
extern int bcm_td2p_aggid_info_init(int unit);
extern int bcm_td2p_set_port_hw_agg_map(int unit, bcm_port_t port, int aggid);
extern int bcm_td2p_port_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid,
                                   int *aggid);
extern int bcm_td2p_port_aggid_del(int unit, bcm_port_t port);
extern int bcm_td2p_port_trunkid_get(int unit, bcm_port_t port, int *tgid);
extern int bcm_td2p_port_last_member_check(int unit, bcm_port_t port, int *val);
extern int bcm_td2p_port_to_aggid(int unit, bcm_port_t port, int *aggid);

/* Stat functions */
extern int bcmi_td2p_stat_port_attach(int unit, bcm_port_t port);
extern int bcmi_td2p_stat_port_detach(int unit, bcm_port_t port);

/* Flex port */
extern int
bcm_td2p_flexport_pbmp_update(int unit, bcm_pbmp_t *pbmp);

extern int bcm_td2p_vp_vlan_membership_get_all(int unit, bcm_vlan_t vlan,
                int array_max, bcm_gport_t *gport_array,
                int *flags_array, int *array_size);

extern int bcmi_td2p_vlan_block_set(int unit, bcm_vlan_t vid,
                                    bcm_vlan_block_t *block);
extern int bcmi_td2p_vlan_block_get(int unit, bcm_vlan_t vid,
                                    bcm_vlan_block_t *block);
#if defined(INCLUDE_BHH)
extern int bcm_fp_oam_mpls_tp_channel_type_tx_get(int unit,
                                                  bcm_oam_mpls_tp_channel_type_t channel_type,
                                                  int *value);
extern int bcm_fp_oam_mpls_tp_channel_type_tx_set(int unit,
                                                  bcm_oam_mpls_tp_channel_type_t channel_type,
                                                  int value);
#endif
#endif /* BCM_TRIDENT2PLUS_SUPPORT  */

#endif /* !_BCM_INT_TRIDENT2PLUS_H_ */
