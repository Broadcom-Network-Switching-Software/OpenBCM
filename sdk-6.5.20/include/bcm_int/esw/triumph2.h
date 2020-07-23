/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        triumph2.h
 * Purpose:     Function declarations for Triumph 2 bcm functions
 */

#ifndef _BCM_INT_TRIUMPH2_H_
#define _BCM_INT_TRIUMPH2_H_
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/vlan.h>
#include <soc/tnl_term.h>
#include <bcm/qos.h>



#define _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP    1
#define _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS      2
#define _BCM_QOS_MAP_TYPE_DSCP_TABLE         3
#define _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE     4
#define _BCM_QOS_MAP_TYPE_ING_QUEUE_OFFSET_MAP_TABLE   5
#define _BCM_QOS_MAP_TYPE_SERVICE_PRI_MAP_TABLE    6
#define _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP   7
#define _BCM_QOS_MAP_TYPE_EXP_TO_PHB_CNG_MAP 8

#if defined(INCLUDE_L3)
extern int _bcm_tr2_l3_tnl_term_add(int unit, uint32 *entry_ptr, 
                                    bcm_tunnel_terminator_t *tnl_info);
extern int _bcm_tr2_l3_tnl_term_entry_init(int unit, 
                                           bcm_tunnel_terminator_t *tnl_info,
                                           soc_tunnel_term_t *entry);
extern int _bcm_tr2_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info);
extern int _bcm_tr2_l3_intf_urpf_default_route_set(int unit, 
                                                   bcm_vlan_t vid,
                                                   int enable);
extern int _bcm_tr2_l3_intf_urpf_default_route_get (int unit, 
                                                    bcm_vlan_t vid,
                                                    int *enable);
extern int _bcm_tr2_l3_intf_urpf_mode_set(int unit, bcm_vlan_t vid,
                                          bcm_vlan_urpf_mode_t urpf_mode);
extern int _bcm_tr2_l3_intf_urpf_mode_get(int unit, bcm_vlan_t vid,
                                          bcm_vlan_urpf_mode_t *urpf_mode);

extern int bcm_tr2_l3_ecmp_defragment_buffer_init(int unit);
extern void bcm_tr2_l3_ecmp_defragment_buffer_deinit(int unit);
extern int bcm_tr2_l3_ecmp_defragment_buffer_set(int unit, int size);
extern int bcm_tr2_l3_ecmp_defragment_buffer_get(int unit, int *size);
extern int bcm_tr2_l3_ecmp_defragment(int unit);
extern int bcm_tr2_l3_ecmp_defragment_no_lock(int unit, int ecmp_level);

extern int _bcm_tr2_l3_ecmp_member_copy(int unit, int src_index, int dst_index);
extern int _bcm_tr2_l3_ecmp_member_clear(int unit, int index);
extern int _bcm_tr2_l3_ecmp_group_base_ptr_update(int unit, int group, int base_ptr);

extern int bcm_tr2_ipmc_repl_init(int unit);
extern int bcm_tr2_ipmc_repl_detach(int unit);
extern int bcm_tr2_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                         bcm_vlan_vector_t vlan_vec);
extern int bcm_tr2_ipmc_repl_get(int unit, int index, bcm_port_t port,
                         bcm_vlan_vector_t vlan_vec);
extern int bcm_tr2_ipmc_repl_add(int unit, int index, bcm_port_t port,
                         bcm_vlan_t vlan);
extern int bcm_tr2_ipmc_repl_delete(int unit, int index, bcm_port_t port,
                            bcm_vlan_t vlan);
extern int bcm_tr2_ipmc_repl_delete_all(int unit, int index,
                                bcm_port_t port);
extern int bcm_tr2_ipmc_egress_intf_add(int unit, int index, bcm_port_t port,
                                bcm_l3_intf_t *l3_intf);
extern int bcm_tr2_ipmc_egress_intf_delete(int unit, int index, bcm_port_t port,
                                   bcm_l3_intf_t *l3_intf);
extern int bcm_tr2_ipmc_egress_intf_set(int unit, int mc_index,
                                       bcm_port_t port, int if_count,
                                       bcm_if_t *if_array, int check_port);
extern int bcm_tr2_ipmc_egress_intf_get(int unit, int mc_index,
                                       bcm_port_t port,
                                       int if_max, bcm_if_t *if_array,
                                       int *if_count);
extern int _bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                                        int nh_index);
extern int _bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                           int if_max, int nh_index);
extern int _tr2_ipmc_glp_get(int unit, bcm_ipmc_addr_t *ipmc, ipmc_1_entry_t *entry);
extern int bcm_tr2_ipmc_repl_availability_get(int unit, int *available_percent);
extern int bcm_tr2_ipmc_repl_list_split(int unit, int array_size,
                    bcm_if_t *encap_id_array, int *sublist_id_array, 
                    int sublist_max, int *sublist_count);
extern int bcm_tr2_multicast_vpls_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                           bcm_gport_t mpls_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_subport_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                              bcm_gport_t subport, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_mim_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                          bcm_gport_t mim_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_multicast_wlan_encap_get(int unit, bcm_multicast_t group, bcm_gport_t port,
                                           bcm_gport_t wlan_port_id, bcm_if_t *encap_id);
extern int bcm_tr2_mim_init(int unit);
extern int bcm_tr2_mim_detach(int unit);
extern int bcm_tr2_mim_vpn_create(int unit, bcm_mim_vpn_config_t *info);
extern int bcm_tr2_mim_vpn_destroy(int unit, bcm_mim_vpn_t vpn);
extern int bcm_tr2_mim_vpn_destroy_all(int unit);
extern int bcm_tr2_mim_vpn_get(int unit, bcm_mim_vpn_t vpn, 
                              bcm_mim_vpn_config_t *info);
extern int bcm_tr2_mim_vpn_traverse(int unit, bcm_mim_vpn_traverse_cb cb, 
                                    void *user_data);
extern int bcm_tr2_mim_port_add(int unit, bcm_mim_vpn_t vpn, bcm_mim_port_t *mim_port);
extern int bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mim_port_id);
extern int bcm_tr2_mim_port_delete_all(int unit, bcm_vpn_t vpn);
extern int bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, bcm_mim_port_t *mim_port);
extern int bcm_tr2_mim_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                        bcm_mim_port_t *port_array, int *port_count);
extern int _bcm_tr2_mim_port_resolve(int unit, bcm_gport_t mim_port,
                                    bcm_module_t *modid, bcm_port_t *port,
                                    bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_mim_port_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_mim_port_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int _bcm_tr2_mim_svp_field_set(int unit, bcm_gport_t vp, 
                                      soc_field_t field, int value);
extern int bcm_tr2_wlan_init(int unit);
extern int bcm_tr2_wlan_detach(int unit);
extern int bcm_tr2_wlan_client_add(int unit, bcm_wlan_client_t *info);
extern int bcm_tr2_wlan_client_delete(int unit, bcm_mac_t mac);
extern int bcm_tr2_wlan_client_delete_all(int unit);
extern int bcm_tr2_wlan_client_traverse(int unit,
                                        bcm_wlan_client_traverse_cb cb,
                                        void *user_data);
extern int bcm_tr2_wlan_client_get(int unit, bcm_mac_t mac, 
                                   bcm_wlan_client_t *info);
extern int _bcm_tr2_wlan_port_resolve(int unit, bcm_gport_t wlan_port,
                                    bcm_module_t *modid, bcm_port_t *port,
                                    bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_wlan_port_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_wlan_port_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int bcm_tr2_wlan_port_add(int unit, bcm_wlan_port_t *wlan_port);
extern int bcm_tr2_wlan_port_delete(int unit, bcm_gport_t wlan_port_id);
extern int bcm_tr2_wlan_port_delete_all(int unit);
extern int bcm_tr2_wlan_port_get(int unit, bcm_gport_t wlan_port_id, 
                                 bcm_wlan_port_t *wlan_port);
extern int bcm_tr2_wlan_port_traverse(int unit,
                                      bcm_wlan_port_traverse_cb cb,
                                      void *user_data);
extern int bcm_tr2_wlan_tunnel_initiator_create(int unit, 
                                                bcm_tunnel_initiator_t *info);
extern int bcm_tr2_wlan_tunnel_initiator_destroy(int unit, 
                                                 bcm_gport_t wlan_tunnel_id);
extern int bcm_tr2_wlan_tunnel_initiator_get(int unit, 
                                             bcm_tunnel_initiator_t *info);
extern int bcm_tr2_tunnel_terminator_vlan_set(int unit, bcm_gport_t tunnel, 
                                              bcm_vlan_vector_t vlan_vec);
extern int bcm_tr2_tunnel_terminator_vlan_get(int unit, bcm_gport_t tunnel, 
                                              bcm_vlan_vector_t *vlan_vec);
extern int bcm_tr2_wlan_lport_field_set(int unit, bcm_gport_t wlan_port_id, 
                                        soc_field_t field, int value);
extern int bcm_tr2_wlan_lport_field_get(int unit, bcm_gport_t wlan_port_id, 
                                        soc_field_t field, int *value);
extern int _bcm_tr2_wlan_port_set(int unit, bcm_gport_t wlan_port_id, 
                                  soc_field_t field, uint32 value);
extern int bcm_tr2_wlan_port_untagged_vlan_get(int unit, bcm_gport_t port, 
                                               bcm_vlan_t *vid_ptr);
extern int bcm_tr2_wlan_port_untagged_vlan_set(int unit, bcm_gport_t port, 
                                               bcm_vlan_t vid);
extern int bcm_tr2_wlan_port_untagged_prio_get(int unit, bcm_gport_t port, 
                                               int *prio_ptr);
extern int bcm_tr2_wlan_port_untagged_prio_set(int unit, bcm_gport_t port, 
                                               int prio);
extern int _bcm_tr2_svp_field_set(int unit, bcm_gport_t port_id, 
                                  soc_field_t field, int value);
extern int _bcm_tr2_svp_field_get(int unit, bcm_gport_t port_id, 
                                  soc_field_t field, int *value);
extern int _bcm_tr2_mpls_svp_field_set(int unit, bcm_gport_t vp, 
                                       soc_field_t field, int value);
extern int bcm_tr2_mpls_label_stat_enable_set(int unit,
                                              bcm_mpls_label_t label, 
                                              bcm_gport_t port, int enable,
                                              int cnt_inx);
extern int _bcm_tr2_l3_ecmp_grp_get(int unit, int ecmp_grp, int ecmp_group_size, int *nh_idx);
extern int _bcm_tr2_l3_ecmp_grp_add(int unit, int ecmp_grp, void *buf, void *info);
extern int _bcm_tr2_l3_ecmp_grp_del(int unit, int ecmp_grp, void *info);
extern void bcm_tr2_failover_unlock(int unit);
extern int  bcm_tr2_failover_lock(int unit);
extern int  bcm_tr2_failover_init(int unit);
extern int  bcm_tr2_failover_sync(int unit);
extern int  bcm_tr2_failover_cleanup(int unit);
extern int  bcm_tr2_failover_create(int unit, uint32 flags, bcm_failover_t *failover_id);
extern int  bcm_tr2_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int  bcm_tr2_mmu_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int bcm_tr2_failover_set(int unit, bcm_failover_t failover_id, int value);
extern int bcm_tr2_failover_get(int unit, bcm_failover_t failover_id, int  *value);
extern int bcm_tr2_failover_prot_nhi_create (int unit, int nh_index);
extern int bcm_tr2_failover_prot_nhi_set(int unit, int nh_index, uint32 prot_nh_index, bcm_failover_t failover_id);
extern int bcm_tr2_failover_prot_nhi_get(int unit, int nh_index, bcm_failover_t  *failover_id, int *prot_nh_index);
extern int bcm_tr2_failover_prot_nhi_cleanup  (int unit, int nh_index);
extern int bcm_tr2_failover_prot_nhi_update  (int unit, int old_nh_index, int new_nh_index);
extern int bcm_tr2_failover_id_validate (int unit, bcm_failover_t failover_id);
extern int bcm_tr2_failover_mmu_id_validate (int unit, bcm_failover_t failover_id);
extern int bcm_tr2_failover_ecmp_prot_nhi_set(int unit, int ecmp, int index, int nh_index, 
                              bcm_failover_t failover_id, int prot_nh_index);
extern int bcm_tr2_failover_ecmp_prot_nhi_get(int unit, int ecmp, int index, int nh_index, 
                              bcm_failover_t *failover_id, int *prot_nh_index);
extern int bcm_tr2_failover_ecmp_prot_nhi_cleanup(int unit, int ecmp, int index,
                              int nh_index);
extern int bcm_tr2_failover_id_check(int unit, bcm_failover_t failover_id);
extern int bcm_tr2_failover_egr_check (int unit, bcm_l3_egress_t  *egr);
extern int bcm_tr2_failover_mpls_check (int unit, bcm_mpls_port_t  *mpls_port);

extern int bcm_tr2_vlan_virtual_init(int unit);
extern int bcm_tr2_vlan_virtual_detach(int unit);
extern int bcm_tr2_vlan_vp_create(int unit, bcm_vlan_port_t *vlan_vp);
extern int bcm_tr2_vlan_vp_destroy(int unit, bcm_gport_t gport);
extern int bcm_tr2_vlan_vp_find(int unit, bcm_vlan_port_t *vlan_vp);
extern int bcm_tr2_vlan_port_learn_set(int unit, bcm_gport_t vlan_port_id,
                                       uint32 flags);
extern int bcm_tr2_vlan_vp_match_add(int unit,
                         bcm_vlan_port_t *vlan_vp, int vp);
extern int bcm_tr2_vlan_vp_match_delete(int unit,
                         bcm_vlan_port_t *vlan_vp, int vp);
extern int _bcm_tr2_mim_match_add(int unit, bcm_mim_port_t *mim_port,
                                   int vp, bcm_mim_vpn_t vpn);
extern int _bcm_tr2_mim_match_delete(int unit, int vp, bcm_mim_port_t mim_port);
extern int bcm_tr2_vlan_port_phys_gport_get(int unit, int vp, bcm_gport_t *gp);
extern void  bcm_tr2_vlan_port_match_count_adjust(int unit, int vp, int step);
extern int _bcm_tr2_vlan_port_resolve(int unit, bcm_gport_t vlan_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t gport, 
                                   int is_untagged);
extern int bcm_tr2_vlan_gport_delete(int unit, bcm_vlan_t vlan,
                                      bcm_gport_t gport); 
extern int bcm_tr2_vlan_gport_delete_all(int unit, bcm_vlan_t vlan);
extern int bcm_tr2_vlan_gport_get(int unit, bcm_vlan_t vlan, bcm_gport_t gport,
                                   int *is_untagged); 
extern int bcm_tr2_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
        bcm_gport_t *gport_array, int *is_untagged_array, int *array_size);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_tr2_vlan_virtual_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_tr2_vlan_vp_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */

#if defined BCM_WARM_BOOT_SUPPORT
extern int _bcm_notify_qos_reinit_to_fp (int unit);
#endif

extern int bcm_tr2_cosq_init(int unit);
extern int bcm_tr2_cosq_detach(int unit, int software_state_only);
extern int bcm_tr2_cosq_config_set(int unit, int numq);
extern int bcm_tr2_cosq_config_get(int unit, int *numq);
extern int bcm_tr2_cosq_mapping_set(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_tr2_cosq_mapping_get(int unit, bcm_port_t port,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_tr2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);
extern int bcm_tr2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);
extern int bcm_tr2_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_tr2_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_tr2_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_tr2_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *burst);
extern int bcm_tr2_cosq_discard_set(int unit, uint32 flags);
extern int bcm_tr2_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_tr2_cosq_discard_port_set(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int drop_start, int drop_slope,
                                        int average_time);
extern int bcm_tr2_cosq_discard_port_get(int unit, bcm_port_t port,
                                        bcm_cos_queue_t cosq, uint32 color,
                                        int *drop_start, int *drop_slope,
                                        int *average_time);
extern int bcm_tr2_cosq_gport_discard_set(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr2_cosq_gport_discard_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq,
                                         bcm_cosq_gport_discard_t *discard);
extern int bcm_tr2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int weights[], int delay);
extern int bcm_tr2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int weights[], int *delay);
extern int bcm_tr2_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                                 uint32 flags, bcm_gport_t *gport);
extern int bcm_tr2_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_tr2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                                      void *user_data);

extern int bcm_tr2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                            uint32 flags);
extern int bcm_tr2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq, 
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                            uint32 *flags);
extern int bcm_tr2_cosq_gport_bandwidth_burst_set(int unit,
                                                  bcm_gport_t gport,
                                                  bcm_cos_queue_t cosq, 
                                                  uint32 kbits_burst_min,
                                                  uint32 kbits_burst_max);
extern int bcm_tr2_cosq_gport_bandwidth_burst_get(int unit,
                                                  bcm_gport_t gport,
                                                  bcm_cos_queue_t cosq, 
                                                  uint32 *kbits_burst_min,
                                                  uint32 *kbits_burst_max);
extern int bcm_tr2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, int mode,
                                       int weight);
extern int bcm_tr2_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                                       bcm_cos_queue_t cosq, int *mode,
                                       int *weight);
extern int bcm_tr2_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                                    bcm_cos_queue_t cosq, bcm_gport_t input_gport);
extern int bcm_tr2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                                        bcm_cos_queue_t *cosq, bcm_gport_t *input_gport);
extern int _bcm_tr2_cosq_port_resolve(int unit, bcm_gport_t gport,
                                     bcm_module_t *modid, bcm_port_t *port,
                                     bcm_trunk_t *trunk_id, int *id);

extern int _bcm_tr2_port_vlan_priority_map_set(int unit, bcm_port_t port, 
                                               int pkt_pri, int cfi,
                                               int internal_pri, 
                                               bcm_color_t color);
extern int _bcm_tr2_port_vlan_priority_map_get(int unit, bcm_port_t port, 
                                               int pkt_pri, int cfi,
                                               int *internal_pri, 
                                               bcm_color_t *color);
extern int _bcm_tr2_port_priority_color_set(int unit, bcm_port_t port, int prio, bcm_color_t color);

extern int bcm_tr2x_oam_init(int unit);
extern int bcm_tr2x_oam_detach(int unit);
extern int bcm_tr2x_oam_lock(int unit);
extern int bcm_tr2x_oam_unlock(int unit);
extern int bcm_tr2x_oam_group_create(int unit,
    bcm_oam_group_info_t *group_info);
extern int bcm_tr2x_oam_group_get(int unit, bcm_oam_group_t group, 
    bcm_oam_group_info_t *group_info);
extern int bcm_tr2x_oam_group_destroy(int unit, bcm_oam_group_t group);
extern int bcm_tr2x_oam_group_destroy_all(int unit);
extern int bcm_tr2x_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb, 
    void *user_data);
extern int bcm_tr2x_oam_endpoint_create(int unit, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_tr2x_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint, 
    bcm_oam_endpoint_info_t *endpoint_info);
extern int bcm_tr2x_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint);
extern int bcm_tr2x_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group);
extern int bcm_tr2x_oam_endpoint_traverse(int unit, bcm_oam_group_t group, 
    bcm_oam_endpoint_traverse_cb cb, void *user_data);
extern int bcm_tr2x_oam_event_register(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb, 
    void *user_data);
extern int bcm_tr2x_oam_event_unregister(int unit, 
    bcm_oam_event_types_t event_types, bcm_oam_event_cb cb);

extern int bcm_tr2_subport_init(int unit);
extern int bcm_tr2_subport_cleanup(int unit);
extern int bcm_tr2_subport_group_create(int unit, bcm_subport_group_config_t *config,
                            bcm_gport_t *group);
extern int bcm_tr2_subport_group_destroy(int unit, bcm_gport_t group);
extern int bcm_tr2_subport_group_get(int unit, bcm_gport_t group,
                            bcm_subport_group_config_t *config);
extern int bcm_tr2_subport_port_add(int unit, bcm_subport_config_t *config,
                            bcm_gport_t *port);
extern int bcm_tr2_subport_port_delete(int unit, bcm_gport_t port);
extern int bcm_tr2_subport_port_get(int unit, bcm_gport_t port,
                            bcm_subport_config_t *config);
extern int bcm_tr2_subport_port_traverse(int unit, bcm_subport_port_traverse_cb cb,
                             void *user_data);
extern int _bcm_tr2_subport_group_resolve(int unit, bcm_gport_t gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int _bcm_tr2_subport_port_resolve(int unit, bcm_gport_t gport,
                              bcm_module_t *modid, bcm_port_t *port,
                              bcm_trunk_t *trunk_id, int *id);
extern int bcm_tr2_subport_learn_set(int unit, bcm_gport_t port, uint32 flags);
extern int bcm_tr2_subport_learn_get(int unit, bcm_gport_t port, uint32 *flags);
extern int _bcm_tr2_port_lanes_set(int unit, bcm_port_t port, int value);
extern int _bcm_tr2_port_lanes_get(int unit, bcm_port_t port, int *value);
extern int _bcm_tr2_ing_pri_cng_map_default_entry_add(int unit, 
                                                      soc_profile_mem_t *prof);
extern int _bcm_tr2_port_vpd_bitmap_alloc(int unit);
extern void _bcm_tr2_port_vpd_bitmap_free(int unit);
extern int _bcm_port_vlan_prot_index_alloc(int unit, int *prof_ptr);
extern int _bcm_port_vlan_prot_index_free(int unit, int prof_ptr);

/* Flexible counters support */

#define _BCM_FLEX_STAT_TYPE(_id) (((_id) >> 16) & 0xFFFF)
#define _BCM_FLEX_STAT_COUNT_INX(_id) ((_id) & 0xFFFF)
#define _BCM_FLEX_STAT_COUNT_ID(_type,_ix) ((((_type) & 0xFFFF) << 16) | \
					((_ix) & 0xFFFF))

typedef enum _bcm_flex_stat_type_e {
    _bcmFlexStatTypeService,
    _bcmFlexStatTypeGport,
    _bcmFlexStatTypeEgressService,
    _bcmFlexStatTypeEgressGport,
    _bcmFlexStatTypeFp,
    _bcmFlexStatTypeVrf,
    _bcmFlexStatTypeVxlt,
    _bcmFlexStatTypeEgrVxlt,
    _bcmFlexStatTypeMplsLabel,
    _bcmFlexStatTypeNum,
     /* count ingress/egress as 1*/
    _bcmFlexStatTypeHwNum = _bcmFlexStatTypeEgressService, 
    _bcmFlexStatTypeAllHwNum = _bcmFlexStatTypeFp 
} _bcm_flex_stat_type_t;

#define _BCM_FLEX_STAT_HANDLE_WORDS 2

typedef struct {
    uint32 words[_BCM_FLEX_STAT_HANDLE_WORDS];
} _bcm_flex_stat_handle_t;

#define	_BCM_FLEX_STAT_HANDLE_WORD_GET(fsh, word)   ((fsh).words[(word)])
#define	_BCM_FLEX_STAT_HANDLE_WORD_SET(fsh, word, val)	\
                                                ((fsh).words[(word)] = (val))
#define	_BCM_FLEX_STAT_HANDLE_CLEAR(fsh) \
        (_BCM_FLEX_STAT_HANDLE_WORD_GET(fsh, 0) = \
         _BCM_FLEX_STAT_HANDLE_WORD_GET(fsh, 1) = 0)

#define _BCM_FLEX_STAT_HANDLE_ASSIGN(fsh1, fsh2) ((fsh1) = (fsh2))        
#define _BCM_FLEX_STAT_HANDLE_COPY(fsh1, fsh2) \
        memcpy(&(fsh1), &(fsh2), sizeof(_bcm_flex_stat_handle_t))        

typedef struct _bcm_flex_stat_handle_to_index_s {
    _bcm_flex_stat_handle_t handle;
    uint32 index;
    _bcm_flex_stat_type_t type;
} _bcm_flex_stat_handle_to_index_t;

typedef enum _bcm_flex_stat_memacc_type_e {
    _BCM_FS_MEMACC_SERVICE_ING_PKT,
    _BCM_FS_MEMACC_SERVICE_ING_BYTE,
    _BCM_FS_MEMACC_SERVICE_EGR_PKT,
    _BCM_FS_MEMACC_SERVICE_EGR_BYTE,
    _BCM_FS_MEMACC_VINTF_ING_PKT,
    _BCM_FS_MEMACC_VINTF_ING_BYTE,
    _BCM_FS_MEMACC_VINTF_EGR_PKT,
    _BCM_FS_MEMACC_VINTF_EGR_BYTE,
    _BCM_FS_MEMACC_NUM
} _bcm_flex_stat_memacc_type_t;

/* Structure for tracking flexible stat info */
typedef struct _bcm_flex_stat_info_s {
    int stat_mem_size;
    SHR_BITDCL  *stats;
    int handle_list_size;
    _bcm_flex_stat_handle_to_index_t *handle_list;
    uint16 *ref_cnt;   /* reference count */ 
} _bcm_flex_stat_info_t;

#define _BCM_FLEX_STAT_GRANS  2  /* Packets and Bytes */
#define _BCM_FLEX_STAT_DIRS   2  /* Ingress and Egress */
#define _BCM_FLEX_STAT_BUFFS  2  /* Ping pong fetch buffers */

typedef enum _bcm_flex_stat_e {
    _bcmFlexStatIngressPackets,  /* Packets that ingress on the class */
    _bcmFlexStatIngressBytes,    /* Bytes that ingress on the class */
    _bcmFlexStatEgressPackets,   /* Packets that egress on the class */
    _bcmFlexStatEgressBytes,     /* Bytes that egress on the class */
    _bcmFlexStatNum
} _bcm_flex_stat_t;

#define _BCM_FLEX_STAT_HW_INGRESS  (1 << 0)   /* apply to ingress chip */
#define _BCM_FLEX_STAT_HW_EGRESS   (1 << 1)   /* apply to egress chip */
#define _BCM_FLEX_STAT_HW_BOTH     (_BCM_FLEX_STAT_HW_INGRESS | \
                                    _BCM_FLEX_STAT_HW_EGRESS)

typedef int (*_bcm_flex_stat_src_index_set_f)(int unit,
                                              _bcm_flex_stat_handle_t handle,
                                              int fs_idx, void *cookie);

extern int _bcm_esw_flex_stat_detach(int unit, _bcm_flex_stat_type_t type);
extern int _bcm_esw_flex_stat_init(int unit, _bcm_flex_stat_type_t type);

extern void _bcm_esw_flex_stat_ext_reinit_add(int unit,
                                          _bcm_flex_stat_type_t type,
                                          int fs_idx,
                                          _bcm_flex_stat_handle_t handle);
extern void _bcm_esw_flex_stat_reinit_add(int unit,
                                          _bcm_flex_stat_type_t type,
                                          int fs_idx, uint32 handle);
extern void _bcm_esw_flex_stat_release_handles(int unit,
                                               _bcm_flex_stat_type_t type);
extern int _bcm_esw_flex_stat_stat_set(int unit,
                                       _bcm_flex_stat_type_t type,
                                       int fs_idx,
                                       bcm_vlan_stat_t vl_stat,
                                       uint64 val);
extern int _bcm_esw_flex_stat_stat_get(int unit,
                                       _bcm_flex_stat_type_t type,
                                       int fs_idx,
                                       bcm_vlan_stat_t vl_stat,
                                       uint64 *val);
extern void _bcm_esw_flex_stat_ext_handle_free(int unit,
                                           _bcm_flex_stat_type_t type,
                                           _bcm_flex_stat_handle_t handle);
extern void _bcm_esw_flex_stat_handle_free(int unit,
                                           _bcm_flex_stat_type_t type,
                                           uint32 handle);
extern int _bcm_esw_vlan_flex_stat_hw_index_set(int unit,
                                            _bcm_flex_stat_handle_t handle,
                                                int fs_idx, void *cookie);
extern int _bcm_esw_port_flex_stat_hw_index_set(int unit,
                                            _bcm_flex_stat_handle_t handle,
                                                int fs_idx, void *cookie);

extern int _bcm_esw_flex_stat_ext_enable_set(int unit,
                                         _bcm_flex_stat_type_t type,
                         _bcm_flex_stat_src_index_set_f fs_set_cb,
                                         void *cookie,
                                         _bcm_flex_stat_handle_t handle, 
                                         int enable,
                                         int cnt_inx);
extern int _bcm_esw_flex_stat_enable_set(int unit,
                                         _bcm_flex_stat_type_t type,
                         _bcm_flex_stat_src_index_set_f fs_set_cb,
                                         void *cookie,
                                         uint32 handle, 
                                         int enable,
                                         int cnt_inx);
extern int _bcm_esw_flex_stat_ext_get(int unit, int sync_mode, 
                                  _bcm_flex_stat_type_t type,
                                  _bcm_flex_stat_handle_t handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 *val);
extern int _bcm_esw_flex_stat_get(int unit, int sync_mode,
                                  _bcm_flex_stat_type_t type,
                                  uint32 handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 *val);
extern int _bcm_esw_flex_stat_ext_get32(int unit, int sync_mode,
                                    _bcm_flex_stat_type_t type,
                                    _bcm_flex_stat_handle_t handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 *val);
extern int _bcm_esw_flex_stat_get32(int unit, int sync_mode,
                                    _bcm_flex_stat_type_t type,
                                    uint32 handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 *val);
extern int _bcm_esw_flex_stat_ext_set(int unit, 
                                  _bcm_flex_stat_type_t type,
                                  _bcm_flex_stat_handle_t handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 val);
extern int _bcm_esw_flex_stat_set(int unit, 
                                  _bcm_flex_stat_type_t type,
                                  uint32 handle, 
                                  _bcm_flex_stat_t stat, 
                                  uint64 val);
extern int _bcm_esw_flex_stat_ext_set32(int unit, 
                                    _bcm_flex_stat_type_t type,
                                    _bcm_flex_stat_handle_t handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 val);
extern int _bcm_esw_flex_stat_set32(int unit, 
                                    _bcm_flex_stat_type_t type,
                                    uint32 handle, 
                                    _bcm_flex_stat_t stat, 
                                    uint32 val);
extern int _bcm_esw_flex_stat_ext_multi_get(int unit,
                                        _bcm_flex_stat_type_t type,
                                        _bcm_flex_stat_handle_t handle,
                                        int nstat,
                                        _bcm_flex_stat_t *stat_arr,
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_multi_get(int unit,
                                        _bcm_flex_stat_type_t type,
                                        uint32 handle,
                                        int nstat,
                                        _bcm_flex_stat_t *stat_arr,
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_ext_multi_get32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          _bcm_flex_stat_handle_t handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);
extern int _bcm_esw_flex_stat_multi_get32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          uint32 handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);
extern int _bcm_esw_flex_stat_ext_multi_set(int unit, 
                                        _bcm_flex_stat_type_t type,
                                        _bcm_flex_stat_handle_t handle, 
                                        int nstat, 
                                        _bcm_flex_stat_t *stat_arr, 
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_multi_set(int unit, 
                                        _bcm_flex_stat_type_t type,
                                        uint32 handle, 
                                        int nstat, 
                                        _bcm_flex_stat_t *stat_arr, 
                                        uint64 *value_arr);
extern int _bcm_esw_flex_stat_ext_multi_set32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          _bcm_flex_stat_handle_t handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);
extern int _bcm_esw_flex_stat_multi_set32(int unit, 
                                          _bcm_flex_stat_type_t type,
                                          uint32 handle, 
                                          int nstat, 
                                          _bcm_flex_stat_t *stat_arr, 
                                          uint32 *value_arr);
extern int _bcm_esw_flex_stat_free_index_assign(int unit,
                                                _bcm_flex_stat_type_t fs_type);
extern int _bcm_esw_flex_stat_count_index_remove(int unit, 
                                                 _bcm_flex_stat_type_t fs_type,
                                                  int fs_inx);
extern int _bcm_esw_port_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx,uint32 flags);
extern int _bcm_esw_mpls_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx,uint32 flags);
extern int _bcm_esw_mim_flex_stat_index_set(int unit, bcm_gport_t port,
                                            int fs_idx, uint32 flags);
extern int _bcm_esw_subport_flex_stat_index_set(int unit, bcm_gport_t port,
                                                int fs_idx, uint32 flags);
extern int _bcm_esw_wlan_flex_stat_index_set(int unit, bcm_gport_t port,
                                             int fs_idx,uint32 flags);
extern int _bcm_esw_vlan_flex_stat_index_set(int unit, bcm_vlan_t vlan,
                                             int fs_idx,uint32 flags);
extern int _bcm_tr_subport_gport_used(int unit, bcm_gport_t port);
extern int _bcm_tr2_vlan_qmid_set(int unit, bcm_vlan_t vlan, int qm_ptr,
                                  int use_inner_tag);
extern int _bcm_tr2_vlan_qmid_get(int unit, bcm_vlan_t vlan, int *qm_ptr,
                                  int *use_inner_tag);
extern int _bcm_tr2_qos_pvmid_set(int unit, bcm_vlan_t vlan, int pvmid);
extern int _bcm_tr2_qos_pvmid_get(int unit, bcm_vlan_t vlan, int *pvmid);
extern int _bcm_tr2_mim_port_phys_gport_get(int unit, int vp, bcm_gport_t *gp);
extern void _bcm_tr2_mim_port_match_count_adjust(int unit, int vp, int step);

typedef struct _bcm_vlan_translate_stat_info_s {
    bcm_gport_t port;
    bcm_vlan_translate_key_t key_type;
    bcm_vlan_t outer_vlan;
    bcm_vlan_t inner_vlan;
} _bcm_vlan_translate_stat_info_t;

extern int _bcm_tr2_vlan_translate_range_vp_add(int unit,
                                                bcm_gport_t port,
                                                bcm_vlan_t outer_vlan_low,
                                                bcm_vlan_t outer_vlan_high,
                                                bcm_vlan_t inner_vlan_low,
                                                bcm_vlan_t inner_vlan_high,
                                                int vp,
                                                bcm_vlan_action_set_t *action);
extern int _bcm_tr2_vlan_translate_range_vp_delete(int unit,
                                                bcm_gport_t port,
                                                bcm_vlan_t outer_vlan_low,
                                                bcm_vlan_t outer_vlan_high,
                                                bcm_vlan_t inner_vlan_low,
                                                bcm_vlan_t inner_vlan_high,                                                
                                                int vp);
extern int bcm_tr2_qos_init(int unit);
extern int bcm_tr2_qos_detach(int unit);
extern int bcm_tr2_qos_map_create(int unit, uint32 flags, int *map_id);
extern int bcm_tr2_qos_map_destroy(int unit, int map_id);
extern int bcm_tr2_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, 
                               int map_id, int internal);
extern int bcm_tr2_qos_map_delete(int unit, uint32 flags, 
                                  bcm_qos_map_t *map, int map_id);
extern int bcm_tr2_qos_multi_get(int unit, int array_size, int *map_ids_array,
                                 int *flags_array, int *array_count);
extern int bcm_tr2_qos_map_multi_get(int unit, uint32 flags,
                                     int map_id, int array_size, 
                                     bcm_qos_map_t *array, int *array_count);
extern int bcm_tr2_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, 
                                    int egr_map);
extern int bcm_tr2_qos_port_map_type_get(int unit, bcm_gport_t port, uint32 flags,
                                         int* map_id);
extern int bcm_tr2_qos_port_vlan_map_set(int unit, bcm_gport_t port, bcm_vlan_t vid,
                                        int ing_map, int egr_map);
extern int bcm_tr2_qos_port_vlan_map_get(int unit, bcm_gport_t port, bcm_vlan_t vid,
                                        int *ing_map, int *egr_map);
extern int _bcm_tr2_qos_id2idx(int unit, int map_id, int *hw_idx);
extern int _bcm_tr2_qos_idx2id(int unit, int hw_idx, int type, int *map_id);
extern int _bcm_tr2_port_tab_set(int unit, bcm_port_t port, 
                                 soc_field_t field, int value);
extern int _bcm_tr2_port_tab_get(int unit, bcm_port_t port,
                                 soc_field_t field, int* value);
extern int _bcm_tr2_vp_dscp_map_mode_set(int unit, bcm_gport_t port,
                                         int mode);
extern int _bcm_tr2_vp_dscp_map_mode_get(int unit, bcm_gport_t port,
                                         int *mode);
extern int _bcm_tr2_vp_dscp_map_set(int unit, bcm_gport_t port, int srccp, 
                                    int mapcp, int prio);
extern int _bcm_tr2_vp_dscp_map_get(int unit, bcm_gport_t port, int srccp, 
                                    int *mapcp, int *prio);
extern int _bcm_tr2_vp_ing_pri_cng_set(int unit, bcm_port_t port, int untagged,
                                       int pkt_pri, int cfi,
                                       int int_pri, bcm_color_t color);
extern int _bcm_tr2_vp_ing_pri_cng_get(int unit, bcm_port_t port, int untagged,
                                       int pkt_pri, int cfi,
                                       int *int_pri, bcm_color_t *color);
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_tr2_init(int unit, _field_control_t *fc);
extern int _bcm_field_tr2_stat_hw_mode_to_bmap(int unit, uint16 mode,
                                               _field_stage_id_t stage_id,
                                               uint32 *hw_bmap,
                                               uint8 *hw_entry_count);
#endif /* BCM_FIELD_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_tr2_cosq_sync(int unit);
extern int _bcm_tr2_qos_sync(int unit);
extern int _bcm_tr2_ipmc_repl_reload(int unit);
extern int _bcm_tr2x_oam_sync(int unit);
extern int bcm_tr2_l3_ecmp_defragment_buffer_wb_alloc_size_get(int unit,
        int *size);
extern int bcm_tr2_l3_ecmp_defragment_buffer_sync(int unit,
        uint8 **scache_ptr, int offset);
extern int bcm_tr2_l3_ecmp_defragment_buffer_recover(int unit,
        uint8 **scache_ptr, int offset);
extern int bcm_tr2_mim_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_tr2_cosq_sw_dump(int unit);
extern void _bcm_tr2_ipmc_repl_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_tr2_qos_sw_dump(int unit);
extern void _bcm_tr2x_oam_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_tr2_l3_ecmp_defragment_buffer_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
extern int bcm_tr2_ipmc_remap_set(int unit, bcm_multicast_t from, int to);
extern int bcm_tr2_ipmc_remap_get(int unit, bcm_multicast_t from, int *to);

extern int _bcm_tr3_vlan_vpn_is_eline( int unit, bcm_vpn_t vpn, uint8 *isEline);
extern int _bcm_tr3_vlan_vpn_is_valid( int unit, bcm_vpn_t vpn);
extern int _bcm_tr3_vlan_vp_destroy_all(int unit, bcm_vpn_t vpn);
extern int
_bcm_tr2_vlan_vp_port_cnt_update(int unit, bcm_gport_t gport, int vp, int incr);
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
int
bcm_td3_mpls_combo_add (int unit, uint32 flags, bcm_mpls_exp_map_t *mpls_map,
            int map_id, uint32 hw_idx, uint32 entry_per_index);
#endif
#endif /* BCM_TRIUMPH2_SUPPORT */
#endif  /* !_BCM_INT_TRIUMPH2_H_ */
