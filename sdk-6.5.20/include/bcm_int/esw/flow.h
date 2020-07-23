/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Flow definitions internal to the BCM library.
 */


#ifndef _BCM_INT_FLOW_H
#define _BCM_INT_FLOW_H

#include <bcm/types.h>
#include <soc/drv.h>
#include <bcm/flow.h>
#include <include/bcm_int/esw/vxlan.h>
#include <include/soc/tnl_term.h>
#ifdef INCLUDE_L3

#define _BCM_FLOW_VFI_INVALID 0

/*
 * Do not change - Implementation assumes
 * _BCM_FLOW_VPN_TYPE_ELINE == _BCM_FLOW_VPN_TYPE_ELAN
 */
#define _BCM_FLOW_VPN_TYPE_ELINE         _BCM_VPN_TYPE_VFI
#define _BCM_FLOW_VPN_TYPE_ELAN          _BCM_VPN_TYPE_VFI

#define _BCM_FLOW_VPN_SET(_vpn_, _type_, _id_) \
        _BCM_VPN_SET(_vpn_, _type_, _id_)

#define _BCM_FLOW_VPN_GET(_id_, _type_,  _vpn_) \
        _BCM_VPN_GET(_id_, _type_, _vpn_)

#define _BCM_FLOW_CLEANUP(_rv_) \
        if ( (_rv_) < 0) { \
            goto cleanup; \
        }


/* index to memory usage array, do not change */
#define _BCM_FLOW_VXLT1_INX   0
#define _BCM_FLOW_VXLT2_INX   1
#define _BCM_FLOW_MPLS_INX    2
#define _BCM_FLOW_EVXLT1_INX  3
#define _BCM_FLOW_EVXLT2_INX  4
#define _BCM_FLOW_TNL_INX     5 
#define _BM_FLOW_MEM_USE_MAX  6  /* last index + 1 */

typedef struct _bcm_flow_init_tunnel_entry_s {
    uint16  idx;     /* index to egr_ip_tunnel table */
} _bcm_flow_init_tunnel_entry_t;

/* Flow match config structure */
typedef struct bcm_flow_match_vp_s {
    uint32 vnid;                        /* VN_ID. */
    bcm_vlan_t vlan;                    /* Outer VLAN ID to match. */
    bcm_vlan_t inner_vlan;              /* Inner VLAN ID to match. */
    bcm_gport_t port;                   /* Match port / trunk */
    bcm_ip_t sip;                       /* source IPv4 address */
    bcm_ip6_t sip6;                     /* source IPv6 address */
    bcm_mpls_label_t mpls_label;
    bcm_flow_match_criteria_t criteria; /* flow match criteria. */
    uint32 valid_elements;              /* bitmap of valid fields for the match
                                           action */
    bcm_flow_handle_t flow_handle;      /* flow handle derived from flow name */
    uint32 flow_option;                 /* flow option derived from flow option
                                           name */
} bcm_flow_match_vp_t;

/*
 * Software book keeping for Flex Flow related information
 */
typedef struct _bcm_flow_bookkeeping_s {
    int         initialized;        /* Set to TRUE when FLOW module initialized */
    sal_mutex_t    flow_mutex;    /* Protection mutex. */
    bcm_vlan_t                      *flow_vpn_vlan; /* Outer VLAN for VPN */

    /* match function */
    uint16 *iif_ref_cnt; /* iif usage reference count  */
    uint16 *vp_ref_cnt;  /* svp usage reference count  */
    uint32 mem_use_cnt[_BM_FLOW_MEM_USE_MAX];  /* memory referenc count in */

    /* encap set */
    SHR_BITDCL *dvp_attr_bitmap;  /* EGR_DVP_ATTRIBUTE index usage bitmap */
      
    /* 
     * The initiator tunnel here means these IP header fields such as SIP,DIP.
     * They do not always reside in the same hardware table. A single software
     * tunnel table is constructed to point to all these fields. 
     * soft tunnel index:
     *    L2 tunnel:   dvp
     *    L3 tunnel:   max_dvp + egr_intf ID
     */ 
    _bcm_flow_init_tunnel_entry_t *init_tunnel;

    /* IP tunnel fragmentID(sequence number) table base index*/
    uint32 frag_base_inx; 
    bcm_flow_match_vp_t* match_key;
} _bcm_flow_bookkeeping_t;

extern _bcm_flow_bookkeeping_t *_bcm_flow_bk_info[BCM_MAX_NUM_UNITS];
#define FLOW_INFO(_unit_)      (_bcm_flow_bk_info[_unit_])

/*
 * EGR_DVP_ATTRIBUTE table usage bitmap operations
 */
#define _BCM_FLOW_EGR_DVP_USED_GET(_u_, _dvp_) \
        SHR_BITGET(FLOW_INFO(_u_)->dvp_attr_bitmap, (_dvp_))
#define _BCM_FLOW_EGR_DVP_USED_SET(_u_, _dvp_) \
        SHR_BITSET(FLOW_INFO((_u_))->dvp_attr_bitmap, (_dvp_))
#define _BCM_FLOW_EGR_DVP_USED_CLR(_u_, _dvp_) \
        SHR_BITCLR(FLOW_INFO((_u_))->dvp_attr_bitmap, (_dvp_))


/* Generic memory allocation routine. */
#define BCM_TD3_FLOW_ALLOC(_ptr_,_size_,_descr_)                      \
            do {                                                     \
                if ((NULL == (_ptr_))) {                             \
                   _ptr_ = sal_alloc((_size_),(_descr_));            \
                }                                                    \
                if((_ptr_) != NULL) {                                \
                    sal_memset((_ptr_), 0, (_size_));                \
                }                                                    \
            } while (0)

/**** macros used for flexflow modules only(under flexflow dir) ****/

#define _BCM_FLOW_LOGICAL_FIELD_MAX       20
#define _BCM_FLOW_IPV6_ADDR_WORD_LEN      4

#define _BCM_FLOW_IS_FLEX_VIEW(_info) \
	((_info)->flow_handle >= SOC_FLOW_DB_FLOW_ID_START)

/* flow match */

/* flow encap */

#define _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI        0x05
#define _BCM_FLOW_ENCAP_L2GRE_KEY_TYPE_VFI_DVP    0x06
#define _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI        0x08
#define _BCM_FLOW_ENCAP_VXLAN_KEY_TYPE_VFI_DVP    0x09
#define _BCM_FLOW_ENCAP_KEY_TYPE_VFI_DVP_GROUP    0x0a
#define _BCM_FLOW_ENCAP_KEY_TYPE_VRF_MAPPING      0x0b
#define _BCM_FLOW_ENCAP_KEY_TYPE_VLAN_XLATE_VFI   0x0c

#define _BCM_FLOW_VXLAN_EGRESS_DEST_VP_TYPE       2
#define _BCM_FLOW_L2GRE_EGRESS_DEST_VP_TYPE       3

#define  _BCM_FLOW_DEST_VP_TYPE_ACCESS         0x0
#define  _BCM_FLOW_L2GRE_INGRESS_DEST_VP_TYPE  0x2
#define  _BCM_FLOW_VXLAN_INGRESS_DEST_VP_TYPE  0x3

/* internal use only. must avoid conflict with API encap flags */
#define  _BCM_FLOW_ENCAP_FLAG_TPID_INDEX       (1 << 31)

#define _BCM_FLOW_SD_TAG_FORMAT(_info) (_BCM_FLOW_IS_FLEX_VIEW(_info) || \
        ((_info)->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) || \
        ((_info)->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP))

#define _BCM_FLOW_TUNNEL_DATA_FORMAT(_info) \
        (((_info)->criteria == BCM_FLOW_ENCAP_CRITERIA_VRF_MAPPING) || \
         ((_info)->criteria == BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP))

/* flow match */

#define _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP       0x8
#define _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_VNID      0x9
#define _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_SIP_VNID  0xa
#define _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP       0x6
#define _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_VPNID     0x7
#define _BCM_FLOW_MATCH_KEY_TYPE_L2GRE_SIP_VPNID 0x4
#define _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_VNID      0x13
#define _BCM_FLOW_MATCH_KEY_TYPE_VXLAN_FLEX_SIP_VNID  0x1c

#define _BCM_FLOW_MATCH_PORT_CTRL_SET     0
#define _BCM_FLOW_MATCH_PORT_CTRL_DELETE  1
#define _BCM_FLOW_MATCH_PORT_CTRL_GET     2

#define _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE_WIDE 2
#define _BCM_FLOW_TD3_L3_TUNNEL_QUAD_WIDE   1

#define _BCM_FLOW_TD3_L3_TUNNEL_WIDTH_SINGLE 1
#define _BCM_FLOW_TD3_L3_TUNNEL_DOUBLE 2
#define _BCM_FLOW_TD3_L3_TUNNEL_QUAD 4

typedef struct _bcm_flow_frag_field_info_s {
    int pos_min;
    int pos_max;
    uint32 id;
} _bcm_flow_frag_field_info_t;

/* functions only used within flexflow */
extern int
bcmi_esw_flow_vp_is_eline( int unit, int vp, uint8 *isEline);

extern int
_bcm_flow_key_is_valid(int unit, soc_mem_t mem_view_id,
                       uint32 *key, int num_keys);

extern int 
_bcm_flow_mem_view_to_flow_info_get(int unit,
               uint32 mem_view_id,
               uint32 func_id,
               uint32 *flow_hndl,
               uint32 *flow_opt,
               int    instance);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
_bcm_flow_tunnel_initiator_reinit(int unit);
#endif

/**** internal global functions ****/
extern int _bcm_esw_flow_sync(int unit);

extern int bcmi_esw_flow_port_create(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_flow_port_t  *flow_port);

extern int bcmi_esw_flow_port_destroy(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_gport_t vxlan_port_id);

extern int bcmi_esw_flow_port_get(
    int unit, 
    bcm_vpn_t vpn, 
    bcm_flow_port_t *flow_port);

extern int bcmi_esw_flow_port_get_all( 
    int unit, 
    bcm_vpn_t l2vpn, 
    int port_max, 
    bcm_flow_port_t *port_array, 
    int *port_count);

extern int  bcmi_esw_flow_lock(int unit);

extern void  bcmi_esw_flow_unlock(int unit);

extern int bcmi_esw_flow_init(int unit);

extern int bcmi_esw_flow_check_init(int unit);

extern int bcmi_esw_flow_cleanup(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcmi_esw_flow_sw_dump (int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcmi_esw_flow_vpn_is_valid(
    int unit, 
    bcm_vpn_t l2vpn);

extern int bcmi_esw_flow_vpn_is_eline(
    int unit,
    bcm_vpn_t l2vpn,
    uint8 *isEline);

extern int bcmi_esw_flow_match_add(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_match_delete(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_match_get(
    int unit,
    bcm_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_match_traverse(
    int unit,
    bcm_flow_match_traverse_cb cb,
    void *user_data);

extern int bcmi_esw_flow_tunnel_initiator_create(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_tunnel_initiator_destroy(
    int unit,
    bcm_gport_t flow_tunnel_id);

extern int bcmi_esw_flow_tunnel_initiator_destroy_all(
    int unit);

extern int bcmi_esw_flow_tunnel_initiator_get(
    int unit,
    bcm_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_tunnel_initiator_traverse(
    int unit,
    bcm_flow_tunnel_initiator_traverse_cb cb,
    void *user_data);

extern int bcmi_esw_flow_tunnel_terminator_create(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_tunnel_terminator_update(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_tunnel_terminator_destroy(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_tunnel_terminator_destroy_all(
    int unit);

extern int bcmi_esw_flow_tunnel_terminator_get(
    int unit,
    bcm_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);
  
extern int bcmi_esw_flow_port_encap_set(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int  bcmi_esw_flow_tunnel_terminator_traverse(
    int unit,
    bcm_flow_tunnel_terminator_traverse_cb cb,
    void *user_data);

extern int bcmi_esw_flow_port_encap_get(
    int unit,
    bcm_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_encap_add(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_encap_delete(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_encap_get(
    int unit,
    bcm_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

extern int bcmi_esw_flow_encap_traverse(
    int unit,
    bcm_flow_encap_traverse_cb cb,
    void *user_data);

extern int
bcmi_esw_flow_vpn_create(int unit, bcm_vpn_t *vpn,
                            bcm_flow_vpn_config_t *info);
extern int
bcmi_esw_flow_vpn_destroy(int unit, bcm_vpn_t vpn);

extern int
bcmi_esw_flow_vpn_destroy_all(int unit);

extern int
bcmi_esw_flow_vpn_get(int unit, bcm_vpn_t vpn,
                         bcm_flow_vpn_config_t *info);
extern int
bcmi_esw_flow_vpn_traverse(int unit,
                              bcm_flow_vpn_traverse_cb cb,
                              void *user_data);

extern int bcmi_esw_flow_port_delete_all(int unit, bcm_vpn_t vpn);

extern int bcmi_esw_flow_dvp_vlan_xlate_key_set(int unit, 
                   bcm_gport_t port_id, int key_type);
extern int bcmi_esw_flow_dvp_vlan_xlate_key_get(int unit, 
                   bcm_gport_t port_id, int *key_type);

/* Flex Counter */
extern int bcmi_esw_flow_stat_object_get(int unit,
                          bcm_flow_handle_t flow_handle,
                          bcm_flow_option_id_t flow_option_id,
                          bcm_flow_function_type_t function_type,
                          bcm_stat_object_t *stat_object);

extern int bcmi_esw_flow_stat_attach(int unit,
                          bcm_flow_stat_info_t *flow_stat_info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id);

extern int bcmi_esw_flow_stat_detach(int unit,
                          bcm_flow_stat_info_t *flow_stat_info,
                          uint32 num_of_fields,
                          bcm_flow_logical_field_t *field,
                          uint32 stat_counter_id);

extern int bcmi_esw_flow_stat_id_get(int unit,
                         bcm_flow_stat_info_t *flow_stat_info,
                         uint32 num_of_fields,
                         bcm_flow_logical_field_t *field,
                         bcm_stat_object_t stat_object,
                         uint32 *stat_counter_id);

extern int  bcmi_esw_flow_entry_to_tnl_term(int unit,
                                    uint32 *entry,
                                    soc_tunnel_term_t *tnl_term);

extern int bcmi_esw_flow_tnl_term_to_entry(int unit,
                                    soc_tunnel_term_t *tnl_term,
                                    uint32 *entry);

extern int
bcmi_flow_ip6_addr_is_valid(bcm_ip6_t addr);

extern int
bcmi_esw_flow_vpnid_get(int unit, int vfi_index, bcm_vlan_t *vid);

extern int
bcmi_esw_flow_mshg_prune_egress_set(
        int unit,
        bcm_switch_network_group_t src_group_id,
        bcm_switch_network_group_t dst_group_id,
        int prune_enable);

extern int
bcmi_esw_flow_mshg_prune_egress_init(int unit);
extern int
_bcm_esw_flow_port_source_vp_lag_set(int unit, bcm_gport_t gport, int vp_lag_vp);
extern int
_bcm_esw_flow_port_source_vp_lag_clear(int unit, bcm_gport_t gport, int vp_lag_vp);
extern int
_bcm_esw_flow_port_source_vp_lag_get(int unit, bcm_gport_t gport, int* vp_lag_vp);
extern int
bcmi_esw_flow_match_svp_get(int unit, bcm_gport_t gport, int* vp_lag_vp);
extern int
bcmi_esw_flow_match_svp_replace(int unit, int gport, int new_vp);
extern int
bcmi_esw_flow_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array);
extern int
bcmi_esw_flow_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                int trunk_member_count, bcm_port_t *trunk_member_array);
#endif /* INCLUDE_L3 */
#endif  /* !_BCM_INT_FLOW_H */
