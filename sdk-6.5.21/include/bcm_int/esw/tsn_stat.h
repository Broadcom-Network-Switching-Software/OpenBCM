/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 * This file contains TSN stat definitions internal to the BCM library.
 */

#ifndef __BCM_INT_TSN_STAT_H__
#define __BCM_INT_TSN_STAT_H__

#include <bcm/types.h>
#include <bcm/tsn.h>

#if defined(BCM_TSN_SUPPORT)
/* TSN stat portcnt table data structure */
typedef struct tsn_stat_portcnt_table_info_s {
    bcm_tsn_stat_t portcnt_stat;
    soc_mem_t portcnt_mem;
    soc_field_t portcnt_field;
} tsn_stat_portcnt_table_info_t;

/* Per-chip device info for TSN stat portcnt */
typedef struct tsn_stat_portcnt_dev_info_s {
    const tsn_stat_portcnt_table_info_t *portcnt_table_info;
    const int num_stats;
} tsn_stat_portcnt_dev_info_t;

/* reserved group id */
#define BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED (0)

/* action type for tsn stat counter */
typedef enum bcmi_tsn_stat_counter_action_e {
    bcmiTsnStatCounterActionWrite = 0,
    bcmiTsnStatCounterActionRead = 1,
    bcmiTsnStatCounterActionReadSync = 2,
    bcmiTsnStatCounterActionCount = 3
} bcmi_tsn_stat_counter_action_t;

/* per-chip control drivers */
typedef struct tsn_stat_flowcnt_dev_info_s {
    int (*tsn_stat_flowcnt_init) (int);
#if defined(BCM_WARM_BOOT_SUPPORT)
    int (*tsn_stat_flowcnt_get_scache_size) (int, int *);
    int (*tsn_stat_flowcnt_sync) (int, uint8 *);
    int (*tsn_stat_flowcnt_reload_l1) (int);
    int (*tsn_stat_flowcnt_reload_l2) (int, uint8 *);
#endif /* BCM_WARM_BOOT_SUPPORT */
    int (*tsn_stat_flowcnt_cleanup) (int);
    int (*tsn_stat_flowcnt_counter_sync_all) (int);
    int (*tsn_stat_flowcnt_counter_sync_stat) (int, bcm_tsn_stat_t);
    int (*tsn_stat_flowcnt_group_id_encode)(int, bcm_tsn_stat_group_type_t ,
                                            uint32, bcm_tsn_stat_group_t *);
    int (*tsn_stat_flowcnt_group_id_decode)(int, bcm_tsn_stat_group_t,
                                            bcm_tsn_stat_group_type_t *,
                                            uint32 *);
    int (*tsn_stat_flowcnt_group_refcnt_get)(int, bcm_tsn_stat_group_type_t,
                                             uint32, int *);
    int (*tsn_stat_flowcnt_group_enable_value_set)(int,
                                                   bcm_tsn_stat_group_type_t,
                                                   uint32, uint32);
    int (*tsn_stat_flowcnt_group_enable_value_get)(int,
                                                   bcm_tsn_stat_group_type_t,
                                                   uint32, uint32 *);
    int (*tsn_stat_flowcnt_group_enable_value_exist_check)
            (int, bcm_tsn_stat_group_type_t, uint32);
    int (*tsn_stat_flowcnt_group_enable_value_insert)(int,
                                                      bcm_tsn_stat_group_type_t,
                                                      uint32, uint32 *);
    int (*tsn_stat_flowcnt_group_enable_value_delete)(int,
                                                      bcm_tsn_stat_group_type_t,
                                                      uint32);
    int (*tsn_stat_flowcnt_group_enable_value_encode)(int,
                                                      bcm_tsn_stat_group_type_t,
                                                      bcm_tsn_stat_t *,
                                                      uint32, uint32 *);
    int (*tsn_stat_flowcnt_group_enable_value_decode)(int,
                                                      bcm_tsn_stat_group_type_t,
                                                      uint32, bcm_tsn_stat_t *,
                                                      uint32, int *);
    int (*tsn_stat_flowcnt_group_traverse)(int, bcm_tsn_stat_group_traverse_cb,
                                           void *);
    int (*tsn_stat_flowcnt_counter_access)(int, bcm_tsn_sr_flow_t,
                                           bcmi_tsn_stat_counter_action_t,
                                           uint32, bcm_tsn_stat_t *,
                                           uint64 *, uint64 *);
    int (*tsn_stat_flowcnt_counter_update)(int unit,
                                           soc_mem_t mem,
                                           int mem_idx,
                                           uint64 new_count);
    int (*tsn_stat_flowcnt_flow_set)(int, bcm_tsn_sr_flow_t,
                                     bcm_tsn_stat_group_type_t, uint32);
    int (*tsn_stat_flowcnt_flow_get)(int, bcm_tsn_sr_flow_t,
                                     bcm_tsn_stat_group_type_t, uint32 *);
    int (*tsn_stat_flowcnt_flow_get_fbmp)(int, bcm_tsn_stat_t,
                                          SHR_BITDCL*, uint32);
} tsn_stat_flowcnt_dev_info_t;

/*
 * Per-chip control drivers for TSN stat threshold
 */
typedef struct tsn_stat_threshold_ctrl_info_s {
    int (*tsn_stat_threshold_ctrl_init)(int);
    int (*tsn_stat_threshold_ctrl_set)(
        int, bcm_tsn_stat_threshold_source_t, bcm_tsn_stat_t,
        bcm_tsn_stat_threshold_config_t *);
    int (*tsn_stat_threshold_ctrl_get)(
        int, bcm_tsn_stat_threshold_source_t, bcm_tsn_stat_t,
        bcm_tsn_stat_threshold_config_t *);
} tsn_stat_threshold_ctrl_info_t;

/* per-chip control drivers for TAF gate */
typedef struct tsn_taf_gate_stat_dev_info_s {
    int (*tsn_taf_gate_stat_init)(int);
    int (*tsn_taf_gate_stat_cleanup)(int);
} tsn_taf_gate_stat_dev_info_t;

/*
 * Structure for TSN stat device specific info
 */
typedef struct bcmi_esw_tsn_stat_dev_info_s {
    /* Per-chip TSN stat portcnt device info */
    const tsn_stat_portcnt_dev_info_t *tsn_stat_portcnt_dev_info;
    /* Per-chip TSN stat flowcnt device info */
    const tsn_stat_flowcnt_dev_info_t *tsn_stat_flowcnt_dev_info;
    /* Per-chip TSN stat threshold control drivers */
    const tsn_stat_threshold_ctrl_info_t *tsn_stat_threshold_ctrl_info;
} bcmi_esw_tsn_stat_dev_info_t;

/* TSN stat portcnt usage bitmap operations modes */
typedef enum bcmi_tsn_stat_portcnt_used_operation_e {
    BCM_TSN_STAT_PORTCNT_USED_SET,
    BCM_TSN_STAT_PORTCNT_USED_CLEAR
} bcmi_tsn_stat_portcnt_used_operation_t;

/* Library-private functions exported from tsn_stat.c */
#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcmi_esw_tsn_stat_portcnt_sync(int unit);
extern int
bcmi_esw_tsn_stat_portcnt_reload(int unit);
extern int
bcmi_esw_tsn_stat_flowcnt_sync(int unit);
extern int
bcmi_esw_tsn_stat_flowcnt_reload(int unit);
extern int
bcmi_esw_tsn_taf_sync(int unit);
extern int
bcmi_esw_tsn_taf_reload(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
extern int
bcmi_esw_tsn_stat_init(int unit);
extern int
bcmi_esw_tsn_stat_cleanup(int unit);
extern int
bcmi_esw_tsn_stat_portcnt_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint64 val);
extern int
bcmi_esw_tsn_stat_portcnt_get(
    int unit,
    bcm_gport_t port,
    int sync_mode,
    bcm_tsn_stat_t stat,
    uint64 *val);
extern int
bcmi_esw_tsn_stat_portcnt_used_update(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    bcmi_tsn_stat_portcnt_used_operation_t op);
extern int
bcmi_esw_tsn_stat_portcnt_stat_sync(int unit, bcm_tsn_stat_t stat);
extern int
bcmi_esw_tsn_stat_portcnt_hw_sw_get(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_gport_t port,
    uint64 *hw_value,
    uint64 *sw_value);
extern int
bcmi_esw_tsn_stat_portcnt_hw_set(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_gport_t port,
    uint64 hw_value);
extern int
bcmi_esw_tsn_stat_threshold_set(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config);
extern int
bcmi_esw_tsn_stat_threshold_get(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config);

extern int
bcmi_esw_tsn_stat_flowcnt_group_create(int unit,
                                       bcm_tsn_stat_group_type_t group_type,
                                       int count, bcm_tsn_stat_t *stat_arr,
                                       bcm_tsn_stat_group_t *stat_id);
extern int
bcmi_esw_tsn_stat_flowcnt_group_set(int unit, bcm_tsn_stat_group_t stat_id,
                                    int count, bcm_tsn_stat_t *stat_arr);
extern int
bcmi_esw_tsn_stat_flowcnt_group_get(int unit, bcm_tsn_stat_group_t stat_id,
                                    bcm_tsn_stat_group_type_t *ret_group_type,
                                    int max, bcm_tsn_stat_t *stat_arr,
                                    int *count);
extern int
bcmi_esw_tsn_stat_flowcnt_group_destroy(int unit, bcm_tsn_stat_group_t stat_id);
extern int
bcmi_esw_tsn_stat_flowcnt_group_traverse(int unit,
                                         bcm_tsn_stat_group_traverse_cb cb,
                                         void *user_data);
extern int
bcmi_esw_tsn_stat_flowcnt_flow_set(int unit, bcm_tsn_sr_flow_t flow_id,
                                   bcm_tsn_stat_group_type_t group_type,
                                   bcm_tsn_stat_group_t new_stat_id);
extern int
bcmi_esw_tsn_stat_flowcnt_flow_get(int unit, bcm_tsn_sr_flow_t flow_id,
                                   bcm_tsn_stat_group_type_t group_type,
                                   bcm_tsn_stat_group_t *stat_id);

extern int
bcmi_esw_tsn_stat_flowcnt_set(int unit, bcm_tsn_sr_flow_t flow_id,
                              uint32 arr_cnt,
                              bcm_tsn_stat_t *stat_type_arr,
                              uint64 *stat_value_arr);
extern int
bcmi_esw_tsn_stat_flowcnt_get(int unit, bcm_tsn_sr_flow_t flow_id,
                              int sync_mode, uint32 arr_cnt,
                              bcm_tsn_stat_t *stat_type_arr,
                              uint64 *stat_value_arr);
extern int
bcmi_esw_tsn_stat_flowcnt_hw_sw_get(int unit, bcm_tsn_stat_t stat,
                                    bcm_tsn_sr_flow_t flow_id,
                                    uint64 *hw_value, uint64 *sw_value);
extern int
bcmi_esw_tsn_stat_flowcnt_hw_set(int unit, bcm_tsn_stat_t stat,
                                 bcm_tsn_sr_flow_t flow_id,
                                 uint64 hw_value);
extern int
bcmi_esw_tsn_stat_flowcnt_fbmp_get(int unit, bcm_tsn_stat_t stat,
                                   SHR_BITDCL *fbmp, uint32 fbmp_size);
extern int
bcmi_esw_tsn_stat_flowcnt_stat_sync(int unit, bcm_tsn_stat_t stat);

extern int
bcmi_esw_tsn_stat_counter_update(
    int unit,
    soc_mem_t mem,
    int mem_idx,
    uint64 new_count);

#endif /* BCM_TSN_SUPPORT */
#endif /* __BCM_INT_TSN_STAT_H__ */

