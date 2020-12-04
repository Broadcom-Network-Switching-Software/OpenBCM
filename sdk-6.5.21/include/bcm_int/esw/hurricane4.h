/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hurricane4.h
 * Purpose:     Function declarations for Hurricane4 Internal functions.
 */

#ifndef _BCM_INT_HURRICANE4_H_
#define _BCM_INT_HURRICANE4_H_

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/cosq.h>
#include <bcm/oob.h>
#include <bcm_int/esw/pfc_deadlock.h>
#include <soc/profile_mem.h>

#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/esw/port.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_alu.h>
#include <bcm_int/esw/flowtracker/ft_group.h>

/****************cosq start *****************************************/
/* entries_per_profile comes from MAXIDX in PHB_MAPPING_TBL_1
 * MAXIDX     => 4608,  #(72 ports x 64 profiles) */
#define _BCM_HR4_PHB_MAP_TAB1_ENTRIES_PER_PROFILE       (72)

#define _BCM_PFC_DEADLOCK_MASK                          0xFF

typedef enum {
    _BCM_HR4_COSQ_INDEX_STYLE_WRED_QUEUE,
    _BCM_HR4_COSQ_INDEX_STYLE_WRED_PORT,
    _BCM_HR4_COSQ_INDEX_STYLE_WRED_DEVICE,
    _BCM_HR4_COSQ_INDEX_STYLE_UCAST_QUEUE,
    _BCM_HR4_COSQ_INDEX_STYLE_MCAST_QUEUE,
    _BCM_HR4_COSQ_INDEX_STYLE_EGR_POOL,
    _BCM_HR4_COSQ_INDEX_STYLE_COS,
    _BCM_HR4_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP,
    _BCM_HR4_COSQ_INDEX_STYLE_COUNT
} _bcm_hr4_cosq_index_style_t;

extern int bcmi_hr4_port_fn_drv_init(int unit);

/* COSQ Module */
extern int
bcm_hr4_cosq_init(int unit);

extern int
bcm_hr4_cosq_config_get(int unit, int *numq);

extern int
bcm_hr4_cosq_config_set(int unit, int numq);

extern int
bcm_hr4_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_hr4_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq);

extern int
bcm_hr4_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                              bcm_gport_t *input_gport, bcm_cos_queue_t *cosq);
extern int
bcm_hr4_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                       int *numq, uint32 *flags);
extern int
bcm_hr4_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data);
extern int
bcm_hr4_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                             bcm_cos_queue_t cosq, bcm_gport_t *out_gport);
extern int
bcm_hr4_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data);
extern int
_bcm_hr4_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                           bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                           int *qnum);
extern int
bcm_hr4_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id);
extern int
bcm_hr4_cosq_sched_weight_max_get(int unit, int mode, int *weight_max);

extern int
bcm_hr4_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int *mode, int *weight);

extern int
bcm_hr4_cosq_gport_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                             int mode, int weight);

extern int
bcm_hr4_cosq_port_sched_get(int unit, bcm_pbmp_t pbm, int *mode, int *weights,
                            int *delay);

extern int
bcm_hr4_cosq_port_sched_set(int unit, bcm_pbmp_t pbm, int mode, const int
                            *weights, int delay);
extern int
bcm_hr4_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max, uint32 *flags);

extern int
bcm_hr4_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                 uint32 kbits_sec_max, uint32 flags);

extern int
bcm_hr4_cosq_port_bandwidth_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 *min_quantum, uint32 *max_quantum,
                                uint32 *burst_quantum, uint32 *flags);

extern int
bcm_hr4_cosq_port_bandwidth_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                uint32 min_quantum, uint32 max_quantum,
                                uint32 burst_quantum, uint32 flags);
extern int
bcm_hr4_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int pps);

extern int
bcm_hr4_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                          int *pps);

extern int
bcm_hr4_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int burst);

extern int
bcm_hr4_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                            int *burst);

extern int
_bcm_hr4_cosq_index_resolve(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, _bcm_hr4_cosq_index_style_t style,
                            bcm_port_t *local_port, int *index, int *count);

extern int bcm_hr4_cosq_control_set(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int arg);
extern int bcm_hr4_cosq_control_get(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_cosq_control_t type, int *arg);
extern int
bcm_hr4_cosq_config_get(int unit, int *numq);
extern int
bcm_hr4_cosq_config_set(int unit, int numq);

extern int
bcm_hr4_cosq_discard_set(int unit, uint32 flags);

extern int
bcm_hr4_cosq_discard_get(int unit, uint32 *flags);

extern int
bcm_hr4_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int drop_start, int drop_slope,
                              int average_time);

extern int
bcm_hr4_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                              uint32 color, int *drop_start, int *drop_slope,
                              int *average_time);

extern int
bcm_hr4_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int
bcm_hr4_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard);

extern int bcm_hr4_cosq_wred_resolution_entry_create(int unit, int *index);

extern int bcm_hr4_cosq_wred_resolution_entry_destroy(int unit, int index);

extern int bcm_hr4_cosq_wred_resolution_set(int unit, int index,
                                            bcm_cosq_discard_rule_t *rule);

extern int bcm_hr4_cosq_wred_resolution_get(int unit, int index, int max,
                                            bcm_cosq_discard_rule_t *rule,
                                            int *count);
extern int
bcm_hr4_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t cosq);

extern int
bcm_hr4_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                         bcm_cos_queue_t *cosq);

extern int
bcm_hr4_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t gport, bcm_cos_queue_t cosq);

extern int
bcm_hr4_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                               bcm_cos_t int_pri, uint32 flags,
                               bcm_gport_t *gport, bcm_cos_queue_t *cosq);

extern int
bcm_hr4_cosq_field_classifier_get(int unit, int classifier_id,
                                  bcm_cosq_classifier_t *classifier);

extern int
bcm_hr4_cosq_field_classifier_id_create(int unit, bcm_cosq_classifier_t
                                        *classifier, int *classifier_id);

extern int
bcm_hr4_cosq_field_classifier_id_destroy(int unit, int classifier_id);

extern int
bcm_hr4_cosq_field_classifier_map_set(int unit, int classifier_id, int count,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array);

extern int
bcm_hr4_cosq_field_classifier_map_get(int unit, int classifier_id,int array_max,
        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array, int *array_count);

extern int
bcm_hr4_cosq_field_classifier_map_clear(int unit, int classifier_id);

extern int
bcm_hr4_cosq_stat_set(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, uint64 value);
extern int
bcm_hr4_cosq_stat_get(int unit, bcm_gport_t port,
                      bcm_cos_queue_t cosq,
                      bcm_cosq_stat_t stat, int sync_mode,
                      uint64 *value);
extern int
bcm_hr4_cosq_gport_info_get(int unit, bcm_gport_t gport,
                             bcm_cosq_gport_level_info_t *info);

extern int
bcm_hr4_hgcos_select_port_control_set(int unit,  bcm_port_t port,
                                      bcm_port_control_t type, int value);
extern int
bcm_hr4_hgcos_select_port_control_get(int unit,  bcm_port_t port,
                                      bcm_port_control_t type, int *value);

extern int
bcm_hr4_cosq_safc_class_mapping_set(int unit, bcm_gport_t port, int array_count,
                                    bcm_cosq_safc_class_mapping_t *mapping_array);

extern int
bcm_hr4_cosq_safc_class_mapping_get(int unit, bcm_gport_t port, int array_max, 
                                    bcm_cosq_safc_class_mapping_t *mapping_array,
                                    int *array_count);
extern int
bcm_hr4_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count);

extern int
bcm_hr4_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count);

extern int
bcm_hr4_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array);

extern int
bcm_hr4_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array,
                                   int *array_count);

extern int bcm_hr4_cosq_port_attach(int unit, bcm_port_t port);
extern int bcm_hr4_cosq_port_detach(int unit, bcm_port_t port);

extern int bcmi_hr4_port_cosmap_update(int unit, pbmp_t pbmp, int enable);
extern void bcm_hr4_cosq_sw_dump(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int bcm_hr4_cosq_sync(int unit);
#endif

extern int
bcm_hr4_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable);

extern int
bcm_hr4_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq, int *enable);

extern int
bcm_hr4_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq, bcm_cosq_dir_t direction,
                                 int array_max, bcm_cosq_buffer_id_t *buf_id_array,
                                 int *array_count);

extern void bcm_hr4_pstats_deinit(int unit);
extern int  bcm_hr4_pstats_init(int unit);

extern int
_bcm_hr4_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              uint32 flags, bcm_port_t *ing_port,
                              bcm_cos_t *priority);

/* pfc deadlock */
#define HR4_PFC_DEADLOCK_MAX_COS 26

/* Below structure to contain register/field name specific to chip */
typedef struct _bcm_hr4_pfc_hw_resorces_s {
    soc_reg_t timer_count[HR4_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_status[HR4_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_mask[HR4_PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_en[HR4_PFC_DEADLOCK_MAX_COS];
    soc_reg_t chip_config[4];
    soc_reg_t port_config[2];
    soc_field_t cos_field[HR4_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_init_val[HR4_PFC_DEADLOCK_MAX_COS];
    soc_field_t time_unit_field;
    soc_field_t recovery_action_field;
    soc_field_t deadlock_status_field;
} _bcm_hr4_pfc_hw_resorces_t;

typedef struct _bcm_hr4_pfc_deadlock_control_s {
    uint8 pfc_deadlock_cos_max;  /* Max COS supported for PFC Deadlock
                                  * Detection and Recovery */
    uint8 pfc_deadlock_cos_used; /* Total COS configured for PFC Deadlock
                                  * Detection and Recovery */
    uint8 hw_cos_idx_inuse[HR4_PFC_DEADLOCK_MAX_COS];   /* Boolean for used Cos indices */
    uint8 cb_enabled;            /* Indicator if CB is enabled */
    bcm_switch_pfc_deadlock_detection_interval_t time_unit; /* 0 - 100ms,
                                                             * 1 - 10ms */
    uint32 cb_interval;          /* CB interval duration */
    bcm_cos_t pfc_pri2cos[HR4_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    bcm_cos_t pfc_cos2pri[HR4_PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                                      * computation */
    _bcm_pfc_deadlock_config_t cos_config[HR4_PFC_DEADLOCK_MAX_COS];
    _bcm_hr4_pfc_hw_resorces_t hw_regs_fields;
    bcm_switch_pfc_deadlock_action_t recovery_action;
    uint32 cb_count; /* Debug ctr to keep track of #times CB fn is called */
    _bcm_cosq_inv_mapping_get_f cosq_inv_mapping_get; /* Function to retrieve
                                                       * Port/Priority from a
                                                       * given Gport */
} _bcm_hr4_pfc_deadlock_control_t;

#define _BCM_HR4_PFC_DEADLOCK_HW_RES_INIT(deadlock_hw)        \
    do {                                                      \
        int i = 0;                                            \
        for (i = 0; i < HR4_PFC_DEADLOCK_MAX_COS; i++) {      \
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

#define _BCM_HR4_UNIT_PFC_DEADLOCK_CONTROL(u) _bcm_hr4_pfc_deadlock_control[(u)]
#define _BCM_HR4_PFC_DEADLOCK_CONFIG(u,cos) \
            (&((_bcm_hr4_pfc_deadlock_control[(u)])->cos_config[(cos)]))
#define _BCM_HR4_PFC_DEADLOCK_COS_ENABLED(u,cos) \
            ((((_bcm_hr4_pfc_deadlock_control[(u)])->cos_config[(cos)].flags) & \
            _BCM_PFC_DEADLOCK_F_ENABLE) ? TRUE : FALSE)
#define _BCM_HR4_PFC_DEADLOCK_CB_INTERVAL(u) \
            ((_bcm_hr4_pfc_deadlock_control[(u)])->cb_interval)
#define _BCM_HR4_PFC_DEADLOCK_CB_ENABLED(u) \
            (((_bcm_hr4_pfc_deadlock_control[(u)])->cb_enabled) ? (TRUE) :(FALSE))
#define _BCM_HR4_PFC_DEADLOCK_CB_COUNT(u) \
            ((_bcm_hr4_pfc_deadlock_control[(u)])->cb_count)
#define _BCM_HR4_PFC_DEADLOCK_TIME_UNIT(u) \
            ((_bcm_hr4_pfc_deadlock_control[(u)])->time_unit)
#define _BCM_HR4_PFC_DEADLOCK_RECOVERY_ACTION(u) \
            ((_bcm_hr4_pfc_deadlock_control[(u)])->recovery_action)

extern _bcm_hr4_pfc_deadlock_control_t *_bcm_hr4_pfc_deadlock_control[BCM_MAX_NUM_UNITS];

extern int _bcm_hr4_pfc_deadlock_recovery_reset(int unit);

extern int
_bcm_hr4_pfc_deadlock_control_set(int unit, bcm_switch_control_t type, int arg);

extern int
_bcm_hr4_pfc_deadlock_control_get(int unit, bcm_switch_control_t type, int *arg);

extern int bcm_hr4_pfc_deadlock_recovery_start(int unit,
                                                   bcm_port_t port, int pri);
extern int bcm_hr4_pfc_deadlock_recovery_exit(int unit,
                                                  bcm_port_t port, int pri);

extern int _bcm_hr4_pfc_deadlock_init(int unit);
extern int _bcm_hr4_pfc_deadlock_hw_res_init(int unit);

extern int _bcm_hr4_pfc_deadlock_deinit(int unit);
extern int _bcm_hr4_pfc_deadlock_reinit(int unit);


extern int
_bcm_hr4_pfc_deadlock_hw_cos_index_get(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_hr4_pfc_deadlock_hw_cos_index_set(int unit,
                                       bcm_cos_t priority,
                                       int *hw_cos_index);
extern int
_bcm_hr4_pfc_deadlock_chip_config_get(int unit,
                                      bcm_cos_t priority,
                                      soc_reg_t* chip_config);
extern int
_bcm_hr4_pfc_deadlock_hw_oper(int unit,
                              _bcm_pfc_deadlock_oper_t operation,
                               bcm_cos_t priority,
                              _bcm_pfc_deadlock_config_t *config);
extern int
_bcm_hr4_pfc_deadlock_q_config_helper(int unit,
                                      _bcm_pfc_deadlock_oper_t operation,
                                      bcm_gport_t gport,
                                      bcm_cosq_pfc_deadlock_queue_config_t *config,
                                      uint8 *enable_status);
extern int
bcm_hr4_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                                        bcm_port_t port, uint32 *rval32);
extern int
_bcm_hr4_pfc_deadlock_recovery_end(int unit, int cos, bcm_port_t port);
extern int
_bcm_hr4_pfc_deadlock_monitor(int unit);

extern int
bcm_hr4_pfc_deadlock_queue_info_get(int unit, bcm_gport_t gport, bcm_port_t *mmu_port,
                                    bcm_port_t *local_port, int *type, int *split);

extern int
bcm_hr4_dump_port_hsp(int unit, bcm_port_t port);
#if defined(INCLUDE_XFLOW_MACSEC)
extern
int
_bcmi_xflow_macsec_hr4_port_enable_set(int unit, int nport, bcm_port_t *port, int enable);
#endif
#endif /* BCM_HURRICANE4_SUPPORT  */
#endif /* !_BCM_INT_HURRICANE4_H_ */
