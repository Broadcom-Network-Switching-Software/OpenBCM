/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PFC Deadlock Detection & Recovery
 */

#ifndef _BCM_INT_PFC_DEADLOCK_H
#define _BCM_INT_PFC_DEADLOCK_H

#include <bcm/cosq.h>
#include <bcm_int/esw/cosq.h>

#define _BCM_PFC_DEADLOCK_F_ENABLE          0x1 /* Flag to indicate if feature
                                                 * enabled for given cos/pri */

#define PFC_DEADLOCK_MAX_COS BCM_COS_COUNT

typedef int (*_bcm_cosq_inv_mapping_get_f)(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq, uint32 flags,
                                       bcm_port_t *port, bcm_cos_t *priority);
typedef enum _bcm_pfc_deadlock_oper_e {
    _bcmPfcDeadlockOperGet = 0, /* For Get operation */
    _bcmPfcDeadlockOperSet = 1, /* For Set operation */
    _bcmPfcDeadlockOperCount = 2 /* Max Value */
} _bcm_pfc_deadlock_oper_t;

/* _bcm_pfc_deadlock_config_t will be cached for WB recovery.
 * Hence adding new members will cause increase in Cache size, which needs to be
 * taken care while allocating space.
 */
typedef struct _bcm_pfc_deadlock_config_s {
    uint8 flags; /* Flags to carry additional info regarding the config */
    uint16 port_recovery_count[SOC_MAX_NUM_PORTS]; /* port recovery count */
    bcm_cos_t priority; /* Priority for the current config */
    uint32 detection_timer; /* Detection timer for declaring deadlock */
    uint32 recovery_timer; /* Recovery timer for Action */
    bcm_pbmp_t enabled_ports; /* Bitmap of Enabled ports */
    bcm_pbmp_t deadlock_ports; /* Bitmap of Ports currently in Deadlock state */
} _bcm_pfc_deadlock_config_t;

/* Below structure to contain register/field name specific to chip */
typedef struct _bcm_pfc_hw_resorces_s {
    soc_reg_t timer_count[PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_status[PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_mask[PFC_DEADLOCK_MAX_COS];
    soc_reg_t timer_en[PFC_DEADLOCK_MAX_COS];
    soc_reg_t chip_config[2];
    soc_reg_t port_config;
    soc_field_t cos_field[PFC_DEADLOCK_MAX_COS];
    soc_field_t time_init_val[PFC_DEADLOCK_MAX_COS];
    soc_field_t time_unit_field;
    soc_field_t recovery_action_field;
    soc_field_t deadlock_status_field;
} _bcm_pfc_hw_resorces_t;

typedef struct _bcm_pfc_deadlock_control_s {
    uint8 pfc_deadlock_cos_max;  /* Max COS supported for PFC Deadlock
                                  * Detection and Recovery */
    uint8 pfc_deadlock_cos_used; /* Total COS configured for PFC Deadlock
                                  * Detection and Recovery */
    uint8 hw_cos_idx_inuse[2];   /* Boolean for used Cos indices */
    uint8 cb_enabled;            /* Indicator if CB is enabled */
    bcm_switch_pfc_deadlock_detection_interval_t time_unit; /* 0 - 100ms,
                                                             * 1 - 10ms */
    uint32 cb_interval;          /* CB interval duration */
    bcm_cos_t pfc_pri2cos[PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                           * computation */
    bcm_cos_t pfc_cos2pri[PFC_DEADLOCK_MAX_COS]; /* Mapping to reduce run-time
                                           * computation */
    _bcm_pfc_deadlock_config_t cos_config[PFC_DEADLOCK_MAX_COS];
    _bcm_pfc_hw_resorces_t hw_regs_fields;
    bcm_switch_pfc_deadlock_action_t recovery_action;
    uint32 cb_count; /* Debug ctr to keep track of #times CB fn is called */
    _bcm_cosq_inv_mapping_get_f cosq_inv_mapping_get; /* Function to retrieve
                                                       * Port/Priority from a
                                                       * given Gport */
} _bcm_pfc_deadlock_control_t;

typedef struct _bcm_pfc_deadlock_cb_s {
    bcm_cosq_pfc_deadlock_recovery_event_cb_t pfc_deadlock_cb;
    void * pfc_deadlock_userdata;
} _bcm_pfc_deadlock_cb_t;


#define _BCM_PFC_DEADLOCK_HW_RES_INIT(deadlock_hw)            \
    do {                                                      \
        int i = 0;                                            \
        for (i = 0; i < PFC_DEADLOCK_MAX_COS; i++) {          \
            (deadlock_hw)->timer_count[i] = (INVALIDr);       \
            (deadlock_hw)->timer_status[i] = (INVALIDr);      \
            (deadlock_hw)->timer_mask[i] = (INVALIDr);        \
            (deadlock_hw)->timer_en[i] = (INVALIDr);          \
            (deadlock_hw)->cos_field[i] = (INVALIDf);         \
            (deadlock_hw)->time_init_val[i] = (INVALIDf);     \
        }                                                     \
        (deadlock_hw)->port_config = (INVALIDr);              \
        (deadlock_hw)->chip_config[0] = (INVALIDr);           \
        (deadlock_hw)->chip_config[1] = (INVALIDr);           \
        (deadlock_hw)->time_unit_field = (INVALIDf);          \
        (deadlock_hw)->recovery_action_field = (INVALIDf);    \
        (deadlock_hw)->deadlock_status_field = (INVALIDf);    \
    } while(0);

extern _bcm_pfc_deadlock_control_t *_bcm_pfc_deadlock_control[BCM_MAX_NUM_UNITS];
extern _bcm_pfc_deadlock_cb_t *_bcm_pfc_deadlock_cb[BCM_MAX_NUM_UNITS];

#define _BCM_UNIT_PFC_DEADLOCK_CONTROL(u) _bcm_pfc_deadlock_control[(u)]
#define _BCM_UNIT_PFC_DEADLOCK_CB(u) _bcm_pfc_deadlock_cb[(u)]
#define _BCM_PFC_DEADLOCK_CONFIG(u,cos) \
            (&((_bcm_pfc_deadlock_control[(u)])->cos_config[(cos)]))
#define _BCM_PFC_DEADLOCK_COS_ENABLED(u,cos) \
            ((((_bcm_pfc_deadlock_control[(u)])->cos_config[(cos)].flags) & \
              _BCM_PFC_DEADLOCK_F_ENABLE) ? TRUE : FALSE)
#define _BCM_PFC_DEADLOCK_CB_INTERVAL(u) \
            ((_bcm_pfc_deadlock_control[(u)])->cb_interval)
#define _BCM_PFC_DEADLOCK_CB_ENABLED(u) \
            (((_bcm_pfc_deadlock_control[(u)])->cb_enabled) ? (TRUE) : (FALSE))
#define _BCM_PFC_DEADLOCK_CB_COUNT(u) \
            ((_bcm_pfc_deadlock_control[(u)])->cb_count)
#define _BCM_PFC_DEADLOCK_TIME_UNIT(u) \
            ((_bcm_pfc_deadlock_control[(u)])->time_unit)
#define _BCM_PFC_DEADLOCK_RECOVERY_ACTION(u) \
            ((_bcm_pfc_deadlock_control[(u)])->recovery_action)

extern int _bcm_pfc_deadlock_init(int unit);
extern int _bcm_pfc_deadlock_deinit(int unit);
#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_pfc_deadlock_alloc_size(int unit);
extern int _bcm_pfc_deadlock_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern int _bcm_pfc_deadlock_recovery_reset(int unit);
extern int _bcm_pfc_deadlock_config_set(int unit, bcm_cos_t priority,
                            bcm_cosq_pfc_deadlock_config_t *config);
extern int _bcm_pfc_deadlock_config_get(int unit, bcm_cos_t priority,
                            bcm_cosq_pfc_deadlock_config_t *config);
extern int _bcm_pfc_deadlock_queue_config_set(int unit, bcm_gport_t gport,
                            bcm_cosq_pfc_deadlock_queue_config_t *config);
extern int _bcm_pfc_deadlock_queue_config_get(int unit, bcm_gport_t gport,
                            bcm_cosq_pfc_deadlock_queue_config_t *config);
extern int _bcm_pfc_deadlock_queue_status_get(int unit, bcm_gport_t gport,
                                              uint8 *enable);
extern int _bcm_pfc_deadlock_info_get(int unit, bcm_cos_t priority,
                            bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info);
extern int _bcm_pfc_deadlock_control_set(int unit, bcm_switch_control_t type,
                                         int arg);
extern int _bcm_pfc_deadlock_control_get(int unit, bcm_switch_control_t type,
                                         int *arg);
extern int _bcm_pfc_deadlock_max_cos_get(int unit, int *entries);
extern int _bcm_pfc_deadlock_update_cos_used(int unit);
extern int _bcm_td3_pfc_deadlock_update_cos_used(int unit);
extern int _bcm_hr4_pfc_deadlock_update_cos_used(int unit);
extern int _bcm_hx5_pfc_deadlock_update_cos_used(int unit);
extern int _bcm_fb6_pfc_deadlock_update_cos_used(int unit);
extern int _bcm_pfc_deadlock_recovery_update(int unit, int cos);
extern int _bcm_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
                            bcm_port_t port, uint32 *rval32);
extern int _bcm_pfc_deadlock_ignore_pfc_xoff_clear(int unit, int cos,
                            bcm_port_t port);
extern int _bcm_cosq_pfc_deadlock_recovery_event_register(int unit, 
                  bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
                  void *userdata);
extern int _bcm_cosq_pfc_deadlock_recovery_event_unregister(int unit, 
                     bcm_cosq_pfc_deadlock_recovery_event_cb_t callback, 
                     void *userdata);
#ifdef BCM_TOMAHAWK3_SUPPORT
extern int _bcm_th3_pfc_deadlock_recovery_event_register(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata);
extern int _bcm_th3_pfc_deadlock_recovery_event_unregister(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata);
extern int _bcm_th3_pfc_deadlock_info_get(int unit, int priority,
                               bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info);
extern int _bcm_th3_pfc_deadlock_init(int unit);
extern int _bcm_th3_pfc_deadlock_deinit(int unit);
extern int _bcm_th3_pfc_deadlock_port_detach(int unit, bcm_port_t port);

extern int _bcm_th3_pfc_deadlock_port_status_get
                       (int unit, bcm_gport_t gport, uint8 *deadlock_status);
extern int _bcm_th3_pfc_switch_control_set
                    (int unit, bcm_switch_control_t type, int arg);
extern int _bcm_th3_pfc_switch_control_get
                    (int unit, bcm_switch_control_t type, int *arg);

extern int _bcm_th3_pfc_deadlock_recovery_reset(int unit);

extern int _bcm_th3_pfc_deadlock_reinit(int unit, uint8 **scache_ptr, uint16 recov_ver);
extern int
bcm_th3_pfc_deadlock_recovery_start(int unit, int port, int pfc_priority);
extern int
bcm_th3_pfc_deadlock_recovery_exit(int unit, int port, int pfc_priority);

#endif
#endif /* _BCM_INT_PFC_DEADLOCK_H */
