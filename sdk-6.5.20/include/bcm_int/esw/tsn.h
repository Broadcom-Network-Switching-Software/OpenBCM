/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tsn.h
 * Purpose:     Definitions for TSN.
 */

#ifndef _BCM_INT_TSN_H_
#define _BCM_INT_TSN_H_

#include <soc/defs.h>
#include <bcm/tsn.h>

#if defined(BCM_TSN_SUPPORT)

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_TSN_WB_DEFAULT_VERSION            SOC_SCACHE_VERSION(1, 0)
#define BCM_TSN_WB_SCACHE_PART_EGR_MTU                   (1)
#define BCM_TSN_WB_SCACHE_PART_EGR_STU                   (2)
#define BCM_TSN_WB_SCACHE_PART_TSN_STAT_PORTCNT          (3)
#define BCM_TSN_WB_SCACHE_PART_TSN_STAT_FLOWCNT          (4)
#define BCM_TSN_WB_SCACHE_PART_TSN_FLOWS                 (5)
#define BCM_TSN_WB_SCACHE_PART_TAF                       (6)
#if defined(BCM_TSN_SR_SUPPORT)
#define BCM_TSN_WB_SCACHE_PART_AUTO_LEARN                (7)
#define BCM_TSN_WB_SCACHE_PART_PRP_FORWARDING            (8)
#define BCM_TSN_WB_SCACHE_PART_SR_FLOWS                  (9)
#define BCM_TSN_WB_SCACHE_PART_MIN  (BCM_TSN_WB_SCACHE_PART_EGR_MTU)
#define BCM_TSN_WB_SCACHE_PART_MAX  (BCM_TSN_WB_SCACHE_PART_SR_FLOWS)
#else
#define BCM_TSN_WB_SCACHE_PART_MIN  (BCM_TSN_WB_SCACHE_PART_EGR_MTU)
#define BCM_TSN_WB_SCACHE_PART_MAX  (BCM_TSN_WB_SCACHE_PART_TAF)
#endif /* BCM_TSN_SR_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Macro:
 *     TSN_ALLOC
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
#define TSN_ALLOC(_unit_, _ptr_, _ptype_, _size_, _descr_, _dma_, _rv_) \
            do { \
                if (NULL == (_ptr_)) { \
                    if (0 == (_dma_)) { \
                        (_ptr_) = (_ptype_ *)sal_alloc((_size_), (_descr_)); \
                    } else { \
                        (_ptr_) = (_ptype_ *)soc_cm_salloc((_unit_), (_size_), \
                                                            (_descr_)); \
                    } \
                } \
                if ((_ptr_) != NULL) { \
                    sal_memset((_ptr_), 0, (_size_)); \
                    (_rv_) = BCM_E_NONE; \
                } else { \
                    LOG_ERROR(BSL_LS_BCM_TSN, \
                              (BSL_META("TSN Error: Allocation failure %s\n"), \
                               (_descr_))); \
                    (_rv_) = BCM_E_MEMORY; \
                } \
            } while (0)

/*
 * Macro:
 *     TSN_FREE
 * Purpose:
 *      Generic memory free routine.
 * Parameters:
 *    _unit_    - Unit.
 *    _ptr_     - Memory pointer to be free
 *    _dma_     - use sal_free or soc_cm_free.
 */
#define TSN_FREE(_unit_, _ptr_, _dma_) \
            do { \
                if (0 == (_dma_)) { \
                    sal_free((_ptr_)); \
                } else { \
                    soc_cm_sfree((_unit_), (_ptr_)); \
                } \
            } while (0)

#if defined(BCM_TSN_SR_SUPPORT)

/*
 * SR MAC proxy management
 */
extern int
bcmi_esw_tsn_sr_mac_proxy_insert(
    int unit,
    bcm_pbmp_t mp_pbmp,
    int *mp_index);
extern int
bcmi_esw_tsn_sr_mac_proxy_delete(int unit, int mp_index);
extern int
bcmi_esw_tsn_sr_mac_proxy_get(int unit, int mp_index, bcm_pbmp_t *mp_pbmp);

/* Per-chip SR MAC Proxy info */
typedef struct tsn_sr_mac_proxy_info_s {
    const soc_mem_t mem;
    const soc_field_t field;
} tsn_sr_mac_proxy_info_t;

/*
 * SR flow ID conversion between software flow ID and hardware flow ID
 */
extern int
bcmi_esw_tsn_sr_hw_flow_id_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    uint32 *hw_id);
extern int
bcmi_esw_tsn_sr_sw_flow_id_get(
    int unit, uint32 hw_id,
    bcm_tsn_sr_flow_t *flow_id);

/*
 * SR flowset reference count control (increase or decrease reference count)
 */
extern int
bcmi_esw_tsn_sr_flowset_ref_inc(int unit, bcm_tsn_sr_flowset_t flowset);
extern int
bcmi_esw_tsn_sr_flowset_ref_dec(int unit, bcm_tsn_sr_flowset_t flowset);

/*
 * SR flow to flowset converstion (identify the flowset based on a flow)
 */
extern int
bcmi_esw_tsn_sr_flowset_identify(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_flowset_t *flowset);

/*
 * Handle to an SR sequence number history slice.
 * SN history pool management is something that requires full optimization
 * for minimal allocation time to reduce overall time taken for flow learning.
 * Since different chips may have different seqnum pool parameters (number of
 * pools, pool size, min alocation, etc.) and different parameters would
 * require fine-tuned algorithms, the implementation is chip-specific.
 * This is the handle provided by chip-specific algorithm as the seqnum slice
 * used for a flow.
 */
typedef void *bcmi_tsn_sr_seqnum_slice_t;

/*
 * SR RX flow internal configuration for writing by platform specific code.
 * The public config structure is wrapped here with additional internal
 * fields (for extension) required by the platform specific code.
 */
typedef struct tsn_sr_rx_flow_s {
    bcmi_tsn_sr_seqnum_slice_t      sn_slice;
    bcm_tsn_sr_rx_flow_config_t     config;
} bcmi_tsn_sr_rx_flow_t;

/*
 * SR TX flow internal configuration for writing by platform specific code.
 * The public config structure is wrapped here with additional internal
 * fields (for extension) required by the platform specific code.
 */
typedef struct tsn_sr_tx_flow_s {
    bcm_tsn_sr_tx_flow_config_t     config;
} bcmi_tsn_sr_tx_flow_t;

/*
 * SR flow management platform specific data
 */
typedef struct bcmi_tsn_sr_flows_dev_info_s {
    /* Get number of RX/TX flows */
    int (*get_num_flows)(int unit, int *rx_flows, int *tx_flows);
    /* Get HW flow ID from flow index */
    int (*get_hw_flow_id)(int unit, int is_tx, int fidx, uint32 *hw_id);
    /* Get flow index from HW flow ID */
    int (*get_sw_flow_idx)(int unit, uint32 hw_id, int *is_tx, int *fidx);
    /* Check RX flow config (based on platform HW parameters) */
    int (*check_rx_flow_config)(int unit, bcm_tsn_sr_rx_flow_config_t *cfg);
    /* Check TX flow config (based on platform HW parameters) */
    int (*check_tx_flow_config)(int unit, bcm_tsn_sr_tx_flow_config_t *cfg);
    /* Clear RX flow config in HW */
    int (*clear_rx_flow)(int unit, int idx);
    /* Clear TX flow config in HW */
    int (*clear_tx_flow)(int unit, int idx);
    /* Write RX flow config to HW */
    int (*write_rx_flow)(int unit, int idx, bcmi_tsn_sr_rx_flow_t *config);
    /* Write TX flow config to HW */
    int (*write_tx_flow)(int unit, int idx, bcmi_tsn_sr_tx_flow_t *config);
#ifdef BCM_WARM_BOOT_SUPPORT
    /* Read RX flow config from HW (for warm boot) */
    int (*read_rx_flow)(int unit, int idx, bcmi_tsn_sr_rx_flow_t *config);
    /* Read TX flow config from HW (for warm boot) */
    int (*read_tx_flow)(int unit, int idx, bcmi_tsn_sr_tx_flow_t *config);
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* Get RX flow status */
    int (*get_rx_flow_status)(
            int unit, int idx, bcm_tsn_sr_rx_flow_status_t *status);
    /* Get TX flow status */
    int (*get_tx_flow_status)(
            int unit, int idx, bcm_tsn_sr_tx_flow_status_t *status);
    /* Reset RX flow  */
    int (*reset_rx_flow)(int unit, int idx, uint32 flags);
    /* Allocate seqnum history (optimized algorithm) */
    int (*seqnum_slice_alloc)(
            int unit, int winsize, bcmi_tsn_sr_seqnum_slice_t *slice);
    /* Free seqnum history (optimized algorithm) */
    int (*seqnum_slice_free)(int unit, bcmi_tsn_sr_seqnum_slice_t slice);
    /* Read values from flow seqnum history window */
    int (*seqnum_hist_read)(
            int unit, int idx, int winsize, bcmi_tsn_sr_seqnum_slice_t slice,
            int offset, int max_size, uint8 *history, int *actual);
    /* Write values from flow seqnum history window */
    int (*seqnum_hist_write)(
            int unit, int idx, int winsize, bcmi_tsn_sr_seqnum_slice_t slice,
            int offset, int size, uint8 *history);
    /* Reset values in flow seqnum history window */
    int (*seqnum_hist_reset)(
            int unit, int idx, int winsize, bcmi_tsn_sr_seqnum_slice_t slice,
            uint32 reset_mode);
    /* Clear RX flow config - mtu in HW */
    int (*clear_rx_flow_mtu)(int unit, int idx);
    /* Clear TX flow config - mtu in HW */
    int (*clear_tx_flow_mtu)(int unit, int idx);
} bcmi_tsn_sr_flows_dev_info_t;

/* SR Port Configuration */
typedef struct tsn_sr_port_dev_info_s {
#ifdef BCM_WARM_BOOT_SUPPORT
    int (*sr_port_prp_forwarding_wb_sync)(int unit,
                                          soc_scache_handle_t scache_handle);
    int (*sr_port_prp_forwarding_wb_reload)(int unit,
                                            soc_scache_handle_t scache_handle);
#endif /* BCM_WARM_BOOT_SUPPORT */
    int (*sr_port_config_set)(int, bcm_gport_t, bcm_tsn_sr_port_config_t *);
    int (*sr_port_config_get)(int, bcm_gport_t, bcm_tsn_sr_port_config_t *);
} tsn_sr_port_dev_info_t;

#endif /* BCM_TSN_SR_SUPPORT */

/*
 * TSN flow ID conversion between software flow ID and hardware flow ID
 */
extern int
bcmi_esw_tsn_hw_flow_id_get(int unit, bcm_tsn_flow_t flow_id, uint32 *hw_id);
extern int
bcmi_esw_tsn_sw_flow_id_get(int unit, uint32 hw_id, bcm_tsn_flow_t *flow_id);

/*
 * TSN flowset reference count control (increase or decrease reference count)
 */
extern int
bcmi_esw_tsn_flowset_ref_inc(int unit, bcm_tsn_flowset_t flowset);
extern int
bcmi_esw_tsn_flowset_ref_dec(int unit, bcm_tsn_flowset_t flowset);

/*
 * TSN flow to flowset converstion (identify the flowset based on a flow)
 */
extern int
bcmi_esw_tsn_flowset_identify(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flowset_t *flowset);

/*
 * TSN MTU reference count control (increase or decrease reference count)
 */
extern int
bcmi_esw_tsn_mtu_profile_ref_inc(
    int unit,
    int mtu_profile_id);
extern int
bcmi_esw_tsn_mtu_profile_ref_dec(
    int unit,
    int mtu_profile_id);

/*
 * TSN STU reference count control (increase or decrease reference count)
 */
extern int
bcmi_esw_tsn_stu_profile_ref_inc(
    int unit,
    int stu_profile_id);
extern int
bcmi_esw_tsn_stu_profile_ref_dec(
    int unit,
    int stu_profile_id);

/*
 * TSN MTU profile ID conversion between
 * software profile ID and hardware profile ID
 */
extern int
bcmi_esw_tsn_mtu_hw_profile_id_get(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    int *hw_profile_id);
extern int
bcmi_esw_tsn_mtu_sw_profile_id_get(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    int hw_profile_id,
    int *mtu_profile_id);

/*
 * TSN STU profile ID conversion between
 * software profile ID and hardware profile ID
 */
extern int
bcmi_esw_tsn_stu_hw_profile_id_get(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_profile_type_t *type,
    int *hw_profile_id);
extern int
bcmi_esw_tsn_stu_sw_profile_id_get(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    int hw_profile_id,
    int *stu_profile_id);

/*
 * TSN flow management platform specific data
 */
typedef struct bcmi_tsn_flows_dev_info_s {
    /* Get number of TSN flows */
    int (*get_num_flows)(int unit, int *flows);
    /* Get HW flow ID from flow index */
    int (*get_hw_flow_id)(int unit, int fidx, uint32 *hw_id);
    /* Get flow index from HW flow ID */
    int (*get_sw_flow_idx)(int unit, uint32 hw_id, int *fidx);
    /* Check TSN flow config (based on platform HW parameters) */
    int (*check_flow_config)(int unit, bcm_tsn_flow_config_t *config);
    /* Write flow config to HW */
    int (*write_flow)(int unit, int idx, bcm_tsn_flow_config_t *config);
#ifdef BCM_WARM_BOOT_SUPPORT
    /* Read flow config from HW (for warm boot) */
    int (*read_flow)(int unit, int idx, bcm_tsn_flow_config_t *config);
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* Clear flow config in HW */
    int (*clear_flow)(int unit, int idx);
    /* Clear flow config - mtu in HW */
    int (*clear_flow_mtu)(int unit, int idx);
} bcmi_tsn_flows_dev_info_t;

#if defined(BCM_TSN_SR_SUPPORT)

/* SR Auto Learn Group ports type enumerations */
typedef enum bcmi_tsn_sr_auto_learn_group_port_type_e {
    bcmi_sr_auto_learn_group_l2mc = 0, /* L2MC ports group */
    bcmi_sr_auto_learn_group_macp = 1, /* Mac Proxy ports group */
    bcmi_sr_auto_learn_group_count = 2 /* Must be the last */
} bcmi_tsn_sr_auto_learn_group_port_type_t;

/* SR Auto Learn TX/RX flow enumerations */
typedef enum bcmi_tsn_sr_auto_learn_flow_e {
    bcmi_sr_auto_learn_flow_tx = 0, /* SR Tx flow */
    bcmi_sr_auto_learn_flow_rx = 1, /* SR Rx flow */
    bcmi_sr_auto_learn_flow_count = 2 /* Must be the last */
} bcmi_tsn_sr_auto_learn_flow_t;

/* per-chip SR Auto learn drivers */
typedef struct tsn_sr_auto_learn_info_s {
    char *table_name;
    int (* table_size_get)(
        int unit,
        uint32 *table_size);
    uint32 max_flow_num[bcmi_sr_auto_learn_flow_count];
    int (*group_port_set)(
        int unit,
        bcmi_tsn_sr_auto_learn_group_port_type_t group_port_type,
        bcm_pbmp_t port_pbmp,
        int val);
    int (*group_port_delete)(
        int unit,
        bcmi_tsn_sr_auto_learn_group_port_type_t group_port_type,
        bcm_pbmp_t port_pbmp);
    int (*group_port_reset_all)(
        int unit);
    int (*flow_id_ctrl_set)(
        int unit,
        bcmi_tsn_sr_auto_learn_flow_t flow_type,
        uint32 current_fid,
        uint32 last_fid,
        uint32 id_size);
    int (*flow_id_ctrl_get)(
        int unit,
        bcmi_tsn_sr_auto_learn_flow_t flow_type,
        uint32 *current,
        uint32 *last,
        uint32 *id_size);
    int (*flow_id_pool_set)(
        int unit,
        bcmi_tsn_sr_auto_learn_flow_t flow_type,
        uint32 prev,
        uint32 fid);
    int (*flow_id_pool_get)(
        int unit,
        bcmi_tsn_sr_auto_learn_flow_t flow_type,
        uint32 prev,
        uint32 *fid);
    int (*enable)(
        int unit,
        int enable);
    int (*enable_get)(
        int unit,
        int *enabled);
} tsn_sr_auto_learn_info_t;
#endif /* BCM_TSN_SR_SUPPORT */

/*
 * TSN event management platform specific data
 */
 typedef enum bcmi_tsn_evt_inst_type_e {
    bcmiTsnEvtCESys = 0,
    bcmiTsnEvtCEFlow = 1,
    bcmiTsnEvtCEPort = 2,
    bcmiTsnEvtWLASys = 3,
    bcmiTsnEvtWLAFlow = 4,
    bcmiTsnEvtWLBSys = 5,
    bcmiTsnEvtWLBFlow = 6,
    bcmiTsnEvtSNSys = 7,
    bcmiTsnEvtSNFlow = 8,
    bcmiTsnEvtCount = 9
} bcmi_tsn_evt_inst_type_t;

typedef int (*bcmi_esw_tsn_flow_event_notify_cb)(
    int unit,
    bcm_tsn_event_type_t type,
    int multiple,
    int hw_flow_id);

typedef int (*bcmi_esw_tsn_cnt_exceed_notify_cb)(
    int unit,
    bcm_tsn_stat_t stat_type,
    bcm_tsn_stat_threshold_source_t th_src,
    int multiple,
    int index); /* port_id or hw_flow_id */

typedef struct bcmi_tsn_events_dev_info_s {
    /* Register the event notify callback */
    int (*register_flow_event_cb)(int unit,
                                  bcmi_esw_tsn_flow_event_notify_cb cb);
    int (*register_cnt_exceed_cb)(int unit,
                                  bcmi_esw_tsn_cnt_exceed_notify_cb cb);
    /* event validate */
    int (*validate_event)(int unit, bcmi_tsn_evt_inst_type_t inst_type);
    /* Enable/disable event mask */
    int (*event_mask_set)(int unit, bcm_tsn_event_type_t event, uint32 enable);
    /* Update rx flow information associate with event type */
    int (*update_rx_flow)(int unit, int type, int idx,
                          int val);
    /* Get the flow event state */
    int (*event_flow_stat_get)(int unit, bcm_tsn_event_type_t event,
                               SHR_BITDCL *flowstate,
                               SHR_BITDCL *filterflowstate,
                               uint32 flow_number);
    /* Clear the flow event state */
    int (*event_flow_stat_clear)(int unit, bcm_tsn_event_type_t event,
                                 SHR_BITDCL *flowstate, uint32 flow_number);
    /* Poll function invoked periodically */
    int (*poll)(int unit);
} bcmi_tsn_events_dev_info_t;

/* per-chip control drivers */
typedef struct tsn_chip_ctrl_info_s {
    int (*tsn_chip_ctrl_set)(int, bcm_tsn_control_t, uint32);
    int (*tsn_chip_ctrl_get)(int, bcm_tsn_control_t, uint32 *);
} tsn_chip_ctrl_info_t;

/* per-chip Priority Map drivers */
typedef struct tsn_pri_map_info_s {
    char *table_name;
    uint32 table_size;
    uint32 entry_size;
    int (*tsn_pri_map_set)(
        int unit,
        uint32 sw_idx,
        bcm_tsn_pri_map_config_t *config);
    int (*tsn_pri_map_get)(
        int unit,
        uint32 sw_idx,
        bcm_tsn_pri_map_config_t *config);
    int (*tsn_pri_map_delete)(
        int unit,
        uint32 sw_idx);
    int (*tsn_pri_map_hw_index_get)(
        int unit,
        uint32 sw_idx,
        uint32 *hw_index);
    int (*tsn_pri_map_sw_idx_get)(
        int unit,
        uint32 hw_index,
        uint32 *sw_idx);
    int (*tsn_pri_map_wb_hw_existed)(
        int unit,
        uint32 sw_idx);
} tsn_pri_map_info_t;

typedef struct tsn_mtu_info_s {
#ifdef BCM_WARM_BOOT_SUPPORT
    int (*egress_mtu_wb_reload)(
        int unit,
        int idx,
        bcm_tsn_pri_map_config_t *config);
#endif /* BCM_WARM_BOOT_SUPPORT */
    const uint32 logical_port_num;
    int (*mtu_profile_mem_idx_get)(
        int unit,
        bcm_tsn_mtu_profile_type_t type,
        int *min,
        int *max);
    int (*mtu_profile_create)(
        int unit,
        bcm_tsn_mtu_profile_type_t type,
        bcm_tsn_mtu_config_t *config,
        uint32 *index);
    int (*mtu_profile_set)(
        int unit,
        int index,
        bcm_tsn_mtu_profile_type_t type,
        bcm_tsn_mtu_config_t *config);
    int (*mtu_profile_get)(
        int unit,
        int index,
        bcm_tsn_mtu_profile_type_t type,
        bcm_tsn_mtu_config_t *config);
    int (*mtu_profile_destroy)(
        int unit,
        bcm_tsn_mtu_profile_type_t type,
        int index);
    int (*ingress_mtu_config_set)(
        int unit,
        bcm_tsn_ingress_mtu_config_t *config);
    int (*ingress_mtu_config_get)(
        int unit,
        bcm_tsn_ingress_mtu_config_t *config);
    int (*egress_mtu_port_control_set)(
        int unit,
        bcm_gport_t port,
        bcm_tsn_pri_map_config_t *config);
    int (*mtu_profile_decode)(
        int unit,
        int mtu_profile_id,
        bcm_tsn_mtu_profile_type_t *type,
        int *index);
    int (*mtu_profile_encode)(
        int unit,
        int index,
        bcm_tsn_mtu_profile_type_t type,
        int *mtu_profile_id);
    int (*mtu_profile_traverse)(
        int unit,
        bcm_tsn_mtu_profile_traverse_cb cb,
        void *user_data);
    int (*mtu_profile_mem_refcnt_get)(
        int unit,
        bcm_tsn_mtu_profile_type_t type,
        int index,
        int *ref_count);
    int (*ingress_mtu_port_control_set)(
        int unit,
        bcm_gport_t port,
        int index);
    int (*ingress_mtu_port_control_get)(
        int unit,
        bcm_gport_t port,
        int *index);
} tsn_mtu_info_t;

typedef struct tsn_stu_info_s {
#ifdef BCM_WARM_BOOT_SUPPORT
    int (*egress_stu_wb_reload)(
        int unit,
        int idx,
        bcm_tsn_pri_map_config_t *config);
#endif /* BCM_WARM_BOOT_SUPPORT */
    const uint32 logical_port_num;
    int (*stu_profile_mem_idx_get)(
        int unit,
        bcm_tsn_stu_profile_type_t type,
        int *min,
        int *max);
    int (*stu_profile_create)(
        int unit,
        bcm_tsn_stu_profile_type_t type,
        bcm_tsn_stu_config_t *config,
        uint32 *index);
    int (*stu_profile_set)(
        int unit,
        int index,
        bcm_tsn_stu_profile_type_t type,
        bcm_tsn_stu_config_t *config);
    int (*stu_profile_get)(
        int unit,
        int index,
        bcm_tsn_stu_profile_type_t type,
        bcm_tsn_stu_config_t *config);
    int (*stu_profile_destroy)(
        int unit,
        bcm_tsn_stu_profile_type_t type,
        int index);
    int (*ingress_stu_config_set)(
        int unit,
        bcm_tsn_ingress_stu_config_t *config);
    int (*ingress_stu_config_get)(
        int unit,
        bcm_tsn_ingress_stu_config_t *config);
    int (*egress_stu_port_control_set)(
        int unit,
        bcm_gport_t port,
        bcm_tsn_pri_map_config_t *config);
    int (*stu_profile_decode)(
        int unit,
        int stu_profile_id,
        bcm_tsn_stu_profile_type_t *type,
        int *index);
    int (*stu_profile_encode)(
        int unit,
        int index,
        bcm_tsn_stu_profile_type_t type,
        int *stu_profile_id);
    int (*stu_profile_traverse)(
        int unit,
        bcm_tsn_stu_profile_traverse_cb cb,
        void *user_data);
    int (*stu_profile_mem_refcnt_get)(
        int unit,
        bcm_tsn_stu_profile_type_t type,
        int index,
        int *ref_count);
    int (*ingress_stu_port_control_set)(
        int unit,
        bcm_gport_t port,
        int index);
    int (*ingress_stu_port_control_get)(
        int unit,
        bcm_gport_t port,
        int *index);
} tsn_stu_info_t;

/* structure for device specific info */
typedef struct bcmi_esw_tsn_dev_info_s {
#if defined(BCM_TSN_SR_SUPPORT)
    /* SR MAC Proxy info */
    const tsn_sr_mac_proxy_info_t *tsn_sr_mac_proxy_info;

    /* SR flow management */
    const bcmi_tsn_sr_flows_dev_info_t *sr_flows_info;

    /* SR port configuration */
    const tsn_sr_port_dev_info_t *sr_port_info;

    /* per-chip SR Auto Learn drivers */
    const tsn_sr_auto_learn_info_t *tsn_sr_auto_learn_info;
#endif /* BCM_TSN_SR_SUPPORT */

    /* TSN flow management */
    const bcmi_tsn_flows_dev_info_t *tsn_flows_info;

    /* per-chip Priority Map drivers */
    const tsn_pri_map_info_t *tsn_pri_map_info[bcmTsnPriMapTypeCount];

    /* per-chip MTU & STU control drivers */
    const tsn_mtu_info_t *tsn_mtu_info;
    const tsn_stu_info_t *tsn_stu_info;

    /* TSN event management */
    const bcmi_tsn_events_dev_info_t    *tsn_event_info;
    /* per-chip control drivers */
    const tsn_chip_ctrl_info_t *tsn_chip_ctrl_info;
} bcmi_esw_tsn_dev_info_t;

/*
 * TSN Priority Map conversion between software Map ID and hardware Index
 */
extern int
bcmi_esw_tsn_pri_map_hw_index_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_type_t *map_type,
    uint32 *hw_index);

extern int
bcmi_esw_tsn_pri_map_map_id_get(
    int unit,
    bcm_tsn_pri_map_type_t map_type,
    uint32 hw_index,
    bcm_tsn_pri_map_t *map_id);

/*
 * TSN Priority Map reference count control (increase or decrease reference
 * count)
 */
extern int
bcmi_esw_tsn_pri_map_ref_cnt_dec(
    int unit,
    bcm_tsn_pri_map_t map_id);

extern int
bcmi_esw_tsn_pri_map_ref_cnt_inc(
    int unit,
    bcm_tsn_pri_map_t map_id);

extern int
bcmi_esw_tsn_pri_map_ref_cnt_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    uint32 *ref_cnt);

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * TSN Priority Map warmboot recovery function provided for mtu/stu module
 */
extern int
bcmi_esw_tsn_pri_map_mtu_stu_wb_set(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config);

/* TSN Warmboot Sync */
extern int
bcm_esw_tsn_sync(int unit);

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* BCM_TSN_SUPPORT */

#endif /* _BCM_INT_TSN_H_ */
