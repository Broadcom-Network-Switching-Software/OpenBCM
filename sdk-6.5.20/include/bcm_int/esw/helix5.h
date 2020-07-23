/*
 * $Ided:   rc/flex (untracked content)
 $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident3.h
 * Purpose:     Function declarations for Trident3 Internal functions.
 */

#ifndef _BCM_INT_HELIX5_H_
#define _BCM_INT_HELIX5_H_

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/cosq.h>
#include <bcm/oob.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <soc/profile_mem.h>

#if defined(BCM_HELIX5_SUPPORT)
#include <soc/esw/port.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_alu.h>
#include <bcm_int/esw/flowtracker/ft_group.h>

/* entries_per_profile comes from MAXIDX in PHB_MAPPING_TBL_1
 * MAXIDX     => 4608,  #(72 ports x 64 profiles) */
#define _BCM_HX5_PHB_MAP_TAB1_ENTRIES_PER_PROFILE       (72)


#define _BCM_PFC_DEADLOCK_MASK      0xFF
typedef enum {
    _BCM_HX5_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_HX5_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_HX5_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_HX5_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_HX5_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_HX5_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_HX5_COSQ_INDEX_STYLE_COS,
    _BCM_HX5_COSQ_INDEX_STYLE_COUNT
} _bcm_hx5_cosq_index_style_t;

extern int bcmi_hx5_port_fn_drv_init(int unit);

/* COSQ Module */
extern int
bcm_hx5_cosq_init(int unit);

extern int
bcm_hx5_cosq_config_get(int unit, int *numq);

extern int
bcm_hx5_cosq_config_set(int unit, int numq);

extern int
bcm_hx5_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_hx5_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_hx5_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);

extern int
bcm_hx5_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data);

extern int
bcm_hx5_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);
extern int
bcm_hx5_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                       int *numq, uint32 *flags);

extern int
bcm_hx5_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data);
extern int
_bcm_hx5_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                           bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                           int *qnum);
extern int
bcm_hx5_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id);
extern int
bcm_hx5_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_hx5_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int *mode, int *weight);

extern int
bcm_hx5_cosq_gport_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int mode, int weight);

extern int
bcm_hx5_cosq_port_sched_get(int unit, bcm_pbmp_t pbm, int *mode, int *weights,
                            int *delay);

extern int
bcm_hx5_cosq_port_sched_set(int unit, bcm_pbmp_t pbm, int mode, const int
                            *weights, int delay);
extern int
bcm_hx5_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_hx5_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                 uint32 kbits_sec_max, uint32 flags);

extern int
bcm_hx5_cosq_port_bandwidth_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 *min_quantum, uint32 *max_quantum,
                                uint32 *burst_quantum, uint32 *flags);

extern int
bcm_hx5_cosq_port_bandwidth_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 min_quantum, uint32 max_quantum,
                                uint32 burst_quantum, uint32 flags);
extern int
bcm_hx5_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int pps);

extern int
bcm_hx5_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int *pps);

extern int
bcm_hx5_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int burst);

extern int
bcm_hx5_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int *burst);

extern int 
_bcm_hx5_cosq_index_resolve(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, _bcm_hx5_cosq_index_style_t style,
                            bcm_port_t *local_port, int *index, int *count);

extern int bcm_hx5_cosq_control_set(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int arg);
extern int bcm_hx5_cosq_control_get(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int *arg);
extern int
bcm_hx5_cosq_config_get(int unit, int *numq);
extern int
bcm_hx5_cosq_config_set(int unit, int numq);

extern int
bcm_hx5_cosq_discard_set(int unit, uint32 flags);

extern int
bcm_hx5_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_hx5_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int drop_start, int drop_slope,
                              int average_time);

extern int
bcm_hx5_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int *drop_start, int *drop_slope,
                              int *average_time);

extern int
bcm_hx5_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_hx5_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int bcm_hx5_cosq_wred_resolution_entry_create(int unit, int *index);

extern int bcm_hx5_cosq_wred_resolution_entry_destroy(int unit, int index);

extern int bcm_hx5_cosq_wred_resolution_set(int unit, int index,
                                            bcm_cosq_discard_rule_t *rule);

extern int bcm_hx5_cosq_wred_resolution_get(int unit, int index, int max,
                                            bcm_cosq_discard_rule_t *rule,
                                            int *count);
extern int
bcm_hx5_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t cosq);

extern int
bcm_hx5_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t *cosq);

extern int
bcm_hx5_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int
bcm_hx5_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t *gport, bcm_cos_queue_t *cosq);

extern int
bcm_hx5_cosq_field_classifier_get(int unit, int classifier_id,
                                  bcm_cosq_classifier_t *classifier);

extern int
bcm_hx5_cosq_field_classifier_id_create(int unit, bcm_cosq_classifier_t
                                        *classifier, int *classifier_id);

extern int
bcm_hx5_cosq_field_classifier_id_destroy(int unit, int classifier_id);

extern int
bcm_hx5_cosq_field_classifier_map_set(int unit, int classifier_id, int count,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_hx5_cosq_field_classifier_map_get(int unit, int classifier_id,int array_max,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_hx5_cosq_field_classifier_map_clear(int unit, int classifier_id);

extern int
bcm_hx5_cosq_stat_set(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, uint64 value);
extern int
bcm_hx5_cosq_stat_get(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, int sync_mode,
                      uint64 *value);
extern int
bcm_hx5_cosq_gport_info_get(int unit, bcm_gport_t gport,
                             bcm_cosq_gport_level_info_t *info);

extern int
bcm_hx5_hgcos_select_port_control_set(int unit,  bcm_port_t port,
                                      bcm_port_control_t type, int value);
extern int
bcm_hx5_hgcos_select_port_control_get(int unit,  bcm_port_t port,
                                      bcm_port_control_t type, int *value);

extern int
bcm_hx5_cosq_safc_class_mapping_set(int unit, bcm_gport_t port, int array_count,
                                    bcm_cosq_safc_class_mapping_t *mapping_array);

extern int
bcm_hx5_cosq_safc_class_mapping_get(int unit, bcm_gport_t port, int array_max, 
                                    bcm_cosq_safc_class_mapping_t *mapping_array,
                                    int *array_count);


#ifdef INCLUDE_L3
extern int
_bcm_hx5_ecmp_rh_dynamic_size_encode(int dynamic_size, int *encoded_value);

extern int
_bcm_hx5_ecmp_rh_dynamic_size_decode(int encoded_value, int *dynamic_size);
#endif /* INCLUDE_L3 */

extern int
_bcm_hx5_hg_rh_dynamic_size_encode(int dynamic_size, int *encoded_value);

extern int
_bcm_hx5_hg_rh_dynamic_size_decode(int encoded_value, int *dynamic_size);

extern int
_bcm_hx5_lag_rh_dynamic_size_encode(int dynamic_size, int *encoded_value);

extern int
_bcm_hx5_lag_rh_dynamic_size_decode(int encoded_value, int *dynamic_size);

extern int
bcm_hx5_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count);

extern int
bcm_hx5_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count);

extern int
bcm_hx5_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_hx5_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array,
                                   int *array_count);
/*
 *   OBM Classifier functions.
 */
extern int
bcm_hx5_switch_obm_classifier_mapping_multi_set(int unit, bcm_gport_t gport,
        bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
        int array_count, bcm_switch_obm_classifier_t *switch_obm_classifier);

extern int
bcm_hx5_switch_obm_classifier_mapping_multi_get(int unit, bcm_gport_t gport,
        bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
        int array_max, bcm_switch_obm_classifier_t *switch_obm_classifier,
        int *array_count);

extern int
bcm_hx5_obm_classifier_port_control_set(int unit, bcm_port_t port,
        bcm_port_control_t type, int value);

extern int
bcm_hx5_obm_classifier_port_control_get(int unit, bcm_port_t port,
        bcm_port_control_t type, int *value);


extern int bcm_hx5_cosq_port_attach(int unit, bcm_port_t port);
extern int bcm_hx5_cosq_port_detach(int unit, bcm_port_t port);

extern int bcmi_hx5_port_cosmap_update(int unit, pbmp_t pbmp, int enable);
extern void bcm_hx5_cosq_sw_dump(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hx5_cosq_sync(int unit);
#endif

extern int
bcm_hx5_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable);

extern int
bcm_hx5_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq, int *enable);

extern int
bcm_hx5_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, bcm_cosq_dir_t direction,
                                 int array_max, bcm_cosq_buffer_id_t *buf_id_array,
                                 int *array_count);

extern void bcm_hx5_pstats_deinit(int unit);
extern int  bcm_hx5_pstats_init(int unit);

extern int
_bcm_hx5_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              uint32 flags, bcm_port_t *ing_port,
                              bcm_cos_t *priority);

/* pfc deadlock */
#define HX5_PFC_DEADLOCK_MAX_COS 26

/* Below structure to contain register/field name specific to chip */
typedef struct _bcm_hx5_pfc_hw_resorces_s {
    soc_reg_t timer_count[HX5_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_status[HX5_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_mask[HX5_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_en[HX5_PFC_DEADLOCK_MAX_COS];
    soc_reg_t chip_config[4];
    soc_reg_t port_config[2];
    soc_field_t cos_field[HX5_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_init_val[HX5_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_unit_field;
    soc_field_t recovery_action_field;
    soc_field_t deadlock_status_field;
} _bcm_hx5_pfc_hw_resorces_t;

typedef struct _bcm_hx5_pfc_deadlock_control_s {
    uint8 pfc_deadlock_cos_max;  /* Max COS supported for PFC Deadlock
                                  * Detection and Recovery */
    uint8 pfc_deadlock_cos_used; /* Total COS configured for PFC Deadlock
                                  * Detection and Recovery */
    uint8 hw_cos_idx_inuse[HX5_PFC_DEADLOCK_MAX_COS];   /* Boolean for used Cos indices */
    uint8 cb_enabled;            /* Indicator if CB is enabled */
    bcm_switch_pfc_deadlock_detection_interval_t time_unit; /* 0 - 100ms,
                                                             * 1 - 10ms */
    uint32 cb_interval;          /* CB interval duration */
    bcm_cos_t pfc_pri2cos[HX5_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    bcm_cos_t pfc_cos2pri[HX5_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    _bcm_pfc_deadlock_config_t cos_config[HX5_PFC_DEADLOCK_MAX_COS];
    _bcm_hx5_pfc_hw_resorces_t hw_regs_fields;
    bcm_switch_pfc_deadlock_action_t recovery_action;
    uint32 cb_count; /* Debug ctr to keep track of #times CB fn is called */
    _bcm_cosq_inv_mapping_get_f cosq_inv_mapping_get; /* Function to retrieve
                                                       * Port/Priority from a
                                                       * given Gport */
} _bcm_hx5_pfc_deadlock_control_t;

#define _BCM_HX5_PFC_DEADLOCK_HW_RES_INIT(deadlock_hw)        \
    do {                                                      \
        int i = 0;                                            \
        for (i = 0; i < HX5_PFC_DEADLOCK_MAX_COS; i++) {      \
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

#define _BCM_HX5_UNIT_PFC_DEADLOCK_CONTROL(u) _bcm_hx5_pfc_deadlock_control[(u)]
#define _BCM_HX5_PFC_DEADLOCK_CONFIG(u,cos) \
            (&((_bcm_hx5_pfc_deadlock_control[(u)])->cos_config[(cos)]))
#define _BCM_HX5_PFC_DEADLOCK_COS_ENABLED(u,cos) \
            ((((_bcm_hx5_pfc_deadlock_control[(u)])->cos_config[(cos)].flags) & \
            _BCM_PFC_DEADLOCK_F_ENABLE) ? TRUE : FALSE)
#define _BCM_HX5_PFC_DEADLOCK_CB_INTERVAL(u) \
            ((_bcm_hx5_pfc_deadlock_control[(u)])->cb_interval)
#define _BCM_HX5_PFC_DEADLOCK_CB_ENABLED(u) \
            (((_bcm_hx5_pfc_deadlock_control[(u)])->cb_enabled) ? (TRUE) :(FALSE))
#define _BCM_HX5_PFC_DEADLOCK_CB_COUNT(u) \
            ((_bcm_hx5_pfc_deadlock_control[(u)])->cb_count)
#define _BCM_HX5_PFC_DEADLOCK_TIME_UNIT(u) \
            ((_bcm_hx5_pfc_deadlock_control[(u)])->time_unit)
#define _BCM_HX5_PFC_DEADLOCK_RECOVERY_ACTION(u) \
            ((_bcm_hx5_pfc_deadlock_control[(u)])->recovery_action)

extern _bcm_hx5_pfc_deadlock_control_t *_bcm_hx5_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

extern int _bcm_hx5_pfc_deadlock_recovery_reset(int unit);

extern int
_bcm_hx5_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_hx5_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg);

extern int bcm_hx5_pfc_deadlock_recovery_start(int unit,
                                                   bcm_port_t port, int pri);
extern int bcm_hx5_pfc_deadlock_recovery_exit(int unit,
                                                  bcm_port_t port, int pri);


extern int _bcm_hx5_pfc_deadlock_init(int unit);
extern int _bcm_hx5_pfc_deadlock_hw_res_init(int unit);

extern int _bcm_hx5_pfc_deadlock_deinit(int unit);
extern int _bcm_hx5_pfc_deadlock_reinit(int unit);

extern int
_bcm_hx5_pfc_deadlock_hw_cos_index_get(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_hx5_pfc_deadlock_hw_cos_index_set(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_hx5_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config);
extern int
_bcm_hx5_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config);
extern int
_bcm_hx5_pfc_deadlock_q_config_helper(int unit,
                                      _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status);
extern int
bcm_hx5_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                                        bcm_port_t port, uint32 *rval32);
extern int
_bcm_hx5_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port);
extern int
_bcm_hx5_pfc_deadlock_monitor(int unit);

extern int
bcm_hx5_pfc_deadlock_queue_info_get(int unit, bcm_gport_t gport, bcm_port_t *mmu_port,
                                    bcm_port_t *local_port, int *type, int *split);

extern int
bcm_hx5_dump_port_hsp(int unit, bcm_port_t port);

#if defined(BCM_FIELD_SUPPORT)
#if defined(BCM_FLOWTRACKER_SUPPORT)

/* Reserve entries in ingress flowtracker stage slice
 * To prevent performing flowtracking on traffic coming out
 * of broadscan port, we need to add few default entries
 * 1 for Single-wide, 2 for Double-wide and 1 Reserved
 */
#define _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES  4

int _bcm_field_ft_group_checkbmp_get(int unit,
            bcm_flowtracker_group_t id,
            uint32 num_checks,
            bcm_field_flowtrackercheck_t *check_list,
            uint32 *check_bmp_data,
            uint32 *check_bmp_mask);

int _bcm_field_ft_group_checklist_get(int unit,
            bcm_flowtracker_group_t id,
            uint32 check_bmp_data,
            uint32 check_bmp_mask,
            uint32 num_checks,
            bcm_field_flowtrackercheck_t *check_list,
            uint32 *actual_num_checks);

/* init/deinit functions */
extern int _field_hx5_ftstage_init(int unit,
            _field_control_t *fc,
            _field_stage_t *stage_fc);

extern int _field_hx5_ftstage_deinit(int unit,
            _field_stage_t *stage_fc);

extern int _field_hx5_ft_tcam_policy_init(int unit,
            _field_stage_t *stage_fc);

extern int _field_hx5_ft_stage_quals_map_init(int unit,
            bcmi_keygen_qual_cfg_info_db_t *db);

extern int _field_hx5_ft_presel_qualifiers_init(int unit,
            _field_stage_t *stage_fc);

extern int _field_ft_keygen_profile_aab_extractors_db_create(int unit,
            bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
            int ext_mode);

extern int _field_ft_keygen_profile_ab_extractors_db_create(int unit,
            bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
            int ext_mode);

extern int _field_hx5_ft_session_profiles_reference(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles);

/* Preselection related */
extern int _bcm_field_hx5_ft_lt_entry_data_value_set(int unit,
            _field_stage_t *stage_fc,
            _field_group_t *fg,
            int index,
            soc_mem_t lt_data_mem,
            uint32 *data);

extern int _bcm_field_hx5_ft_lt_entry_install(int unit,
            _field_presel_entry_t *f_presel,
            _field_lt_slice_t *lt_fs);

extern int _bcm_field_hx5_ft_lt_entry_hw_remove(int unit,
            _field_presel_entry_t *f_presel);

extern int _field_hx5_ft_presel_delete_last_entry_hw(int unit,
            _field_presel_entry_t *f_presel,
            int last_hw_index);

/* Field Group related */
extern int _field_hx5_ft_slice_validate(int unit,
            _field_stage_t *stage_fc,
            _field_group_t *fg,
            int slice_id);

extern int _field_hx5_ft_ext_code_assign(int unit,
            int selcode_clear,
            _field_group_add_fsm_t *fsm_ptr);

extern int _field_hx5_ft_group_keytype_assign(int unit,
            _field_stage_t *stage_fc,
            _field_group_t *fg);

extern int _field_hx5_ft_group_keytype_clear(int unit,
            _field_stage_t *stage_fc,
            _field_group_t *fg);

extern int _field_hx5_ft_group_keytype_bmp_recover(int unit,
            _field_stage_t *stage_fc,
            _field_group_t *fg);

extern int _field_hx5_ft_group_keygen_remove(int unit,
            _field_presel_entry_t *f_presel,
            int hw_idx);

extern int _field_hx5_ft_group_keygen_install(int unit,
            _field_presel_entry_t *f_presel);

extern int _field_hx5_ft_entries_free_get(int unit,
            _field_slice_t *fs,
            _field_group_t *fg,
            int *free_cnt);

/* Entry related */
extern int _bcm_field_hx5_ft_entry_keytype_set(int unit,
            _field_entry_t *f_ent);

extern int _field_hx5_ft_entry_policy_mem_install(int unit,
            _field_entry_t *f_ent,
            soc_mem_t policy_mem,
            int tcam_idx);

extern int _field_hx5_ft_entry_tcam_policy_install(int unit,
            _field_entry_t *f_ent,
            int tcam_idx);

extern int _bcm_field_hx5_ft_tcam_policy_clear(int unit,
            _field_entry_t *f_ent,
            int tcam_idx);

extern int _bcm_field_hx5_ft_entry_move(int unit,
            _field_entry_t *f_ent,
            int parts_count,
            int *tcam_idx_old,
            int *tcam_idx_new);

extern int _field_hx5_ft_stage_entry_enable_set(int unit,
            _field_entry_t *f_ent,
            int enable_flag);

/* Action related */
extern int _bcm_field_hx5_ft_action_params_check(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa);

extern int _bcm_field_hx5_ft_actions_hw_alloc(int unit,
            _field_entry_t *f_ent);

extern int _bcm_field_hx5_ft_actions_hw_free(int unit,
            _field_entry_t *f_ent,
            int flags);

extern int _bcm_field_hx5_ft_group_hw_free(int unit,
            _field_entry_t *f_ent,
            _field_action_t *fa,
            int flags);

/* FT and keygen related */
extern void _field_hx5_ft_keygen_ext_shift_offsets_get(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int *shift_offsets_arr);

extern int _bcm_field_hx5_ft_keygen_extractors_set(int unit,
            int keygen_profile,
            uint16 ext_ctrl_sel_info_count,
            bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
            _field_ft_keygen_ext_codes_t *ext_codes);

extern int _field_ft_tracking_param_qual_map_db_init(int unit,
            _field_ft_info_t *ft_info,
            bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db);

extern void _field_ft_tracking_param_qual_map_db_cleanup(int unit,
            _field_ft_info_t *ft_info);

extern int _field_hx5_ft_session_profiles_install(int unit,
            bcm_flowtracker_group_t flow_group_id,
            bcmi_ft_session_profiles_t *profiles);

extern int _field_hx5_ft_session_profiles_remove(int unit,
            bcm_flowtracker_group_t flow_group_id,
            bcmi_ft_session_profiles_t *profiles);

extern int _field_hx5_ft_qual_offset_ext_shift_apply(int unit,
            int split_allowed,
            _field_ft_keygen_ext_codes_t *ext_codes,
            _field_ft_qual_ext_offset_info_t *qual_ext_offset_info);

extern int _field_hx5_ft_qual_ext_offset_info_get(int unit,
            int keygen_parts_count,
            bcmi_keygen_oper_t *keygen_oper,
            _field_ft_qual_ext_offset_info_t **qual_ext_offset_info);

extern int _bcm_field_hx5_ft_keygen_extractors_mask_set(int unit,
            _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
            _field_ft_keygen_ext_codes_t *ext_codes);

extern int _bcm_field_hx5_ft_extractors_encode(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int *ext_count,
            bcmi_ft_hw_extractor_info_t **ext_ctrl_info);

extern int _bcm_field_hx5_ft_extractors_decode(int unit,
            bcmi_ft_hw_extractor_info_t *ext_ctrl_info,
            int ext_count,
            _field_ft_keygen_ext_codes_t *ext_codes);

extern int _bcm_field_ft_keygen_profile_extractors_mask_set(int unit,
            int offset,
            int width,
            int ext_num,
            int *shift_offsets_arr,
            int mask_offset,
            uint32 *mask,
            _field_ft_keygen_ext_codes_t *ext_codes);

extern void _field_hx5_ft_qual_ext_offset_info_free(int unit,
            _field_ft_qual_ext_offset_info_t **qual_ext_offset_info);

extern int _field_hx5_ft_normalize_controls_set(
            int unit,
            bcm_flowtracker_group_t id,
            soc_mem_t policy_mem,
            uint32 enable,
            uint32 *data_buf);

extern int _bcm_field_keygen_qual_offset_info_dump(
            int unit,
            int keygen_parts_count,
            bcmi_keygen_oper_t *keygen_oper);

extern int _bcm_field_hx5_ft_keygen_hw_extractors_dump(
            int unit,
            _field_ft_keygen_ext_codes_t *ext_codes);

extern int _bcm_field_ft_keygen_extractors_init(
            _field_ft_keygen_ext_codes_t *ext_codes);

extern int _bcm_field_hx5_ft_group_update(
            int unit,
            bcm_field_group_t group,
            bcm_field_qset_t *qset);

extern int _field_hx5_ft_keygen_ext_ctrl_info_get(
            int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            bcmi_keygen_ext_ctrl_sel_info_t **out_ext_ctrl_sel_info,
            uint16 *out_ext_ctrl_sel_info_count);

extern int _field_hx5_ft_keygen_ext_ctrl_info_skip(
            int unit,
            bcmi_keygen_cfg_t *keygen_cfg,
            uint16 *skipped_size);

extern int _field_ft_keygen_oper_skip_ext_ctrl_form(
            int unit,
            bcm_field_qualify_t qual_id,
            int type_a2_exts,
            int type_b_exts,
            bcmi_keygen_oper_t *keygen_oper);

extern int _field_ft_keygen_oper_skip_qual_offset_info_form(
            int unit,
            int num_parts,
            bcm_field_qualify_t qual_id,
            int type_a2_exts,
            int type_b_exts,
            bcmi_keygen_oper_t *keygen_oper);

extern int _field_ft_keygen_skip_qual_ext_ctrl_merge(
            int unit,
            int num_parts,
            bcmi_keygen_oper_t *skip_keygen_oper,
            bcmi_keygen_oper_t *keygen_oper);

extern int _bcm_field_ft_group_ext_codes_recover(int unit);
#endif
#endif

#if defined (INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)
extern int bcm_hx5_cosq_tcb_config_get(int unit, bcm_cosq_buffer_id_t buffer_id,
                                       bcm_cosq_tcb_config_t *config);
extern int bcm_hx5_cosq_tcb_config_set(int unit, bcm_cosq_buffer_id_t buffer_id,
                                       bcm_cosq_tcb_config_t *config);
extern int bcm_hx5_cosq_tcb_gport_threshold_mapping_get(int unit,
                                     bcm_gport_t gport, bcm_cos_queue_t cosq,
                                     bcm_cosq_buffer_id_t buffer_id, int *index);
extern int bcm_hx5_cosq_tcb_gport_threshold_mapping_set(int unit,
                                      bcm_gport_t gport, bcm_cos_queue_t cosq,
                                      bcm_cosq_buffer_id_t buffer_id,
                                      int index);
extern int bcm_hx5_cosq_tcb_enable(int unit, bcm_cosq_buffer_id_t buffer_id,
                                   int enable);
extern int bcm_hx5_cosq_tcb_freeze(int unit, bcm_cosq_buffer_id_t buffer_id,
                                   int enable);
extern int bcm_hx5_cosq_tcb_init(int unit);
extern int _bcm_hx5_cosq_tcb_deinit(int unit);
extern int bcm_hx5_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id, int queue, int *index);
extern int bcm_hx5_cosq_tcb_buffer_multi_get(int unit, 
                                  bcm_cosq_buffer_id_t buffer_id, int count, 
                                  bcm_cosq_tcb_buffer_t *entries, int *num);
extern int bcm_hx5_cosq_tcb_event_multi_get(int unit, 
                                 bcm_cosq_buffer_id_t buffer_id, int count, 
                                 bcm_cosq_tcb_event_t *entries, int *num, 
                                 int *overflow_count);
extern int bcm_hx5_cosq_tcb_threshold_profile_get(int unit, 
                                         bcm_cosq_buffer_id_t buffer_id, int index,
                                         bcm_cosq_tcb_threshold_profile_t *threshold);
extern int bcm_hx5_cosq_tcb_threshold_profile_destroy(int unit, 
                                         bcm_cosq_buffer_id_t buffer_id, int index);
extern int bcm_hx5_cosq_tcb_threshold_profile_create(int unit, int flags,
                                      bcm_cosq_buffer_id_t buffer_id, 
                                      bcm_cosq_tcb_threshold_profile_t *threshold,
                                      int *index);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hx5_cosq_tcb_reinit(int unit);
#endif
#endif

extern int
bcmi_hx5_switch_chip_info_get(int unit,
                              bcm_switch_chip_info_t info_type,
                              int max_size,
                              void *data_buf,
                              int *actual_size);

#endif /* BCM_HELIX5_SUPPORT  */

#endif /* !_BCM_INT_HELIX5_H_ */
