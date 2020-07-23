/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk2.h
 * Purpose:     Function declarations for Tomahawk2 Internal functions.
 */

#ifndef _BCM_INT_TOMAHAWK2_H_
#define _BCM_INT_TOMAHAWK2_H_

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)

#include <bcm/types.h>
#include <bcm/l3.h>
#include <soc/defs.h>
#include <soc/esw/port.h>
#include <bcm/failover.h>
#include <bcm/cosq.h>
#include <bcm/pstats.h>


#if defined(INCLUDE_L3)

/* L3 ECMP DLB */

typedef struct _th2_ecmp_dlb_port_membership_s {
    int nh_index;   /* ECMP DLB member next hop index */
    int ref_count;  /* Number of identical nexthops within DLB Group */
    int dlb_id;     /* DLB Group */
    struct _th2_ecmp_dlb_port_membership_s *next;
} _th2_ecmp_dlb_port_membership_t;

typedef struct _th2_ecmp_dlb_port_info_s {
    int nh_count;   /* Counting different next hops assigned to this port */
    int dlb_count;  /* Counting DLB Groups assigned to this port */
    _th2_ecmp_dlb_port_membership_t *port_membership_list;
              /* Linked list of ECMP DLB nh-indexes assigned to this
               * port and the ECMP DLB Groups the next hop
               * belong to.
               */
} _th2_ecmp_dlb_port_info_t;

typedef struct _th2_ecmp_dlb_bookkeeping_s {
    int ecmp_dlb_property_force_set;
    int ecmp_dlb_port_info_size;         /* Size of ecmp_dlb_port_info array */
    _th2_ecmp_dlb_port_info_t *ecmp_dlb_port_info;
    SHR_BITDCL *ecmp_dlb_id_used_bitmap;
    SHR_BITDCL *ecmp_dlb_id_dgm_bitmap;
    SHR_BITDCL *ecmp_dlb_flowset_block_bitmap; /* Each block corresponds to
                                                  256 entries */
    int ecmp_dlb_sample_rate;
    int ecmp_dlb_tx_load_min_th;
    int ecmp_dlb_tx_load_max_th;
    int ecmp_dlb_qsize_min_th;
    int ecmp_dlb_qsize_max_th;
    int ecmp_dlb_physical_qsize_min_th;
    int ecmp_dlb_physical_qsize_max_th;
    uint8 *ecmp_dlb_load_weight;
    uint8 *ecmp_dlb_qsize_weight;
    soc_profile_mem_t *ecmp_dlb_quality_map_profile;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 recovered_from_scache; /* Indicates if the following members of this
                                    structure have been recovered from scache:
                                    ecmp_dlb_sample_rate,
                                    ecmp_dlb_tx_load_min_th,
                                    ecmp_dlb_tx_load_max_th,
                                    ecmp_dlb_qsize_min_th,
                                    ecmp_dlb_qsize_max_th,
                                    ecmp_dlb_physical_qsize_min_th,
                                    ecmp_dlb_physical_qsize_max_th,
                                    ecmp_dlb_load_weight,
                                    ecmp_dlb_qsize_weight,
                                    ecmp_dlb_id_dgm_bitmap */
#endif /* BCM_WARM_BOOT_SUPPORT */
} _th2_ecmp_dlb_bookkeeping_t;

extern void _bcm_th2_ecmp_dlb_deinit(int unit);
extern int _bcm_th2_ecmp_dlb_init(int unit);
extern int bcm_th2_l3_egress_dlb_attr_set(int unit,
                                          int nh_index,
                                          bcm_l3_egress_t *egr,
                                          bcm_l3_egress_t *old_egr);
extern int bcm_th2_l3_egress_dlb_attr_destroy(int unit,
                                              int nh_index,
                                              bcm_l3_egress_t *egr);
extern int bcm_th2_l3_egress_dlb_attr_get(int unit,
                                          int nh_index,
                                          bcm_l3_egress_t *egr);
extern int bcm_th2_l3_egress_ecmp_dlb_create(int unit,
                                             bcm_l3_egress_ecmp_t *ecmp,
                                             int intf_count,
                                             bcm_if_t *intf_array,
                                             int primary_count,
                                             SHR_BITDCL *dlb_member_bitmap);
extern int bcm_th2_l3_egress_ecmp_dlb_destroy(int unit, bcm_if_t mpintf);
extern int bcm_th2_l3_egress_ecmp_dlb_get(int unit, bcm_l3_egress_ecmp_t *ecmp);
extern int _bcm_th2_ecmp_dlb_config_set(int unit,
                                        bcm_switch_control_t type,
                                        int arg);
extern int _bcm_th2_ecmp_dlb_config_get(int unit,
                                        bcm_switch_control_t type,
                                        int *arg);
extern int _bcm_th2_l3_egress_ecmp_port_status_set(int unit,
                                                   bcm_port_t port, int status);
extern int _bcm_th2_l3_egress_ecmp_port_status_get(int unit,
                                                   bcm_port_t port, int *status);
extern int bcm_th2_l3_egress_ecmp_member_status_set(int unit,
                                                    bcm_if_t intf,
                                                    int status);
extern int bcm_th2_l3_egress_ecmp_member_status_get(int unit,
                                                    bcm_if_t intf,
                                                    int *status);
extern int bcm_th2_l3_egress_ecmp_dlb_ethertype_set(int unit,
                                                    uint32 flags,
                                                    int ethertype_count,
                                                    int *ethertype_array);
extern int bcm_th2_l3_egress_ecmp_dlb_ethertype_get(int unit,
                                                    uint32 *flags,
                                                    int ethertype_max,
                                                    int *ethertype_array,
                                                    int *ethertype_count);
extern int bcm_th2_l3_ecmp_dlb_member_delete(int unit,
                                             bcm_l3_egress_ecmp_t *ecmp,
                                             bcm_if_t intf,
                                             bcm_if_t *intf_array,
                                             int *intf_count);
extern int bcm_th2_l3_ecmp_dlb_dgm_member_delete(int unit,
                                                 bcm_l3_egress_ecmp_t *ecmp,
                                                 bcm_l3_ecmp_member_t *ecmp_member,
                                                 bcm_l3_ecmp_member_t *ecmp_member_array,
                                                 int *ecmp_member_count);
extern int bcm_th2_l3_ecmp_dlb_member_create(int unit,
                                             bcm_l3_egress_ecmp_t *ecmp,
                                             bcm_if_t *intf_array,
                                             int intf_count);
extern int bcm_th2_l3_ecmp_dlb_dgm_member_create(int unit,
                                                 bcm_l3_egress_ecmp_t *ecmp,
                                                 bcm_l3_ecmp_member_t *ecmp_member_array,
                                                 int ecmp_member_count);
extern int bcm_th2_l3_ecmp_dlb_member_add(int unit,
                                          bcm_l3_egress_ecmp_t *ecmp,
                                          bcm_if_t intf, bcm_if_t *intf_array,
                                          int *intf_count);
extern int bcm_th2_l3_ecmp_dlb_dgm_member_add(int unit,
                                              bcm_l3_egress_ecmp_t *ecmp,
                                              bcm_l3_ecmp_member_t *ecmp_member,
                                              bcm_l3_ecmp_member_t *ecmp_member_array,
                                              int *ecmp_member_count);
extern int bcm_th2_l3_ecmp_dlb_member_get(int unit,
                                          bcm_l3_egress_ecmp_t *ecmp,
                                          int intf_size, bcm_if_t *intf_array,
                                          int *intf_count);
extern int bcm_th2_l3_ecmp_dlb_dgm_member_get(int unit,
                                              bcm_l3_egress_ecmp_t *ecmp_info,
                                              int ecmp_member_size,
                                              bcm_l3_ecmp_member_t *ecmp_member_array,
                                              int *ecmp_member_count);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_th2_ecmp_dlb_wb_alloc_size_get(int unit, int *size);
extern int bcm_th2_ecmp_dlb_member_sync(int unit, uint8 **scache_ptr);
extern int bcm_th2_ecmp_dlb_member_scache_recover(int unit, uint8 **scache_ptr);
extern int bcm_th2_ecmp_dlb_sync(int unit, uint8 **scache_ptr);
extern int bcm_th2_ecmp_dlb_scache_recover(int unit, uint8 **scache_ptr);
extern int bcm_th2_ecmp_dlb_hw_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_th2_ecmp_dlb_sw_dump(int unit);
extern void bcm_th2_dgm_sw_dump(int unit);
#endif /* ifndef BCM_SW_STATE_DUMP_DISABLE */

/* Number of protection switching groups
 * represented per entry in the PROT_SW table TX_INITIAL_PROT_GROUP_TABLE
 */
#define BCM_TH2_FAILOVER_PROT_GROUPS_PER_ENTRY 128
extern int _bcm_th2_failover_ingress_id_validate(int unit,
                                                  bcm_failover_t failover_id);

#define BCM_TH2_ECMP_IS_DLB_OR_DGM(_u_, _mode_) \
    ((soc_feature(_u_, soc_feature_ecmp_dlb_optimized) && \
     (((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) || \
      ((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) || \
      ((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL))) || \
     (soc_feature(_u_, soc_feature_dgm) && \
      ((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_DGM)))

#define BCM_TH2_ECMP_IS_DLB(_u_, _mode_) \
    (soc_feature(_u_, soc_feature_ecmp_dlb_optimized) && \
     (((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL) || \
      ((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED) || \
      ((_mode_) == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL)))

extern int _bcm_th2_failover_ingress_id_validate(int unit,
                                                  bcm_failover_t failover_id);

extern int  bcm_th2_failover_init(int unit);
extern void bcm_th2_failover_unlock(int unit);
extern int  bcm_th2_failover_lock(int unit);
extern int  bcm_th2_failover_cleanup(int unit);
extern int  bcm_th2_failover_create(int unit, uint32 flags,
                                    bcm_failover_t *failover_id);
extern int  bcm_th2_failover_destroy(int unit, bcm_failover_t  failover_id);
extern int  bcm_th2_failover_status_set(int unit,
                bcm_failover_element_t *failover, int value);
extern int  bcm_th2_failover_status_get(int unit,
                bcm_failover_element_t *failover, int  *value);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_th2_failover_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* INCLUDE_L3 */

extern int bcmi_th2_port_fn_drv_init(int unit);
extern int bcmi_th2_port_soc_resource_init(int unit, int nport,
                                 bcm_port_resource_t *resource,
                                 soc_port_resource_t *soc_resource);
extern int bcmi_th2_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_clport_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_port_attach_mmu(int unit, int port);
extern int bcmi_th2_port_detach_asf(int unit, bcm_port_t port);
extern int bcm_th2_ipmc_repl_port_attach(int unit, bcm_port_t port);
extern int bcm_th2_ipmc_repl_port_detach(int unit, bcm_port_t port);
extern int bcmi_th2_port_enable(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_port_disable(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_th2_port_speed_1g_update(int unit, bcm_port_t port, int speed);
extern int bcmi_th2_speed_ability_get(int unit, bcm_port_t port,
                                      bcm_port_abil_t *mask);
extern int bcmi_th2_tsc_xgxs_reset(int unit, pbmp_t pbmp);
extern int bcmi_th2_speed_mix_validate(int unit, uint32 speed_mask);

#if defined (INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)
extern int bcm_th2_cosq_tcb_init(int unit);
extern int _bcm_th2_cosq_tcb_deinit(int unit);
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern void bcm_th2_cosq_tcb_sw_dump(int unit);
#endif /* BCM_SW_STATE_DUMP_DISABLE */
extern int bcm_th2_cosq_tcb_config_set(int unit, bcm_cosq_buffer_id_t buffer_id, 
                                  bcm_cosq_tcb_config_t *config);
extern int bcm_th2_cosq_tcb_config_get(int unit, bcm_cosq_buffer_id_t buffer_id, 
                                  bcm_cosq_tcb_config_t *config);
extern int bcm_th2_cosq_tcb_gport_threshold_mapping_set(int unit, 
                                               bcm_gport_t port, 
                                               bcm_cos_queue_t cosq, 
                                               bcm_cosq_buffer_id_t buffer_id, 
                                               int index);
extern int bcm_th2_cosq_tcb_gport_threshold_mapping_get(int unit,
                                               bcm_gport_t port, 
                                               bcm_cos_queue_t cosq, 
                                               bcm_cosq_buffer_id_t buffer_id, 
                                               int *index);
extern int bcm_th2_cosq_tcb_threshold_profile_create(int unit, int flags,
                                            bcm_cosq_buffer_id_t buffer_id, 
                                            bcm_cosq_tcb_threshold_profile_t *threshold,
                                            int *index);
extern int bcm_th2_cosq_tcb_threshold_profile_get(int unit, 
                                         bcm_cosq_buffer_id_t buffer_id, int index,
                                         bcm_cosq_tcb_threshold_profile_t *threshold);
extern int bcm_th2_cosq_tcb_threshold_profile_destroy(int unit, 
                                         bcm_cosq_buffer_id_t buffer_id, int index);
extern int bcm_th2_cosq_tcb_buffer_multi_get(int unit, 
                                  bcm_cosq_buffer_id_t buffer_id, int count, 
                                  bcm_cosq_tcb_buffer_t *entries, int *num);
extern int bcm_th2_cosq_tcb_event_multi_get(int unit, 
                                 bcm_cosq_buffer_id_t buffer_id, int count, 
                                 bcm_cosq_tcb_event_t *entries, int *num, 
                                 int *overflow_count);
extern int bcm_th2_cosq_tcb_control_get(int unit, bcm_cosq_buffer_id_t buffer_id, 
                                        bcm_cosq_tcb_control_t type, int *arg);
extern int bcm_th2_cosq_tcb_enable(int unit, bcm_cosq_buffer_id_t buffer_id, 
                                          int enable);
extern int bcm_th2_cosq_tcb_freeze(int unit, bcm_cosq_buffer_id_t buffer_id, 
                                          int enable);
extern int bcm_th2_cosq_tcb_current_phase_get(int unit, 
                                   bcm_cosq_buffer_id_t buffer_id, int *status);
extern int bcm_th2_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id,
                                             int queue, bcm_gport_t *uc_gport);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_th2_cosq_tcb_reinit(int unit);
#endif
#endif /* INCLUDE_TCB && BCM_TCB_SUPPORT */

extern void bcm_th2_pstats_deinit(int unit);
extern int bcm_th2_pstats_init(int unit);

#endif /* BCM_TOMAHAWK2_SUPPORT  */

#endif /* !_BCM_INT_TOMAHAWK2_H_ */
