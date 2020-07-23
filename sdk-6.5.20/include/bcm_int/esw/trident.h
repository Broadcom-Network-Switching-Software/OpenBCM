/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident.h
 * Purpose:     Function declarations for Trident  bcm functions
 */

#ifndef _BCM_INT_TRIDENT_H_
#define _BCM_INT_TRIDENT_H_
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/trill.h>
#include <soc/tnl_term.h>
#include <bcm/qos.h>
#include <bcm/trill.h>
#include <bcm/niv.h>

typedef enum {
    _BCM_TD_COSQ_INDEX_STYLE_BUCKET,
    _BCM_TD_COSQ_INDEX_STYLE_BUCKET_MODE,
    _BCM_TD_COSQ_INDEX_STYLE_WRED,
    _BCM_TD_COSQ_INDEX_STYLE_WRED_DROP_THRESH,
    _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER,
    _BCM_TD_COSQ_INDEX_STYLE_PERQ_XMT,
    _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
    _BCM_TD_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TD_COSQ_INDEX_STYLE_UC_EGR_POOL,
    _BCM_TD_COSQ_INDEX_STYLE_COUNT
} _bcm_td_cosq_index_style_t;

#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_trident_init(int unit, _field_control_t *fc);
extern int _bcm_field_td_counter_get(int unit,
                                     _field_stage_t *stage_fc,
                                     soc_mem_t counter_x_mem,
                                     uint32 *mem_x_buf,
                                     soc_mem_t counter_y_mem,
                                     uint32 *mem_y_buf,
                                     int idx,
                                     uint64 *packet_count,
                                     uint64 *byte_count);
extern int _bcm_field_td_counter_set(int unit,
                                     _field_stage_t *stage_fc,
                                     soc_mem_t counter_x_mem,
                                     uint32 *mem_x_buf,
                                     soc_mem_t counter_y_mem,
                                     uint32 *mem_y_buf,
                                     int idx,
                                     uint64 *packet_count,
                                     uint64 *byte_count);
#endif /* BCM_FIELD_SUPPORT */

extern int  bcm_td_l2_myStation_add (int unit, bcm_l2_addr_t *l2addr);
extern int  bcm_td_l2_myStation_delete (int unit, bcm_mac_t mac, bcm_vlan_t vid, int *l2_index);
extern int  bcm_td_l2_myStation_get  (int unit, bcm_mac_t mac, bcm_vlan_t vid, bcm_l2_addr_t *l2addr);
extern int  bcm_td_metro_myStation_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port, 
                                        int tunnel_term);
extern int  bcm_td_metro_myStation_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_port_t port,
                                        int tunnel_term);
extern int  bcm_td_metro_myStation_delete_all(int unit);

extern int bcm_td_l2cache_myStation_set(int unit, int index,
                                        bcm_l2_cache_addr_t *l2addr);
extern int bcm_td_l2cache_myStation_get(int unit, int index, 
                                        bcm_l2_cache_addr_t *l2addr);
extern int bcm_td_l2cache_myStation_delete(int unit, int index);
extern int bcm_td_l2cache_myStation_lookup(int unit,
                                           bcm_l2_cache_addr_t *l2caddr,
                                           int *result_index);

extern int bcm_trident_trunk_modify(int, bcm_trunk_t,
                                 bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                                 trunk_private_t *, int, bcm_trunk_member_t *);
extern int bcm_trident_trunk_get(int, bcm_trunk_t,
                                 bcm_trunk_info_t *, int, bcm_trunk_member_t *,
                                 int *, trunk_private_t *);
extern int bcm_trident_trunk_destroy(int, bcm_trunk_t, trunk_private_t *);
extern int bcm_trident_trunk_mcast_join(int, bcm_trunk_t, bcm_vlan_t, 
                                 bcm_mac_t, trunk_private_t *);
#ifdef INCLUDE_L3
extern int bcm_td_trill_init(int unit);
extern int bcm_td_trill_cleanup(int unit);
extern int bcm_td_trill_trunk_member_add (int unit, bcm_trunk_t trunk_id, 
                int trunk_member_count, bcm_port_t *trunk_member_array);
extern int bcm_td_trill_trunk_member_delete(int unit, bcm_trunk_t trunk_id, 
        int trunk_member_count, bcm_port_t *trunk_member_array); 
extern int bcm_td_trill_port_add(int unit, bcm_trill_port_t *trill_port);
extern int bcm_td_trill_port_delete(int unit, bcm_gport_t trill_port_id);
extern int bcm_td_trill_port_delete_all(int unit);
extern int bcm_td_trill_port_get(int unit, bcm_trill_port_t *trill_port);
extern int bcm_td_trill_port_get_all(int unit, int port_max, bcm_trill_port_t *trill_port, int *port_count );
extern int bcm_td_trill_lock (int unit);
extern void bcm_td_trill_unlock (int unit);
extern int _bcm_td_trill_port_resolve(int unit, bcm_gport_t trill_port_id, bcm_if_t encap_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id);
extern int bcm_td_trill_multicast_add(int unit, uint32 flags, bcm_trill_name_t root_name, bcm_vlan_t vlan, 
                          bcm_mac_t c_dmac, bcm_multicast_t group);
extern int bcm_td_trill_multicast_delete(int unit, uint32 flags, bcm_trill_name_t root_name,
                             bcm_vlan_t vid, bcm_mac_t c_dmac);
extern int bcm_td_trill_multicast_delete_all(int unit, bcm_trill_name_t root_name);
extern int bcm_td_trill_multicast_entry_get( int unit, bcm_trill_multicast_entry_t *trill_mc_entry);
extern int bcm_td_trill_multicast_check(int unit, bcm_multicast_t l2mc_group);

extern int bcm_td_trill_multicast_traverse(int unit, 
                             bcm_trill_multicast_entry_traverse_cb  cb, void *user_data);
extern int _bcm_td_l3_ecmp_grp_add(int unit, int ecmp_grp, 
                             void *buf, void *info);
extern int _bcm_td_l3_ecmp_grp_get(int unit, int ecmp_idx, 
                             int ecmp_count, int *nh_idx);
extern int _bcm_td_l3_ecmp_grp_del(int unit, int ecmp_grp, void *info);
extern int bcm_td_trill_egress_set(int unit, int nh_index, uint32 flags);
extern int bcm_td_trill_egress_reset(int unit, int nh_index);
extern int bcm_td_trill_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index);
extern int bcm_td_trill_ethertype_set(int unit, uint16 etherType);
extern int bcm_td_trill_ethertype_get(int unit, int *etherType);
extern int bcm_td_trill_mac_set(int unit, bcm_switch_control_t type,
                            uint32 mac_field, uint8 flag);
extern int bcm_td_trill_mac_get(int unit, bcm_switch_control_t type,
                            uint32 *mac_field, uint8 flag);
extern int bcm_td_trill_ISIS_ethertype_set(int unit, uint16 etherType);
extern int bcm_td_trill_ISIS_ethertype_get(int unit, int *etherType);
extern int bcm_td_trill_MinTtl_set(int unit, int min_ttl);
extern int bcm_td_trill_MinTtl_get(int unit, int *min_ttl);
extern int bcm_td_trill_TtlCheckEnable_set(int unit, int enable);
extern int bcm_td_trill_TtlCheckEnable_get(int unit, int *enable);
extern int bcm_td_trill_source_trill_name_idx_get(int unit, 
                                                                   bcm_trill_name_t source_trill_name,
                                                                   int *trill_name_index);
extern int bcm_td_trill_source_trill_idx_name_get(int  unit, 
                                                                   int  trill_name_index,
                                                                   bcm_trill_name_t  *source_trill_name);
extern int bcm_td_trill_vlan_multicast_group_add(int unit, bcm_vlan_t vlan, vlan_tab_entry_t   *vtab);
extern int bcm_td_trill_vlan_multicast_group_delete(int unit, bcm_vlan_t vlan,  vlan_tab_entry_t   *vtab);

extern int bcm_td_trill_egress_add(int unit, bcm_multicast_t l3mc_group);
extern int bcm_td_trill_egress_delete(int unit, bcm_multicast_t mc_index);
extern  int bcm_td_trill_multicast_source_add( int unit,
                                   bcm_trill_name_t root_name, 
                                   bcm_trill_name_t source_name, 
                                   bcm_gport_t port,
                                   bcm_if_t  encap_intf);
extern  int bcm_td_trill_multicast_source_delete( int unit,
                                   bcm_trill_name_t root_name, 
                                   bcm_trill_name_t source_name, 
                                   bcm_gport_t port,
                                   bcm_if_t  encap_intf);
extern  int bcm_td_trill_multicast_source_get( int unit,
                                   bcm_trill_name_t root_name, 
                                   bcm_trill_name_t source_name, 
                                   bcm_gport_t *port);
extern int bcm_td_trill_multicast_source_traverse(int unit, 
                             bcm_trill_multicast_source_traverse_cb  cb, void *user_data);
extern int bcm_td_trill_multicast_source_delete_all(int unit);
extern int bcm_td_trill_stat_get(int unit, bcm_gport_t port,
                            bcm_trill_stat_t stat, uint64 *val);
extern int bcm_td_trill_stat_get32(int unit, bcm_gport_t port,
                              bcm_trill_stat_t stat, uint32 *val);
extern int bcm_td_trill_stat_clear(int unit, bcm_gport_t port,
                            bcm_trill_stat_t stat);
extern int bcm_td_multicast_trill_group_update(int unit, 
                            int ipmc_index, uint8 trill_tree_id);
extern int bcm_td_multicast_trill_group_get(int unit,
                            int ipmc_index, uint8 *trill_tree_id);
extern int bcm_td_multicast_trill_rootname_get(int unit, 
                      bcm_multicast_t group, bcm_trill_name_t  *root_name);
extern int _bcm_esw_trill_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t c_dmac, 
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int  _bcm_esw_trill_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t c_dmac, uint8 trill_tree_id);
extern int  _bcm_esw_trill_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t c_dmac,
                                    uint8 trill_tree_id, bcm_multicast_t group);
extern int _bcm_esw_trill_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group);
extern int _bcm_esw_trill_multicast_entry_delete_all (int unit, uint8 trill_tree_id);

extern int _bcm_esw_trill_multicast_transit_entry_reset(int unit, bcm_trill_name_t root_name);
extern int _bcm_esw_trill_multicast_transit_entry_reset_all (int unit, uint8 trill_tree_id);
extern int _bcm_esw_trill_multicast_transit_entry_set(int unit, int flags,
                   bcm_trill_name_t root_name, int mc_index, uint8  trill_tree_id);
extern int _bcm_esw_trill_multicast_transit_entry_get(int unit, bcm_trill_name_t root_name, bcm_multicast_t *group);

extern int _bcm_esw_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name);
extern int _bcm_esw_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name);
extern int _bcm_esw_trill_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int _bcm_esw_trill_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int _bcm_esw_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);

extern int bcm_td_trill_multicast_transit_entry_set(int unit, int flags,
                   bcm_trill_name_t root_name, int mc_index, uint8  trill_tree_id);
extern int bcm_td_trill_multicast_transit_entry_reset(int unit, bcm_trill_name_t root_name);
extern int bcm_td_trill_multicast_transit_entry_reset_all (int unit, uint8 tree_id);
extern int bcm_td_trill_multicast_transit_entry_get(int unit, bcm_trill_name_t root_name, bcm_multicast_t *group);
extern int bcm_td_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name);
extern int bcm_td_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name);
extern int bcm_td_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st);

extern int bcm_td_l2_trill_multicast_entry_add(int unit, uint32 flags, int key, 
                             bcm_vlan_t vid, bcm_mac_t mac, 
                             uint8 trill_tree_id, bcm_multicast_t group);
extern int bcm_td_l2_trill_vlan_multicast_entry_modify (int unit, bcm_vlan_t vlan, int set_bit);
extern int bcm_td_l2_trill_multicast_entry_update(int unit, int key, 
                             bcm_vlan_t vid, bcm_mac_t mac, 
                             uint8 trill_tree_id, bcm_multicast_t group);
extern int bcm_td_l2_trill_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                             bcm_mac_t mac, uint8 trill_tree_id);
extern int bcm_td_l2_trill_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                             bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group);
extern int bcm_td_l2_trill_multicast_entry_delete_all (int unit, uint8 tree_id);
extern int bcm_td_l2_trill_network_multicast_entry_traverse (int unit, 
                             _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int bcm_td_l2_trill_access_multicast_entry_traverse (int unit, 
                             _bcm_td_trill_multicast_entry_traverse_t *trav_st);
extern int _bcm_td_l3_intf_qos_set(int unit, uint32 *l3_if_entry_p, 
                             _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_td_l3_intf_qos_get(int unit, uint32 *l3_if_entry_p, 
                             _bcm_l3_intf_cfg_t *intf_info);
extern int _bcm_td_l3_routed_int_pri_init (int unit);
extern int _bcm_xgs3_trunk_nh_store_get(int unit, bcm_trunk_t trunk_id, int *nh_index);

extern int bcm_td_vp_force_vlan_get(int unit, bcm_gport_t gport, 
                                    bcm_vlan_t *vlan, uint32 *flags);
extern int bcm_td_vp_force_vlan_set(int unit, bcm_gport_t gport,
                                    bcm_vlan_t vlan, uint32 flags);

#endif /* INCLUDE_L3 */

extern int bcm_td_cosq_init(int unit);
extern int bcm_td_cosq_detach(int unit, int software_state_only);
extern int bcm_td_cosq_config_set(int unit, int numq);
extern int bcm_td_cosq_config_get(int unit, int *numq);
extern int bcm_td_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_td_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_td_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_td_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);
extern int bcm_td_cosq_sched_weight_max_get(int unit, int mode,
                                            int *weight_max);
extern int bcm_td_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);
extern int bcm_td_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);
extern int bcm_td_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_td_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_td_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_td_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);
extern int bcm_td_cosq_discard_set(int unit, uint32 flags);
extern int bcm_td_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_td_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_td_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_td_cosq_stat_set(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq, bcm_cosq_stat_t stat,
                                uint64 value);
extern int bcm_td_cosq_stat_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq, bcm_cosq_stat_t stat,
                                int sync_mode, uint64 *value);
extern int bcm_td_cosq_congestion_queue_set(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int index);
extern int bcm_td_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq, int *index);
extern int bcm_td_cosq_congestion_quantize_set(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int weight_code, int setpoint);
extern int bcm_td_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq,
                                               int *weight_code,
                                               int *setpoint);
extern int bcm_td_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int min, int max);
extern int bcm_td_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                                 bcm_cos_queue_t cosq,
                                                 int *min, int *max);
extern int bcm_td_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                              int enable);
extern int bcm_td_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int arg);
extern int bcm_td_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_control_t type, int *arg);

extern int bcm_td_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_td_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_td_cosq_gport_get(int unit, bcm_gport_t gport,
                                 bcm_gport_t *port, int *numq, uint32 *flags);
extern int bcm_td_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);
extern int bcm_td_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t gport,
                                         bcm_cos_queue_t cosq);
extern int bcm_td_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t *gport,
                                         bcm_cos_queue_t *cosq);
extern int bcm_td_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max, uint32 flags);
extern int bcm_td_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_td_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min,
                                                 uint32 kbits_burst_max);
extern int bcm_td_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);
extern int bcm_td_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_td_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       int *mode, int *weight);
extern int bcm_td_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_td_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_td_cosq_port_pfc_op(int unit, bcm_port_t port,
                                   bcm_switch_control_t sctype,
                                   _bcm_cosq_op_t op, bcm_gport_t *gport,
                                    int gport_count);
extern int bcm_td_cosq_port_pfc_get(int unit, bcm_port_t port,
                                    bcm_switch_control_t sctype,
                                    bcm_gport_t *gport, int gport_count,
                                    int *actual_gport_count);
extern int bcm_td_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_td_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_gport_t input_gport,
                                    bcm_cos_queue_t cosq);
extern int bcm_td_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_gport_t *input_gport,
                                        bcm_cos_queue_t *cosq);
extern int _bcm_td_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id);
extern int _bcm_td_cosq_index_resolve(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq,
                                      _bcm_td_cosq_index_style_t style,
                                      bcm_port_t *local_port, int *index,
                                      int *count);
extern int bcm_td_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array);
extern int bcm_td_cosq_pfc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_pfc_class_mapping_t *mapping_array,
    int *array_count);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

extern int bcm_td_port_init(int unit);
extern int bcm_td_port_deinit(int unit);
extern int bcm_td_port_ing_pri_cng_set(int unit, bcm_port_t port,
                                       int untagged, int pkt_pri, int cfi,
                                       int int_pri, bcm_color_t color);
extern int bcm_td_port_ing_pri_cng_get(int unit, bcm_port_t port,
                                       int untagged, int pkt_pri, int cfi,
                                       int *int_pri, bcm_color_t *color);
extern int bcm_td_port_egress_set(int unit, bcm_port_t port, int modid,
                                  bcm_pbmp_t pbmp);
extern int bcm_td_port_egress_get(int unit, bcm_port_t port, int modid,
                                  bcm_pbmp_t *pbmp);
extern int _bcm_td_mod_port_tpid_enable_write(int unit, bcm_module_t mod,
        bcm_port_t port, int tpid_enable);
extern int _bcm_td_mod_port_tpid_enable_read(int unit, bcm_module_t mod,
        bcm_port_t port, int *tpid_enable);
extern int _bcm_td_mod_port_tpid_set(int unit, bcm_module_t mod,
        bcm_port_t port, uint16 tpid);
extern int _bcm_td_mod_port_tpid_get(int unit, bcm_module_t mod,
        bcm_port_t port, uint16 *tpid);
extern int _bcm_td_mod_port_tpid_add(int unit, bcm_module_t mod,
        bcm_port_t port, uint16 tpid);
extern int _bcm_td_mod_port_tpid_delete(int unit, bcm_module_t mod,
        bcm_port_t port, uint16 tpid);
extern int _bcm_td_mod_port_tpid_delete_all(int unit, bcm_module_t mod,
        bcm_port_t port);
extern int bcm_td_port_niv_type_set(int unit, bcm_port_t port, int value);
extern int bcm_td_port_niv_type_get(int unit, bcm_port_t port, int *value);

#ifdef INCLUDE_L3
extern int _bcm_td_port_vif_match_multi_get(int unit, bcm_gport_t gport,
                                            int size,
                                            bcm_port_match_info_t *match_array,
                                            int *count);
extern int bcm_td_vp_control_set(int unit, bcm_gport_t gport, 
                                 bcm_port_control_t type, int value);
extern int bcm_td_vp_control_get(int unit, bcm_gport_t gport, 
                                 bcm_port_control_t type, int *value);
extern int bcm_td_vp_group_init(int unit);
extern int bcm_td_vp_group_detach(int unit);
extern int bcm_td_vp_vlan_member_set(int unit, bcm_gport_t gport, uint32 flags);
extern int bcm_td_vp_vlan_member_get(int unit, bcm_gport_t gport, uint32 *flags);
extern int bcm_td_ing_vp_group_move(int unit, int vp, bcm_vlan_t vlan, int add);
extern int bcm_td_eg_vp_group_move(int unit, int vp, bcm_vlan_t vlan, int add);
extern int bcm_td_ing_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td_eg_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_td_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap);
extern int bcm_td_egr_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td_ing_vp_group_unmanaged_set(int unit, int flag);
extern int bcm_td_ing_vp_group_unmanaged_get(int unit);
extern int bcm_td_egr_vp_group_unmanaged_get(int unit);
extern int bcm_td_vlan_vp_group_set(int unit, soc_mem_t vlan_mem, int vlan, 
                                    int vp_group,  int enable);
extern int bcm_td_vlan_vp_group_get(int unit, soc_mem_t vlan_mem, int vlan, 
                                    int vp_group,  int *enable);
extern int bcm_td_vlan_vp_group_get_all(int unit, int vlan, int array_max,
             bcm_gport_t *gport_array, int *flags_array, int *port_cnt);
extern int _td_egr_dvp_attribute_field_name_get(int unit,
           egr_dvp_attribute_entry_t * dvp_entry, soc_field_t legacy_name,
           soc_field_t * result_name);


#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_td_vp_group_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */

#ifdef INCLUDE_L3
extern int bcm_trident_niv_init(int unit);
extern int bcm_trident_niv_cleanup(int unit);
extern int bcm_trident_niv_port_add(int unit, bcm_niv_port_t *niv_port);
extern int bcm_trident_niv_port_delete(int unit, bcm_gport_t niv_port_id);
extern int bcm_trident_niv_port_delete_all(int unit);
extern int bcm_trident_niv_port_get(int unit, bcm_niv_port_t *niv_port);
extern int bcm_trident_niv_port_traverse(int unit,
                                    bcm_niv_port_traverse_cb cb,
                                    void *user_data);
extern int _bcm_trident_niv_port_resolve(int unit, bcm_gport_t niv_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id);
extern int bcm_trident_niv_ethertype_set(int unit, int ethertype);
extern int bcm_trident_niv_ethertype_get(int unit, int *ethertype);
extern int bcm_trident_niv_forward_add(int unit,
                                       bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_trident_niv_forward_delete(int unit,
                                       bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_trident_niv_forward_delete_all(int unit);
extern int bcm_trident_niv_forward_get(int unit,
                                       bcm_niv_forward_t *iv_fwd_entry);
extern int bcm_trident_niv_forward_traverse(int unit,
                                       bcm_niv_forward_traverse_cb cb,
                                       void *user_data);
extern int bcm_trident_niv_untagged_add(int unit, bcm_vlan_t vlan, int vp,
                                       int flags, int *egr_vt_added);
extern int bcm_trident_niv_untagged_delete(int unit, bcm_vlan_t vlan, int vp);
extern int bcm_trident_niv_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                       int *flags);
extern int bcm_trident_niv_port_untagged_vlan_set(int unit, bcm_port_t port,
                                       bcm_vlan_t vlan);
extern int bcm_trident_niv_port_untagged_vlan_get(int unit, bcm_port_t port,
                                       bcm_vlan_t *vid_ptr);
extern int bcm_trident_niv_port_learn_set(int unit, bcm_gport_t niv_port_id, 
                                           uint32 flags);
extern int bcm_trident_niv_port_learn_get(int unit, bcm_gport_t niv_port_id, 
                                           uint32 *flags);
extern int bcm_trident_niv_egress_add(int unit, bcm_gport_t niv_port,
        bcm_niv_egress_t *niv_egress);
extern int bcm_trident_niv_egress_delete(int unit, bcm_gport_t niv_port,
        bcm_niv_egress_t *niv_egress);
extern int bcm_trident_niv_egress_set(int unit, bcm_gport_t niv_port,
        int array_size, bcm_niv_egress_t *niv_egress_array);
extern int bcm_trident_niv_egress_get(int unit, bcm_gport_t niv_port,
        int array_size, bcm_niv_egress_t *niv_egress_array, int *count);
extern int bcm_trident_niv_egress_delete_all(int unit, bcm_gport_t niv_port);
extern int _bcm_trident_niv_match_add(int unit, bcm_gport_t port,
                                      uint16 virtual_interface_id,
                                      bcm_vlan_t match_vlan, int vp);
extern int _bcm_trident_niv_match_delete(int unit, bcm_gport_t port,
                                         uint16 virtual_interface_id,
                                         bcm_vlan_t match_vlan, int *old_vp);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_trident_niv_required_scache_size_get(int unit, uint32 *size);
extern int bcm_trident_niv_sync(int unit, uint8 **scache_ptr);
extern int bcm_trident_niv_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_trident_niv_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */

extern int bcm_td_modport_map_init(int unit);
extern int bcm_td_modport_map_detach(int unit);
extern int bcm_td_stk_port_modport_op(int unit, int op, bcm_port_t ing_port,
                          bcm_module_t dest_modid, bcm_port_t *dest_port,
                          int dest_port_count);
extern int bcm_td_stk_port_modport_get(int unit, bcm_port_t ing_port,
                          bcm_module_t dest_modid, int dest_port_max,
                          bcm_port_t *dest_port_array,
                          int *dest_port_count);
extern int bcm_td_stk_modport_voq_op(int unit, bcm_port_t ing_port,
                          bcm_module_t dest_modid, int *grp_id, int op);
extern int bcm_td_stk_modport_map_update(int unit);
extern int bcm_td_stk_trunk_override_ucast_set(int unit, bcm_port_t ing_port,
                          bcm_trunk_t hgtid, int modid, int enable);
extern int bcm_td_stk_trunk_override_ucast_get(int unit, bcm_port_t ing_port, 
                          bcm_trunk_t hgtid, int modid, int *enable); 
extern int bcm_td_modport_map_mode_set(int unit, bcm_switch_control_t type,
                                       int arg);
extern int bcm_td_modport_map_mode_get(int unit, bcm_switch_control_t type,
                                       int *arg);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td_modport_map_scache_size_get(int unit, int *size);
extern int bcm_td_modport_map_sync(int unit, uint8 **scache_ptr);
extern int bcm_td_modport_map_reinit(int unit, uint8 **scache_ptr);
extern int bcm_td_modport_map_enable_scache_size_get(int unit, int *size);
extern int bcm_td_modport_map_enable_reinit(int unit, uint8 **scache_ptr);
extern int bcm_td_modport_map_enable_sync(int unit, uint8 **scache_ptr);
extern int bcm_td_stk_trunk_override_hi_scache_size_get(int unit, int *size);
extern int bcm_td_stk_trunk_override_hi_sync(int unit, uint8 **scache_ptr);
extern int bcm_td_stk_trunk_override_hi_reinit(int unit, uint8 **scache_ptr);
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
extern int bcm_td_linkflap_trunk_map_scache_size_get(int unit, int *size);
extern int bcm_td_linkflap_trunk_map_reinit(int unit, uint8 **scache_ptr);
extern int bcm_td_linkflap_trunk_map_sync(int unit, uint8 **scache_ptr);
#endif /*defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)*/
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_td_modport_map_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/* Utility routines for metering and egress rate control */
#define _BCM_TD_METER_FLAG_NON_LINEAR           0x01
#define _BCM_TD_METER_FLAG_PACKET_MODE          0x02
#define _BCM_TD_METER_FLAG_FP_POLICER           0x04

extern int _bcm_td_rate_to_bucket_encoding(int unit, uint32 rate_quantum,
                                           uint32 burst_quantum, uint32 flags,
                                           int refresh_bitsize,
                                           int bucket_max_bitsize,
                                           uint32 *refresh_rate,
                                           uint32 *bucketsize,
                                           uint32 *granularity);
extern uint32 _bcm_td_bucket_encoding_to_rate(int unit, uint32 refresh_rate,
                                              uint32 bucket_max,
                                              uint32 granularity,
                                              uint32 flags,
                                              uint32 *rate_quantum,
                                              uint32 *burst_quantum);

extern uint32 _bcm_td_default_burst_size(int unit, int port, uint32 kbits_sec);
extern uint32 _bcm_td_default_burst_size_by_weight(int unit, int port, 
                                             uint32 kbits_sec, 
                                             int port_weights);

extern int
bcm_td_cosq_gport_destmod_attach(int unit, bcm_gport_t gport,
                               bcm_port_t ing_port,
                               bcm_module_t dest_modid, 
                               int fabric_egress_port);

extern int
bcm_td_cosq_gport_destmod_detach(int unit, bcm_gport_t gport,
                               bcm_port_t ing_port,
                               bcm_module_t dest_modid, 
                               int fabric_egress_port);

extern int
bcm_td_cosq_cpu_cosq_enable_set(int unit, 
                                bcm_cos_queue_t cosq, 
                                int enable);

extern int
bcm_td_cosq_cpu_cosq_enable_get(int unit, 
                                bcm_cos_queue_t cosq, 
                                int *enable);

extern int
_bcm_td_port_enqueue_set(int unit, bcm_port_t gport,
                                    int enable);
extern int
_bcm_td_port_enqueue_get(int unit, bcm_port_t gport,
                                            int *enable);
extern int bcm_td_port_drain_cells(int unit, int port);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td_trill_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int bcm_td_stk_modport_map_port_attach(int unit, bcm_port_t port);
extern int bcm_td_stk_modport_map_port_detach(int unit, bcm_port_t port);

#endif /* BCM_TRIDENT_SUPPORT */
#endif  /* !_BCM_INT_TRIDENT_H_ */

