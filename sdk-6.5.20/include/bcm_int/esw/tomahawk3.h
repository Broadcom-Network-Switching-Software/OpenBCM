/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3.h
 * Purpose:     Function declarations for Tomahawk3 Internal functions.
 */

#ifndef _BCM_INT_TOMAHAWK3_H_
#define _BCM_INT_TOMAHAWK3_H_

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <bcm/types.h>
#include <bcm/oob.h>
#include <soc/esw/port.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/tomahawk3_dispatch.h>
#include <soc/tnl_term.h>
#include <bcm_int/esw/mpls.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>

#define _BCM_TH3_PORT_RATE_MIN_KILOBITS 32
#define _BCM_TH3_PORT_RATE_MAX_KILOBITS 400*1000*1000

extern int _th3_l2_init[BCM_MAX_NUM_UNITS];
/*
 * Define:
 *  TH3_L2_INIT
 * Purpose:
 *  Causes a routine to return BCM_E_INIT (or some other
 *  error) if L2 software module is not yet initialized.
 */
#define TH3_L2_INIT(unit) do {                             \
    if (_th3_l2_init[unit] < 0) return _th3_l2_init[unit]; \
    if (_th3_l2_init[unit] == 0) return BCM_E_INIT;        \
    } while (0);

extern int bcm_th3_l2_init(int unit);
extern int bcm_th3_l2_term(int unit);
extern int bcm_th3_l2_station_add(int unit, int *station_id,
                                  bcm_l2_station_t *station);
extern int bcm_th3_l2_station_get(int unit, int station_id,
                                  bcm_l2_station_t *station);
extern int bcm_th3_l2_station_delete(int unit, int station_id);
extern int bcm_th3_l2_station_delete_all(int unit);
extern int bcm_th3_l2_learn_start(int unit, int cfg_interval);
extern int _bcm_l2_match_ctrl_init(int unit);
extern void _bcm_l2_match_ctrl_deinit(int unit);
extern int _bcm_th3_l2_station_control_init(int unit);
extern int _bcm_th3_l2_station_control_deinit(int unit);
extern int _bcm_th3_l2_station_control_get(int unit,
                                           _bcm_l2_station_control_t **sc);
extern int bcm_th3_l2_station_size_get(int unit, int *size);
extern int bcm_th3_l2_station_traverse(int unit,
                                       bcm_l2_station_traverse_cb trav_fn,
                                       void *user_data);
extern int _bcm_th3_l2_station_entry_get(int unit, int sid,
                                         _bcm_l2_station_entry_t **ent_p);
extern int _bcm_th3_l2_station_reload(int unit);
extern int _bcm_th3_l2_station_entry_last_sid_set(int unit, int overlay, int sid);
extern int bcm_th3_l2_cache_init(int unit);
extern int bcm_th3_l2_addr_add(int unit, bcm_l2_addr_t *l2addr);
extern int _bcm_th3_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry);
extern int bcm_th3_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid);
extern int _bcm_th3_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
               int check_l2_only, bcm_l2_addr_t *l2addr);
extern int bcm_th3_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
               bcm_l2_addr_t *cf_array, int cf_max, int *cf_count);
extern int bcm_th3_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
              bcm_l2_addr_t *l2addr);
extern int _bcm_th3_l2_to_l2x(int unit, l2x_entry_t *l2x_entry,
               bcm_l2_addr_t *l2addr, int key_only);
extern int _bcm_th3_l2_gport_parse(int unit, bcm_l2_addr_t *l2addr,
               _bcm_l2_gport_params_t *params);
extern int _bcm_th3_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr,
               l2x_entry_t *l2x_entry);
extern int bcm_th3_mcast_addr_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_th3_mcast_addr_remove(int unit, sal_mac_addr_t mac,
                                     bcm_vlan_t vid);
extern int bcm_th3_mcast_port_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                                  bcm_mcast_addr_t *mcaddr);
extern int bcm_th3_mcast_init(int unit);
extern int _bcm_th3_mcast_detach(int unit);
extern int bcm_th3_mcast_addr_add_w_l2mcindex(int unit,
                                              bcm_mcast_addr_t *mcaddr);
extern int bcm_th3_mcast_addr_remove_w_l2mcindex(int unit,
                                                 bcm_mcast_addr_t *mcaddr);
extern int bcm_th3_mcast_port_add(int unit, bcm_mcast_addr_t *mcaddr);
extern int bcm_th3_mcast_port_remove(int unit, bcm_mcast_addr_t *mcaddr);
extern void _bcm_th3_l2_register_callback(int unit,
                                          int pflags,
                                          l2x_entry_t *entry_del,
                                          l2x_entry_t *entry_add,
                                          void *fn_data);
extern int _bcm_th3_l2_cross_connect_add(
    int unit, bcm_vlan_t outer_vlan, uint8 *destt, int *destv);
#if defined(BCM_WARM_BOOT_SUPPORT)
extern int _bcm_th3_l2_wb_sync(int unit);
extern int _bcm_th3_l2_cross_connect_entry_add_all(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_th3_l2_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int bcmi_th3_port_fn_drv_init(int unit);
extern int bcmi_th3_port_soc_resource_init(int unit, int nport,
                                bcm_port_resource_t *resource,
                                soc_port_resource_t *soc_resource);
extern int bcmi_th3_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_cdport_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_port_attach_mmu(int unit, int port);
extern int bcmi_th3_port_detach_asf(int unit, bcm_port_t port);
extern int bcmi_th3_port_enable(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_port_disable(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th3_port_speed_1g_update(int unit, bcm_port_t port, int speed);
extern int bcmi_th3_speed_ability_get(int unit, bcm_port_t port,
                                      bcm_port_abil_t *mask);
extern int bcmi_th3_tsc_xgxs_reset(int unit, pbmp_t pbmp);

/* FIELD */
#if defined(BCM_FIELD_SUPPORT)
extern int _bcm_field_th3_common_actions_init(int unit,
                                              _field_stage_t *stage_fc);
extern int _bcm_field_th3_extractors_init(int unit, _field_stage_t *stage_fc);
extern int _bcm_field_th3_qualifiers_init(int unit, _field_stage_t *stage_fc);
extern int
_bcm_field_th3_keygen_profile1_entry_pack(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg, int part, soc_mem_t mem,
                               ifp_key_gen_program_profile_entry_t *prof1_entry);
extern int
_bcm_field_th3_keygen_em_profile_entry_pack(int unit, _field_stage_t *stage_fc,
                                            _field_group_t *fg, int part, soc_mem_t mem,
                                            exact_match_key_gen_program_profile_entry_t *prof_entry);
extern int
_field_th3_presel_qualifiers_init(int unit, _field_stage_t *stage_fc);

extern int
_bcm_field_th3_udf_chunks_to_int_qset(int unit, uint32 udf_chunks_bmap,
                                     bcm_field_qset_t *qset);
extern int
_field_th3_qualify_IpFrag_get(int unit, bcm_field_entry_t entry,
                          bcm_field_qualify_t qual_id,
                          bcm_field_IpFrag_t *frag_info);
extern int
_field_th3_qualify_IpFrag(int unit, bcm_field_entry_t entry,
                      bcm_field_qualify_t qual_id,
                      bcm_field_IpFrag_t frag_info);
extern int
_bcm_field_th3_redirect_profile_get(int unit,
                                soc_profile_mem_t **redirect_profile);
extern int
_bcm_field_th3_redirect_profile_ref_count_get(int unit,
                                              int index,
                                              int *ref_count);
extern int
_bcm_field_th3_redirect_profile_delete(int unit, int index);

extern int
_bcm_field_th3_qual_class_size_get(int unit, bcm_field_qualify_t qual,
                                                   uint16 *class_size);
extern int
_field_th3_class_ctype_supported(int unit,
                                 _field_class_type_t ctype);
extern int
_bcm_field_th3_internal_em_group_create(int unit,
                                    _field_control_t *fc);
extern int
_bcm_field_th3_em_udf_keygen_mask_get(int unit,
                                      _field_group_t *fg,
                                      uint16 qid, uint32 *mask);

#endif

/* VLAN */
#define BCM_TH3_EGR_ACTION_PROFILE_DEFAULT  \
        BCM_TRX_EGR_VLAN_ACTION_PROFILE_DEFAULT

/* VLAN Port Bitmap Profile Mgmt function declarations */
extern int bcm_tomahawk3_vlan_detach(int unit);
extern int bcm_th3_vlan_port_add(
    int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp, pbmp_t ing_pbmp);
extern int bcm_th3_vlan_port_remove(
    int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp, pbmp_t ing_pbmp);
extern int _bcm_th3_vlan_pbmp_profile_init(int unit);
extern int _bcm_th3_vlan_pbmp_profile_destroy(int unit);
extern int _bcm_th3_vlan_pbmp_profile_entry_add(
                int unit, bcm_pbmp_t port_bitmap, uint32 *index);
extern int _bcm_th3_vlan_pbmp_profile_entry_delete(int unit, uint32 index);
extern int _bcm_th3_vlan_pbmp_profile_entry_get(
                int unit, uint32 index, bcm_pbmp_t *pbmp);
extern int _bcm_th3_vlan_pbmp_index_get(
                int unit, bcm_vlan_t vlan_id, uint32 *index);
extern int _bcm_th3_vlan_pbmp_index_set(
                int unit, bcm_vlan_t vlan_id, uint32 index);
extern int _bcm_th3_vlan_pbmp_port_add(
                int unit, void *entry, bcm_pbmp_t *pbmp);
extern int _bcm_th3_vlan_pbmp_port_remove(
                int unit, void *entry, bcm_pbmp_t *pbmp);
extern int _bcm_th3_vlan_action_profile_init(int unit);
extern int _bcm_th3_vlan_action_profile_detach(int unit);
extern int _bcm_th3_ing_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int *index);
extern void _bcm_th3_ing_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int index);
extern int _bcm_th3_ing_vlan_action_profile_entry_delete(
                                                    int unit, uint32 index);
extern int _bcm_th3_egr_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      int *index);
extern void _bcm_th3_egr_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 index);
extern int _bcm_th3_egr_vlan_action_profile_entry_delete(
                                                    int unit, uint32 index);
extern void _bcm_th3_egr_vlan_action_profile_entry_increment(
                                                    int unit, uint32 index);
extern int _bcm_th3_vlan_port_egress_default_action_set(int unit,
                            bcm_port_t port, bcm_vlan_action_set_t *action);
extern int _bcm_th3_vlan_port_egress_default_action_get(int unit,
                            bcm_port_t port, bcm_vlan_action_set_t *action);
extern int _bcm_th3_vlan_port_egress_default_action_delete(
                                                    int unit, bcm_port_t port);
extern int _bcm_th3_vlan_port_default_action_set(int unit,
                            bcm_port_t port, bcm_vlan_action_set_t *action);
extern int _bcm_th3_vlan_port_default_action_get(int unit,
                            bcm_port_t port, bcm_vlan_action_set_t *action);
extern int _bcm_th3_vlan_port_default_action_delete(int unit, bcm_port_t port);
extern int _bcm_th3_vlan_xconnect_db_init(int unit);
extern int _bcm_th3_vlan_xconnect_db_destroy(int unit);
extern int _bcm_th3_vlan_xconnect_db_entry_add(
    int unit, bcm_vlan_t ovlan, uint8 *destt, int *destv);
extern int _bcm_th3_vlan_xconnect_db_entry_delete(int unit, bcm_vlan_t ovlan);
extern int _bcm_th3_vlan_xconnect_db_entry_delete_all(int unit);
extern int _bcm_th3_vlan_xconnect_traverse(
    int unit, bcm_vlan_cross_connect_traverse_cb cb, void *user_data);
#if defined(BCM_WARM_BOOT_SUPPORT)
extern int _bcm_th3_vlan_sync(int unit);
extern int bcm_th3_cosq_sync(int unit);
extern int _bcm_th3_vlan_xconnect_db_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void _bcm_th3_vlan_sw_tag_action_profile_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* L3 */
#ifdef INCLUDE_L3
#define BCM_TH3_L3_ECMP_WEIGHTED_MAX_PATHS (4096)
#define BCM_TH3_L3_ECMP_NON_WEIGHTED_MAX_PATHS (128)
#define BCM_TH3_L3_ECMP_MAX_DLB_GROUPS (128)
#define BCM_TH3_L3_ECMP_DLB_MAX_PATHS (64)

/* When MAX_ECMP_MODE is configured, 2 entries are used up
   for each ECMP group, so need to reserve enough space in this
   mode by reserving 2 entries for each group */

#define BCM_TH3_L3_ECMP_DLB_START(_unit) \
                    ((soc_mem_index_count(unit, L3_ECMP_COUNTm)) -   \
                     BCM_TH3_L3_ECMP_MAX_DLB_GROUPS)

#define BCM_TH3_L3_ECMP_DLB_END(_unit) \
                    (soc_mem_index_max(unit, L3_ECMP_COUNTm))

/* ECMP LB mode encodings match hardware values */
#define BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_256  0x3
#define BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_512  0x4
#define BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_1024 0x5
#define BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_2048 0x6
#define BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_4096 0x7

extern int _bcm_th3_l3_ecmp_dlb_get_id(int unit, int ecmp_grp_idx);

extern int _bcm_th3_l3_egress_ecmp_dlb_resource_validate(int unit, 
                                  bcm_l3_egress_ecmp_t *ecmp, int dlb_id);

extern int
_bcm_th3_l3_ecmp_dlb_port_member_set(int unit, int *port_array, int primary_count,
                                     int unique_intfs, int dlb_id);

extern int
_bcm_th3_l3_ecmp_dlb_nhi_member_set(int unit, int *intf_array, int primary_count,
                                    int unique_intfs, int dlb_id);

extern int
_bcm_th3_l3_ecmp_dlb_port_get(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                 int dlb_id, int primary_count, int intf_count,
                                 bcm_if_t *intf_array, int *port_array);

extern int
_bcm_th3_l3_ecmp_dlb_scaling_factor_encode(int unit, int scaling_factor,
                                        int *scaling_factor_hw);

extern int
_bcm_th3_l3_ecmp_dlb_scaling_factor_decode(int unit, int scaling_factor_hw,
                                        int *scaling_factor);

extern int
_bcm_th3_ecmp_dlb_member_recover(int unit);

extern int
bcm_th3_l3_ecmp_dlb_mon_config_set(int unit,
                                   bcm_if_t ecmp_intf,
                                   bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

extern int
bcm_th3_l3_ecmp_dlb_mon_config_get(int unit,
                                   bcm_if_t intf,
                                   bcm_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg);

extern int _bcm_th3_l3_tnl_term_entry_init(int unit, bcm_tunnel_terminator_t *tnl_info,
                                soc_tunnel_term_t *entry);
extern int _bcm_th3_l3_tnl_term_add(int unit, uint32 *entry_ptr, bcm_tunnel_terminator_t *tnl_info);
extern int _bcm_th3_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info);
extern soc_profile_mem_t *_bcm_get_tnl_port_bitmap_profile(int unit);
extern int _bcm_tnl_port_bitmap_profile_init(int unit);

extern int bcm_th3_mpls_l3_nh_info_get(int unit, bcm_mpls_tunnel_switch_t *info, int nh_index);
extern int
_bcm_th3_mpls_egr_nh_label_set (int unit,
                                       bcm_mpls_tunnel_switch_t *info,
                                       bcm_l3_egress_t *egr_obj,
                                       int label_action,
                                       int hw_map_idx,
                                       egr_l3_next_hop_entry_t *pegr_nh);
extern int
bcm_th3_mpls_swap_nh_info_add (int unit, bcm_l3_egress_t *egr, int nh_index, uint32 flags);
extern int
bcm_th3_mpls_l3_label_add (int unit, bcm_l3_egress_t *egr, int nh_index, uint32 flags);


extern int bcm_th3_mpls_swap_nh_info_get(int unit, bcm_l3_egress_t *egr, int nh_index);

extern int bcm_th3_failover_init(int unit);
extern int bcm_th3_failover_cleanup(int unit);
extern int bcm_th3_failover_egress_set(int unit, bcm_if_t intf,
                                       bcm_l3_egress_t *failover_egr);
extern int bcm_th3_failover_egress_get(int unit, bcm_if_t intf,
                                       bcm_l3_egress_t *failover_egr);
extern int bcm_th3_failover_egress_clear(int unit, bcm_if_t intf);
extern int bcm_th3_failover_egress_status_set(int unit, bcm_if_t intf,
                                              int enable);
extern int bcm_th3_failover_egress_status_get(int unit, bcm_if_t intf,
                                              int *enable);

#endif /* INCLUDE_L3 */

extern int bcm_th3_trunk_modify(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_count,
        bcm_trunk_member_t *member_array, trunk_private_t *t_info,
        int op, bcm_trunk_member_t *member);
extern int bcm_th3_trunk_get(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info, int member_max,
        bcm_trunk_member_t *member_array, int *member_count,
        trunk_private_t *t_info);
extern int bcm_th3_trunk_destroy(int unit, bcm_trunk_t tid,
        trunk_private_t *t_info);
extern int bcm_th3_trunk_mcast_join(int unit, bcm_trunk_t tid,
        bcm_vlan_t vid, sal_mac_addr_t mac, trunk_private_t *t_info);

/* Cosq */

extern int
_bcm_th3_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                           int *p_pool_start, int *p_pool_end);
extern int _bcm_th3_cosq_port_inp_pri_to_pg_profile_set(int unit,
                    bcm_port_t port, int profile_id);
extern int _bcm_th3_cosq_port_inp_pri_to_pg_profile_get(int unit,
                    bcm_port_t port, int *profile_id);
extern int _bcm_th3_cosq_port_pg_profile_set(int unit,
                    bcm_port_t port, int profile_id);
extern int _bcm_th3_cosq_port_pg_profile_get(int unit,
                    bcm_port_t port, int *profile_id);

extern void bcm_th3_cosq_sw_dump(int unit);
extern int _bcm_th3_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port);
extern int bcm_th3_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id);
extern int _bcm_th3_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum);
extern int _bcm_th3_cosq_gport_tree_create(int unit, bcm_port_t port);
extern int bcm_th3_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport);
extern int bcm_th3_cosq_gport_delete(int unit, bcm_gport_t gport);
extern int bcm_th3_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq);
extern int bcm_th3_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                            bcm_cos_queue_t cosq,
                            bcm_gport_t *out_gport);

extern int bcm_th3_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                                      int mode, const int *weights, int delay);
extern int bcm_th3_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                                      int *mode, int *weights, int *delay);
extern int bcm_th3_cosq_config_get(int unit, int *numq);
extern int bcm_th3_cosq_config_set(int unit, int numq);
extern int bcm_th3_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                         bcm_gport_t parent_gport, bcm_cos_queue_t cosq);
extern int bcm_th3_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                           bcm_cos_queue_t cosq, int mode, int weight);
extern int bcm_th3_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int *mode, int *weight);
extern int bcm_th3_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data);
extern int bcm_th3_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                                               bcm_cosq_gport_traverse_cb cb,
                                               void *user_data);
extern int bcm_th3_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);
extern int bcm_th3_cosq_sched_weight_max_get(int unit, int mode,
                                            int *weight_max);
extern int bcm_th3_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                                 int *numq, uint32 *flags);
/* WRED API's */
extern int bcm_th3_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time);
extern int bcm_th3_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time);
extern int bcm_th3_cosq_discard_set(int unit, uint32 flags);
extern int bcm_th3_cosq_discard_get(int unit, uint32 *flags);
extern int bcm_th3_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_buffer_id_t buffer,
                              bcm_cosq_gport_discard_t *discard);
extern int bcm_th3_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_buffer_id_t buffer,
                              bcm_cosq_gport_discard_t *discard);
extern int bcm_th3_cosq_wred_resolution_entry_create(int unit, int *index);
extern int bcm_th3_cosq_wred_resolution_entry_destroy(int unit, int index);
extern int bcm_th3_cosq_wred_resolution_set(int unit, int index,
                                           bcm_cosq_discard_rule_t *rule);
extern int bcm_th3_cosq_wred_resolution_get(int unit, int index, int max,
                                           bcm_cosq_discard_rule_t *rule,
                                           int *count);

/* Shaper API's */
/* Utility routines for metering and egress rate control */
#define _BCM_TH3_METER_FLAG_NON_LINEAR           0x01
#define _BCM_TH3_METER_FLAG_PACKET_MODE          0x02
#define _BCM_TH3_METER_FLAG_FP_POLICER           0x04

extern uint32 _bcm_th3_default_burst_size(int unit, int port, uint32 kbits_sec);
extern uint32 _bcm_th3_bucket_encoding_to_rate(int unit, uint32 refresh_rate,
                                uint32 bucket_max, uint32 granularity,
                                uint32 flags, uint32 *rate_quantum,
                                uint32 *burst_quantum);
extern int _bcm_th3_rate_to_bucket_encoding(int unit, uint32 rate_quantum,
                                uint32 burst_quantum, uint32 flags,
                                int refresh_bitsize, int bucket_max_bitsize,
                                uint32 *refresh_rate, uint32 *bucketsize,
                                uint32 *granularity);
extern int bcm_th3_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 kbits_sec_min,
                                           uint32 kbits_sec_max,
                                           uint32 flags);
extern int bcm_th3_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           uint32 *kbits_sec_min,
                                           uint32 *kbits_sec_max,
                                           uint32 *flags);
extern int bcm_th3_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 kbits_burst_min,
                                                 uint32 kbits_burst_max);
extern int bcm_th3_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 uint32 *kbits_burst_min,
                                                 uint32 *kbits_burst_max);
extern int bcm_th3_cosq_port_burst_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int burst);
extern int bcm_th3_cosq_port_burst_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq,
                                      int *burst);
extern int bcm_th3_cosq_port_pps_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int pps);
extern int bcm_th3_cosq_port_pps_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *pps);
extern int bcm_th3_port_rate_egress_set(int unit, bcm_port_t port,
                                       uint32 bandwidth, uint32 burst,
                                       uint32 mode);
extern int bcm_th3_port_rate_egress_get(int unit, bcm_port_t port,
                                       uint32 *bandwidth, uint32 *burst,
                                       uint32 *mode);
extern int bcmi_th3_device_speed_check(int unit, int speed, int lanes);
extern int bcmi_tomahawk3_reconfigure_ports(int unit,
            soc_port_schedule_state_t *port_schedule_state);
extern int
bcmi_th3_port_fast_reboot_epc_link_bmap_save(int unit, pbmp_t mask);



extern int bcm_th3_cosq_init(int unit);
extern int bcm_th3_cosq_detach(int unit, int software_state_only);

typedef enum {
    _BCM_TH3_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_TH3_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TH3_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TH3_COSQ_INDEX_STYLE_COS,
    _BCM_TH3_COSQ_INDEX_STYLE_COUNT
} _bcm_th3_cosq_index_style_t;

extern int bcm_th3_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq, bcm_cosq_dir_t direction,
                                  int array_max, bcm_cosq_buffer_id_t *buf_id_array,
                                  int *array_count);
extern int bcm_th3_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq,
                                           int enable);
extern int bcm_th3_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq,
                                           int *enable);
extern int bcm_th3_cosq_control_set(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_buffer_id_t buffer,
                                   bcm_cosq_control_t type, int arg);
extern int bcm_th3_cosq_control_get(int unit, bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_buffer_id_t buffer,
                                   bcm_cosq_control_t type, int *arg);
extern int
bcm_th3_cosq_service_pool_set(int unit, bcm_service_pool_id_t id,
                                      bcm_cosq_service_pool_t cosq_service_pool);

extern int
bcm_th3_cosq_service_pool_get(int unit, bcm_service_pool_id_t id,
                                      bcm_cosq_service_pool_t *cosq_service_pool);

extern int bcm_th3_cosq_mapping_set(int unit, bcm_port_t gport,
                                   bcm_cos_t priority, bcm_cos_queue_t cosq);
extern int bcm_th3_cosq_mapping_get(int unit, bcm_port_t gport,
                                   bcm_cos_t priority, bcm_cos_queue_t *cosq);
extern int bcm_th3_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t gport,
                                         bcm_cos_queue_t cosq);
extern int bcm_th3_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                                         bcm_cos_t int_pri, uint32 flags,
                                         bcm_gport_t *gport,
                                         bcm_cos_queue_t *cosq);

extern int _bcm_th3_get_num_ucq(int unit);
extern int _bcm_th3_get_num_mcq(int unit);
extern int _bcm_th3_cosq_pipe_get(int unit, bcm_gport_t gport, int *pipe);
extern int _bcm_th3_mmu_set_shared_size(int unit, int itm, int shd_size);
extern int _bcm_th3_mmu_get_shared_size(int unit, int itm, int *shd_size);
extern int _bcm_th3_cosq_ingress_sp_set_by_pg(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t pri_grp, int arg);
extern int _bcm_th3_cosq_ingress_hdrm_pool_set_by_pg(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t pri_grp, int arg);
extern int _bcm_th3_cosq_ingress_sp_get_by_pg(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t pri_grp, int *arg);
extern int _bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t pri_grp, int *arg);
extern int _bcm_th3_cosq_egr_pool_set(int unit, bcm_gport_t gport, 
                          bcm_cos_queue_t cosq,
                          bcm_cosq_buffer_id_t buffer,
                          bcm_cosq_control_t type, int arg);
extern int _bcm_th3_cosq_egr_pool_get(int unit, bcm_gport_t gport, 
                          bcm_cos_queue_t cosq,
                          bcm_cosq_buffer_id_t buffer,
                          bcm_cosq_control_t type, int *arg);
extern int _bcm_th3_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_th3_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count);
extern int _bcm_th3_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg);
extern int _bcm_th3_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int* arg);
extern int _bcm_th3_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int arg);
extern int _bcm_th3_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int* arg);
extern int _bcm_th3_cosq_qgroup_limit_enables_set(int unit, bcm_gport_t gport,
                               bcm_cos_t cosq, bcm_cosq_control_t type,
                               int arg);
extern int _bcm_th3_cosq_qgroup_limit_enables_get(int unit, bcm_gport_t gport,
                               bcm_cos_t cosq, bcm_cosq_control_t type,
                               int* arg);
extern int _bcm_th3_cosq_qgroup_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int arg);
extern int _bcm_th3_cosq_qgroup_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int* arg);

extern int bcm_th3_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 *min_quantum,
                                          uint32 *max_quantum,
                                          uint32 *burst_quantum,
                                          uint32 *flags);

extern int bcm_th3_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                          bcm_cos_queue_t cosq,
                                          uint32 min_quantum,
                                          uint32 max_quantum,
                                          uint32 burst_quantum,
                                          uint32 flags);

extern int _bcm_th3_cosq_sched_node_child_get(int unit, bcm_gport_t sched_gport,
            int *num_uc_child, int *uc_queue, int *num_mc_child, int *mc_queue);

/* Classifier */
extern int
bcm_th3_cosq_field_classifier_get(
    int unit, int classifier_id, bcm_cosq_classifier_t *classifier);

extern int
bcm_th3_cosq_field_classifier_id_create(
    int unit, bcm_cosq_classifier_t *classifier, int *classifier_id);

extern int
bcm_th3_cosq_field_classifier_id_destroy(
    int unit, int classifier_id);

extern int
bcm_th3_cosq_field_classifier_map_set(
    int unit, int classifier_id, int count,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_th3_cosq_field_classifier_map_get(
    int unit, int classifier_id,
    int array_max, bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_th3_cosq_field_classifier_map_clear(
    int unit, int classifier_id);

/* Flexport */
extern int bcm_th3_cosq_port_detach(int unit, bcm_port_t port);
extern int bcm_th3_cosq_port_attach(int unit, bcm_port_t port);

/* SW WAR for COSQ */
extern int
_bcm_th3_cosq_field_reserved_classifier_get(int unit,
                                            int *classifer_id,
                                            bcm_port_t *port);

extern int
_bcm_field_th3_qualify_udf_get(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length);

extern int
_bcm_field_th3_qualify_udf(
    int unit,
    bcm_field_entry_t eid,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask);

/* IPMC Repl */
#define TH3_AGG_ID_HW_INVALID 0xff
#define TH3_IPMC_MAX_INTF_COUNT_PER_ENTRY 64
/*
 * Max interface count - 256
 * Fixed budgetfor mirror copies - 8
 * Fixed budget for copy to CPU - 1
 * Limit replication copy count 256-8-1
 */
#define TH3_IPMC_MAX_INTF_COUNT 247

extern int
bcm_th3_ipmc_repl_port_attach(int unit, bcm_port_t port);

extern int
bcm_th3_ipmc_repl_port_detach(int unit, bcm_port_t port);

extern int
bcm_th3_repl_list_start_ptr_get(int unit, int repl_group, bcm_port_t port,
            int *start_ptr);
extern int
bcm_th3_repl_list_start_ptr_set(int unit, int repl_group, bcm_port_t port,
                int start_ptr, int intf_count);
typedef int (*bcm_th3_repl_head_alloc_f)(int, int, int, int *);
extern bcm_th3_repl_head_alloc_f bcm_th3_repl_head_alloc;

typedef int (*bcm_th3_repl_head_free_f) (int, int, int, int);
extern bcm_th3_repl_head_free_f bcm_th3_repl_head_free;

#if defined (BCM_FIELD_SUPPORT)
extern int
_bcm_field_th3_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual);
extern int
_bcm_field_th3_qualify_ip_type_encode_get(int unit, uint32 hw_data,
                                                     uint32 hw_mask,
                                                     bcm_field_IpType_t *type);
extern int
_bcm_field_th3_forwardingType_set(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 bcm_field_ForwardingType_t type,
                                 uint32 *data, uint32 *mask);
extern int
_bcm_field_th3_forwardingType_get(int unit, _field_stage_id_t stage_id,
                                 bcm_field_entry_t entry,
                                 uint32 data,  uint32 mask,
                                 bcm_field_ForwardingType_t *type);
extern int
_bcm_field_th3_qualify_ip_type(int unit, bcm_field_entry_t entry,
                               bcm_field_IpType_t type,
                               bcm_field_qualify_t qual);

extern int
_bcm_field_th3_qualify_ip_type_encode_get(int unit,
                                          uint32 hw_data,
                                          uint32 hw_mask,
                                          bcm_field_IpType_t *type);

extern int
_field_th3_stage_action_support_check(int unit,
                                     unsigned stage,
                                     bcm_field_action_t action,
                                     int *result);

extern int _bcm_field_th3_qualify_LoopbackType(
        bcm_field_LoopbackType_t loopback_type,
        uint32                   *tcam_data,
        uint32                   *tcam_mask);

extern int
_bcm_field_th3_qualify_LoopbackType_get(
            uint8                    tcam_data,
            uint8                    tcam_mask,
            bcm_field_LoopbackType_t *loopback_type);

extern int
_bcm_field_th3_qualify_TunnelType(bcm_field_TunnelType_t tunnel_type,
                                  uint32                 *tcam_data,
                                  uint32                 *tcam_mask
                                  );

extern int
_bcm_field_th3_qualify_TunnelType_get(uint8                  tcam_data,
                                      uint8                  tcam_mask,
                                      bcm_field_TunnelType_t *tunnel_type
                                      );
#endif /* BCM_FIELD_SUPPORT */

/* OOBFC */
extern int
bcm_th3_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                              int status, int dir);
extern int
bcm_th3_oob_fc_tx_port_control_get(int unit, bcm_port_t localport,
                                 int *status, int dir);

extern int
bcm_th3_oob_fc_tx_config_get(int unit,
                            bcm_oob_fc_tx_config_t *config);

extern int
bcm_th3_oob_fc_tx_config_set(int unit,
                            bcm_oob_fc_tx_config_t *config);


extern int
bcm_th3_oob_fc_tx_info_get(int unit, bcm_oob_fc_tx_info_t *info);
extern int bcm_oob_th3_init(int unit);

extern int
bcm_th3_oob_port_mapping_init(int unit);

extern int
bcm_th3_oob_fx_egress_profile_id_set(int unit, bcm_port_t port,
                                           int profile_id);
extern
int bcm_th3_oob_fx_egress_profile_id_get(int unit, bcm_port_t port,
                                           int *profile_id);

/* PSTATS */
extern int bcm_th3_pstats_init(int unit);
extern void  bcm_th3_pstats_deinit(int unit);

/* MMU counter APIs */
extern int bcm_th3_cosq_stat_set(int unit, bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, uint64 value);
extern int bcm_th3_cosq_stat_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_stat_t stat, int sync_mode,
                                uint64 *value);
extern int
_bcm_field_th3_ddrop_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa);
extern int
_bcm_field_th3_dredirect_profile_alloc(int unit, _field_entry_t *f_ent,
                                   _field_action_t *fa);

/* ETRAP APIs */
extern int _bcm_th3_etrap_init(int unit);

extern int _bcm_th3_etrap_detach(int unit);

extern int
_bcm_th3_etrap_action_match_set(int unit,
                                bcm_flowtracker_elephant_match_action_t action,
                                uint32 match_types,
                                bcm_flowtracker_elephant_match_data_t *match_data);

extern int
_bcm_th3_etrap_action_match_get(int unit,
                                bcm_flowtracker_elephant_match_action_t action,
                                uint32 *match_types,
                                bcm_flowtracker_elephant_match_data_t *match_data);

extern int
_bcm_th3_etrap_int_pri_remap_set(int unit,
                                 bcm_cos_t int_pri,
                                 bcm_cos_t new_int_pri);

extern int
_bcm_th3_etrap_int_pri_remap_get(int unit,
                                 bcm_cos_t int_pri,
                                 bcm_cos_t *new_int_pri);

extern int
_bcm_th3_etrap_control_set(int unit,
                           bcm_flowtracker_elephant_control_t type,
                           int arg);

extern int
_bcm_th3_etrap_control_get(int unit,
                           bcm_flowtracker_elephant_control_t type,
                           int *arg);

extern int
_bcm_th3_etrap_hash_config_set(int unit,
                               bcm_flowtracker_elephant_hash_table_t hash_table,
                               int instance_num,
                               int bank_num,
                               bcm_flowtracker_elephant_hash_type_t hash_type,
                               int right_rotate_bits);

extern int
_bcm_th3_etrap_hash_config_get(int unit,
                               bcm_flowtracker_elephant_hash_table_t hash_table,
                               int instance_num,
                               int bank_num,
                               bcm_flowtracker_elephant_hash_type_t *hash_type,
                               int *right_rotate_bits);

/* Warmboot */
extern int bcm_th3_cosq_burst_monitor_dma_config_set(int unit,
                                             uint32 host_mem_size,
                                             uint32 **host_mem_add);
extern int bcm_th3_cosq_burst_monitor_dma_config_get(int unit,
                                             uint32 *host_mem_size,
                                             uint32 **host_mem_add);
extern int bcm_th3_cosq_burst_monitor_dma_config_reset(int unit);
extern int bcm_th3_cosq_reinit(int unit);
extern int _bcm_th3_cosq_scheduler_reinit(int unit);
#if defined(BCM_WARM_BOOT_SUPPORT)
extern int _bcm_th3_tunnel_reinit(int unit, uint8 **tnl_scache);
extern int _bcm_th3_tunnel_warm_boot_alloc(int unit, uint32 *sz);
extern int _bcm_th3_tunnel_sync(int unit, uint8 **tnl_scache);
#endif

#endif /* BCM_TOMAHAWK3_SUPPORT  */

#endif /* !_BCM_INT_TOMAHAWK3_H_ */

