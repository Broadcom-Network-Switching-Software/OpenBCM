/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tas.h
 * Purpose:     Definitions for TAS.
 */

#ifndef _BCM_INT_TAS_H_
#define _BCM_INT_TAS_H_

#include <bcm/types.h>
#include <bcm/cosq.h>

#ifdef BCM_TAS_SUPPORT
/*
 * Macro:
 *     TAS_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _unit_    - Unit.
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 *    _dma_     - use sal_alloc or soc_cm_alloc.
 *    _rv_      - return value
 */
#define TAS_ALLOC(_unit_, _ptr_, _ptype_, _size_, _descr_, _dma_, _rv_) \
            do { \
                if (NULL == (_ptr_)) { \
                    if (0 == (_dma_)) { \
                        (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                    } else { \
                        (_ptr_) = (_ptype_ *)soc_cm_salloc((_unit_), (_size_), \
                                                            (_descr_)); \
                    } \
                    if ((_ptr_) != NULL) { \
                        sal_memset((_ptr_), 0, (_size_)); \
                        _rv_ = BCM_E_NONE; \
                    }  else { \
                        LOG_ERROR(BSL_LS_BCM_TAS, \
                                  (BSL_META("TAS Error: Allocation failure "\
                                   "%s\n"), (_descr_))); \
                        _rv_ = BCM_E_MEMORY; \
                    } \
                }\
            } while (0)

#define TAS_FREE(_unit_, _ptr_, _dma_) \
            do {\
                    if (0 == (_dma_)) { \
                        sal_free((_ptr_)); \
                    } else { \
                        soc_cm_sfree((_unit_), (_ptr_)); \
                    }\
            } while (0)

/* Define maximum nanosecond value is 10^9 */
#define TAS_PTP_TIME_NANOSEC_MAX    (1000000000)

/* TAS profile event type */
typedef enum {
    bcmiTASProfileEventTODWindow = 0,
    bcmiTASProfileEventProfileChange = 1,
    bcmiTASProfileEventECCErr = 2,
    bcmiTASProfileEventNextCycleTimeErr = 3,
    bcmiTASProfileEventBaseTimeErr = 4,
    bcmiTASProfileEventPtrErr = 5,
    bcmiTASProfileEventTASDisable = 6,
    bcmiTASProfileEventCount = 7
} bcmi_tas_profile_event_type_t;

/* Callback function to notify the config_change_time when the profile state
  from comitted to pending */
typedef int (*profile_commit_cb_fn)(int unit, bcm_port_t lport,
                                    bcm_ptp_timestamp_t config_change_time,
                                    void *fn_data);

/* Callback function to nofity the profile event */
typedef int (*profile_event_cb_fn)(int unit, bcm_port_t lport,
                                   bcmi_tas_profile_event_type_t event);

typedef struct tas_drv_s {
    int (*tas_control_set)(int, bcm_port_t, bcm_cosq_tas_control_t, int);
    int (*tas_control_get)(int, bcm_port_t, bcm_cosq_tas_control_t, int *);
    int (*tas_status_get)(int, bcm_port_t, bcm_cosq_tas_status_t, int *);
    int (*tas_profile_init)(int unit, profile_event_cb_fn cb);
    int (*tas_profile_deinit)(int unit);
    int (*tas_profile_param_check)(int unit, bcm_cosq_tas_profile_t *profile);
    int (*tas_profile_basetime_check)(int unit, bcm_ptp_timestamp_t basetime);
    int (*tas_profile_delay_param_get)(int unit,
                                       int tas_delay_param,
                                       bcm_port_t port, uint32 qmap,
                                       int *value);
    int (*tas_profile_idx_get)(int unit, bcm_port_t lport, int *config_idx);
    int (*tas_profile_entries_write)(int unit, bcm_port_t lport, int config_idx,
                                     bcm_cosq_tas_entry_t *entries,
                                     int num_entries);
    int (*tas_profile_schedule_config)(int unit, bcm_port_t lport,
                                       bcm_ptp_timestamp_t base_time,
                                       uint32 cycle_time,
                                       uint32 cycle_extension,
                                       profile_commit_cb_fn cb,
                                       void *fn_data);
    int (*tas_profile_change)(int unit, bcm_port_t lport);
#ifdef BCM_WARM_BOOT_SUPPORT
    int (*tas_profile_sync)(int unit);
#endif
} tas_drv_t;

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcmi_esw_tas_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
bcmi_esw_tas_init(int unit);

extern int
bcmi_esw_tas_deinit(int unit);

extern int
bcmi_esw_tas_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_status_t type,
    int *arg);

extern int
bcmi_esw_tas_control_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int *arg);

extern int
bcmi_esw_tas_control_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_control_t type,
    int arg);

extern int
bcmi_esw_tas_profile_create(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid);

extern int
bcmi_esw_tas_profile_commit(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid);

extern int
bcmi_esw_tas_profile_destroy(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid);

extern int
bcmi_esw_tas_profile_destroy_all(
    int unit,
    bcm_gport_t port);

extern int
bcmi_esw_tas_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile);

extern int
bcmi_esw_tas_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t *profile);

extern int
bcmi_esw_tas_profile_traverse(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_traverse_cb cb,
    void *user_data);

extern int
bcmi_esw_tas_profile_status_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t *status);

/* TAS property types */
/* spn_TAS_CALENDAR_AUTO_ADJUST_FOR_TXOVERRUN */
#define TAS_PROP_TXOVERRUN      (0x1)
/* spn_TAS_CALENDAR_AUTO_ADJUST_FOR_HOLDADVANCE */
#define TAS_PROP_HOLDADVANCE    (0x2)
/* spn_TAS_CALENDAR_AUTO_ADJUST_REF_MAXSDU */
#define TAS_PROP_MAXSDU         (0x3)
extern int
bcmi_esw_tas_property_get(
    int unit,
    int tas_prop,
    int port,
    int cos,
    int *value);

/* TAS delay paramters */
#define TAS_DPARAM_TGCR_FIXED     (0x1) /* Tgcr value independent from queue */
#define TAS_DPARAM_TGCR_DYNAMIC   (0x2) /* Tgcr value depends on queue */
#define TAS_DPARAM_THA            (0x3) /* Tha value */
extern int
bcmi_esw_tas_profile_delay_param_get(
    int unit,
    int tas_delay_param,
    bcm_port_t port,
    uint32 qmap,
    int *value);

extern int
bcmi_tas_current_time(int unit, bcm_ptp_timestamp_t *current_time);

extern uint64
bcmi_tas_ptp_to_ns(bcm_ptp_timestamp_t ptp_time);

extern void
bcmi_tas_ns_to_ptp(uint64 ns, bcm_ptp_timestamp_t *ptp_time);

#endif /* BCM_TAS_SUPPORT */
#endif /* _BCM_INT_TAS_H_ */
