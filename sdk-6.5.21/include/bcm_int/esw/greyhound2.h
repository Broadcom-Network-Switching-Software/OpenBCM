/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_GREYHOUND2_H_
#define _BCM_INT_GREYHOUND2_H_
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/cosq.h>
#ifdef BCM_FIELD_SUPPORT
#include <bcm_int/esw/field.h>
#endif /* BCM_FIELD_SUPPORT */
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch_match.h>
#ifdef BCM_TAS_SUPPORT
#include <bcm_int/esw/tas.h>
#endif /* BCM_TAS_SUPPORT */
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/preemption_cnt.h>
#if defined(BCM_TSN_SUPPORT)
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_taf.h>
#include <bcm_int/esw/tsn_stat.h>
#endif /* BCM_TSN_SUPPORT */
#include <soc/greyhound2.h>
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif /* BCM_FIRELIGHT_SUPPORT */

extern int  bcm_td_l2_myStation_add(int unit, bcm_l2_addr_t *l2addr);
extern int  bcm_td_l2_myStation_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid,
                                       int *l2_index);
extern int  bcm_td_l2_myStation_get(int unit, bcm_mac_t mac, bcm_vlan_t vid,
                                    bcm_l2_addr_t *l2addr);

extern int bcm_td_l2cache_myStation_set(int unit, int index,
                                        bcm_l2_cache_addr_t *l2addr);
extern int bcm_td_l2cache_myStation_get(int unit, int index,
                                        bcm_l2_cache_addr_t *l2addr);
extern int bcm_td_l2cache_myStation_delete(int unit, int index);
extern int bcm_td_l2cache_myStation_lookup(int unit,
                                           bcm_l2_cache_addr_t *l2caddr,
                                           int *result_index);


#ifdef BCM_FIELD_SUPPORT
extern int _bcm_field_gh2_init(int unit, _field_control_t *fc);
#endif /* BCM_FIELD_SUPPORT */


extern int bcmi_gh2_cosq_drop_status_enable_set(int unit, bcm_port_t port,
                                                int enable);

extern int
bcm_gh2_switch_control_port_set(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int arg);
extern int
bcm_gh2_switch_control_port_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t type,
    int *arg);
extern int
    bcm_gh2_switch_control_set(int unit, bcm_switch_control_t type, int arg);
extern int
    bcm_gh2_switch_control_get(int unit, bcm_switch_control_t type, int *arg);
extern int _bcm_gh2_switch_init(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcmi_gh2_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcmi_gh2_cosq_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
extern int bcmi_gh2_cosq_init(int unit);
extern int bcmi_gh2_cosq_detach(int unit, int software_state_only);
extern int bcmi_gh2_cosq_config_set(int unit, int numq);
extern int bcmi_gh2_cosq_config_get(int unit, int *numq);
extern int bcmi_gh2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq,
                                            uint32 kbits_sec_min,
                                            uint32 kbits_sec_max,
                                            uint32 kbits_sec_burst,
                                            uint32 flags);
extern int bcmi_gh2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                            bcm_cos_queue_t cosq,
                                            uint32 *kbits_sec_min,
                                            uint32 *kbits_sec_max,
                                            uint32 *kbits_sec_burst,
                                            uint32 *flags);
extern int bcmi_gh2_cosq_discard_set(int unit, uint32 flags);
extern int bcmi_gh2_cosq_discard_get(int unit, uint32 *flags);
extern int bcmi_gh2_cosq_gport_discard_set(int unit, bcm_gport_t port,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_gport_discard_t *discard);
extern int bcmi_gh2_cosq_gport_discard_get(int unit, bcm_gport_t port,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_gport_discard_t *discard);
extern int bcmi_gh2_cosq_discard_port_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 color,
                                          int drop_start,
                                          int drop_slope,
                                          int average_time);
extern int bcmi_gh2_cosq_discard_port_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 color,
                                          int *drop_start,
                                          int *drop_slope,
                                          int *average_time);
extern int
bcmi_gh2_cosq_wred_port_control_set(int unit,
                                    bcm_gport_t gport,
                                    bcm_port_control_t type,
                                    int value);
extern int
bcmi_gh2_cosq_wred_port_control_get(int unit,
                                    bcm_gport_t gport,
                                    bcm_port_control_t type,
                                    int *value);


extern int bcmi_gh2_cosq_sched_weight_max_get(int unit, int mode,
                                              int *weight_max);
extern int bcmi_gh2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                        int mode, const int weights[],
                                        int delay);
extern int bcmi_gh2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                        int *mode, int weights[], int *delay);
extern int bcmi_gh2_cosq_mapping_set(int unit, bcm_port_t port,
                                     bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcmi_gh2_cosq_mapping_get(int unit, bcm_port_t port,
                                     bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcmi_gh2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             uint32 kbits_sec_min,
                                             uint32 kbits_sec_max, uint32 flags);
extern int bcmi_gh2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             uint32 *kbits_sec_min,
                                             uint32 *kbits_sec_max,
                                             uint32 *flags);
extern int bcmi_gh2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq, int mode,
                                         int weight);
extern int bcmi_gh2_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                                         bcm_cos_queue_t cosq, int *mode,
                                         int *weight);
extern int bcmi_gh2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                   bcm_cos_queue_t cosq,
                                                   uint32 kbits_burst);
extern int bcmi_gh2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                   bcm_cos_queue_t cosq,
                                                   uint32 *kbits_burst);
extern int bcmi_gh2_cosq_control_set(int unit, bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_control_t type, int arg);
extern int bcmi_gh2_cosq_control_get(int unit, bcm_gport_t gport,
                                     bcm_cos_queue_t cosq,
                                     bcm_cosq_control_t type, int *arg);
extern int bcmi_gh2_cosq_stat_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq,
                                  bcm_cosq_stat_t stat, int sync_mode,
                                  uint64 *value);
extern int bcmi_gh2_cosq_stat_set(int unit, bcm_gport_t port,
                                  bcm_cos_queue_t cosq,
                                  bcm_cosq_stat_t stat, uint64 value);
extern int bcmi_gh2_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                   uint32 flags, bcm_gport_t *gport);
extern int bcmi_gh2_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcmi_gh2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                        void *user_data);
extern int bcmi_gh2_cosq_gport_get(int unit, bcm_gport_t gport,
                                   bcm_gport_t *port,
                                   int *numq, uint32 *flags);
extern int bcmi_gh2_cosq_gport_attach(int unit, bcm_gport_t gport,
                                      bcm_gport_t to_gport,
                                      bcm_cos_queue_t to_cosq);
extern int bcmi_gh2_cosq_gport_detach(int unit, bcm_gport_t gport,
                                      bcm_gport_t input_gport,
                                      bcm_cos_queue_t cosq);
extern int bcmi_gh2_cosq_gport_attach_get(int unit, bcm_gport_t gport,
                                          bcm_gport_t *input_gport,
                                          bcm_cos_queue_t *cosq);
extern int bcmi_gh2_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                                         bcm_cos_queue_t cosq,
                                         bcm_gport_t *out_gport);
extern int bcmi_gh2_cosq_port_resolve(int unit, bcm_gport_t gport,
                                      bcm_module_t *modid,
                                      bcm_port_t *port, bcm_trunk_t *trunk_id,
                                      int *id);
extern int bcmi_gh2_cosq_index_resolve(int unit, bcm_gport_t gport, int *cosq);

extern int
bcmi_gh2_cosq_port_pfc_op(int unit,
                          bcm_port_t port,
                          bcm_switch_control_t sctype,
                          _bcm_cosq_op_t op,
                          int cosq);
extern int
bcmi_gh2_cosq_port_pfc_get(int unit,
                           bcm_port_t port,
                           bcm_switch_control_t sctype,
                           int *cosq);

extern int bcmi_gh2_preemption_capability_set(int unit, bcm_port_t port,
                                              uint32 arg);
extern int bcmi_gh2_preemption_queue_bitmap_set(int unit, bcm_port_t port,
                                                uint32 arg);
extern int bcmi_gh2_preemption_hold_request_mode_set(int unit, bcm_port_t port,
                                                     uint32 arg);
extern int bcmi_gh2_preemption_frag_config_tx_set(int unit, bcm_port_t port,
                                                  int is_final,
                                                  uint32 value);
extern int bcmi_gh2_preemption_capability_get(int unit, bcm_port_t port,
                                              uint32* arg);
extern int bcmi_gh2_preemption_queue_bitmap_get(int unit, bcm_port_t port,
                                                uint32* arg);
extern int bcmi_gh2_preemption_hold_request_mode_get(int unit, bcm_port_t port,
                                                     uint32* arg);
extern int bcmi_gh2_port_mac_type_get(int unit, soc_port_t port,
                                       bcmi_port_mac_type_t *port_type);
extern int bcmi_gh2_preemption_frag_config_tx_get(int unit, bcm_port_t port,
                                                  int is_final,
                                                  uint32* value);
extern int bcmi_gh2_cosq_event_mask_set(int unit,
                                        bcm_cosq_event_type_t event_type,
                                        int val);
extern int bcmi_gh2_cosq_events_validate(int unit,
                                         bcm_cosq_event_types_t event_types);

extern int bcmi_gh2_port_force_lb_set(int unit);
#if defined(INCLUDE_L3)
/* Library-private functions exported from vxlan.c */
extern int
bcmi_gh2_vxlan_udp_dest_port_set(
    int unit,
    bcm_switch_control_t type,
    int udp_destport);
extern int
bcmi_gh2_vxlan_udp_dest_port_get(
    int unit,
    bcm_switch_control_t type,
    int *udp_destport);
extern int
bcmi_gh2_vxlan_udp_source_port_set(
    int unit,
    bcm_switch_control_t type,
    int hash_enable);
extern int
bcmi_gh2_vxlan_udp_source_port_get(
    int unit,
    bcm_switch_control_t type,
    int *hash_enable);
extern int
bcmi_gh2_vxlan_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index);
extern int
bcmi_gh2_vxlan_tunnel_initiator_cmp(
    int unit,
    void *buf,
    int index,
    int *cmp_result);
extern int
bcmi_gh2_vxlan_tunnel_initiator_hash_calc(int unit, void *buf, uint16 *hash);

extern int
bcmi_gh2_vxlan_port_resolve(
    int unit,
    bcm_gport_t l2gre_port_id,
    bcm_if_t encap_id,
    bcm_module_t *modid,
    bcm_port_t *port,
    bcm_trunk_t *trunk_id,
    int *id);

extern int
bcmi_gh2_vxlan_lock(int unit);
extern void
bcmi_gh2_vxlan_unlock(int unit);
extern int
bcmi_gh2_vxlan_init(int unit);
extern int
bcmi_gh2_vxlan_cleanup(int unit);
extern int
bcmi_gh2_vxlan_vpn_create(int unit, bcm_vxlan_vpn_config_t *info);
extern int
bcmi_gh2_vxlan_vpn_destroy(int unit, bcm_vpn_t l2vpn);
extern int
bcmi_gh2_vxlan_vpn_destroy_all(int unit);
extern int
bcmi_gh2_vxlan_vpn_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_vxlan_vpn_config_t *info);
extern int
bcmi_gh2_vxlan_vpn_traverse(
    int unit,
    bcm_vxlan_vpn_traverse_cb cb,
    void *user_data);
extern int
bcmi_gh2_vxlan_port_traverse(
    int unit,
    bcm_vxlan_port_traverse_cb cb,
    void *user_data);
extern int
bcmi_gh2_vxlan_tunnel_terminator_create(
    int unit,
    bcm_tunnel_terminator_t *tnl_info);
extern int
bcmi_gh2_vxlan_tunnel_terminator_update(
    int unit,
    bcm_tunnel_terminator_t *tnl_info);
extern int
bcmi_gh2_vxlan_tunnel_terminator_destroy(
    int unit,
    bcm_gport_t vxlan_tunnel_id);
extern int
bcmi_gh2_vxlan_tunnel_terminator_destroy_all(int unit);
extern int
bcmi_gh2_vxlan_tunnel_terminator_get(
    int unit,
    bcm_tunnel_terminator_t *tnl_info);
extern int
bcmi_gh2_vxlan_tunnel_terminator_traverse(
    int unit,
    bcm_tunnel_terminator_traverse_cb cb,
    void *user_data);
extern int
bcmi_gh2_vxlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info);
extern int
bcmi_gh2_vxlan_tunnel_initiator_destroy(
    int unit,
    bcm_gport_t vxlan_tunnel_id);
extern int
bcmi_gh2_vxlan_tunnel_initiator_destroy_all(int unit);
extern int
bcmi_gh2_vxlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info);
extern int
bcmi_gh2_vxlan_tunnel_initiator_traverse(
    int unit,
    bcm_tunnel_initiator_traverse_cb cb,
    void *user_data);
extern int
bcmi_gh2_vxlan_trunk_member_delete(
    int unit,
    bcm_trunk_t trunk_id,
    int trunk_member_count,
    bcm_port_t *trunk_member_array);
extern int
bcmi_gh2_vxlan_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t  *vxlan_port);
extern int
bcmi_gh2_vxlan_port_delete(
    int unit,
    bcm_vpn_t vpn,
    bcm_gport_t vxlan_port_id);
extern int
bcmi_gh2_vxlan_port_delete_all(int unit, bcm_vpn_t vpn);
extern int
bcmi_gh2_vxlan_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_vxlan_port_t *vxlan_port);
extern int
bcmi_gh2_vxlan_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_vxlan_port_t *port_array,
    int *port_count);
extern int
bcmi_gh2_vxlan_port_to_nh_ecmp_index(
    int unit,
    int vp,
    int *ecmp,
    int *nh_ecmp_index);
extern int
bcmi_gh2_vxlan_port_from_nh_ecmp_index(
    int unit,
    int ecmp,
    int nh_ecmp_index,
    int *vp);
extern int
bcmi_gh2_vxlan_vfi_used_get(int unit, int vfi);
extern int
bcmi_gh2_vxlan_vfi_to_vpn_get(int unit, int vfi_index, bcm_vlan_t *vid);
extern int
bcmi_gh2_vxlan_vp_used_get(int unit,int vp);
extern int
bcmi_gh2_vxlan_egress_entry_mac_replace(
    int unit,
    int nh_index,
    bcm_l3_egress_t *egr);

extern int
bcmi_gh2_l3_intf_qos_set(
    int unit,
    uint32 *l3_if_entry_p,
    _bcm_l3_intf_cfg_t *intf_info);
extern int
bcmi_gh2_l3_intf_qos_get(
    int unit,
    uint32 *l3_if_entry_p,
    _bcm_l3_intf_cfg_t *intf_info);
extern int
bcmi_gh2_vlan_control_vpn_set(
    int unit,
    bcm_vlan_t vid,
    uint32 valid_fields,
    bcm_vlan_control_vlan_t * control);
extern int
bcmi_gh2_vlan_control_vpn_get(
    int unit,
    bcm_vlan_t vid,
    uint32 valid_fields,
    bcm_vlan_control_vlan_t * control);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcmi_gh2_vxlan_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void
bcmi_gh2_vxlan_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#endif /* INCLUDE_L3 */

#if defined(BCM_PREEMPTION_SUPPORT)
extern int bcmi_gh2_preemption_counter_dev_func_init(
               bcmi_preemption_counte_func_t* dev_func);
#endif /* BCM_PREEMPTION_SUPPORT */

#ifdef BCM_TAS_SUPPORT
extern int bcmi_gh2_tas_drv_init(int unit, const tas_drv_t **func);
extern int bcmi_gh2_tas_port_validate(int unit, bcm_port_t port);
#endif /* BCM_TAS_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
extern int
bcmi_gh2_tsn_info_init(
    int unit,
    const bcmi_esw_tsn_dev_info_t **devinfo);
extern int
bcmi_gh2_tsn_noninit_dev_info_get(
    int unit,
    const bcmi_esw_tsn_dev_info_t **devinfo);
extern int
bcmi_gh2_tsn_stat_info_init(
    int unit,
    const bcmi_esw_tsn_stat_dev_info_t **devinfo);
extern int
bcmi_gh2_taf_info_init(
    int unit,
    const bcmi_tsn_taf_dev_info_t **devinfo);
extern int
bcmi_gh2_tsn_info_detach(int unit);
#endif /* BCM_TSN_SUPPORT */

extern int
bcmi_gh2_global_meter_dev_info_init(
    int unit,
    const bcmi_global_meter_dev_info_t **dev_info);
extern int
bcmi_gh2_switch_chip_info_get(int unit,
                              bcm_switch_chip_info_t info_type,
                              int max_size,
                              void *data_buf,
                              int *actual_size);
#ifdef BCM_SWITCH_MATCH_SUPPORT
/* ################ Switch Match related ################### */
extern int
bcmi_gh2_switch_match_dev_info_init(
    int unit,
    const bcmi_switch_match_dev_info_t **dev_info);
#endif /* BCM_SWITCH_MATCH_SUPPORT */

#endif /* BCM_GREYHOUND2_SUPPORT */
#endif  /* !_BCM_INT_GREYHOUND2_H_ */

