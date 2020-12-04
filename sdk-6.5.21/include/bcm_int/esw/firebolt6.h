/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt6.h
 * Purpose:     Function declarations for firebolt6 Internal functions.
 */

#ifndef _BCM_INT_FIREBOLT6_H_
#define _BCM_INT_FIREBOLT6_H_

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/cosq.h>
#include <bcm/oob.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/esw/field.h>
#include <soc/profile_mem.h>

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/esw/port.h>
#include <soc/firebolt6.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/trident3.h>

#define _BCM_MBR_ENCODE_VALUES_NUM          4

typedef enum {
    _BCM_FB6_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_FB6_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_FB6_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_FB6_COSQ_INDEX_STYLE_COS,
    _BCM_FB6_COSQ_INDEX_STYLE_COUNT,
    _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER
} _bcm_fb6_cosq_index_style_t;

extern int bcmi_fb6_port_fn_drv_init(int unit);
extern int bcmi_fb6_speed_ability_get(int unit, bcm_port_t port,
                                         bcm_port_abil_t *mask);
extern int
bcm_firebolt6_port_obm_info_get(int unit, bcm_port_t port,
                                int *obm_id, int *lane);
/* COSQ Module */
extern int
bcm_fb6_cosq_init(int unit);


extern int
bcm_fb6_port_param_check(int unit, bcm_port_t port);

extern int
bcm_fb6_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                       uint32 flags, bcm_gport_t *gport);
extern int
bcm_fb6_cosq_gport_delete(int unit, bcm_gport_t gport);

extern int
bcm_fb6_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_fb6_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_fb6_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

extern int
bcm_fb6_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data);

extern int
bcm_fb6_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                       int *numq, uint32 *flags);

extern int
bcm_fb6_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);
extern int
_bcm_fb6_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                           bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                           int *qnum);
extern int
bcm_fb6_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id);

extern int
bcm_fb6_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data);

extern int
bcm_fb6_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int burst);

extern int
bcm_fb6_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int *burst);

extern int
bcm_fb6_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int pps);

extern int
bcm_fb6_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int *pps);

extern int
_bcm_fb6_cosq_index_resolve(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, _bcm_fb6_cosq_index_style_t style,
                            bcm_port_t *local_port, int *index, int *count);
extern int
bcm_fb6_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_fb6_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int *mode, int *weight);

extern int
bcm_fb6_cosq_gport_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int mode, int weight);

extern int
bcm_fb6_cosq_port_sched_get(int unit, bcm_pbmp_t pbm, int *mode, int *weights,
                            int *delay);

extern int
bcm_fb6_cosq_port_sched_set(int unit, bcm_pbmp_t pbm, int mode, const int
                            *weights, int delay);

extern int
bcm_fb6_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_fb6_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                 uint32 kbits_sec_max, uint32 flags);

extern int
bcm_fb6_cosq_port_bandwidth_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 *min_quantum, uint32 *max_quantum,
                                uint32 *burst_quantum, uint32 *flags);

extern int
bcm_fb6_cosq_port_bandwidth_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 min_quantum, uint32 max_quantum,
                                uint32 burst_quantum, uint32 flags);

extern int
bcm_fb6_cosq_config_get(int unit, int *numq);

extern int
bcm_fb6_cosq_config_set(int unit, int numq);

extern int
bcm_fb6_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t cosq);

extern int
bcm_fb6_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t *cosq);

extern int
bcm_fb6_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int
bcm_fb6_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t *gport, bcm_cos_queue_t *cosq);

extern int
bcm_fb6_cosq_discard_set(int unit, uint32 flags);

extern int
bcm_fb6_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_fb6_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int drop_start, int drop_slope,
                              int average_time);

extern int
bcm_fb6_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int *drop_start, int *drop_slope,
                              int *average_time);

extern int
bcm_fb6_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_fb6_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_fb6_cosq_wred_resolution_entry_create(int unit, int *index);

extern int
bcm_fb6_cosq_wred_resolution_entry_destroy(int unit, int index);

extern int
bcm_fb6_cosq_wred_resolution_set(int unit, int index,
                                 bcm_cosq_discard_rule_t *rule);

extern int
bcm_fb6_cosq_wred_resolution_get(int unit, int index, int max,
                                 bcm_cosq_discard_rule_t *rule,
                                 int *count);

extern int bcm_fb6_cosq_control_set(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int arg);

extern int bcm_fb6_cosq_control_get(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int *arg);

extern int
bcm_fb6_cosq_field_classifier_get(int unit, int classifier_id,
        bcm_cosq_classifier_t *classifier);

extern int
bcm_fb6_cosq_field_classifier_id_create(int unit, bcm_cosq_classifier_t
        *classifier, int *classifier_id);

extern int
bcm_fb6_cosq_field_classifier_id_destroy(int unit, int classifier_id);

extern int
bcm_fb6_cosq_field_classifier_map_set(int unit, int classifier_id, int count,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_fb6_cosq_field_classifier_map_get(int unit, int classifier_id,int array_max,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_fb6_cosq_field_classifier_map_clear(int unit, int classifier_id);

extern int
bcm_fb6_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable);

extern int
bcm_fb6_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq,
                                 int *enable);

extern int
bcm_fb6_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count);

extern int
bcm_fb6_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count);

extern int
bcm_fb6_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_fb6_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array,
                                   int *array_count);

extern int
_bcm_fb6_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        uint32 flags, bcm_port_t *ing_port,
        bcm_cos_t *priority);

extern int
bcm_fb6_pfc_deadlock_queue_info_get(int unit, bcm_gport_t gport, bcm_port_t *local_port,
         int *type, int *split, int *index, int *offset);

extern int
_bcm_fb6_pfc_deadlock_q_config_helper(int unit,
        _bcm_pfc_deadlock_oper_t operation,
        bcm_gport_t gport,
        bcm_cosq_pfc_deadlock_queue_config_t *config,
        uint8 *enable_status);


/* pfc deadlock */
#define FB6_PFC_DEADLOCK_MAX_COS 32

/* Below structure to contain register/field name specific to chip */
typedef struct _bcm_fb6_pfc_hw_resorces_s {
    soc_reg_t timer_count[FB6_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_status[FB6_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_mask[FB6_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_en[FB6_PFC_DEADLOCK_MAX_COS];
    soc_reg_t chip_config[4];
    soc_reg_t port_config[4];
    soc_field_t cos_field[FB6_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_init_val[FB6_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_unit_field;
    soc_field_t recovery_action_field;
    soc_field_t deadlock_status_field;
} _bcm_fb6_pfc_hw_resorces_t;

typedef struct _bcm_fb6_pfc_deadlock_control_s {
    uint8 pfc_deadlock_cos_max;  /* Max COS supported for PFC Deadlock
                                  * Detection and Recovery */
    uint8 pfc_deadlock_cos_used; /* Total COS configured for PFC Deadlock
                                  * Detection and Recovery */
    uint8 hw_cos_idx_inuse[FB6_PFC_DEADLOCK_MAX_COS];   /* Boolean for used Cos indices */
    uint8 cb_enabled;            /* Indicator if CB is enabled */
    bcm_switch_pfc_deadlock_detection_interval_t time_unit; /* 0 - 100ms,
                                                             * 1 - 10ms */
    uint32 cb_interval;          /* CB interval duration */
    bcm_cos_t pfc_pri2cos[FB6_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    bcm_cos_t pfc_cos2pri[FB6_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    _bcm_pfc_deadlock_config_t cos_config[FB6_PFC_DEADLOCK_MAX_COS];
    _bcm_fb6_pfc_hw_resorces_t hw_regs_fields;
    bcm_switch_pfc_deadlock_action_t recovery_action;
    uint32 cb_count; /* Debug ctr to keep track of #times CB fn is called */
    _bcm_cosq_inv_mapping_get_f cosq_inv_mapping_get; /* Function to retrieve
                                                       * Port/Priority from a
                                                       * given Gport */
} _bcm_fb6_pfc_deadlock_control_t;

#define _BCM_FB6_PFC_DEADLOCK_HW_RES_INIT(deadlock_hw)        \
    do {                                                      \
        int i = 0;                                            \
        for (i = 0; i < FB6_PFC_DEADLOCK_MAX_COS; i++) {      \
            (deadlock_hw)->timer_count[i] = (INVALIDr);       \
            (deadlock_hw)->timer_status[i] = (INVALIDr);      \
            (deadlock_hw)->timer_mask[i] = (INVALIDr);        \
            (deadlock_hw)->timer_en[i] = (INVALIDr);          \
            (deadlock_hw)->cos_field[i] = (INVALIDf);         \
            (deadlock_hw)->time_init_val[i] = (INVALIDf);     \
        }                                                     \
        (deadlock_hw)->port_config[0] = (INVALIDr);              \
        (deadlock_hw)->port_config[1] = (INVALIDr);              \
        (deadlock_hw)->chip_config[0] = (INVALIDr);           \
        (deadlock_hw)->chip_config[1] = (INVALIDr);           \
        (deadlock_hw)->time_unit_field = (INVALIDf);          \
        (deadlock_hw)->recovery_action_field = (INVALIDf);    \
        (deadlock_hw)->deadlock_status_field = (INVALIDf);    \
    } while(0);

#define _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(u) _bcm_fb6_pfc_deadlock_control[(u)]
#define _BCM_FB6_PFC_DEADLOCK_CONFIG(u,cos) \
            (&((_bcm_fb6_pfc_deadlock_control[(u)])->cos_config[(cos)]))
#define _BCM_FB6_PFC_DEADLOCK_COS_ENABLED(u,cos) \
            ((((_bcm_fb6_pfc_deadlock_control[(u)])->cos_config[(cos)].flags) & \
            _BCM_PFC_DEADLOCK_F_ENABLE) ? TRUE : FALSE)
#define _BCM_FB6_PFC_DEADLOCK_CB_INTERVAL(u) \
            ((_bcm_fb6_pfc_deadlock_control[(u)])->cb_interval)
#define _BCM_FB6_PFC_DEADLOCK_CB_ENABLED(u) \
            (((_bcm_fb6_pfc_deadlock_control[(u)])->cb_enabled) ? (TRUE) :(FALSE))
#define _BCM_FB6_PFC_DEADLOCK_CB_COUNT(u) \
            ((_bcm_fb6_pfc_deadlock_control[(u)])->cb_count)
#define _BCM_FB6_PFC_DEADLOCK_TIME_UNIT(u) \
            ((_bcm_fb6_pfc_deadlock_control[(u)])->time_unit)
#define _BCM_FB6_PFC_DEADLOCK_RECOVERY_ACTION(u) \
            ((_bcm_fb6_pfc_deadlock_control[(u)])->recovery_action)

extern _bcm_fb6_pfc_deadlock_control_t *_bcm_fb6_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

extern int _bcm_fb6_pfc_deadlock_recovery_reset(int unit);

extern int
_bcm_fb6_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_fb6_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg);

extern int bcm_fb6_pfc_deadlock_recovery_start(int unit,
                                                   bcm_port_t port, int pri);
extern int bcm_fb6_pfc_deadlock_recovery_exit(int unit,
                                                  bcm_port_t port, int pri);


extern int _bcm_fb6_pfc_deadlock_init(int unit);
extern int _bcm_fb6_pfc_deadlock_hw_res_init(int unit);

extern int _bcm_fb6_pfc_deadlock_deinit(int unit);
extern int _bcm_fb6_pfc_deadlock_reinit(int unit);

extern int
_bcm_fb6_pfc_deadlock_hw_cos_index_get(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_fb6_pfc_deadlock_hw_cos_index_set(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_fb6_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config);
extern int
_bcm_fb6_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config);
extern int
_bcm_fb6_pfc_deadlock_q_config_helper(int unit,
                                      _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status);
extern int
bcm_fb6_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                                        bcm_port_t port, uint32 *rval32);
extern int
_bcm_fb6_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port);
extern int
_bcm_fb6_pfc_deadlock_monitor(int unit);
extern int bcmi_fb6_port_mmu_attach(int unit, int port);
extern int bcm_fb6_cosq_port_attach(int unit, bcm_port_t port);
extern int bcm_fb6_cosq_port_detach(int unit, bcm_port_t port);
extern int bcmi_fb6_port_cosmap_update(int unit, pbmp_t pbmp, int enable);

extern void bcm_fb6_pstats_deinit(int unit);
extern int  bcm_fb6_pstats_init(int unit);

extern int
bcm_fb6_cosq_stat_set(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, uint64 value);
extern int
bcm_fb6_cosq_stat_get(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, int sync_mode,
                      uint64 *value);

extern int
bcm_fb6_cosq_gport_info_get(int unit, bcm_gport_t gport,
                             bcm_cosq_gport_level_info_t *info);

extern int
bcm_fb6_cosq_subport_congestion_config_set(int unit, bcm_gport_t gport,
                                           bcm_cosq_subport_congestion_type_t congestion_type,
                                           bcm_cosq_subport_congestion_config_t *congestion_config);

extern int
bcm_fb6_cosq_subport_congestion_config_get(int unit, bcm_gport_t gport,
                                           bcm_cosq_subport_congestion_type_t congestion_type,
                                           bcm_cosq_subport_congestion_config_t *congestion_config);
extern int
bcm_fb6_cosq_subport_pfc_class_mapping_set(int unit, bcm_gport_t gport,
                                           bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map,
                                           int *cos_to_pfc_map_profile_id);
extern int
bcm_fb6_cosq_subport_pfc_class_mapping_get(int unit, bcm_gport_t gport,
                                           bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map);

extern int
bcm_fb6_cosq_safc_class_mapping_set(int unit, bcm_gport_t port, int array_count,
                                bcm_cosq_safc_class_mapping_t *mapping_array);

extern int
bcm_fb6_cosq_safc_class_mapping_get(int unit, bcm_gport_t port, int array_max,
                                bcm_cosq_safc_class_mapping_t *mapping_array,
                                int *array_count);

extern void bcm_fb6_cosq_sw_dump(int unit);
bcm_error_t
bcmi_fb6_repl_head_icc_move(int unit, int old_head_index,
                            int new_head_index);

extern int
bcm_fb6_dump_port_hsp(int unit, bcm_port_t port);

extern int
bcm_fb6_dump_subport_hsp(int unit, bcm_port_t port, int subport);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_fb6_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#if 0
#define TD3_ECMP_MODE_SINGLE 0x0
#define TD3_ECMP_MODE_HIERARCHICAL 0x1

/* entries_per_profile comes from MAXIDX in PHB_MAPPING_TBL_1
 * MAXIDX     => 8703,  #(136 ports x 64 profiles) */
#define _BCM_TD3_PHB_MAP_TAB1_ENTRIES_PER_PROFILE       (132)

extern int _bcm_td3_port_phb_map_tab1_default_entry_add
               (int unit, soc_profile_mem_t *prof);


extern int bcm_td3_port_init(int unit);

extern int bcm_td3_port_ing_pri_cng_set(int unit, bcm_port_t port,
                                        int untagged, int pkt_pri, int cfi,
                                        int int_pri, bcm_color_t color);
extern int bcm_td3_port_ing_pri_cng_get(int unit, bcm_port_t port,
                                        int untagged, int pkt_pri, int cfi,
                                        int *int_pri, bcm_color_t *color);

extern int bcm_td3_port_cfg_init(int unit, bcm_port_t port,
                                 bcm_vlan_data_t *vlan_data);
extern int bcm_td3_port_cfg_get(int, bcm_port_t, bcm_port_cfg_t *);
extern int
bcm_td3_port_cfg_set(int unit, bcm_port_t port, bcm_port_cfg_t *port_cfg);

extern int _bcm_td3_egr_port_tab_conv(int unit, soc_field_t *field,
                                      soc_mem_t* mem);
extern int _bcm_td3_port_tab_conv(int unit, soc_field_t *field, soc_mem_t *mem);

/* VLAN Module */
extern int 
_bcm_vlan_vfi_untag_profile_entry_op(int unit, void **entries, 
                                     int entries_per_set, int add, uint32 *index);
extern int 
_bcm_vlan_vfi_untag_profile_mem_reference(int unit, int idx,int entries_per_set);
extern int 
_bcm_vlan_vfi_untag_profile_ref_count(int unit, int index, int *ref_count);
extern int 
bcm_td3_vlan_vfi_untag_init(int unit, bcm_vlan_t vid, pbmp_t pbmp);

extern int
_bcm_td3_vlan_vfi_profile_entry_set(int unit, bcm_vlan_t vlan_vfi,
                                    void *entry_data);
extern int
bcm_td3_vlan_vfi_untag_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp);
extern int
bcm_td3_vlan_vfi_untag_delete(int unit, bcm_vlan_t vid, pbmp_t pbmp);
extern int
bcm_td3_vlan_table_ut_port_get(int unit, bcm_vlan_t vid, pbmp_t *ut_pbmp);
extern int
bcm_td3_vlan_vfi_untag_destroy(int unit, bcm_vlan_t vlan);

/* VLAN XLATE */
extern int _bcm_td3_vlan_mac_action_add(int unit, bcm_mac_t mac, 
                                        bcm_vlan_action_set_t *action);
extern int
_bcm_td3_vlan_mac_action_get(int unit, bcm_mac_t mac,
                             bcm_vlan_action_set_t *action);
extern int
_bcm_td3_vlan_mac_delete(int unit, bcm_mac_t mac);

extern int
_bcm_td3_vlan_mac_delete_all(int unit);

extern int
_bcm_td3_vlan_mac_action_traverse(int unit,
                                  bcm_vlan_mac_action_traverse_cb cb,
                                  void *user_data);

extern int
bcm_td3_cosq_config_get(int unit, int *numq);

extern int
bcm_td3_cosq_config_set(int unit, int numq);

extern int
bcm_td3_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_td3_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_td3_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

extern int
bcm_td3_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data);

extern int
bcm_td3_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);

typedef enum {
    _BCM_TD3_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_TD3_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_TD3_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_TD3_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_TD3_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_TD3_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_TD3_COSQ_INDEX_STYLE_COS,
    _BCM_TD3_COSQ_INDEX_STYLE_COUNT
} _bcm_td3_cosq_index_style_t;

extern int
_bcm_td3_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                           bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                           int *qnum);

extern int
_bcm_td3_cosq_port_cos_resolve(int unit, bcm_port_t port, bcm_cos_t cos,
                               _bcm_td3_cosq_index_style_t style,
                               bcm_gport_t *gport);

extern int
bcm_td3_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_td3_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int *mode, int *weight);

extern int
bcm_td3_cosq_gport_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int mode, int weight);

extern int
bcm_td3_cosq_port_sched_get(int unit, bcm_pbmp_t pbm, int *mode, int *weights,
                            int *delay);

extern int
bcm_td3_cosq_port_sched_set(int unit, bcm_pbmp_t pbm, int mode, const int
                            *weights, int delay);

extern int
bcm_td3_cosq_discard_set(int unit, uint32 flags);

extern int
bcm_td3_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_td3_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int drop_start, int drop_slope,
                              int average_time);

extern int
bcm_td3_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int *drop_start, int *drop_slope,
                              int *average_time);

extern int
bcm_td3_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_td3_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_td3_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_td3_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                 uint32 kbits_sec_max, uint32 flags);

extern int
bcm_td3_cosq_port_bandwidth_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 *min_quantum, uint32 *max_quantum,
                                uint32 *burst_quantum, uint32 *flags);

extern int
bcm_td3_cosq_port_bandwidth_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 min_quantum, uint32 max_quantum,
                                uint32 burst_quantum, uint32 flags);

extern int bcm_td3_cosq_port_attach(int unit, bcm_port_t port);
extern int bcm_td3_cosq_port_detach(int unit, bcm_port_t port);

extern int
bcm_td3_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id);

extern int
bcm_td3_egr_mpls_combo_map_default(uint32 unit);

extern int 
bcm_td3_egr_mpls_combo_map_create(uint32 unit,
                                  int *egr_mpls_combo_map_created,
                                  soc_profile_mem_t **egr_mpls_combo_map);

extern int
bcm_td3_egr_mpls_combo_dump(uint32 unit,
                          soc_profile_mem_t *profile);

extern void
bcm_td3_egr_mpls_combo_map_info_init(uint32 unit);

extern int 
bcm_td3_qos_map_create(uint32 unit, uint32 *index, int entry_per_index);

extern int bcm_td3_cosq_control_set(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int arg);
extern int bcm_td3_cosq_control_get(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int *arg);
extern int 
_bcm_td3_cosq_index_resolve(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, _bcm_td3_cosq_index_style_t style,
                            bcm_port_t *local_port, int *index, int *count);

extern int
bcm_td3_cosq_bst_profile_set(int unit,
                            bcm_gport_t port,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile);

extern int
bcm_td3_cosq_bst_profile_get(int unit,
                            bcm_gport_t port,
                            bcm_cos_queue_t cosq,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile);

extern int
bcm_td3_cosq_bst_stat_get(int unit,
                         bcm_gport_t port,
                         bcm_cos_queue_t cosq,
                         bcm_bst_stat_id_t bid,
                         uint32 options,
                         uint64 *pvalue);

extern int
bcm_td3_cosq_bst_stat_multi_get(int unit,
                               bcm_gport_t port,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               int max_values,
                               bcm_bst_stat_id_t *id_list,
                               uint64 *pvalues);

extern int
bcm_td3_cosq_bst_stat_clear(int unit, bcm_gport_t port,
                           bcm_cos_queue_t cosq, bcm_bst_stat_id_t bid);

extern int bcm_td3_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid);

extern int
_bcm_td3_bst_hdrm_stat_clear(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq);

extern int
_bcm_td3_cosq_bst_hdrm_stat_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               uint64 *pvalue);

extern int
bcm_td3_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t cosq);

extern int
bcm_td3_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t *cosq);

extern int
bcm_td3_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int
bcm_td3_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t *gport, bcm_cos_queue_t *cosq);

extern int
bcm_td3_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count);

extern int
bcm_td3_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count);

extern int
bcm_td3_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_td3_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array,
                                   int *array_count);

/* OOB support */
/* OOB FC Tx side global configuration API */
extern int bcm_td3_oob_fc_tx_config_set(
    int unit,
    bcm_oob_fc_tx_config_t *config);

/* OOB FC Tx side global configuration get API */
extern int bcm_td3_oob_fc_tx_config_get(
    int unit,
    bcm_oob_fc_tx_config_t *config);

extern int
_bcm_td3_oob_fc_tx_port_control_set(int unit, bcm_port_t port,
                                   int status, int dir);
extern int
_bcm_td3_oob_fc_tx_port_control_get(int unit, bcm_port_t port,
                                   int *status, int dir);

/* OOB FC Tx side global info get API */
extern int bcm_td3_oob_fc_tx_info_get(
    int unit,
    bcm_oob_fc_tx_info_t *info);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Set
 * API
 */
extern int bcm_td3_oob_fc_rx_port_tc_mapping_multi_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_count,
    uint32 *tc,
    uint32 *pri_bmp);

/*
 * OOB FC Rx Interface side Traffic Class to Priority Mapping Multi Get
 * API
 */
extern int bcm_td3_oob_fc_rx_port_tc_mapping_multi_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    int array_max,
    uint32 *tc,
    uint32 *pri_bmp,
    int *array_count);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping set API */
extern int bcm_td3_oob_fc_rx_port_tc_mapping_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 pri_bmp);

/* OOB FC Rx Interface side Traffic Class to Priority Mapping get API */
extern int bcm_td3_oob_fc_rx_port_tc_mapping_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 tc,
    uint32 *pri_bmp);

/* OOB FC Rx Interface side Configuration set API */
extern int bcm_td3_oob_fc_rx_config_set(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_count,
    bcm_gport_t *gport_array);

/* OOB FC Rx Interface side Configuration get API */
extern int bcm_td3_oob_fc_rx_config_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_oob_fc_rx_config_t *config,
    int array_max,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB FC Rx Interface side Channel offset value for a port */
extern int bcm_td3_oob_fc_rx_port_offset_get(
    int unit,
    bcm_oob_fc_rx_intf_id_t intf_id,
    bcm_gport_t gport,
    uint32 *offset);

/* OOB STATS global configuration set API */
extern int bcm_td3_oob_stats_config_set(
    int unit,
    bcm_oob_stats_config_t *config);

/* OOB STATS global configuration get API */
extern int bcm_td3_oob_stats_config_get(
    int unit,
    bcm_oob_stats_config_t *config);


/* OOB STATS configuration of service pool list multi set API */
extern int bcm_td3_oob_stats_pool_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array);

/* OOB STATS configuration of service pool list multi get API */
extern int bcm_td3_oob_stats_pool_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    uint8 *dir_array,
    bcm_service_pool_id_t *pool_array,
    int *array_count);

/* OOB STATS configuration of service pool list set API */
extern int bcm_td3_oob_stats_pool_mapping_set(
    int unit,
    int offset,
    uint8 dir,
    bcm_service_pool_id_t pool);

/* OOB STATS configuration of service pool list get API */
extern int bcm_td3_oob_stats_pool_mapping_get(
    int unit,
    int offset,
    uint8 *dir,
    bcm_service_pool_id_t *pool);

/* OOB STATS configuration of Queue list set API */
extern int bcm_td3_oob_stats_queue_mapping_multi_set(
    int unit,
    int array_count,
    int *offset_array,
    bcm_gport_t *gport_array);

/* OOB STATS configuration of Queue list get API */
extern int bcm_td3_oob_stats_queue_mapping_multi_get(
    int unit,
    int array_max,
    int *offset_array,
    bcm_gport_t *gport_array,
    int *array_count);

/* OOB STATS configuration of Queue list set API */
extern int bcm_td3_oob_stats_queue_mapping_set(
    int unit,
    int offset,
    bcm_gport_t gport);

/* OOB STATS configuration of Queue list get API */
extern int bcm_td3_oob_stats_queue_mapping_get(
    int unit,
    int offset,
    bcm_gport_t *gport);



/*
 * 8 Priority Groups supported.
 */
#define TD3_PRIOROTY_GROUP_ID_MIN 0
#define TD3_PRIOROTY_GROUP_ID_MAX 7
/*
 * Inpur priority range.
 * PFC : 0-7
 * SAFC : 0-15
 */
#define TD3_PFC_INPUT_PRIORITY_MIN  0
#define TD3_PFC_INPUT_PRIORITY_MAX  7
#define TD3_SAFC_INPUT_PRIORITY_MIN TD3_PFC_INPUT_PRIORITY_MIN
#define TD3_SAFC_INPUT_PRIORITY_MAX 15

extern int
bcm_td3_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group);
extern int
bcm_td3_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group);
extern int
bcm_td3_port_priority_group_config_set(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);
extern int
bcm_td3_port_priority_group_config_get(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config);
extern int
_bcm_td3_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              uint32 flags, bcm_port_t *ing_port,
                              bcm_cos_t *priority);

/* pfc deadlock */
extern int
_bcm_td3_pfc_deadlock_init(int unit);

extern int
_bcm_td3_pfc_deadlock_hw_cos_index_get(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_td3_pfc_deadlock_hw_cos_index_set(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_td3_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config);
extern int
_bcm_td3_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config);
extern int
_bcm_td3_pfc_deadlock_q_config_helper(int unit,
                                      _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status);
extern int
bcm_td3_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                                        bcm_port_t port, uint32 *rval32);
extern int
_bcm_td3_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port);
extern int
_bcm_td3_pfc_deadlock_recovery_begin(int unit, int cos, int pipe, int pipe_mmu_port);
extern int
_bcm_td3_pfc_deadlock_recovery_reset(int unit);
extern int
_bcm_td3_pfc_deadlock_monitor(int unit);


extern int
bcm_td3_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int pps);

extern int
bcm_td3_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int *pps);

extern int
bcm_td3_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int burst);

extern int
bcm_td3_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int *burst);

extern int
bcm_td3_port_rate_egress_set(int unit, bcm_port_t port, uint32 bandwidth,
                             uint32 burst, uint32 mode);

extern int
bcm_td3_port_rate_egress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                             uint32 *burst, uint32 *mode);

extern int
bcm_td3_port_drain_cells(int unit, int port);

extern int
bcm_td3_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable);

extern int
bcm_td3_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq,
                                 int *enable);

extern int
bcm_td3_cosq_field_classifier_get(int unit, int classifier_id,
                                  bcm_cosq_classifier_t *classifier);

extern int
bcm_td3_cosq_field_classifier_id_create(int unit, bcm_cosq_classifier_t
                                        *classifier, int *classifier_id);

extern int
bcm_td3_cosq_field_classifier_id_destroy(int unit, int classifier_id);

extern int
bcm_td3_cosq_field_classifier_map_set(int unit, int classifier_id, int count,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_td3_cosq_field_classifier_map_get(int unit, int classifier_id,int array_max,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_td3_cosq_field_classifier_map_clear(int unit, int classifier_id);

extern int
bcm_td3_cosq_safc_class_mapping_set(int unit, bcm_gport_t port, int array_count,
                                bcm_cosq_safc_class_mapping_t *mapping_array);

extern int
bcm_td3_cosq_safc_class_mapping_get(int unit, bcm_gport_t port, int array_max,
                                bcm_cosq_safc_class_mapping_t *mapping_array,
                                int *array_count);

/* Subport functions */
extern int bcm_td3_subport_coe_init(int unit);
#if defined(INCLUDE_L3)
/* L3 ECMP DLB */
extern void _bcm_td3_ecmp_dlb_deinit(int unit);
extern int _bcm_td3_ecmp_dlb_init(int unit);
extern int bcm_td3_l3_egress_dlb_attr_set(int unit, int nh_index,
        bcm_l3_egress_t *egr);
extern int bcm_td3_l3_egress_dlb_attr_destroy(int unit, int nh_index);
extern int bcm_td3_l3_egress_dlb_attr_get(int unit, int nh_index,
        bcm_l3_egress_t *egr);
extern int bcm_td3_l3_egress_ecmp_dlb_create(int unit,
        bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array);
extern int bcm_td3_l3_egress_ecmp_dlb_destroy(int unit,
        bcm_if_t mpintf);
extern int bcm_td3_l3_egress_ecmp_dlb_get(int unit,
        bcm_l3_egress_ecmp_t *ecmp);
extern int bcm_td3_l3_egress_dlb_attr_get(int unit, int nh_index, bcm_l3_egress_t *egr);
extern  int bcm_td3_l3_egress_ecmp_dlb_ethertype_set(
    int unit, uint32 flags, int ethertype_count, int *ethertype_array);

extern int bcm_td3_l3_egress_ecmp_dlb_ethertype_get(int unit, 
    uint32 *flags, int ethertype_max, int *ethertype_array, int *ethertype_count);

extern int _bcm_td3_ecmp_dlb_config_set(int unit, bcm_switch_control_t type,
        int arg);
extern int _bcm_td3_ecmp_dlb_config_get(int unit, bcm_switch_control_t type,
        int *arg);
extern void bcm_td3_ecmp_dlb_sw_dump(int unit);
extern int
bcm_td3_l3_conflict_get(int unit, bcm_l3_key_t *ipkey, bcm_l3_key_t *cf_array,
                        int cf_max, int *cf_count);

#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
extern int _bcm_td3_flow_port_resolve(
    int unit,
    bcm_gport_t flow_port_id,
    bcm_if_t encap_id,
    bcm_module_t *modid,
    bcm_port_t *port,
    bcm_trunk_t *trunk_id,
    int *id);

extern int _bcm_td3_l3_flex_nh_add(
    int unit,
    int idx,
    bcm_l3_egress_t *nh_entry,
    uint32 *egr_nh,
    int ipmc);

extern int _bcm_td3_l3_flex_nh_entry_parse(
    int unit,
    uint32 *egr_entry_ptr,
    uint32 index,
    bcm_l3_egress_t *nh_entry);

extern int _bcm_td3_flow_next_hop_set(
    int unit,
    int nh_index,
    uint32 flags,
    int vp,
    int drop);

extern int _bcm_td3_flow_egress_reset(
    int unit,
    int nh_index);

extern int _bcm_td3_flow_egress_get(
    int unit,
    bcm_l3_egress_t *egr,
    int nh_index);

extern int _bcm_td3_l3_flex_intf_add(
    int unit,
    _bcm_l3_intf_cfg_t *intf_info,
    uint32 *l3_if_entry_p);

extern int _bcm_td3_l3_flex_intf_get(
    int unit,
    _bcm_l3_intf_cfg_t *intf_info,
    uint32 *l3_if_entry_p);

extern int _bcm_td3_l3_flex_entry_get(
    int unit,
    _bcm_l3_cfg_t *l3cfg,
     int *nh_idx,
     int *embd);

extern int _bcm_td3_l3_flex_ent_parse(
     int unit,
     soc_mem_t mem,
     _bcm_l3_cfg_t *l3cfg,
     int *nh_idx,
     void *l3x_entry);

extern int _bcm_td3_l3_flex_entry_add(
    int unit,
     _bcm_l3_cfg_t *l3cfg,
     int nh_idx);

extern int _bcm_td3_l3_flex_entry_del(
    int unit,
     _bcm_l3_cfg_t *l3cfg);

extern int _bcm_td3_lpm_flex_ent_init(
    int unit,
     _bcm_defip_cfg_t *lpm_cfg,
    defip_entry_t *lpm_entry);

extern int _bcm_td3_lpm_prepare_flex_defip_entry(
    int unit,
    _bcm_defip_cfg_t *lpm_cfg,
    int nh_ecmp_idx,
    defip_entry_t *lpm_entry);

extern int _bcm_td3_lpm_flex_ent_get_key(
    int unit,
    _bcm_defip_cfg_t *lpm_cfg,
    defip_entry_t *lpm_entry,
    uint32 key_type);

extern int _bcm_td3_lpm_flex_ent_parse(
     int unit,
     uint32 *lpm_entry,
     uint32 key_type,
     soc_mem_t mem,
     _bcm_defip_cfg_t *lpm_cfg,
     int *nh_idx);

extern int _bcm_defip_pair128_flex_get_key(
     int unit,
     uint32 *hw_entry,
     _bcm_defip_cfg_t *lpm_cfg);

extern int _bcm_l3_defip_pair128_flex_add(
     int unit,
     _bcm_defip_cfg_t *lpm_cfg,
     int nh_ecmp_idx,
     uint32 *hw_entry);

extern int _bcm_td3_mpls_gre_label_add(
    int unit,
    bcm_l3_egress_t *egr,
    int nh_index,
    uint32 flags);

extern int _bcm_td3_mpls_gre_label_get(
    int unit,
    int vc_swap_index,
    bcm_l3_egress_t *egr);

extern int _bcm_td3_mpls_gre_label_delete(
    int unit,
    int index,
    soc_mem_t view_id);
#endif /* INCLUDE_L3 */

extern int _bcm_td3_flex_l2_from_l2x(
    int unit,
    bcm_l2_addr_t *l2addr,
    l2x_entry_t *l2x_entry,
    int *mb_index);

extern int _bcm_td3_flex_l2_to_l2x(
    int unit,
    l2x_entry_t *l2x_entry,
    bcm_l2_addr_t *l2addr,
    int key_only);

extern int _bcm_td3_flex_l2_replace_data_mask_setup(
    int unit,
    _bcm_l2_replace_t *rep);

extern int _bcm_td3_flex_l2_bulk_replace_modport(
    int unit,
     _bcm_l2_replace_t *rep_st);

extern int
bcm_td3_cosq_stat_set(int unit, bcm_gport_t port,
                     bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value);
extern int
bcm_td3_cosq_stat_get(int unit, bcm_gport_t port,
                     bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode,
                     uint64 *value);
extern int bcmi_td3_port_fn_drv_init(int unit);
extern int bcmi_td3_port_soc_resource_init(int unit, int nport,
                                 bcm_port_resource_t *resource,
                                 soc_port_resource_t *soc_resource);
extern int bcmi_td3_port_resource_validate(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_td3_pre_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_td3_post_flexport_tdm(int unit,
                            soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_td3_port_attach_mmu(int unit, int port);
extern int bcmi_td3_port_detach_asf(int unit, bcm_port_t port);
extern int bcmi_td3_port_enable(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_td3_port_disable(int unit,
                             soc_port_schedule_state_t *port_schedule_state);
extern int bcmi_td3_port_speed_1g_update(int unit, bcm_port_t port, int speed);
                                      bcm_port_abil_t *mask);
extern void bcm_td3_pstats_deinit(int unit);
extern int  bcm_td3_pstats_init(int unit);
extern int bcmi_td3_port_detach_stack(int unit, bcm_port_t port);
extern int bcmi_td3_port_attach_stack(int unit, bcm_port_t port);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void bcm_td3_cosq_sw_dump(int unit);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_td3_cosq_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* RIOT related defines */

#ifdef BCM_RIOT_SUPPORT


/* TD3 : EGR_L3_NEXT_HOP-KEY_TYPEf */
#define BCMI_TD3_L3_EGR_NH_MCAST_ENTRY_TYPE  7
#define BCMI_TD3_L3_EGR_NH_SD_TAG_ENTRY_TYPE 2


/* L3 interface banks values */
#define BCMI_TD3_L3_INTF_ENT_PER_BANK        2048
#define BCMI_TD3_L3_INTF_BANKS_MAX           8

/* L3 next hop banks values */
#define BCMI_TD3_L3_NH_ENT_PER_BANK          4096
#define BCMI_TD3_L3_NH_BANKS_MAX             16


/* NH destination encodings, masks and macros */
#define BCMI_TD3_L3_NH_DEST_DGLP_SHIFT       16
#define BCMI_TD3_L3_NH_DEST_DGLP_MASK        0xFFFF   /* 15 bits value */
#define BCMI_TD3_L3_NH_DEST_DGLP_EN_MASK     0x3      /* 2 bits value */
#define BCMI_TD3_L3_NH_DEST_DGLP_ENCODING    0x0      /* 15 bits value */
#define BCMI_TD3_L3_NH_DEST_DVP_SHIFT        16       /* 14 bits value */
#define BCMI_TD3_L3_NH_DEST_DVP_MASK         0xFFFF   /* 14 bits value */
#define BCMI_TD3_L3_NH_DEST_DVP_EN_MASK      0x3      /* 4 bits value */
#define BCMI_TD3_L3_NH_DEST_DVP_ENCODING     0x1      /* 15 bits value */
#define BCMI_TD3_L3_NH_DEST_LAG_SHIFT        13       /* 14 bits value */
#define BCMI_TD3_L3_NH_DEST_LAG_MASK         0x1FFF   /* 14 bits value */
#define BCMI_TD3_L3_NH_DEST_LAG_EN_MASK      0xF      /* 4 bits value */
#define BCMI_TD3_L3_NH_DEST_LAG_ENCODING     0x1      /* 15 bits value */


#define BCMI_TD3_L3_NH_DEST_DVP_GET(gport) \
            ((BCMI_TD3_L3_NH_DEST_DVP_ENCODING << BCMI_TD3_L3_NH_DEST_DVP_SHIFT) | \
            (gport & BCMI_TD3_L3_NH_DEST_DVP_MASK))

#define BCMI_TD3_L3_NH_DEST_LAG_GET(gport) \
            ((BCMI_TD3_L3_NH_DEST_LAG_ENCODING << BCMI_TD3_L3_NH_DEST_LAG_SHIFT) | \
            (gport & BCMI_TD3_L3_NH_DEST_LAG_MASK))

#define BCMI_TD3_L3_NH_DEST_DGLP_GET(gport) \
            ((BCMI_TD3_L3_NH_DEST_DGLP_ENCODING << BCMI_TD3_L3_NH_DEST_DGLP_SHIFT) | \
            (gport & BCMI_TD3_L3_NH_DEST_DGLP_MASK))

#define BCMI_TD3_L3_NH_DEST_IS_DGLP(_dest)  \
            ((((_dest) >> BCMI_TD3_L3_NH_DEST_DGLP_SHIFT) & BCMI_TD3_L3_NH_DEST_DGLP_EN_MASK) == \
            BCMI_TD3_L3_NH_DEST_DGLP_ENCODING)
#define BCMI_TD3_L3_NH_PORT_DGLP_GET(_dest) \
            (((_dest) & BCMI_TD3_L3_NH_DEST_DGLP_MASK))

#define BCMI_TD3_L3_NH_DEST_IS_LAG(_dest)  \
            (BCMI_TD3_L3_NH_DEST_IS_DGLP(_dest) && ((((_dest) >> BCMI_TD3_L3_NH_DEST_LAG_SHIFT) & BCMI_TD3_L3_NH_DEST_LAG_EN_MASK) == \
            BCMI_TD3_L3_NH_DEST_LAG_ENCODING))

#define BCMI_TD3_L3_NH_PORT_LAG_GET(_dest) \
            (((_dest) & BCMI_TD3_L3_NH_DEST_LAG_MASK))

#define BCMI_TD3_L3_NH_DEST_IS_DVP(_dest)  \
            ((((_dest) >> BCMI_TD3_L3_NH_DEST_DVP_SHIFT) & BCMI_TD3_L3_NH_DEST_DVP_EN_MASK) == \
            BCMI_TD3_L3_NH_DEST_DVP_ENCODING)

#define BCMI_TD3_L3_NH_PORT_DVP_GET(_dest) \
            (((_dest) & BCMI_TD3_L3_NH_DEST_DVP_MASK))

/* Td3 : EGR_L3_NEXT_HOP-NEXT_PTR field types */
#define BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP    1
#define BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH     2
#define BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_VC_SWAP 3

/* RioT Overlay validity checks */
#define BCMI_RIOT_IS_ENABLED(_u) \
    ((&_bcm_l3_bk_info[_u])->riot_enable)

#define BCMI_L3_NH_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_nh_overlay_entries) > 0))

#define BCMI_L3_INTF_OVERLAY_VALID(_u) \
    ((BCMI_RIOT_IS_ENABLED(_u)) &&            \
    (((&_bcm_l3_bk_info[_u])->l3_intf_overlay_entries) > 0))

#define BCMI_RIOT_VPN_VFI_IS_SET(_u, _vid) \
    ((BCMI_RIOT_IS_ENABLED(_u)) && (_BCM_VPN_VFI_IS_SET((_vid))))

#define BCMI_RIOT_VPN_VFI_IS_NOT_SET(_u, _vid) \
    ((!BCMI_RIOT_IS_ENABLED(_u)) || (!(_BCM_VPN_VFI_IS_SET((_vid)))))

#define BCMI_GPORT_TYPE_GET(_gport)    \
        (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

#define BCMI_L3_NH_FLEX_VIEW(ent_type) (ent_type >= 9)
#define BCMI_LPM_FLEX_VIEW(ent_type) (ent_type > 1)

/* DESTINATION_FORMAT DEST_TYPE encodings */
#define BCMI_TD3_DEST_TYPE_INVALID  0
#define BCMI_TD3_DEST_TYPE0_DVP     1
#define BCMI_TD3_DEST_TYPE0_DGPP    2
#define BCMI_TD3_DEST_TYPE0_NH      3
#define BCMI_TD3_DEST_TYPE1_ECMP    1
#define BCMI_TD3_DEST_TYPE1_L2MC    2
#define BCMI_TD3_DEST_TYPE1_IPMC    3
#define BCMI_TD3_DEST_TYPE2_LAG     1

#define BCMI_TD3_EGR_L3_NEXTHOP_WIDTH  160

/*
 * The below infrastructure is done specially for nh-nh association management.
 * When overlay nh is associated with underlay nh, we will have to keep
 * backward tracing to redraw association if underlay NH is changed.
 */
typedef struct _td3_ol_nh_link_s {
    int o_nh_idx;
    struct _td3_ol_nh_link_s *next_link;
} td3_ol_nh_link_t;

typedef struct _td3_ul_nh_link_s {
    int u_nh_idx;
    td3_ol_nh_link_t *ol_nh_assoc;
    struct _td3_ul_nh_link_s *ul_nh_link;
} td3_ul_nh_link_t;

/* TD3 specific routines */
extern int bcmi_td3_l3_riot_init(int unit);
extern int bcmi_td3_get_port_from_destination(int unit,
               uint32 dest, bcm_l3_egress_t *nh);

extern int bcm_td3_riot_l3_nh_dest_set(int unit, bcm_l3_egress_t *nh_entry, ing_l3_next_hop_entry_t *nh_dest);
/* ol-ul association routines */
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_sw_add(int unit,
               int ol_nh, int ul_nh);
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_delete(int unit, int nh_idx);
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_is_required(int unit, int vp_nh_index);
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_replace(int unit,
               int old_ul, int new_ul);
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_sw_delete(int unit, int ol_nh,
               int ul_nh);
extern int bcmi_td3_l3_nh_assoc_ol_ul_link_reinit(int unit, int ol_nh_idx);
extern int bcmi_td3_riot_l3_nh_dest_set(int unit,
               bcm_l3_egress_t *nh_entry, uint32 *nh_dest);
#endif /* BCM_RIOT_SUPPORT */

#ifdef BCM_SPECIAL_LABEL_SUPPORT

/************************* MPLS related Changes **********************/

#define BCMI_MPLS_LABEL_ID_MAX          (0xfffff)
#define BCMI_MPLS_LABEL_ID_VALID(_label_)             \
             (((_label_) >= 0) &&                         \
             ((_label_) <= BCMI_MPLS_LABEL_ID_MAX))

#define BCMI_MPLS_LABEL_MASK_MAX        (0xfffff)
#define BCMI_MPLS_LABEL_MASK_VALID(_mask_)            \
             (((_mask_) >= 0) &&                      \
             (((_mask_) <= BCMI_MPLS_LABEL_MASK_MAX))

#define BCMI_MPLS_EXP_MAX          (0x7)
#define BCMI_MPLS_EXP_VALID(exp)                      \
             (((exp) >= 0) &&                         \
             ((exp) <= BCMI_MPLS_EXP_MAX))

#define BCMI_MPLS_EXP_MASK_MAX        (0x7)
#define BCMI_MPLS_EXP_MASK_VALID(_mask_)              \
             (((_mask_) >= 0) &&                      \
             (((_mask_) <= BCMI_MPLS_EXP_MASK_MAX))
#define BCMI_MPLS_TTL_MAX          (0xFF)
#define BCMI_MPLS_TTL_VALID(ttl)                      \
             (((ttl) >= 0) &&                         \
             ((ttl) <= BCMI_MPLS_TTL_MAX))

#define BCMI_MPLS_TTL_MASK_MAX        (0xff)
#define BCMI_MPLS_TTL_MASK_VALID(_mask_)              \
             (((_mask_) >= 0) &&                      \
             (((_mask_) <= BCMI_MPLS_TTL_MASK_MAX))

#define BCMI_MPLS_CONSTRUCT_LABEL(val, exp, ttl, bos) \
            (((val) << 12) | ((exp) << 9) |           \
            ((bos) << 8) | (ttl))

#define BCMI_MPLS_LABEL_VAL_FROM_LABEL_GET(mpls_label)\
            (((mpls_label) >> 12) & 0xFFFFF)

#define BCMI_MPLS_EXP_FROM_LABEL_GET(mpls_label)      \
            (((mpls_label) >> 9) & 0x7)

#define BCMI_MPLS_BOS_FROM_LABEL_GET(mpls_label)      \
            (((mpls_label) >> 8) & 0x1)

#define BCMI_MPLS_TTL_FROM_LABEL_GET(mpls_label)      \
            ((mpls_label) & 0xFF)

/***************** Function Decalarations *********************/
extern int bcmi_mpls_special_label_init(int unit);
extern void bcmi_mpls_special_label_deinit(int unit);

extern int bcmi_mpls_special_label_identifier_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info);
extern int bcmi_mpls_special_label_identifier_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info);
extern int bcmi_mpls_special_label_identifier_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info);
extern int bcmi_mpls_special_label_identifier_delete_all(int unit);
extern int bcmi_mpls_special_label_identifier_traverse(int unit,
            bcm_mpls_special_label_identifier_traverse_cb cb,
            void *user_data);


extern int bcmi_mpls_special_label_egress_add(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info);
extern int bcmi_mpls_special_label_egress_get(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t *label_info);
extern int bcmi_mpls_special_label_egress_delete(int unit,
            bcm_mpls_special_label_type_t label_type,
            bcm_mpls_special_label_t label_info);
extern int bcmi_mpls_special_label_egress_delete_all(int unit);
extern int bcmi_mpls_special_label_egress_traverse(int unit,
            bcm_mpls_special_label_egress_traverse_cb cb,
            void *user_data);

extern int bcmi_mpls_special_label_push_action_set(int unit,
            bcm_mpls_special_label_push_element_t *element,
            bcm_mpls_special_label_push_type_t push_type);

extern int bcmi_mpls_special_label_push_action_get(int unit,
            bcm_mpls_special_label_push_element_t *element,
            bcm_mpls_special_label_push_type_t *push_type);

extern int _bcm_td3_def_vfi_profile_add(int unit,
            int is_eline, uint32 *profile_index);

/***************** Structure Decalarations *********************/

typedef struct bcmi_mpls_special_label_control_s {
    uint32 label_value;
    uint32 label_mask;
    int    label_type;
    int    valid;
    int    ref_count;
    int    cancun_added;
}bcmi_mpls_special_label_control_t;

extern bcmi_mpls_special_label_control_t *bcmi_special_label_precedence_state[BCM_MAX_NUM_UNITS];

/***************************************************************/
#endif /*BCM_SPECIAL_LABEL_SUPPORT*/
#endif
#endif /* BCM_FIREBOLT6_SUPPORT  */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
typedef struct bcmi_mc_group_chnl_info_s {
    uint32 l3_pbm_port_refcount[SOC_MAX_NUM_PORTS];
} bcmi_mc_group_chnl_info_t;

extern bcmi_mc_group_chnl_info_t **bcmi_mc_group_chnl_info[BCM_MAX_NUM_UNITS];

/* refcount value of port on that multicast group. */
#define BCMI_MC_GROUP_REFCOUNT_INFO(u, m)                   \
            (((bcmi_mc_group_chnl_info[u])[m])->l3_pbm_port_refcount)

/* refcount value of port on that multicast group. */
#define BCMI_MC_GROUP_PORT_REFCOUNT(u, m, p)                   \
            (((bcmi_mc_group_chnl_info[u])[m])->l3_pbm_port_refcount[p])

#if 0
/* forwarding port of this trunk on that multicast group. */
#define BCMI_MC_GROUP_TRUNK_FWD_SUBPORT(u, m, t)                   \
            (((bcmi_mc_group_chnl_info[u])[m]).trunk_fwd_subport[t])
#endif

/* Clear the refcount for all ports in that multicast group. */
#define BCMI_MC_GROUP_PORT_REFCOUNT_CLEAR(u, m)                                 \
            do {                                                                \
                if (soc_feature(unit, soc_feature_channelized_switching)) {     \
                    sal_memset(                                                 \
                        ((bcmi_mc_group_chnl_info[u])[m])->l3_pbm_port_refcount, \
                            0, SOC_MAX_NUM_PORTS * sizeof(uint32));             \
                }                                                               \
            } while(0);
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */


#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
#define _BCM_FIELD_FB6_INGRESS_FLOWTRACKER_MAX_SMALL_SLICES 1
#define _BCM_FIELD_FB6_INGRESS_FLOWTRACKER_SMALL_SLICE_SZ 2048
#define _BCM_FIELD_FB6_INGRESS_FLOWTRACKER_LARGE_SLICE_SZ 1024

/* Reserve entries in aggregate mmu and egress flowtracker stage
 * slice to prevent HIT when ACID is not set.
 */
#define _BCM_FIELD_FB6_AFT_RESERVED_ENTRIES  1

extern int _field_fb6_amftstage_init(int unit,
            _field_control_t *fc,
            _field_stage_t *stage_fc);

extern int _field_fb6_aeftstage_init(int unit,
            _field_control_t *fc,
            _field_stage_t *stage_fc);

extern int _field_fb6_aft_tcam_policy_init(int unit,
            _field_stage_t *stage_fc);

extern int _field_fb6_aft_entry_policy_mem_install(int unit,
            _field_entry_t *f_ent,
            soc_mem_t policy_mem,
            int tcam_idx);

extern int _field_fb6_aft_entry_qual_tcam_install(int unit,
            _field_entry_t *f_ent,
            soc_mem_t tcam_mem,
            int tcam_idx);

extern int _bcm_field_fb6_ft_user_entry_hw_alloc(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa);

extern int _bcm_field_fb6_ft_user_entry_hw_free(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa,
            uint32 flags);

extern int _bcm_field_fb6_ft_aggregate_class_hw_alloc(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa);

extern int _bcm_field_fb6_ft_aggregate_class_hw_free(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa,
            uint32 flags);

extern int _bcm_field_fb6_entry_sw_to_hw_tcam_idx(int unit,
            _field_slice_t *fs,
            int sw_tcam_idx,
            int *hw_tcam_idx);

extern int _bcm_field_fb6_entry_hw_to_sw_tcam_idx(int unit,
            _field_slice_t *fs,
            int hw_tcam_idx,
            int *sw_tcam_idx);

extern int _bcm_field_fb6_qualify_cos_queueid(int unit,
            bcm_field_entry_t entry,
            bcm_gport_t queue_gport);

extern int _bcm_field_fb6_qualify_cos_queueid_get(int unit,
            bcm_field_entry_t entry,
            bcm_gport_t *queue_gport);

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/* Compression related */
extern int
_field_fb6_compress_init(int unit, _field_stage_t *stage_fc);
extern int
_field_fb6_compress_deinit(int unit, _field_stage_t *stage_fc);
extern int
_bcm_field_fb6_compress_qualify_set(int unit,  _field_entry_t *f_ent,
                                    bcm_field_qualify_t qual,
                                    uint32 *data, uint32 *mask);
extern int
_bcm_field_fb6_compress_entry_qualifier_key_get(int unit,  _field_entry_t *f_ent,
                                                bcm_field_qualify_t qual,
                                                _bcm_field_qual_data_t data,
                                                _bcm_field_qual_data_t mask);
extern int
_bcm_field_fb6_compress_entry_install(int unit,  _field_entry_t *f_ent);
extern int
_bcm_field_fb6_compress_entry_remove(int unit,  _field_entry_t *f_ent);
extern int
_bcm_field_fb6_compress_entry_destroy(int unit,  _field_entry_t *f_ent);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_field_fb6_compress_scache_allocate(int unit);
extern int _bcm_field_fb6_compress_scache_sync(int unit);
extern int _bcm_field_fb6_compress_scache_recovery(int unit);
#endif

extern int
_bcm_fb6_cosq_gport_hw_qnum_get(int unit,
                             bcm_gport_t gport,
                             int *qnum);

extern int
_bcm_fb6_cosq_hw_qnum_gport_get(int unit,
                             int qnum,
                             bcm_gport_t *gport);

#endif /* !_BCM_INT_FIREBOLT6_H_ */

