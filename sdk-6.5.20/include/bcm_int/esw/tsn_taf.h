/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tsn_taf.h
 * Purpose:     Definitions for TSN TAF.
 */

#ifndef _BCM_INT_TSN_TAF_H_
#define _BCM_INT_TSN_TAF_H_

#include <soc/defs.h>
#include <bcm/tsn.h>
#include <bcm_int/esw/tsn_stat.h>

#if defined(BCM_TSN_SUPPORT)
#define BCMI_TSN_TAF_GATE_NUM_MAX            (128)
#define BCMI_TSN_TAF_PROFILE_NUM_MAX         (200)

/* internal TAF profile data structure */
typedef struct bcmi_tsn_taf_schedule_entry_s bcmi_tsn_taf_schedule_entry_t;

typedef struct bcmi_tsn_taf_profile_s {
    /* data maintain at esw layer */
    int                             taf_gate;
    bcm_tsn_taf_profile_state_t     status;
    bcm_tsn_taf_profile_t           data;
    bcm_ptp_timestamp_t             config_change_time;

    /* data maintain at chip-specific layer */
    bcmi_tsn_taf_schedule_entry_t   *schedule;
} bcmi_tsn_taf_profile_t;

/* TAF schedule entry, double linked list */
struct bcmi_tsn_taf_schedule_entry_s {
    /* data maintain at chip-specific layer */
    bcmi_tsn_taf_profile_t *taf_profile;
    bcmi_tsn_taf_schedule_entry_t *prev;
    bcmi_tsn_taf_schedule_entry_t *next;
    bcm_ptp_timestamp_t schedule_time;
};

/* internal TAF event hanlder */
typedef struct bcmi_tsn_taf_event_handler_s bcmi_tsn_taf_event_handler_t;
struct bcmi_tsn_taf_event_handler_s {
    bcm_tsn_taf_event_cb cb;
    void *user_data;
    bcmi_tsn_taf_event_handler_t *next;
};

/* per-chip control drivers */
typedef struct bcmi_tsn_taf_dev_info_s {
    /* function set */
    int (*tsn_taf_init)(int);
    int (*tsn_taf_cleanup)(int);
    int (*tsn_taf_gate_create)(int, int, int *);
    int (*tsn_taf_gate_traverse)(int, bcm_tsn_taf_gate_traverse_cb, void *);
    int (*tsn_taf_gate_destroy)(int, int);
    int (*tsn_taf_gate_check)(int, int);
    int (*tsn_taf_status_get)(int, int, bcm_tsn_taf_status_t, uint32 *);
    int (*tsn_taf_global_control_get)(int, bcm_tsn_taf_control_t, uint32 *);
    int (*tsn_taf_global_control_set)(int, bcm_tsn_taf_control_t, uint32);
    int (*tsn_taf_port_control_get)(int, bcm_gport_t,
                                    bcm_tsn_control_t, uint32 *);
    int (*tsn_taf_port_control_set)(int, bcm_gport_t,
                                    bcm_tsn_control_t, uint32);
    int (*tsn_taf_gate_control_get)(int, int, bcm_tsn_taf_control_t, uint32 *);
    int (*tsn_taf_gate_control_set)(int, int, bcm_tsn_taf_control_t, uint32);
    int (*tsn_taf_gate_pri_map_set)(int, uint32, bcm_tsn_pri_map_config_t *);
    int (*tsn_taf_gate_pri_map_get)(int, uint32, bcm_tsn_pri_map_config_t *);
    int (*tsn_taf_gate_pri_map_delete)(int, uint32);
    int (*tsn_taf_calendar_param_check)(int, int, bcm_tsn_taf_profile_t *);
    int (*tsn_taf_calendar_resource_alloc)(int, int, bcm_tsn_taf_profile_t *);
    int (*tsn_taf_calendar_resource_free)(int, int, bcm_tsn_taf_profile_t *);
    int (*tsn_taf_calendar_kickoff)(int, int, bcmi_tsn_taf_profile_t *);
    int (*tsn_taf_calendar_profile_create)(int, bcm_tsn_taf_profile_t *,
                                           bcm_tsn_taf_profile_id_t *,
                                           bcmi_tsn_taf_profile_t **);
    int (*tsn_taf_calendar_profile_get)(int, bcm_tsn_taf_profile_id_t,
                                        bcmi_tsn_taf_profile_t **);
    int (*tsn_taf_calendar_profile_destroy)(int, bcm_tsn_taf_profile_id_t);
    int (*tsn_taf_calendar_profile_traverse)(int, int,
                                             bcm_tsn_taf_profile_traverse_cb,
                                             void *);
    int (*tsn_taf_schedule_insert)(int, int, bcm_ptp_timestamp_t *,
                                   bcmi_tsn_taf_profile_t *);
    int (*tsn_taf_schedule_remove)(int, int,
                                   bcmi_tsn_taf_profile_t *);

    int (*tsn_taf_schedule_pick)(int, int,
                                 bcmi_tsn_taf_profile_t *,
                                 bcmi_tsn_taf_profile_t **);

    int (*tsn_taf_schedule_exist)(int, bcmi_tsn_taf_profile_t *, int *);
    int (*tsn_taf_schedule_get)(int, bcmi_tsn_taf_profile_t *,
                                bcm_ptp_timestamp_t *);
    int (*tsn_taf_event_register)(int, int, bcm_tsn_taf_event_types_t,
                                  bcm_tsn_taf_event_cb, void *);
    int (*tsn_taf_event_unregister)(int, int, bcm_tsn_taf_event_types_t,
                                    bcm_tsn_taf_event_cb);
    int (*tsn_taf_event_notify)(int, int, bcm_tsn_taf_event_type_t);
    int (*tsn_taf_interrupt_handle_start)(int);
    int (*tsn_taf_interrupt_handle_tod)(int, int *);
    int (*tsn_taf_interrupt_handle_switch)(int, int *, SHR_BITDCL *, uint32);
    int (*tsn_taf_interrupt_handle_error)(int, int *, SHR_BITDCL *, uint32);
    int (*tsn_taf_interrupt_handle_user_cb)(int);
    int (*tsn_taf_interrupt_handle_finish)(int);
    int (*tsn_taf_gate_max_bytes_profile_create)(int, int, uint64, int *);
    int (*tsn_taf_gate_max_bytes_profile_set)(int, int, int, uint64);
    int (*tsn_taf_gate_max_bytes_profile_get)(int, int, int, uint64 *);
    int (*tsn_taf_gate_max_bytes_profile_destroy)(int, int, int);
    int (*tsn_taf_gate_max_bytes_profile_traverse)(
        int, int, bcm_tsn_taf_gate_max_bytes_profile_traverse_cb, void *);
    int (*tsn_taf_cosq_mapping_profile_create)(int, int *);
    int (*tsn_taf_cosq_mapping_profile_set)(int, int, bcm_cos_t,
                                            bcm_cos_queue_t);
    int (*tsn_taf_cosq_mapping_profile_get)(int, int, bcm_cos_t,
                                            bcm_cos_queue_t *);
    int (*tsn_taf_cosq_mapping_profile_destroy)(int, int);
    int (*tsn_taf_cosq_mapping_profile_traverse)(
        int, bcm_tsn_taf_cosq_mapping_profile_traverse_cb, void *);
    int (*tsn_taf_gate_stat_counter_sync)(int, int);
    int (*tsn_taf_gate_stat_counter_access)(int, int, uint32,
                                            bcm_tsn_taf_gate_stat_t *, uint64 *,
                                            bcmi_tsn_stat_counter_action_t);
#if defined(BCM_WARM_BOOT_SUPPORT)
    int (*tsn_taf_get_scache_size)(int, int *);
    int (*tsn_taf_sync)(int, uint8 *);
    int (*tsn_taf_reload)(int, uint8 *);
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* data set */
    uint32 (*tsn_taf_gate_profile_num)(int);
    uint32 tsn_taf_gate_intpri_num;
} bcmi_tsn_taf_dev_info_t;

extern int bcmi_gh2_taf_gate_pri_map_hw_idx_get(
            int unit, uint32 sw_idx, uint32 *hw_idx);
extern int bcmi_gh2_taf_gate_pri_map_sw_idx_get(
            int unit, uint32 hw_idx, uint32 *sw_idx);
extern int bcmi_esw_tsn_taf_port_control_set(
            int unit, bcm_gport_t gport, bcm_tsn_control_t type, uint32 arg);
extern int bcmi_esw_tsn_taf_port_control_get(
            int unit, bcm_gport_t gport, bcm_tsn_control_t type, uint32 *arg);
extern int bcmi_esw_tsn_taf_init(int unit);
extern int bcmi_esw_tsn_taf_coldboot_init(int unit);
extern void bcmi_esw_tsn_taf_cleanup(int unit);
extern tsn_pri_map_info_t bcmi_taf_gate_pri_map_info;
#endif /* BCM_TSN_SUPPORT */
#endif  /* _BCM_INT_TSN_TAF_H_ */
