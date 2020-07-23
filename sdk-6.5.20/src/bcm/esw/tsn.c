 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <bcm/tsn.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_taf.h>
#include <bcm_int/esw/tsn_stat.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#include <bcm_int/esw/tsn_stat.h>
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>

#if defined(BCM_TSN_SUPPORT)
/* Function Enter/Leave Tracing */
#define TSN_FUNCTION_TRACE(_u_, _str_)                                        \
    do {                                                                      \
        LOG_DEBUG(BSL_LS_BCM_TSN,                                             \
                  (BSL_META_U((_u_),                                          \
                              "%s: " _str_ "\n"), FUNCTION_NAME()));          \
    } while (0)

/* Device Information NULL error handling */
#define TSN_CHECK_NULL_DEV_INSTANCE(_u_, _instance_)                          \
    do {                                                                      \
        if (NULL == (_instance_)) {                                           \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "Error: NULL chip specific function \n"))); \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

#if defined(BCM_TSN_SR_SUPPORT)

/**************** TSN SR Mac Proxy ****************/
/*
 * TSN SR Mac Proxy entry data structure
 */
typedef struct bcmi_esw_tsn_sr_mac_proxy_entry_s {
    bcm_pbmp_t mp_pbmp;
    int ref_count;
} bcmi_esw_tsn_sr_mac_proxy_entry_t;

/*
 * TSN SR Mac Proxy bookkeeping info data structure
 */
typedef struct bcmi_esw_tsn_sr_mac_proxy_bk_info_s {
    sal_mutex_t sr_mp_mutex;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *sr_mp_entry;
    int sr_mp_num;
} bcmi_esw_tsn_sr_mac_proxy_bk_info_t;

/* TSN SR Mac Proxy bookkeeping instance lock/unlock */
#define SR_MAC_PROXY_LOCK(sr_mp_bk_info) \
        sal_mutex_take(sr_mp_bk_info->sr_mp_mutex, sal_mutex_FOREVER)
#define SR_MAC_PROXY_UNLOCK(sr_mp_bk_info) \
        sal_mutex_give(sr_mp_bk_info->sr_mp_mutex)
/************** End of TSN SR Mac Proxy **************/

/*
 *
 * Seamless Redundancy (SR) Flow Management
 */

/*
 * SR flow index allocation entry:
 *   We need to minimize the time taken for picking a free flow index.
 *   To achieve this, we use linked list with pre-allocated entries so that
 *   we can do it in O(1).
 *   Flow index is a zero-based serial number which can be converted to
 *   SW flow ID (used by API) or HW flow ID (maintained by the platform).
 */
typedef struct tsn_sr_flowidx_entry_s {
    int                             flow_idx;
    struct tsn_sr_flowidx_entry_s   *next;
} tsn_sr_flowidx_entry_t;

/* SR RX/TX flowset */
typedef struct tsn_sr_flowset_s {
    bcm_tsn_pri_map_t               map_id;     /* priority map ID */
    tsn_sr_flowidx_entry_t          *flow_ide;  /* flow index entries */
    int                             ref_count;  /* reference count */
    int                             num_flows;  /* flow count (cache) */
    union {
        bcm_tsn_sr_rx_flow_config_t rx;
        bcm_tsn_sr_tx_flow_config_t tx;
    }                               config;     /* default flow config */
    sal_mutex_t                     mutex;      /* to protect flowset slot */
} tsn_sr_flowset_t;

/* SR flow direction constant used when accessing shared data structures */
#define SR_FLOW_DIR_RX          (0)
#define SR_FLOW_DIR_TX          (1)
#define SR_FLOW_DIR_COUNT       (2)

/* SR flow management book keeping */
typedef struct tsn_sr_flows_bookkeeping_s {
    /* SR flow index quick allocation */
    tsn_sr_flowidx_entry_t          *flowids_avail[2];  /* RX/TX linked list */
    tsn_sr_flowidx_entry_t          *flowids_allocated; /* for freeing */

    /* SR flowset entries */
    tsn_sr_flowset_t                *flowsets[2];       /* RX/TX arrays */

    /* SR flow entries (not shared since the type is quite different) */
    bcmi_tsn_sr_rx_flow_t           *flows_rx;          /* indexed array */
    sal_mutex_t                     *flows_rx_mutex;    /* for RX flow slot */
    bcmi_tsn_sr_tx_flow_t           *flows_tx;          /* indexed array */
    sal_mutex_t                     *flows_tx_mutex;    /* for TX flow slot */

    /* for efficiency */
    int                             max_flows[2];       /* RX/TX */

    /* To protect flow list (allocate/free) */
    sal_mutex_t                     fmgmt_mutex;
} tsn_sr_flows_bookkeeping_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* SR RX flowset for warmboot */
typedef struct tsn_sr_rx_flowset_wb_s {
    int                             flow_idx;   /* Flow index */
    bcm_tsn_pri_map_t               map_id;     /* priority map ID */
    bcm_tsn_sr_rx_flow_config_t     config;     /* Flow default config */
} tsn_sr_rx_flowset_wb_t;

/* SR TX flowset for warmboot */
typedef struct tsn_sr_tx_flowset_wb_s {
    int                             flow_idx;   /* Flow index */
    bcm_tsn_pri_map_t               map_id;     /* priority map ID */
    bcm_tsn_sr_tx_flow_config_t     config;     /* Flow default config */
} tsn_sr_tx_flowset_wb_t;
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 *
 * Seamless Redundancy (SR) Auto Learn Management
 */

/*
 * Structure definition for SR Auto Learn bookkeeping info
 */
typedef struct bcmi_esw_tsn_sr_auto_learn_group_bk_data_s {
    int group_id;
    int redundant_pro_idx[bcmi_sr_auto_learn_group_count];
    int interlink_pro_idx;
    bcm_tsn_sr_auto_learn_group_config_t config;
} bcmi_esw_tsn_sr_auto_learn_group_bk_data_t;

typedef struct bcmi_esw_tsn_sr_auto_learn_bk_data_s {
    int enabled;
    bcm_tsn_sr_auto_learn_config_t config;
    uint32 flowsets_cnt[bcmi_sr_auto_learn_flow_count];
    bcm_tsn_sr_flowset_t *flowsets[bcmi_sr_auto_learn_flow_count];
} bcmi_esw_tsn_sr_auto_learn_bk_data_t;

typedef struct bcmi_esw_tsn_sr_auto_learn_bk_s {
    SHR_BITDCL *intf_bitmap;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *group_data;
    uint32 valid_count;
    uint32 valid_sw_idx_start;
    uint32 valid_sw_idx_end;
    uint32 empty_sw_idx_start;
    uint32 empty_sw_idx_end;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t data;
    sal_mutex_t bk_mutex;
    sal_mutex_t mutex;
    int initialized; /* for lazy initialization */
} bcmi_esw_tsn_sr_auto_learn_bk_t;
#endif /* BCM_TSN_SR_SUPPORT */

/*
 *
 * TSN Flow (TSN Circuit ID) Management
 */

/*
 * TSN flow index allocation entry (similar mechanism as SR flows)
 */
typedef struct tsn_flowidx_entry_s {
    int                             flow_idx;
    struct tsn_flowidx_entry_s      *next;
} tsn_flowidx_entry_t;

/* TSN flowset */
typedef struct tsn_flowset_s {
    bcm_tsn_pri_map_t               map_id;     /* priorty map ID */
    tsn_flowidx_entry_t             *flow_ide;  /* flow index entries */
    int                             ref_count;  /* reference count */
    int                             num_flows;  /* flow count (cache) */
    bcm_tsn_flow_config_t           config;     /* default flow config */
    sal_mutex_t                     mutex;      /* to protect flowset slot */
} tsn_flowset_t;

/* TSN flow management book keeping */
typedef struct tsn_flows_bookkeeping_s {
    /* TSN flow index quick allocation */
    tsn_flowidx_entry_t             *flowids_avail;     /* linked list */
    tsn_flowidx_entry_t             *flowids_allocated; /* for freeing */

    /* TSN flowset entries */
    tsn_flowset_t                   *flowsets;          /* indexed arrays */

    /* TSN flow entries (not shared since the type is quite different) */
    bcm_tsn_flow_config_t           *flows;             /* indexed array */
    sal_mutex_t                     *flows_mutex;       /* for flow slot */

    /* for efficiency */
    int                             max_flows;

    /* To protect flow list (allocate/free) */
    sal_mutex_t                     fmgmt_mutex;
} tsn_flows_bookkeeping_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* TSN flowset for warmboot */
typedef struct tsn_flowset_wb_s {
    int                             flow_idx;   /* Flow index */
    bcm_tsn_pri_map_t               map_id;     /* priority map ID */
    bcm_tsn_flow_config_t           config;     /* Flow default config */
} tsn_flowset_wb_t;
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Initialization flag for TSN bookkeeping info structure */
static int tsn_bk_info_initialized = 0;

/* SW structure for storing the ingress & egress mtu priority map config ID */
typedef struct bcmi_esw_tsn_mtu_bk_info_s {
    int *mtu_refcount[bcmTsnMtuProfileTypeCount];
    bcm_tsn_pri_map_t *pri_map_port;
    sal_mutex_t mutex;
} bcmi_esw_tsn_mtu_bk_info_t;

/* SW structure for storing the ingress & egress stu priority map config ID */
typedef struct bcmi_esw_tsn_stu_bk_info_s {
    int *stu_refcount[bcmTsnStuProfileTypeCount];
    bcm_tsn_pri_map_t *pri_map_port;
    sal_mutex_t mutex;
} bcmi_esw_tsn_stu_bk_info_t;

/*
 * Structure definition for Priority Map bookkeeping info
 */
typedef struct bcmi_esw_tsn_pri_map_bk_info_data_s {
    bcm_tsn_pri_map_t map_id;
    bcm_tsn_pri_map_config_t config;
    uint32 ref_cnt;
} bcmi_esw_tsn_pri_map_bk_info_data_t;

typedef struct bcmi_esw_tsn_pri_map_bk_info_s {
    SHR_BITDCL *intf_bitmap[bcmTsnPriMapTypeCount];
    bcmi_esw_tsn_pri_map_bk_info_data_t *data[bcmTsnPriMapTypeCount];
    uint32 valid_count[bcmTsnPriMapTypeCount];
    uint32 valid_sw_idx_start[bcmTsnPriMapTypeCount];
    uint32 valid_sw_idx_end[bcmTsnPriMapTypeCount];
    uint32 empty_sw_idx_start[bcmTsnPriMapTypeCount];
    uint32 empty_sw_idx_end[bcmTsnPriMapTypeCount];
    sal_mutex_t mutex, bk_mutex;
} bcmi_esw_tsn_pri_map_bk_info_t;

/*
 *
 * TSN Event Management
 */

/*
 * Structure definition for event instance info
 * NOTE: event instance indicats the event with speficied source type
 */
typedef struct tsn_evt_inst_s {
    struct tsn_evt_inst_s   *next;      /* next instance */
    bcm_tsn_event_cb        cb;         /* registered callback */
    void                    *user_data; /* registered user data */
    bcm_port_t              port;       /* specified port */
    bcm_tsn_sr_flow_t       sr_flow;    /* specified sr_flow */
} tsn_evt_inst_t;

/* event instance resource allocation info */
typedef struct tsn_evt_inst_alloc_s {
    bcmi_tsn_evt_inst_type_t    inst_type;  /* event instance type */
    bcm_tsn_event_type_t        evt_type;   /* associated event type */
    int                         source_type;    /* associated source type */
    int                         inst_limits;    /* pre-allocated limits */
} tsn_evt_inst_alloc_t;

/* Structure definition for registered event instance info */
typedef struct tsn_evt_regist_s {
    bcmi_tsn_evt_inst_type_t    inst_type;   /* event instance type */
    bcm_tsn_event_type_t        evt_type;    /* associated event type */
    int                         source_type; /* associated source type */
    tsn_evt_inst_t              *evt_inst;   /* event instance link list */
    sal_mutex_t                 mutex;  /* protect event instance link list */
} tsn_evt_regist_t;

/* Structure definition for registered event statistics info */
typedef struct tsn_evt_stat_s {
    int         event_ref;  /* event reference count */
    int         *port_ref;  /* port basis event referece count */
    int         *flow_ref;  /* flow basis event referece count */
    SHR_BITDCL  *pbmp;  /* port bitmap on this event */
    SHR_BITDCL  *fbmp;  /* flow bitmap on this event */
    SHR_BITDCL  *ffbmp;  /* filter flow bitmap on this event */
} tsn_evt_stat_t;

/* Structure definition for event management bookkeeping info */
typedef struct tsn_event_bk_s {
    /* Central event instance resource */
    tsn_evt_inst_t      *einst_avail[bcmiTsnEvtCount];  /* event linked list */
    tsn_evt_inst_t      *einst_allocated;    /* for freeing */

    /* Registered event arrays */
    tsn_evt_regist_t    eregist[bcmiTsnEvtCount];
    int                 max_flows; /* convenient to allocate flow database */
    int                 max_rx_flows; /* use to differentiate the flow dir */
    int                 max_ports; /* convenient to allocate port database */

    tsn_evt_stat_t      evt_stat[bcmTsnEventTypeCount];
    sal_mutex_t         evt_mutex;

    /* Kept the previous state used to enable age-out event poll */
    uint32              pre_enable;
    uint32              pre_interval;
    uint32              pre_rmode;

    /* Poll funtion information */
    VOL sal_thread_t    poll_tid;       /* thread id */
    VOL sal_usecs_t     poll_interval;  /* wakeup interval */
    sal_sem_t           poll_notify;    /* notify signal */
    int                 poll_enable;    /* poll thread is enable or not */
} tsn_event_bk_t;

/*
 * Structure definition for TSN bookkeeping info,
 * including bookkeeping and chip specific info
 */
typedef struct bcmi_esw_tsn_bk_info_s {
    /* bookkeeping structure defined below */
#if defined(BCM_TSN_SR_SUPPORT)
    /* TSN SR MAC Proxy bookkeeping info */
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t         tsn_sr_mac_proxy_bk_info;
    /* TSN SR flow management */
    tsn_sr_flows_bookkeeping_t                  sr_flows;
    /* TSN SR Auto Learn management */
    bcmi_esw_tsn_sr_auto_learn_bk_t             tsn_sr_auto_learn_bk_info;
#endif /* BCM_TSN_SR_SUPPORT */
    /* TSN flow management */
    tsn_flows_bookkeeping_t                     tsn_flows;

    bcmi_esw_tsn_pri_map_bk_info_t              tsn_pri_map_bk_info;

    /* MTU & STU management */
    bcmi_esw_tsn_mtu_bk_info_t                  tsn_mtu_bk_info;
    bcmi_esw_tsn_stu_bk_info_t                  tsn_stu_bk_info;

    /* Event management */
    tsn_event_bk_t                              tsn_events;
    /* device specific info */
    const bcmi_esw_tsn_dev_info_t               *tsn_dev_info;
} bcmi_esw_tsn_bk_info_t;

/* TSN bookkeeping info */
STATIC bcmi_esw_tsn_bk_info_t *tsn_bk_info[BCM_MAX_NUM_UNITS] = { NULL };

#if defined(BCM_TSN_SR_SUPPORT)

/* SW SR Flow ID conversion: flow ID is 1-based (0 is invalid) */
#define SR_FLOW_ID_TX_MASK       0x40000000
#define SR_FLOW_ID_FROM_IDX(_dir, _idx)                                     \
    ((bcm_tsn_sr_flow_t)(                                                   \
        ((_idx) | ((_dir) == SR_FLOW_DIR_TX ? SR_FLOW_ID_TX_MASK : 0)) + 1))
#define SR_FLOW_ID_TO_DIR(_id)                                              \
    ((((int)(_id)) & SR_FLOW_ID_TX_MASK) ? SR_FLOW_DIR_TX : SR_FLOW_DIR_RX)
#define SR_FLOW_ID_TO_IDX(_id)                                              \
    ((((int)(_id)) & (~SR_FLOW_ID_TX_MASK)) - 1)

/* SR Flowset ID conversion: use the same encoding as SW flow ID */
#define SR_FLOWSET_ID_FROM_IDX(_dir, _idx)                                  \
    ((bcm_tsn_sr_flowset_t)SR_FLOW_ID_FROM_IDX((_dir), (_idx)))
#define SR_FLOWSET_ID_TO_DIR(_id) SR_FLOW_ID_TO_DIR((bcm_tsn_sr_flow_t)(_id))
#define SR_FLOWSET_ID_TO_IDX(_id) SR_FLOW_ID_TO_IDX((bcm_tsn_sr_flow_t)(_id))

/* SR flow management bookkeeping info quick access */
#define SR_FLOW_MGMT_BKINFO(_unit_)                                         \
    (&tsn_bk_info[_unit_]->sr_flows)
#define SR_FLOW_MGMT_DEVINFO(_unit_)                                        \
    tsn_bk_info[_unit_]->tsn_dev_info->sr_flows_info

/* Protecting the global SR flow management */
#define SR_FLOW_MGMT_LOCK(_unit_)                                           \
    sal_mutex_take(SR_FLOW_MGMT_BKINFO(_unit_)->fmgmt_mutex, sal_mutex_FOREVER)
#define SR_FLOW_MGMT_UNLOCK(_unit_)                                         \
    sal_mutex_give(SR_FLOW_MGMT_BKINFO(_unit_)->fmgmt_mutex)

/* Protecting an SR flowset slot */
#define SR_FLOWSET_LOCK(_fsp_)                                              \
    sal_mutex_take((_fsp_)->mutex, sal_mutex_FOREVER)
#define SR_FLOWSET_UNLOCK(_fsp_)                                            \
    sal_mutex_give((_fsp_)->mutex)

/* Protecting an SR flow slot */
#define SR_FLOW_RX_LOCK(_unit_, _idx_)                                      \
    sal_mutex_take(SR_FLOW_MGMT_BKINFO(_unit_)->flows_rx_mutex[_idx_],      \
                   sal_mutex_FOREVER)
#define SR_FLOW_RX_UNLOCK(_unit_, _idx_)                                    \
    sal_mutex_give(SR_FLOW_MGMT_BKINFO(_unit_)->flows_rx_mutex[_idx_])
#define SR_FLOW_TX_LOCK(_unit_, _idx_)                                      \
    sal_mutex_take(SR_FLOW_MGMT_BKINFO(_unit_)->flows_tx_mutex[_idx_],      \
                   sal_mutex_FOREVER)
#define SR_FLOW_TX_UNLOCK(_unit_, _idx_)                                    \
    sal_mutex_give(SR_FLOW_MGMT_BKINFO(_unit_)->flows_tx_mutex[_idx_])

/* Valid flags for bcm_tsn_sr_auto_learn_group_config_t */
#define BCMI_TSN_SR_AUTO_LEARN_GROUP_VALID_FLAGS                            \
    (BCM_TSN_SR_AUTO_LEARN_GROUP_PROXY_FILTERING)

/*
 * SR Auto Learn Group id encoding:
 * 1-bit (reserved) + 1-bit'1 (Auto Learn Magic Bit) + 22-bit (reserved)
 * + 8-bit sw_idx
 */
#define BCMI_SR_AUTO_LEARN_SW_IDX_MASK           (0xFF)
#define BCMI_SR_AUTO_LEARN_SHIFT                 (30)
#define BCMI_SR_AUTO_LEARN                       (1)

/*
 * SR Auto Learn Group related define
 */
#define BCMI_SR_AUTO_LEARN_GROUP_ID_INVALID (-1)
#define BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID (-1)

/*
 * Translate between SR Auto Learn Group id and SW_IDX
 * The sw_idx is not the hardware index of register/memory.
 * It just represents the array index of SW bookkeeping group_data[sw_idx]
 */
#define GROUP_ID_TO_SW_IDX(_group_id_) \
    ((_group_id_) & BCMI_SR_AUTO_LEARN_SW_IDX_MASK)

#define SW_IDX_TO_GROUP_ID(_sw_idx_) \
    ((_sw_idx_) | BCMI_SR_AUTO_LEARN << BCMI_SR_AUTO_LEARN_SHIFT)

#define AUTO_LEARN_DEV_INIT(_u_)                                            \
    do {                                                                    \
        if (!soc_feature(unit, soc_feature_tsn_sr) ||                       \
            !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {            \
            LOG_ERROR(BSL_LS_BCM_TSN,                                       \
                      (BSL_META_U(unit,                                     \
                                  "SR Auto Learn Error:"                    \
                                  " feature not enable \n")));              \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
        if (!tsn_bk_info[(_u_)] ||                                          \
            !tsn_bk_info[(_u_)]->tsn_dev_info ||                            \
            !tsn_bk_info[(_u_)]->tsn_dev_info->tsn_sr_auto_learn_info) {    \
            LOG_ERROR(BSL_LS_BCM_TSN,                                       \
                      (BSL_META_U(unit,                                     \
                                  "SR Auto Learn Error:"                    \
                                  " dev not initialized\n")));              \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
    } while (0)

#define AUTO_LEARN_INIT(_u_)                                                \
    do {                                                                    \
        if (!soc_feature(unit, soc_feature_tsn_sr) ||                       \
            !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {            \
            LOG_ERROR(BSL_LS_BCM_TSN,                                       \
                      (BSL_META_U(unit,                                     \
                                  "SR Auto Learn Error:"                    \
                                  " feature not enable \n")));              \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
        if (!tsn_bk_info[(_u_)] ||                                          \
            !tsn_bk_info[(_u_)]->tsn_sr_auto_learn_bk_info.initialized) {   \
            LOG_WARN(BSL_LS_BCM_TSN,                                        \
                      (BSL_META_U(unit,                                     \
                                  "SR Auto Learn Warning:"                  \
                                  " not initialized\n")));                  \
            return BCM_E_INIT;                                              \
        }                                                                   \
        if (!tsn_bk_info[(_u_)] ||                                          \
            !tsn_bk_info[(_u_)]->tsn_dev_info ||                            \
            !tsn_bk_info[(_u_)]->tsn_dev_info->tsn_sr_auto_learn_info ||    \
            !tsn_bk_info[(_u_)]->tsn_sr_auto_learn_bk_info.intf_bitmap ||   \
            !tsn_bk_info[(_u_)]->tsn_sr_auto_learn_bk_info.group_data) {    \
            LOG_ERROR(BSL_LS_BCM_TSN,                                       \
                      (BSL_META_U(unit,                                     \
                                  "SR Auto Learn Error:"                    \
                                  " not allocated\n")));                    \
            return BCM_E_UNAVAIL;                                           \
        }                                                                   \
    } while (0)

/* SR Auto Learn instance fetch/lock/unlock */
#define AUTO_LEARN_BK_INFO(_u_) \
    (&tsn_bk_info[(_u_)]->tsn_sr_auto_learn_bk_info)

/* Protect bcmi_esw_tsn_pri_map_bk_info_t */
#define AUTO_LEARN_BK_LOCK(_u_) \
    (sal_mutex_take(AUTO_LEARN_BK_INFO((_u_))->bk_mutex, sal_mutex_FOREVER))

#define AUTO_LEARN_BK_UNLOCK(_u_) \
    (sal_mutex_give(AUTO_LEARN_BK_INFO((_u_))->bk_mutex))

/*
 * make bcmi_esw_tsn_sr_auto_learn_create/set/get/destroy/traverse
 * to be atomic executed
 */
#define AUTO_LEARN_LOCK(_u_) \
    (sal_mutex_take(AUTO_LEARN_BK_INFO((_u_))->mutex, sal_mutex_FOREVER))

#define AUTO_LEARN_UNLOCK(_u_) \
    (sal_mutex_give(AUTO_LEARN_BK_INFO((_u_))->mutex))

/*
 * SR Auto Learn usage bitmap operations
 */
#define AUTO_LEARN_INTF_USED_GET(_u_, _intf_) \
    ((AUTO_LEARN_BK_INFO(_u_)->intf_bitmap) ? \
        (SHR_BITGET(AUTO_LEARN_BK_INFO(_u_)->intf_bitmap, (_intf_))) : 0)
#define AUTO_LEARN_INTF_USED_SET(_u_, _intf_) \
    (SHR_BITSET(AUTO_LEARN_BK_INFO(_u_)->intf_bitmap, (_intf_)))
#define AUTO_LEARN_INTF_USED_CLR(_u_, _intf_) \
    (SHR_BITCLR(AUTO_LEARN_BK_INFO(_u_)->intf_bitmap, (_intf_)))

#endif /* BCM_TSN_SR_SUPPORT */

/* TSN SW Flow ID conversion: flow ID is 1-based (0 is invalid) */
#define TSN_FLOW_ID_FROM_IDX(_idx)          ((bcm_tsn_flow_t)((_idx) + 1))
#define TSN_FLOW_ID_TO_IDX(_id)             ((int)(_id) - 1)

/* Flowset ID conversion: use the same encoding as SW flow ID */
#define TSN_FLOWSET_ID_FROM_IDX(_idx)       \
        ((bcm_tsn_flowset_t)TSN_FLOW_ID_FROM_IDX(_idx))
#define TSN_FLOWSET_ID_TO_IDX(_id)          \
        TSN_FLOW_ID_TO_IDX((bcm_tsn_flow_t)(_id))

/* TSN flow management bookkeeping info quick access */
#define TSN_FLOW_MGMT_BKINFO(_unit_)                                        \
    (&tsn_bk_info[_unit_]->tsn_flows)
#define TSN_FLOW_MGMT_DEVINFO(_unit_)                                       \
    tsn_bk_info[_unit_]->tsn_dev_info->tsn_flows_info

/* Protecting the global TSN flow management */
#define TSN_FLOW_MGMT_LOCK(_unit_)                                          \
    sal_mutex_take(TSN_FLOW_MGMT_BKINFO(_unit_)->fmgmt_mutex, sal_mutex_FOREVER)
#define TSN_FLOW_MGMT_UNLOCK(_unit_)                                        \
    sal_mutex_give(TSN_FLOW_MGMT_BKINFO(_unit_)->fmgmt_mutex)

/* Protecting a TSN flowset slot */
#define TSN_FLOWSET_LOCK(_fsp_)                                             \
    sal_mutex_take((_fsp_)->mutex, sal_mutex_FOREVER)
#define TSN_FLOWSET_UNLOCK(_fsp_)                                           \
    sal_mutex_give((_fsp_)->mutex)

/* Protecting a TSN flow slot */
#define TSN_FLOW_LOCK(_unit_, _idx_)                                        \
    sal_mutex_take(TSN_FLOW_MGMT_BKINFO(_unit_)->flows_mutex[_idx_],        \
                   sal_mutex_FOREVER)
#define TSN_FLOW_UNLOCK(_unit_, _idx_)                                      \
    sal_mutex_give(TSN_FLOW_MGMT_BKINFO(_unit_)->flows_mutex[_idx_])

/*
 * Priority Map id encoding:
 * 1bit (valid) + 3-bit bcm_tsn_pri_map_type + 12-bit (reserved)
 * + 16-bit sw_idx
 */
#define BCMI_TSN_PRI_MAP_SW_IDX_MASK          (0xFFFF)
#define BCMI_TSN_PRI_MAP_TYPE_SHIFT           (28)
#define BCMI_TSN_PRI_MAP_TYPE_MASK            (0x7)
#define BCMI_TSN_PRI_MAP_VALID_SHIFT          (31)

/*
 * Translate between Priority Map ID and SW_IDX & Priority Map Type
 * The sw_idx is not the hardware index of register/memory.
 * It just represents the array index of SW bookkeeping data[map_type][sw_idx]
 */
#define MAP_ID_TO_SW_IDX(_id_) \
        ((_id_) & BCMI_TSN_PRI_MAP_SW_IDX_MASK)

#define SW_IDX_TO_MAP_ID(_id_, _map_type_) \
        ((_id_) | \
         ((_map_type_) << BCMI_TSN_PRI_MAP_TYPE_SHIFT) | \
         (0x1 << BCMI_TSN_PRI_MAP_VALID_SHIFT))

#define PRI_MAP_TYPE_GET(_id_) \
        (((_id_) >> BCMI_TSN_PRI_MAP_TYPE_SHIFT) & BCMI_TSN_PRI_MAP_TYPE_MASK)

#define PRI_MAP_ID_IS_TYPE(_id_, _map_type_) \
        (PRI_MAP_TYPE_GET((_id_)) == (_map_type_))

#define PRI_MAP_INIT(_u_, _map_type_)                                         \
    do {                                                                      \
        if (!soc_feature((_u_), soc_feature_tsn_sr) &&                        \
            (_map_type_) == bcmTsnPriMapTypeSrFlow) {                         \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "TSN Priority Map Error:"                   \
                                  " feature not enable \n")));                \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
        if (!soc_feature((_u_), soc_feature_tsn_mtu_stu) &&                   \
            ((_map_type_) == bcmTsnPriMapTypeEgressMtuProfile ||              \
             (_map_type_) == bcmTsnPriMapTypeEgressStuProfile)) {             \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "TSN Priority Map Error:"                   \
                                  " feature not enable \n")));                \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
        if (!soc_feature((_u_), soc_feature_tsn_taf) &&                       \
            (_map_type_) == bcmTsnPriMapTypeTafGate) {                        \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "TSN Priority Map Error:"                   \
                                  " feature not enable \n")));                \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
        if (!tsn_bk_info[_u_]->tsn_dev_info ||                                \
            !tsn_bk_info[_u_]->tsn_dev_info->tsn_pri_map_info[_map_type_] ||  \
            !tsn_bk_info[_u_]->tsn_pri_map_bk_info.data ||                    \
            !tsn_bk_info[_u_]->tsn_pri_map_bk_info.intf_bitmap) {             \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "TSN Priority Map Error:"                   \
                                  " not allocated\n")));                      \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

/* Priority Map instance fetch/lock/unlock */
#define PRI_MAP_BK_INFO(_u_) \
        (&tsn_bk_info[(_u_)]->tsn_pri_map_bk_info)

/* Protect bcmi_esw_tsn_pri_map_bk_info_t */
#define PRI_MAP_BK_LOCK(_u_) \
        sal_mutex_take(PRI_MAP_BK_INFO((_u_))->bk_mutex, sal_mutex_FOREVER)

#define PRI_MAP_BK_UNLOCK(_u_) \
        sal_mutex_give(PRI_MAP_BK_INFO((_u_))->bk_mutex)

/*
 * make bcmi_esw_tsn_pri_map_create/set/get/destroy/traverse
 * to be atomic executed
 */
#define PRI_MAP_LOCK(_u_) \
        sal_mutex_take(PRI_MAP_BK_INFO((_u_))->mutex, sal_mutex_FOREVER)

#define PRI_MAP_UNLOCK(_u_) \
        sal_mutex_give(PRI_MAP_BK_INFO((_u_))->mutex)

/* Priority Map usage bitmap operations */
#define PRI_MAP_INTF_GET(_u_, _map_type_, _intf_) \
    ((PRI_MAP_BK_INFO(_u_)->intf_bitmap[(_map_type_)]) ? \
      (SHR_BITGET(PRI_MAP_BK_INFO(_u_)->intf_bitmap[_map_type_], (_intf_))) : 0)
#define PRI_MAP_INTF_SET(_u_, _map_type_, _intf_) \
        SHR_BITSET(PRI_MAP_BK_INFO(_u_)->intf_bitmap[(_map_type_)], (_intf_))
#define PRI_MAP_INTF_CLR(_u_, _map_type_, _intf_) \
        SHR_BITCLR(PRI_MAP_BK_INFO(_u_)->intf_bitmap[(_map_type_)], (_intf_))

/* MTU Operation Check */
#define MTU_BK_INIT(_u_, _t_)                                                 \
    do {                                                                      \
        if ((_t_) != bcmTsnMtuProfileTypeIngress &&                           \
            (_t_) != bcmTsnMtuProfileTypeEgress) {                            \
            return BCM_E_PARAM;                                               \
        }                                                                     \
        if ((NULL == tsn_bk_info[(_u_)]->tsn_mtu_bk_info.mtu_refcount[_t_])) {\
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "MTU Error:"                                \
                                  " bookeeping not allocated\n")));           \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

#define MTU_DEV_INIT(_u_)                                                \
    do {                                                                 \
        const tsn_mtu_info_t *mtu_info;                                  \
        if (tsn_bk_info[(_u_)]->tsn_dev_info == NULL) {                  \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "MTU Error:"                           \
                                  " tsn_dev_info not allocated\n")));    \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
        mtu_info = tsn_bk_info[(_u_)]->tsn_dev_info->tsn_mtu_info;       \
        if (mtu_info == NULL) {                                          \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "MTU Error:"                           \
                                  " tsn_mtu_info not allocated\n")));    \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
        if ((NULL == mtu_info->mtu_profile_mem_idx_get) ||               \
            (NULL == mtu_info->mtu_profile_create) ||                    \
            (NULL == mtu_info->mtu_profile_set) ||                       \
            (NULL == mtu_info->mtu_profile_get) ||                       \
            (NULL == mtu_info->mtu_profile_destroy) ||                   \
            (NULL == mtu_info->ingress_mtu_config_set) ||                \
            (NULL == mtu_info->ingress_mtu_config_get) ||                \
            (NULL == mtu_info->egress_mtu_port_control_set) ||           \
            (NULL == mtu_info->mtu_profile_decode) ||                    \
            (NULL == mtu_info->mtu_profile_encode) ||                    \
            (NULL == mtu_info->mtu_profile_traverse) ||                  \
            (NULL == mtu_info->mtu_profile_mem_refcnt_get)) {            \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "MTU Error:"                           \
                                  " dev info not allocated\n")));        \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
    } while (0)

/* MTU instance fetch/lock/unlock */
#define MTU_BK_INFO(_u_) \
        (&tsn_bk_info[(_u_)]->tsn_mtu_bk_info)

#define MTU_BK_LOCK(_u_) \
        sal_mutex_take(MTU_BK_INFO((_u_))->mutex, sal_mutex_FOREVER)

#define MTU_BK_UNLOCK(_u_) \
        sal_mutex_give(MTU_BK_INFO((_u_))->mutex)

/* STU Operation Check */
#define STU_BK_INIT(_u_, _t_)                                                 \
    do {                                                                      \
        if ((_t_) != bcmTsnStuProfileTypeIngress &&                           \
            (_t_) != bcmTsnStuProfileTypeEgress) {                            \
            return BCM_E_PARAM;                                               \
        }                                                                     \
        if ((NULL == tsn_bk_info[(_u_)]->tsn_stu_bk_info.stu_refcount[_t_])) {\
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_u_),                                      \
                                  "STU Error:"                                \
                                  " bookeeping not allocated\n")));           \
            return BCM_E_UNAVAIL;                                             \
        }                                                                     \
    } while (0)

#define STU_DEV_INIT(_u_)                                                \
    do {                                                                 \
        const tsn_stu_info_t *stu_info;                                  \
        if (tsn_bk_info[(_u_)]->tsn_dev_info == NULL) {                  \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "STU Error:"                           \
                                  " tsn_dev_info not allocated\n")));    \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
        stu_info = tsn_bk_info[(_u_)]->tsn_dev_info->tsn_stu_info;       \
        if (stu_info == NULL) {                                          \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "STU Error:"                           \
                                  " tsn_stu_info not allocated\n")));    \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
        if ((NULL == stu_info->stu_profile_mem_idx_get) ||               \
            (NULL == stu_info->stu_profile_create) ||                    \
            (NULL == stu_info->stu_profile_set) ||                       \
            (NULL == stu_info->stu_profile_get) ||                       \
            (NULL == stu_info->stu_profile_destroy) ||                   \
            (NULL == stu_info->ingress_stu_config_set) ||                \
            (NULL == stu_info->ingress_stu_config_get) ||                \
            (NULL == stu_info->egress_stu_port_control_set) ||           \
            (NULL == stu_info->stu_profile_decode) ||                    \
            (NULL == stu_info->stu_profile_encode) ||                    \
            (NULL == stu_info->stu_profile_traverse) ||                  \
            (NULL == stu_info->stu_profile_mem_refcnt_get)) {            \
            LOG_ERROR(BSL_LS_BCM_TSN,                                    \
                      (BSL_META_U((_u_),                                 \
                                  "STU Error:"                           \
                                  " dev info not allocated\n")));        \
            return BCM_E_UNAVAIL;                                        \
        }                                                                \
    } while (0)

/* STU instance fetch/lock/unlock */
#define STU_BK_INFO(_u_) \
        (&tsn_bk_info[(_u_)]->tsn_stu_bk_info)

#define STU_BK_LOCK(_u_) \
        sal_mutex_take(STU_BK_INFO((_u_))->mutex, sal_mutex_FOREVER)
#define STU_BK_UNLOCK(_u_) \
        sal_mutex_give(STU_BK_INFO((_u_))->mutex)

/* TSN event management bookkeeping info quick access */
#define TSN_EVENT_BKINFO(_unit_)                                         \
    (&tsn_bk_info[_unit_]->tsn_events)
#define TSN_EVENT_DEVINFO(_unit_)                                        \
    tsn_bk_info[_unit_]->tsn_dev_info->tsn_event_info

#define TSN_EVENT_FLOW_CB_MAX           (1000)
#define TSN_EVENT_PORT_CB_MAX           (1000)
#define TSN_EVENT_SYSTEM_CB_MAX         (100)

#define TSN_EVENT_POLL_INTERVAL_DEFAULT     (SECOND_USEC)

/* Protecting the global TSN event management */
#define TSN_EVENT_LOCK(_unit_)                                           \
    sal_mutex_take(TSN_EVENT_BKINFO(_unit_)->evt_mutex, sal_mutex_FOREVER)
#define TSN_EVENT_UNLOCK(_unit_)                                         \
    sal_mutex_give(TSN_EVENT_BKINFO(_unit_)->evt_mutex)

#if defined(BCM_TSN_SR_SUPPORT)
/* Internal functions to update the active rx flow idx */
STATIC int bcmi_esw_tsn_event_rx_flow_delete(int unit, int flow_idx);
STATIC int bcmi_esw_tsn_event_rx_flow_add(int unit, int flow_idx);
#endif /* BCM_TSN_SR_SUPPORT */

/* Protecting registered event instance */
#define TSN_REVT_LOCK(_erp_)                                    \
    sal_mutex_take((_erp_)->mutex, sal_mutex_FOREVER)
#define TSN_REVT_UNLOCK(_erp_)                                         \
    sal_mutex_give((_erp_)->mutex)

/* Get the tsn_bk_info[unit] pointer */
STATIC int
tsn_bk_info_instance_get(int unit, bcmi_esw_tsn_bk_info_t **bk_info)
{
    /* Input parameters check. */
    if (NULL == bk_info) {
        return (BCM_E_PARAM);
    }
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Check if TSN feature supported and initialized */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return (BCM_E_UNAVAIL);
    }
    if (tsn_bk_info[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Error: bookkeeping info not "
                              "initialized\n")));
        return (BCM_E_INIT);
    }

    /* Fill info structure. */
    *bk_info = tsn_bk_info[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_esw_tsn_noninit_dev_info_get
 * Purpose:
 *     Internal function to get device sepcific info without tsn_bk initialized.
 * Parameters:
 *     unit - (IN) unit number
 *     devinfo-(OUT) device info
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_noninit_dev_info_get(
    int unit,
    const bcmi_esw_tsn_dev_info_t **devinfo)
{
    int rv = BCM_E_NONE;

    /* Input parameters check. */
    if (NULL == devinfo) {
        return (BCM_E_PARAM);
    }
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Check if TSN feature supported and initialized */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return (BCM_E_UNAVAIL);
    }
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        /* Fill info structure. */
        rv = bcmi_gh2_tsn_noninit_dev_info_get(unit, devinfo);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return rv;
}

#if defined(BCM_TSN_SR_SUPPORT)

/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_cleanup
 * Description:
 *    Clean and free all allocated resources for
 *    TSN SR Mac Proxy profile.
 * Parameters:
 *    unit - (IN) Unit number
 * Return Value:
 *    None
 */
STATIC int
bcmi_esw_tsn_sr_mac_proxy_cleanup(int unit)
{
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_sr_mac_proxy_cleanup\n")));

    /* Free and clear the TSN SR MAC Proxy bookkeeping info structure */
    if (tsn_bk_info[unit] != NULL) {
        sr_mp_bk_info = &(tsn_bk_info[unit]->tsn_sr_mac_proxy_bk_info);

        if (sr_mp_bk_info->sr_mp_entry != NULL) {
            TSN_FREE(unit, sr_mp_bk_info->sr_mp_entry, 0);
            sr_mp_bk_info->sr_mp_entry = NULL;
        }

        if (sr_mp_bk_info->sr_mp_mutex != NULL) {
            sal_mutex_destroy(sr_mp_bk_info->sr_mp_mutex);
            sr_mp_bk_info->sr_mp_mutex = NULL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_init
 * Description:
 *    Initialize and allocate all required resources for
 *    TSN SR Mac Proxy profile.
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_sr_mac_proxy_init(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_esw_tsn_bk_info_t *tsn_bk_info;
    const bcmi_esw_tsn_dev_info_t *tsn_dev_info;
    const tsn_sr_mac_proxy_info_t *sr_mp_dev_info;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_sr_mac_proxy_init\n")));

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &tsn_bk_info));

    /* Set up the unit TSN SR Mac Proxy bookkeeping info structure. */
    sr_mp_bk_info = &(tsn_bk_info->tsn_sr_mac_proxy_bk_info);

    /* Create protection mutex. */
    sr_mp_bk_info->sr_mp_mutex = sal_mutex_create("sr_mp_bk_info.lock");
    if (NULL == sr_mp_bk_info->sr_mp_mutex) {
        return BCM_E_RESOURCE;
    }

    /* Get chip specific assignment */
    tsn_dev_info = tsn_bk_info->tsn_dev_info;
    sr_mp_dev_info = tsn_dev_info->tsn_sr_mac_proxy_info;

    if (sr_mp_dev_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Error: SR MAC Proxy device info not "
                              "initialized\n")));
        return BCM_E_INIT;
    }

    if (!soc_mem_field_valid(unit, sr_mp_dev_info->mem,
                             sr_mp_dev_info->field)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Memory %s or field is Not valid\n"),
                  SOC_MEM_UFNAME(unit, sr_mp_dev_info->mem)));
        return BCM_E_PARAM;
    }

    /* Allocate TSN SR Mac Proxy entry memory by sr_mp_num */
    sr_mp_bk_info->sr_mp_num = soc_mem_index_count(unit, sr_mp_dev_info->mem);
    TSN_ALLOC(unit, sr_mp_bk_info->sr_mp_entry,
              bcmi_esw_tsn_sr_mac_proxy_entry_t,
              (sr_mp_bk_info->sr_mp_num *
              sizeof(bcmi_esw_tsn_sr_mac_proxy_entry_t)),
              "TSN SR Mac Proxy Entry", 0, rv);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "bcmi_esw_tsn_sr_mac_proxy_init: "
                                "failed to allocate memory\n")));
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_reload
 * Description:
 *    Load TSN SR MAC Proxy info
 *    from hardware into software data structures.
 * Parameters:
 *    unit - (IN) Unit number
 */
STATIC int
bcmi_esw_tsn_sr_mac_proxy_reload(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_esw_tsn_bk_info_t *tsn_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;
    const bcmi_esw_tsn_dev_info_t *tsn_dev_info;
    const tsn_sr_mac_proxy_info_t *sr_mp_dev_info;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *mp_entries;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int cur_index;
    bcm_pbmp_t mp_pbmp;
    int index, l2x_size;
    l2x_entry_t *l2x_entry, *l2x_table = NULL;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "bcmi_esw_tsn_sr_mac_proxy_reload\n")));

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &tsn_bk_info));

    /* Get chip specific assignment */
    tsn_dev_info = tsn_bk_info->tsn_dev_info;
    sr_mp_dev_info = tsn_dev_info->tsn_sr_mac_proxy_info;
    if (sr_mp_dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }

    /* Set up the unit TSN SR Mac Proxy bookkeeping info structure. */
    sr_mp_bk_info = &(tsn_bk_info->tsn_sr_mac_proxy_bk_info);
    mp_entries = sr_mp_bk_info->sr_mp_entry;
    if (mp_entries == NULL) {
        return BCM_E_UNAVAIL;
    }

    SR_MAC_PROXY_LOCK(sr_mp_bk_info);
    /*
     * Refresh TSN SR MAC Proxy information from the hardware tables.
     */
    for (cur_index = 0; cur_index < sr_mp_bk_info->sr_mp_num; cur_index++) {
        sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        rv = soc_mem_read(unit, sr_mp_dev_info->mem, MEM_BLOCK_ANY,
                          cur_index, entry);
        if (BCM_FAILURE(rv)) {
            SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
            return rv;
        }

        BCM_PBMP_CLEAR(mp_pbmp);
        soc_mem_pbmp_field_get(unit, sr_mp_dev_info->mem, entry,
                               sr_mp_dev_info->field, &mp_pbmp);

        BCM_PBMP_ASSIGN(mp_entries[cur_index].mp_pbmp, mp_pbmp);
    }
    SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);

    /* Refresh ref_count for L2 usage */
    if (soc_mem_field_valid(unit, L2Xm, SR_MAC_PROXY_PROFILE_PTRf)) {
        l2x_size = sizeof(l2x_entry_t) * soc_mem_index_count(unit, L2Xm);
        TSN_ALLOC(unit, l2x_table, l2x_entry_t, l2x_size,
                  "tsn_sr_mac_proxy l2 reload", 1, rv);
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "bcmi_esw_tsn_sr_mac_proxy_reload: "
                                    "failed to allocate memory\n")));
            return rv;
        }
        if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, L2Xm),
                               soc_mem_index_max(unit, L2Xm),
                               l2x_table) < 0) {
            soc_cm_sfree(unit, l2x_table);
            return SOC_E_INTERNAL;
        }

        SR_MAC_PROXY_LOCK(sr_mp_bk_info);
        for (index = soc_mem_index_min(unit, L2Xm);
            index <= soc_mem_index_max(unit, L2Xm); index++) {
            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                     l2x_entry_t *,
                                                     l2x_table, index);
             if (!soc_L2Xm_field32_get(unit, l2x_entry, VALIDf)) {
                 continue;
             }

             cur_index = soc_L2Xm_field32_get(unit, l2x_entry,
                                              SR_MAC_PROXY_PROFILE_PTRf);
             if ((cur_index > 0) && (cur_index < sr_mp_bk_info->sr_mp_num)) {
                 mp_entries[cur_index].ref_count++;
             }
        }
        SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);

        soc_cm_sfree(unit, l2x_table);
    } else {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_esw_tsn_pri_map_existed_check(
    int unit,
    bcm_tsn_pri_map_config_t *config,
    bcm_tsn_pri_map_t *map_id);

/*
 * Function:
 *      bcmi_esw_tsn_egress_mtu_sync
 * Purpose:
 *      Synchronize the egr mtu sw data into scache
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_egress_mtu_sync(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    soc_scache_handle_t scache_handle;
    uint8 *map_id_scache;
    int idx;
    uint32 alloc;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_EGR_MTU);
    /* allocate the scache pointer size */
    alloc = sizeof(bcm_tsn_pri_map_t) * ctrl_info->logical_port_num;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                alloc, &map_id_scache,
                                BCM_TSN_WB_DEFAULT_VERSION, NULL));
    if (map_id_scache == NULL ||
        bk_info->tsn_mtu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }

    MTU_BK_LOCK(unit);
    for (idx = 0; idx < ctrl_info->logical_port_num; idx++) {
        sal_memcpy(map_id_scache,
                   &(bk_info->tsn_mtu_bk_info.pri_map_port[idx]),
                   sizeof(bcm_tsn_pri_map_t));
        map_id_scache += sizeof(bcm_tsn_pri_map_t);
    }
    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_egress_mtu_reload
 * Purpose:
 *      Reload the scache data into egr mtu sw data structure
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_egress_mtu_reload(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    bcm_tsn_pri_map_config_t config;
    soc_scache_handle_t scache_handle;
    uint8 *map_id_scache;
    int idx, rv, rv1, i, prof_idx, prof_id;
    bcm_tsn_pri_map_t map_id;
    bcm_tsn_mtu_profile_type_t type;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                         "bcmi_esw_tsn_egress_mtu_reload\n")));

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_EGR_MTU);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &map_id_scache, BCM_TSN_WB_DEFAULT_VERSION,
                                 NULL);
    if (bk_info->tsn_mtu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }

    if (rv == BCM_E_NOT_FOUND) {
        rv1 = _bcm_esw_scache_ptr_get(
                    unit, scache_handle, TRUE, sizeof(bcm_tsn_pri_map_t),
                    &map_id_scache, BCM_TSN_WB_DEFAULT_VERSION, NULL);
        if (BCM_E_NOT_FOUND == rv1) {
            /* Proceed with Level 1 Warm Boot */
            rv1 = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(rv1);
        return BCM_E_NONE;
    } else if (rv == BCM_E_NONE) {
        if (map_id_scache == NULL) {
            return BCM_E_INTERNAL;
        }
        MTU_BK_LOCK(unit);
        for (idx = 0; idx < ctrl_info->logical_port_num; idx++) {
            sal_memcpy(&(bk_info->tsn_mtu_bk_info.pri_map_port[idx]),
                       map_id_scache,
                       sizeof(bcm_tsn_pri_map_t));
            map_id_scache += sizeof(bcm_tsn_pri_map_t);
            rv1 = ctrl_info->egress_mtu_wb_reload(unit, idx, &config);
            if (BCM_FAILURE(rv1)) {
                MTU_BK_UNLOCK(unit);
                return rv1;
            }
            /* Check whether the config has been added before */
            rv1 = bcmi_esw_tsn_pri_map_existed_check(unit, &config, &map_id);
            if (rv1 == BCM_E_NOT_FOUND) {
                /* MTU profile reference count update */
                for (i = 0; i < config.num_map_entries; i++) {
                    prof_id = config.map_entries[i].profile_id;
                    ctrl_info->mtu_profile_decode(
                        unit, prof_id, &type, &prof_idx);
                    /* MTU profile entry == 0 is invalid */
                    if (prof_idx != 0) {
                        bcmi_esw_tsn_mtu_profile_ref_inc(unit, prof_id);
                    }
                }
                /* Provide priority map structure the related mtu wb information */
                bcmi_esw_tsn_pri_map_mtu_stu_wb_set(
                    unit, bk_info->tsn_mtu_bk_info.pri_map_port[idx], &config);
            }

        }

        MTU_BK_UNLOCK(unit);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_egress_stu_sync
 * Purpose:
 *      Synchronize the egr stu sw data into scache
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_egress_stu_sync(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    soc_scache_handle_t scache_handle;
    uint8 *map_id_scache;
    int idx;
    uint32 alloc;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_EGR_STU);
    /* allocate the scache pointer size */
    alloc = sizeof(bcm_tsn_pri_map_t) * ctrl_info->logical_port_num;
    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                alloc, &map_id_scache,
                                BCM_TSN_WB_DEFAULT_VERSION, NULL));
    if (map_id_scache == NULL ||
        bk_info->tsn_stu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }

    STU_BK_LOCK(unit);
    for (idx = 0; idx < ctrl_info->logical_port_num; idx++) {
        sal_memcpy(map_id_scache,
                   &(bk_info->tsn_stu_bk_info.pri_map_port[idx]),
                   sizeof(bcm_tsn_pri_map_t));
        map_id_scache += sizeof(bcm_tsn_pri_map_t);
    }
    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_egress_stu_reload
 * Purpose:
 *      Reload the scache data into egr stu sw data structure
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_egress_stu_reload(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    bcm_tsn_pri_map_config_t config;
    soc_scache_handle_t scache_handle;
    uint8 *map_id_scache;
    int idx, rv, rv1, i, prof_idx, prof_id;
    bcm_tsn_pri_map_t map_id;
    bcm_tsn_stu_profile_type_t type;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                         "bcmi_esw_tsn_egress_stu_reload\n")));

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_EGR_STU);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &map_id_scache, BCM_TSN_WB_DEFAULT_VERSION,
                                 NULL);
    if (bk_info->tsn_stu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }
    if (rv == BCM_E_NOT_FOUND) {
        rv1 = _bcm_esw_scache_ptr_get(
                    unit, scache_handle, TRUE, sizeof(bcm_tsn_pri_map_t),
                    &map_id_scache, BCM_TSN_WB_DEFAULT_VERSION, NULL);
        if (BCM_E_NOT_FOUND == rv1) {
            /* Proceed with Level 1 Warm Boot */
            rv1 = BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(rv1);
        return BCM_E_NONE;
    } else if (rv == BCM_E_NONE) {
        if (map_id_scache == NULL) {
            return BCM_E_INTERNAL;
        }
        STU_BK_LOCK(unit);
        for (idx = 0; idx < ctrl_info->logical_port_num; idx++) {
            sal_memcpy(&(bk_info->tsn_stu_bk_info.pri_map_port[idx]),
                       map_id_scache,
                       sizeof(bcm_tsn_pri_map_t));
            map_id_scache += sizeof(bcm_tsn_pri_map_t);
            rv1 = ctrl_info->egress_stu_wb_reload(unit, idx, &config);
            if (BCM_FAILURE(rv1)) {
                STU_BK_UNLOCK(unit);
                return rv1;
            }
            /* Check whether the config has been added before */
            rv1 = bcmi_esw_tsn_pri_map_existed_check(unit, &config, &map_id);
            if (rv1 == BCM_E_NOT_FOUND) {
                /* STU profile reference count update */
                for (i = 0; i < config.num_map_entries; i++) {
                    prof_id = config.map_entries[i].profile_id;
                    ctrl_info->stu_profile_decode(
                        unit, prof_id, &type, &prof_idx);
                    /* STU profile entry == 0 is invalid */
                    if (prof_idx != 0) {
                        bcmi_esw_tsn_stu_profile_ref_inc(unit, prof_id);
                    }
                }
                /* Provide priority map structure the related stu wb information */
                bcmi_esw_tsn_pri_map_mtu_stu_wb_set(
                    unit, bk_info->tsn_stu_bk_info.pri_map_port[idx], &config);
            }
        }

        STU_BK_UNLOCK(unit);
    }
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_port_prp_forwarding_sync
 * Purpose:
 *      Synchronize the prp-forwarding sw data into scache
 * Parameters:
 *      unit - (IN) Unit number

 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_port_prp_forwarding_sync(
    int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_port_dev_info_t *sr_port_config = NULL;
    soc_scache_handle_t scache_handle;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    if (dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }
    sr_port_config = dev_info->sr_port_info;
    if ((sr_port_config == NULL) ||
        (sr_port_config->sr_port_prp_forwarding_wb_sync == NULL)) {
        return BCM_E_UNAVAIL;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_PRP_FORWARDING);

    return sr_port_config->sr_port_prp_forwarding_wb_sync(unit, scache_handle);
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_port_prp_forwarding_reload
 * Purpose:
 *      Reload the scache data into prp-forwarding sw data
 * Parameters:
 *      unit - (IN) Unit number

 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_port_prp_forwarding_reload(
    int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_port_dev_info_t *sr_port_config = NULL;
    soc_scache_handle_t scache_handle;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    if (dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }
    sr_port_config = dev_info->sr_port_info;
    if ((sr_port_config == NULL) ||
        (sr_port_config->sr_port_prp_forwarding_wb_reload == NULL)) {
        return BCM_E_UNAVAIL;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_PRP_FORWARDING);

    return
        sr_port_config->sr_port_prp_forwarding_wb_reload(unit, scache_handle);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_insert
 * Purpose:
 *    Find or create a TSN SR MAC Proxy table entry
 *    matching the given bitmap.
 * Parameters:
 *    unit - (IN) Unit number
 *    mp_pbmp  - (IN) Port bitmap for SR source MAC Proxy
 *    mp_index - (OUT) Index of TSN SR MAC Proxy table with bitmap
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_RESOURCE - No more TSN SR MAC Proxy entries available.
 *    BCM_E_PARAM - Illegal parameter.
 */
int
bcmi_esw_tsn_sr_mac_proxy_insert(
    int unit,
    bcm_pbmp_t mp_pbmp,
    int *mp_index)
{
    int rv = BCM_E_NONE;
    bcmi_esw_tsn_bk_info_t *tsn_bk_info;
    const bcmi_esw_tsn_dev_info_t *tsn_dev_info;
    const tsn_sr_mac_proxy_info_t *sr_mp_dev_info;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *mp_entries;
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_pbmp_t temp_pbmp;
    int cur_index = 0;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_sr_mac_proxy_insert\n")));

    /* Check if TSN SR feature supported */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    /* Check for input parameter */
    if (NULL == mp_index) {
        return BCM_E_PARAM;
    }

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mp_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mp_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &tsn_bk_info));

    /* Get chip specific assignment */
    tsn_dev_info = tsn_bk_info->tsn_dev_info;
    sr_mp_dev_info = tsn_dev_info->tsn_sr_mac_proxy_info;
    if (sr_mp_dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }

    /* Set up the unit TSN SR Mac Proxy bookkeeping info structure. */
    sr_mp_bk_info = &(tsn_bk_info->tsn_sr_mac_proxy_bk_info);
    mp_entries = sr_mp_bk_info->sr_mp_entry;
    if (mp_entries == NULL) {
        return BCM_E_UNAVAIL;
    }

    SR_MAC_PROXY_LOCK(sr_mp_bk_info);

    /* Compare the bitmap with existing entries */
    for (cur_index = 0; cur_index < sr_mp_bk_info->sr_mp_num; cur_index++) {
        if (mp_entries[cur_index].ref_count != 0) {
            if (BCM_PBMP_EQ(mp_entries[cur_index].mp_pbmp, mp_pbmp)) {
                mp_entries[cur_index].ref_count++;
                *mp_index = cur_index;
                SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
                return BCM_E_NONE;
            }
        }
    }

    /*
     * Not in table already, see if any space free
     * (index 0 is reserved by default)
     */
    for (cur_index = 1; cur_index < sr_mp_bk_info->sr_mp_num; cur_index++) {
        if (mp_entries[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            rv = soc_mem_read(unit, sr_mp_dev_info->mem, MEM_BLOCK_ANY,
                              cur_index, entry);
            if (BCM_FAILURE(rv)) {
                SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
                return rv;
            }

            soc_mem_pbmp_field_set(unit, sr_mp_dev_info->mem, entry,
                                   sr_mp_dev_info->field, &mp_pbmp);

            rv = soc_mem_write(unit, sr_mp_dev_info->mem, MEM_BLOCK_ALL,
                               cur_index, entry);
            if (BCM_FAILURE(rv)) {
                SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
                return rv;
            }

            mp_entries[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mp_entries[cur_index].mp_pbmp, mp_pbmp);
            *mp_index = cur_index;

            SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_delete
 * Purpose:
 *    Remove reference to TSN SR MAC Proxy table entry
 *    matching the given bitmap.
 * Parameters:
 *    unit - (IN) Unit number
 *    mp_index - (IN) Index of TSN SR MAC Proxy table with bitmap
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_esw_tsn_sr_mac_proxy_delete(int unit, int mp_index)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *mp_entries;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_sr_mac_proxy_delete\n")));

    /* Check if TSN SR feature supported */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &tsn_bk_info));

    /* Set up the unit TSN SR Mac Proxy bookkeeping info structure. */
    sr_mp_bk_info = &(tsn_bk_info->tsn_sr_mac_proxy_bk_info);
    mp_entries = sr_mp_bk_info->sr_mp_entry;

    /* Check for reasonable entry index (0 is reserved by default) */
    if (mp_index <= 0 || mp_index >= sr_mp_bk_info->sr_mp_num) {
        return BCM_E_PARAM;
    }

    SR_MAC_PROXY_LOCK(sr_mp_bk_info);
    if (mp_entries[mp_index].ref_count > 0) {
        mp_entries[mp_index].ref_count--;
    } else {
        /* Check if the ref_count is zero: return BCM_E_NOT_FOUND */
        SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
        return BCM_E_NOT_FOUND;
    }

    /* Clear software port bitmap if the ref_count is decreased to zero */
    if (mp_entries[mp_index].ref_count == 0) {
        BCM_PBMP_CLEAR(mp_entries[mp_index].mp_pbmp);
    }

    SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_sr_mac_proxy_get
 * Purpose:
 *    Get Port bitmap from software SR MAC Proxy table
 *    with given index of TSN SR MAC Proxy table.
 * Parameters:
 *    unit - (IN) Unit number
 *    mp_index - (IN) Index of TSN SR MAC Proxy table with bitmap
 *    mp_pbmp  - (OUT) Port bitmap for SR source MAC Proxy
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_esw_tsn_sr_mac_proxy_get(int unit, int mp_index, bcm_pbmp_t *mp_pbmp)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *sr_mp_bk_info;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *mp_entries;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_sr_mac_proxy_get\n")));

    /* Check if TSN SR feature supported */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &tsn_bk_info));

    /* Set up the unit TSN SR Mac Proxy bookkeeping info structure. */
    sr_mp_bk_info = &(tsn_bk_info->tsn_sr_mac_proxy_bk_info);
    mp_entries = sr_mp_bk_info->sr_mp_entry;

    /* Check for reasonable entry index (0 is reserved by default) */
    if (mp_index <= 0 || mp_index >= sr_mp_bk_info->sr_mp_num) {
        return BCM_E_PARAM;
    }

    SR_MAC_PROXY_LOCK(sr_mp_bk_info);
    BCM_PBMP_ASSIGN(*mp_pbmp, mp_entries[mp_index].mp_pbmp);
    SR_MAC_PROXY_UNLOCK(sr_mp_bk_info);

    return BCM_E_NONE;
}

/* SR flow: free a chain of one or more continuous flow indexes */
STATIC int
bcmi_esw_tsn_sr_flow_idx_free(int unit, int dir, tsn_sr_flowidx_entry_t *entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowidx_entry_t *fid, *prev, *last;
    int eidx;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (dir < 0 || dir >= SR_FLOW_DIR_COUNT || entry == NULL) {
        return BCM_E_PARAM;
    }

    /* Protect access to the flow indexes */
    SR_FLOW_MGMT_LOCK(unit);

    /* Traverse the list and insert the chain into the list */
    bkinfo = &tsn_bk->sr_flows;
    eidx = entry->flow_idx;
    prev = NULL;
    for (fid = bkinfo->flowids_avail[dir]; fid != NULL; fid = fid->next) {

        /* If the index is smaller than current, insert before current */
        if (eidx < fid->flow_idx) {
            /* Find the last item in the supplied chain */
            for (last = entry; last->next != NULL; last = last->next);
            /* Connect remaining items to the chain */
            last->next = fid;
            /* Insert the chain to the list */
            if (NULL == prev) {
                bkinfo->flowids_avail[dir] = entry;
            } else {
                prev->next = entry;
            }
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_NONE;
        }

        /* all indexes should be different */
        assert(eidx != fid->flow_idx);

        /* record the previous item */
        prev = fid;
    }

    /* The index is larger than all items in the list, append it at the end */
    if (NULL == prev) {
        bkinfo->flowids_avail[dir] = entry;
    } else {
        prev->next = entry;
    }
    SR_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

/* SR flow: allocate one or more continuous flow idx(s) */
STATIC int
bcmi_esw_tsn_sr_flow_idx_allocate(
    int unit,
    int dir,
    int count,
    tsn_sr_flowidx_entry_t **entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowidx_entry_t *fid, *begin, *prev, *bprev;
    int idx, bidx;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (dir < 0 || dir >= SR_FLOW_DIR_COUNT || count <= 0 || entry == NULL) {
        return BCM_E_PARAM;
    }

    /* Protect access to the flow indexes */
    SR_FLOW_MGMT_LOCK(unit);

    /* Check if any index is available */
    bkinfo = &tsn_bk->sr_flows;
    if (NULL == bkinfo->flowids_avail[dir]) {
        SR_FLOW_MGMT_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    /*
     * Quick retrieval for typical case: count == 1.
     * Dynamic software learning may be performed for HSR/PRP where the count
     * should always be 1. We need to make this as quick as possible.
     * Here we should be able to do it in O(1) using pre-allocated linked list.
     */
    if (1 == count) {
        /* Get one entry from the head */
        fid = bkinfo->flowids_avail[dir];
        bkinfo->flowids_avail[dir] = fid->next;
        fid->next = NULL;
        *entry = fid;
        SR_FLOW_MGMT_UNLOCK(unit);
        return BCM_E_NONE;
    }

    /* Traverse the list and search for enough continuous items to allocate */
    idx = bidx = bkinfo->max_flows[dir]; /* invalid current index */
    prev = bprev = begin = NULL;
    for (fid = bkinfo->flowids_avail[dir]; fid != NULL; fid = fid->next) {

        /* Check if it's continuous */
        if (idx + 1 != fid->flow_idx) {
            /* Index not continuous, restart the searching */
            idx = fid->flow_idx;
            bidx = idx;
            begin = fid;
            bprev = prev;
        } else if (fid->flow_idx - bidx + 1 == count) {

            /*
             * Found enough continuous flow indexes: update and return
             */

            /* Update the linked list by removing the chain from the list */
            if (NULL == bprev) {
                /* The chain is the head */
                bkinfo->flowids_avail[dir] = fid->next;
            } else {
                /* Connect previous one to the next of the chain */
                bprev->next = fid->next;
            }

            /* Terminate this chain and return */
            fid->next = NULL;
            *entry = begin;
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_NONE;

        } else {
            /* continuous but not yet reached the count */
            idx++;
        }

        /* record the previous one */
        prev = fid;
    }

    /* Cannot find enough continuous entries */
    SR_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_RESOURCE;
}

/*
 * De-allocate an SR flowset and underlying flows with flowset index
 * This function should be called with flowset mutex locked (if made public)
 * and ref_count = 0.
 */
STATIC int
bcmi_esw_tsn_sr_flowset_free(int unit, int dir, int fsid)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int rv = BCM_E_NONE;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (dir < 0 || dir >= SR_FLOW_DIR_COUNT) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    fset = &bkinfo->flowsets[dir][fsid];

    /* Free flow indexes */
    rv = bcmi_esw_tsn_sr_flow_idx_free(unit, dir, fset->flow_ide);

    /* Clear flowset data */
    fset->flow_ide = NULL;
    fset->num_flows = 0;

    return rv;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_destroy
 * Purpose:
 *     Destroy an SR (TX or RX) flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_flowset_destroy(int unit, bcm_tsn_sr_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    if (fset->num_flows == 0) {
        /* Not allocated */
        return BCM_E_NOT_FOUND;
    }

    /* Check if it's still in use */
    SR_FLOWSET_LOCK(fset);
    if (fset->ref_count != 0) {
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_BUSY;
    }

    /* Handle per-flow RX/TX specific clean up */
    for (i = 0; i < fset->num_flows; i++) {
        if (dir == SR_FLOW_DIR_RX) {
            /* RX: de-allocate SN history slice and clear HW */
            SR_FLOW_RX_LOCK(unit, fsid + i);
            if (SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_free) {
                (void)SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_free(
                        unit, bkinfo->flows_rx[fsid + i].sn_slice);
            }
            bkinfo->flows_rx[fsid + i].sn_slice = NULL;
            if (SR_FLOW_MGMT_DEVINFO(unit)->clear_rx_flow) {
                (void)SR_FLOW_MGMT_DEVINFO(unit)->clear_rx_flow(unit, fsid + i);
            }
            if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
                (void)bcmi_esw_tsn_mtu_profile_ref_dec(
                  unit, bkinfo->flows_rx[fsid + i].config.ingress_mtu_profile);
            }
            SR_FLOW_RX_UNLOCK(unit, fsid + i);
        } else {
            /* TX: simply clear HW */
            SR_FLOW_TX_LOCK(unit, fsid + i);
            if (SR_FLOW_MGMT_DEVINFO(unit)->clear_tx_flow) {
                (void)SR_FLOW_MGMT_DEVINFO(unit)->clear_tx_flow(unit, fsid + i);
            }
            if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
                (void)bcmi_esw_tsn_mtu_profile_ref_dec(
                  unit, bkinfo->flows_tx[fsid + i].config.ingress_mtu_profile);
            }
            SR_FLOW_TX_UNLOCK(unit, fsid + i);
        }
        /* update active flow information for event management */
        (void)bcmi_esw_tsn_event_rx_flow_delete(unit, fsid + i);
    }

    /* De-allocate the flowset (note that mutex is always there) */
    rv = bcmi_esw_tsn_sr_flowset_free(unit, dir, fsid);
    SR_FLOWSET_UNLOCK(fset);
    return rv;
}

/* Allocate an SR flowset with both TX and RX directions */
STATIC int
bcmi_esw_tsn_sr_flowset_alloc(
    int unit,
    int dir,
    bcm_tsn_pri_map_t pri_map,
    int *pfsid)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    bcm_tsn_pri_map_config_t pmcfg;
    tsn_sr_flowidx_entry_t *fids;
    tsn_sr_flowset_t *fset;
    int fsid;
    int num_flows;
    int rv;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (dir < 0 || dir >= SR_FLOW_DIR_COUNT || pfsid == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;

    /* Get number of flows from this priority map */
    num_flows = 1;
    if (pri_map != BCM_TSN_PRI_MAP_INVALID) {
        /* call API to get priority map configuration */
        rv = bcm_esw_tsn_pri_map_get(unit, pri_map, &pmcfg);
        if (BCM_FAILURE(rv) || pmcfg.map_type != bcmTsnPriMapTypeSrFlow) {
            return BCM_E_PARAM;
        }
        /* Number of flows is max flow offset + 1 */
        for (i = 0; i < pmcfg.num_map_entries; i++) {
            if ((pmcfg.map_entries[i].flags &
                 BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) &&
                (pmcfg.map_entries[i].flow_offset > num_flows - 1)) {
                num_flows = pmcfg.map_entries[i].flow_offset + 1;
            }
        }
    }

    /* Allocate flow indexes */
    rv = bcmi_esw_tsn_sr_flow_idx_allocate(unit, dir, num_flows, &fids);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * For efficiency and convenience, the flowset index is the base (first)
     * flow index so that we don't need to have another set of flowset
     * allocation/free mechanism which would take more time in this time
     * critical operation.
     */
    fsid = fids->flow_idx;
    assert(fsid >= 0 && fsid + num_flows <= bkinfo->max_flows[dir]);

    /* Write to the flowset */
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    assert(fset->num_flows == 0);
    fset->map_id = pri_map;
    fset->flow_ide = fids;
    fset->ref_count = 0;
    fset->num_flows = num_flows;
    SR_FLOWSET_UNLOCK(fset);

    /* Return the allocated flowset index */
    *pfsid = fsid;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flowset_create
 * Purpose:
 *     Create an SR RX flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_rx_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_sr_rx_flow_config_t *def_cfg,
    bcm_tsn_sr_flowset_t *flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    bcm_tsn_sr_flowset_t flowset_id;
    bcmi_tsn_sr_rx_flow_t *flow;
    bcmi_tsn_sr_seqnum_slice_t slice;
    int fsid;
    uint32 reset_mode;
    uint32 flags;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (def_cfg == NULL || flowset == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;

    /* sanity check for platform specific routines */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->check_rx_flow_config ||
        !SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_alloc ||
        !SR_FLOW_MGMT_DEVINFO(unit)->write_rx_flow) {
        return BCM_E_UNAVAIL;
    }

    /* Validate flow configuration */
    BCM_IF_ERROR_RETURN(
        SR_FLOW_MGMT_DEVINFO(unit)->check_rx_flow_config(unit, def_cfg));

    /* Allocate flows and flowset. Basic init check is also done inside. */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_sr_flowset_alloc(unit, SR_FLOW_DIR_RX, pri_map, &fsid));
    flowset_id = SR_FLOWSET_ID_FROM_IDX(SR_FLOW_DIR_RX, fsid);

    /* Write to the flowset software copy */
    bkinfo = &tsn_bk->sr_flows;
    fset = &bkinfo->flowsets[SR_FLOW_DIR_RX][fsid];
    SR_FLOWSET_LOCK(fset);
    sal_memcpy(&fset->config.rx, def_cfg, sizeof(fset->config.rx));

    /* Write to individual flows */
    for (i = 0; i < fset->num_flows; i++) {
        SR_FLOW_RX_LOCK(unit, fsid + i);
        flow = &bkinfo->flows_rx[fsid + i];

        /* Allocate sequence number history pool for this flow */
        rv = SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_alloc(
                unit, def_cfg->seqnum_history_win_size, &slice);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
            SR_FLOW_RX_UNLOCK(unit, fsid + i);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }

        /* Store the flow configuration */
        sal_memcpy(&flow->config, def_cfg, sizeof(flow->config));
        flow->sn_slice = slice;

        /* Write to HW */
        rv = SR_FLOW_MGMT_DEVINFO(unit)->write_rx_flow(unit, fsid + i, flow);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
            SR_FLOW_RX_UNLOCK(unit, fsid + i);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            (void)bcmi_esw_tsn_mtu_profile_ref_inc(
                        unit, def_cfg->ingress_mtu_profile);
        }
        /*
         * Reset the flow as the initial state
         */
        flags = BCM_TSN_SR_RX_FLOW_RESET_ALL;
        /* Get reset mode */
        rv = bcm_esw_tsn_control_get(
                unit, bcmTsnControlSrSeqNumWindowResetMode, &reset_mode);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
            SR_FLOW_RX_UNLOCK(unit, fsid + i);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }
        if (reset_mode != BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1) {
            /* Only need to set SN RESET bit for HW reset method 1 */
            flags &= ~BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_STATE;
        }
        /* Call flow reset function */
        rv = bcm_esw_tsn_sr_rx_flow_reset(
                unit, flags, SR_FLOW_ID_FROM_IDX(SR_FLOW_DIR_RX, fsid + i));
        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
            SR_FLOW_RX_UNLOCK(unit, fsid + i);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }

        SR_FLOW_RX_UNLOCK(unit, fsid + i);

        /* provide active flow information for event management */
        (void)bcmi_esw_tsn_event_rx_flow_add(unit, fsid + i);
    }
    SR_FLOWSET_UNLOCK(fset);

    /* Convert to user flowset ID */
    *flowset = flowset_id;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_tx_flowset_create
 * Purpose:
 *     Create an SR TX flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_tx_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_sr_tx_flow_config_t *def_cfg,
    bcm_tsn_sr_flowset_t *flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    bcm_tsn_sr_flowset_t flowset_id;
    bcmi_tsn_sr_tx_flow_t *flow;
    int fsid;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (def_cfg == NULL || flowset == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;

    /* Validate flow configuration */
    if (SR_FLOW_MGMT_DEVINFO(unit)->check_tx_flow_config) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->check_tx_flow_config(unit, def_cfg);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    /* Allocate flows and flowset. Basic init check is also done inside. */
    rv = bcmi_esw_tsn_sr_flowset_alloc(unit, SR_FLOW_DIR_TX, pri_map, &fsid);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    flowset_id = SR_FLOWSET_ID_FROM_IDX(SR_FLOW_DIR_TX, fsid);

    /* Write to the flowset software copy */
    bkinfo = &tsn_bk->sr_flows;
    fset = &bkinfo->flowsets[SR_FLOW_DIR_TX][fsid];
    SR_FLOWSET_LOCK(fset);
    sal_memcpy(&fset->config.tx, def_cfg, sizeof(fset->config.tx));

    /* Write to individual flows */
    for (i = 0; i < fset->num_flows; i++) {
        SR_FLOW_TX_LOCK(unit, fsid + i);
        flow = &bkinfo->flows_tx[fsid + i];

        /* Save a SW copy */
        sal_memcpy(&flow->config, def_cfg, sizeof(flow->config));

        /* Write to HW */
        if (SR_FLOW_MGMT_DEVINFO(unit)->write_tx_flow) {
            rv = SR_FLOW_MGMT_DEVINFO(unit)->write_tx_flow(
                    unit, fsid + i, flow);
            if (BCM_FAILURE(rv)) {
                (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
                SR_FLOW_TX_UNLOCK(unit, fsid + i);
                SR_FLOWSET_UNLOCK(fset);
                return rv;
            }
        } else {
            (void)bcmi_esw_tsn_sr_flowset_destroy(unit, flowset_id);
            SR_FLOW_TX_UNLOCK(unit, fsid + i);
            SR_FLOWSET_UNLOCK(fset);
            return BCM_E_UNAVAIL;
        }
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            (void)bcmi_esw_tsn_mtu_profile_ref_inc(
                        unit, def_cfg->ingress_mtu_profile);
        }
        SR_FLOW_TX_UNLOCK(unit, fsid + i);
    }
    SR_FLOWSET_UNLOCK(fset);

    /* Convert to user flowset ID */
    *flowset = flowset_id;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_tx_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flowset, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_tx_flowset_config_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_sr_tx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;

    /* parameter check */
    if (pri_map == NULL || config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (dir != SR_FLOW_DIR_TX) {
        return BCM_E_BADID;
    }
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Return the value */
    *pri_map = fset->map_id;
    sal_memcpy(config, &fset->config.tx, sizeof(fset->config.tx));
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flowset, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flowset_config_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_sr_rx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;

    /* parameter check */
    if (pri_map == NULL || config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Return the value */
    *pri_map = fset->map_id;
    sal_memcpy(config, &fset->config.rx, sizeof(fset->config.rx));
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_status_get
 * Purpose:
 *     Get current status of specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     status           - (OUT) status structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_flowset_status_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_sr_flowset_status_t *status)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;

    /* parameter check */
    if (status == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Return the value */
    status->active = fset->ref_count > 0 ?  1 : 0;
    status->count = fset->num_flows;
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_ref_inc
 * Purpose:
 *     Increase reference count for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sr_flowset_ref_inc(int unit, bcm_tsn_sr_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }
    /* Update the reference count */
    fset->ref_count++;
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_ref_dec
 * Purpose:
 *     Decrease reference count for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sr_flowset_ref_dec(int unit, bcm_tsn_sr_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir, fsid;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }
    /* Update the reference count */
    if (fset->ref_count > 0) {
        fset->ref_count--;
    }
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_traverse
 * Purpose:
 *     Traverse all created SR flowsets
 * Parameters:
 *     unit             - (IN) unit number
 *     is_rx            - (IN) 1 for RX flowsets; 0 for TX flowsets
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data supplied to the callback function
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_flowset_traverse(
    int unit,
    int is_rx,
    bcm_tsn_sr_flowset_traverse_cb cb,
    void *user_data)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    if (cb == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    /* Loop through the flowset array */
    bkinfo = &tsn_bk->sr_flows;
    dir = is_rx ? SR_FLOW_DIR_RX : SR_FLOW_DIR_TX;
    for (i = 0; i < bkinfo->max_flows[dir]; i++) {
        fset = &bkinfo->flowsets[dir][i];
        SR_FLOWSET_LOCK(fset);
        if (fset->num_flows != 0) {
            rv = cb(unit, SR_FLOWSET_ID_FROM_IDX(dir, i), user_data);
        }
        SR_FLOWSET_UNLOCK(fset);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_flow_get
 * Purpose:
 *     Retrieve an individual SR flow based on the flow offset from a flow set.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     index            - (IN) index of the flow
 *     flow_id          - (OUT) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_flowset_flow_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    int index,
    bcm_tsn_sr_flow_t *flow_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowset_t *fset;
    int dir;
    int fsid;

    /* parameter check */
    if (flow_id == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOWSET_ID_TO_DIR(flowset);
    fsid = SR_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[dir][fsid];
    SR_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }
    if (index < 0 || index >= fset->num_flows) {
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_PARAM;
    }
    assert(fsid + index < bkinfo->max_flows[dir]);

    /* Got SW flow ID */
    *flow_id = SR_FLOW_ID_FROM_IDX(dir, fsid + index);
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_tx_flow_config_get
 * Purpose:
 *     Get configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flowset, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_tx_flow_config_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_TX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Return the configuration */
    SR_FLOW_TX_LOCK(unit, fidx);
    sal_memcpy(config, &bkinfo->flows_tx[fidx].config, sizeof(*config));
    SR_FLOW_TX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_config_get
 * Purpose:
 *     Get configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flowset, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_config_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Return the configuration */
    SR_FLOW_RX_LOCK(unit, fidx);
    sal_memcpy(config, &bkinfo->flows_rx[fidx].config, sizeof(*config));
    SR_FLOW_RX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_tx_flow_config_set
 * Purpose:
 *     Set configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_tx_flow_config_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int rv;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_TX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Validate flow configuration */
    if (SR_FLOW_MGMT_DEVINFO(unit)->check_tx_flow_config) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->check_tx_flow_config(unit, config);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    /*
     * Configure the flow using the supplied configuration
     */
    SR_FLOW_TX_LOCK(unit, fidx);
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Clear the old MTU profile id */
        if (SR_FLOW_MGMT_DEVINFO(unit)->clear_tx_flow_mtu) {
            rv = SR_FLOW_MGMT_DEVINFO(unit)->clear_tx_flow_mtu(unit, fidx);
            if (BCM_FAILURE(rv)) {
                SR_FLOW_TX_UNLOCK(unit, fidx);
                return rv;
            }
        } else {
            SR_FLOW_TX_UNLOCK(unit, fidx);
            return BCM_E_UNAVAIL;
        }
        (void)bcmi_esw_tsn_mtu_profile_ref_dec(
            unit, bkinfo->flows_tx[fidx].config.ingress_mtu_profile);
    }
    /* Copy to SW flow info */
    sal_memcpy(&bkinfo->flows_tx[fidx].config, config, sizeof(*config));
    /* Write to HW */
    if (SR_FLOW_MGMT_DEVINFO(unit)->write_tx_flow) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->write_tx_flow(
                unit, fidx, &bkinfo->flows_tx[fidx]);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_TX_UNLOCK(unit, fidx);
            return rv;
        }
    } else {
        SR_FLOW_TX_UNLOCK(unit, fidx);
        return BCM_E_UNAVAIL;
    }
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        (void)bcmi_esw_tsn_mtu_profile_ref_inc(
            unit, bkinfo->flows_tx[fidx].config.ingress_mtu_profile);
    }
    SR_FLOW_TX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_config_set
 * Purpose:
 *     Set configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_config_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    bcmi_tsn_sr_seqnum_slice_t slice;
    tsn_sr_flowset_t *fset;
    uint32 reset_mode;
    uint32 flags;
    int fidx;
    int dir;
    int rv;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    fset = NULL;
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            fset = &bkinfo->flowsets[dir][i];
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (fset == NULL) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Validate flow configuration */
    if (SR_FLOW_MGMT_DEVINFO(unit)->check_rx_flow_config) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->check_rx_flow_config(unit, config);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    /* Get reset mode here before we modify anything */
    rv = bcm_esw_tsn_control_get(
            unit, bcmTsnControlSrSeqNumWindowResetMode, &reset_mode);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Can't change window size when its flowset is active */
    SR_FLOWSET_LOCK(fset);
    if (fset->ref_count != 0 &&
        config->seqnum_history_win_size !=
            bkinfo->flows_rx[fidx].config.seqnum_history_win_size) {
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_BUSY;
    }

    /*
     * Configure the flow using the supplied configuration
     */
    SR_FLOW_RX_LOCK(unit, fidx);
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Clear the old MTU profile id */
        if (SR_FLOW_MGMT_DEVINFO(unit)->clear_rx_flow_mtu) {
            rv = SR_FLOW_MGMT_DEVINFO(unit)->clear_rx_flow_mtu(unit, fidx);
            if (BCM_FAILURE(rv)) {
                SR_FLOW_RX_UNLOCK(unit, fidx);
                return rv;
            }
        } else {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            return BCM_E_UNAVAIL;
        }
        (void)bcmi_esw_tsn_mtu_profile_ref_dec(
            unit, bkinfo->flows_rx[fidx].config.ingress_mtu_profile);
    }
    /* Re-allocate sequence number history pool if necessary */
    if (config->seqnum_history_win_size !=
        bkinfo->flows_rx[fidx].config.seqnum_history_win_size) {
        if (!SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_alloc ||
            !SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_free) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            SR_FLOWSET_UNLOCK(fset);
            return BCM_E_UNAVAIL;
        }
        /* Try allocating first */
        rv = SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_alloc(
                unit, config->seqnum_history_win_size, &slice);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }
        /* Free the original */
        (void)SR_FLOW_MGMT_DEVINFO(unit)->seqnum_slice_free(
                unit, bkinfo->flows_rx[fidx].sn_slice);
        /* Now we use the new slice */
        bkinfo->flows_rx[fidx].sn_slice = slice;
    }
    /* Copy to SW flow info */
    sal_memcpy(&bkinfo->flows_rx[fidx].config, config, sizeof(*config));
    /* Write to HW */
    if (SR_FLOW_MGMT_DEVINFO(unit)->write_rx_flow) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->write_rx_flow(
                unit, fidx, &bkinfo->flows_rx[fidx]);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            SR_FLOWSET_UNLOCK(fset);
            return rv;
        }
    } else {
        SR_FLOW_RX_UNLOCK(unit, fidx);
        SR_FLOWSET_UNLOCK(fset);
        return BCM_E_UNAVAIL;
    }

    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        (void)bcmi_esw_tsn_mtu_profile_ref_inc(
            unit, bkinfo->flows_rx[fidx].config.ingress_mtu_profile);
    }

    /*
     * Reset the flow as the initial state
     */
    flags = BCM_TSN_SR_RX_FLOW_RESET_ALL;
    /* Update flags based on reset mode */
    if (reset_mode != BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1) {
        /* Only need to set SN RESET bit for HW reset method 1 */
        flags &= ~BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_STATE;
    }
    /* Call flow reset function */
    rv = bcm_esw_tsn_sr_rx_flow_reset(
            unit, flags, SR_FLOW_ID_FROM_IDX(SR_FLOW_DIR_RX, fidx));
    if (BCM_FAILURE(rv)) {
        SR_FLOW_RX_UNLOCK(unit, fidx);
        SR_FLOWSET_UNLOCK(fset);
        return rv;
    }

    SR_FLOW_RX_UNLOCK(unit, fidx);
    SR_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_tx_flow_status_get
 * Purpose:
 *     Get status for the specified flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     status           - (OUT) flow status
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_tx_flow_status_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_status_t *status)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int rv;
    int i;

    /* parameter check */
    if (status == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_TX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Get and return the status */
    SR_FLOW_TX_LOCK(unit, fidx);
    if (SR_FLOW_MGMT_DEVINFO(unit)->get_tx_flow_status) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->get_tx_flow_status(unit, fidx, status);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_TX_UNLOCK(unit, fidx);
            return rv;
        }
    } else {
        SR_FLOW_TX_UNLOCK(unit, fidx);
        return BCM_E_UNAVAIL;
    }
    SR_FLOW_TX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_status_get
 * Purpose:
 *     Get status for the specified flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     status           - (OUT) flow status
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_status_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_status_t *status)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int rv;
    int i;

    /* parameter check */
    if (status == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Get and return the status */
    SR_FLOW_RX_LOCK(unit, fidx);
    if (SR_FLOW_MGMT_DEVINFO(unit)->get_rx_flow_status) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->get_rx_flow_status(unit, fidx, status);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            return rv;
        }
    } else {
        SR_FLOW_RX_UNLOCK(unit, fidx);
        return BCM_E_UNAVAIL;
    }
    SR_FLOW_RX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_reset
 * Purpose:
 *     Reset SR RX flow status
 * Parameters:
 *     unit             - (IN) unit number
 *     flags            - (IN) flags to select items to reset
 *     flow_id          - (IN) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_reset(int unit, uint32 flags, bcm_tsn_sr_flow_t flow_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    uint32 reset_mode;
    int rv;
    int i;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (flags & ~BCM_TSN_SR_RX_FLOW_RESET_ALL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Call platform specific routine to do reset */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->reset_rx_flow ||
        !SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_reset) {
        return BCM_E_UNAVAIL;
    }
    SR_FLOW_RX_LOCK(unit, fidx);
    /* Seqnum history window reset requires special handling */
    if (flags & BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_HISTORY) {
        /* Get reset mode first */
        rv = bcm_esw_tsn_control_get(
                unit, bcmTsnControlSrSeqNumWindowResetMode, &reset_mode);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            return rv;
        }
        /* Reset seqnum window using platform specific routine */
        rv = SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_reset(
                unit, fidx,
                bkinfo->flows_rx[fidx].config.seqnum_history_win_size,
                bkinfo->flows_rx[fidx].sn_slice, reset_mode);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            return rv;
        }
    }
    /* For other flags, call another plafform specific routine */
    if (flags & (~BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_HISTORY)) {
        rv = SR_FLOW_MGMT_DEVINFO(unit)->reset_rx_flow(unit, fidx, flags);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_RX_UNLOCK(unit, fidx);
            return rv;
        }
    }
    SR_FLOW_RX_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_seqnum_history_get
 * Purpose:
 *     Get the sequence number history bits (one bit for one sequence number)
 *     in the RX flow.
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     offset           - (IN) Offset (in bits) in the seqnum history window
 *     max_size         - (IN) Size (in bits) of the buffer
 *     history          - (OUT) Bit buffer to read
 *     actual           - (OUT) Actual size (in bits) read to the buffer
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_seqnum_history_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    int offset,
    int max_size,
    uint8 *history,
    int *actual)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int winsize;
    int rv;
    int i;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (history == NULL || actual == NULL || offset < 0 || max_size <= 0) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Call platform specific routine to do it */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_read) {
        return BCM_E_UNAVAIL;
    }
    SR_FLOW_RX_LOCK(unit, fidx);
    /* Do advanced parameter check */
    winsize = bkinfo->flows_rx[fidx].config.seqnum_history_win_size;
    if (offset >= winsize) {
        SR_FLOW_RX_UNLOCK(unit, fidx);
        return BCM_E_PARAM;
    }
    max_size = (max_size > (winsize - offset) ? (winsize - offset) : max_size);
    /* Call platform specific routine for seqnum operation */
    rv = SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_read(
            unit, fidx,
            bkinfo->flows_rx[fidx].config.seqnum_history_win_size,
            bkinfo->flows_rx[fidx].sn_slice,
            offset, max_size, history, actual);
    SR_FLOW_RX_UNLOCK(unit, fidx);
    return rv;

}

/*
 * Function:
 *     bcmi_esw_tsn_sr_rx_flow_seqnum_history_set
 * Purpose:
 *     Set the sequence number history bits (one bit for one sequence number)
 *     in the RX flow.
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     offset           - (IN) Offset (in bits) in the seqnum history window
 *     size             - (IN) Size (in bits) to write
 *     history          - (IN) Bit buffer to write
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
STATIC int
bcmi_esw_tsn_sr_rx_flow_seqnum_history_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    int offset,
    int size,
    uint8 *history)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int winsize;
    int rv;
    int i;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (history == NULL || offset < 0 || size <= 0) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (dir != SR_FLOW_DIR_RX) {
        return BCM_E_BADID;
    }
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Call platform specific routine to do it */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_write) {
        return BCM_E_UNAVAIL;
    }

    SR_FLOW_RX_LOCK(unit, fidx);
    /* Do advanced parameter check */
    winsize = bkinfo->flows_rx[fidx].config.seqnum_history_win_size;
    if (offset >= winsize || size > winsize) {
        SR_FLOW_RX_UNLOCK(unit, fidx);
        return BCM_E_PARAM;
    }
    /* Call platform specific routine for seqnum operation */
    rv = SR_FLOW_MGMT_DEVINFO(unit)->seqnum_hist_write(
            unit, fidx, winsize, bkinfo->flows_rx[fidx].sn_slice,
            offset, size, history);
    SR_FLOW_RX_UNLOCK(unit, fidx);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_hw_flow_id_get
 * Purpose:
 *     Convert software SR flow ID to hardware SR flow ID
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) SW flow ID
 *     hw_id            - (OUT) HW flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sr_hw_flow_id_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    uint32 *hw_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (hw_id == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Call platform specific routine to convert */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id) {
        return BCM_E_UNAVAIL;
    }
    return SR_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id(
                unit, (dir == SR_FLOW_DIR_TX), fidx, hw_id);
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_sw_flow_id_get
 * Purpose:
 *     Convert hardware SR flow ID to software SR flow ID
 * Parameters:
 *     unit             - (IN) unit number
 *     hw_id            - (IN) HW flow ID
 *     flow_id          - (OUT) SW flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sr_sw_flow_id_get(
    int unit,
    uint32 hw_id,
    bcm_tsn_sr_flow_t *flow_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    const bcmi_esw_tsn_dev_info_t *dev_info;
    int is_tx;
    int dir;
    int fidx;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    if (flow_id == NULL) {
        return BCM_E_PARAM;
    }

    if (tsn_bk_info[unit] == NULL) {
        /* Using device specific info that need no bk_info */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_noninit_dev_info_get(unit, &dev_info));
        if (!dev_info->sr_flows_info->get_sw_flow_idx) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN(
            dev_info->sr_flows_info->get_sw_flow_idx(unit, hw_id,
                                                     &is_tx, &fidx));
        dir = is_tx ? SR_FLOW_DIR_TX : SR_FLOW_DIR_RX;
        *flow_id = SR_FLOW_ID_FROM_IDX(dir, fidx);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;

    /* Call platform specific routine to convert */
    if (!SR_FLOW_MGMT_DEVINFO(unit)->get_sw_flow_idx) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(
        SR_FLOW_MGMT_DEVINFO(unit)->get_sw_flow_idx(
            unit, hw_id, &is_tx, &fidx));
    dir = is_tx ? SR_FLOW_DIR_TX : SR_FLOW_DIR_RX;

    /* Validate converted index */
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Done */
    *flow_id = SR_FLOW_ID_FROM_IDX(dir, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_flowset_identify
 * Purpose:
 *     SR flow to flowset converstion (identify the flowset based on a flow)
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sr_flowset_identify(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_flowset_t *flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int fidx;
    int dir;
    int i;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (flowset == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->sr_flows;
    dir = SR_FLOW_ID_TO_DIR(flow_id);
    fidx = SR_FLOW_ID_TO_IDX(flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows[dir]) {
        return BCM_E_PARAM;
    }

    /* Find the flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[dir][i].num_flows != 0) {
            if (bkinfo->flowsets[dir][i].num_flows > (fidx - i)) {
                /* Convert to user flowset ID */
                *flowset = SR_FLOWSET_ID_FROM_IDX(dir, i);
                return BCM_E_NONE;
            } else {
                /* The flow is not within this flowset */
                return BCM_E_NOT_FOUND;
            }
        }
    }

    /* No flowset found with index less than the flow */
    return BCM_E_NOT_FOUND;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* SR flow: allocate specified flow idx(s) for warmboot restore */
STATIC int
bcmi_esw_tsn_sr_flow_idx_restore(
    int unit,
    int dir,
    int index,
    int count,
    tsn_sr_flowidx_entry_t **entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowidx_entry_t *fid, *prev, *bprev, *begin;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (dir < 0 || dir >= SR_FLOW_DIR_COUNT || count <= 0 || entry == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;

    /* Protect access to the flow indexes */
    SR_FLOW_MGMT_LOCK(unit);

    /* Traverse the list and search for the specified items */
    begin = NULL;
    prev = bprev = NULL;
    for (fid = bkinfo->flowids_avail[dir]; fid != NULL; fid = fid->next) {
        if (fid->flow_idx == index) {
            if (begin == NULL) {
                /* Found the head */
                begin = fid;
                bprev = prev;
            }

            /* Check if we have got all the items */
            count--;
            if (count == 0) {
                /* Done. Removing the chain from the list */
                if (NULL == bprev) {
                    /* The chain was the head */
                    bkinfo->flowids_avail[dir] = fid->next;
                } else {
                    /* Connect previous one to the next of the chain */
                    bprev->next = fid->next;
                }

                /* Terminate this chain and return */
                fid->next = NULL;
                *entry = begin;
                SR_FLOW_MGMT_UNLOCK(unit);
                return BCM_E_NONE;
            }
            index++;
        } else if (fid->flow_idx > index) {
            /* Not found in this sorted list */
            break;
        }

        /* record the previous one */
        prev = fid;
    }

    /* Cannot find the specified item or enough continuous entries */
    SR_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_INTERNAL;
}

/* SR flow management - warmboot scache size allocation */
STATIC int
bcmi_esw_tsn_sr_flow_mgmt_wb_alloc(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    int stable_size = 0;
    int alloc_size;
    int rv;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* Calculate allocate size */
    alloc_size = 0;
    alloc_size += sizeof(uint16);       /* Number of actual RX flows */
    alloc_size += sizeof(uint16);       /* Number of actual TX flows */
    alloc_size +=                       /* RX flows */
        sizeof(tsn_sr_rx_flowset_wb_t) * bkinfo->max_flows[SR_FLOW_DIR_RX];
    alloc_size +=                       /* TX flows */
        sizeof(tsn_sr_tx_flowset_wb_t) * bkinfo->max_flows[SR_FLOW_DIR_TX];

    /* Allocate scache for warmboot */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_SR_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        /* Proceed with level 1 warmboot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/* SR flow management - sync information to scache */
STATIC int
bcmi_esw_tsn_sr_flow_mgmt_sync(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL, *scache;
    tsn_sr_rx_flowset_wb_t rxflow;
    tsn_sr_tx_flowset_wb_t txflow;
    int stable_size = 0;
    tsn_sr_flowset_t *fset;
    uint16 flows;
    int rv;
    int i;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_SR_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(rv);
    assert(scache_ptr != NULL);

    /* Avoid flowsets to be created/destroyed at the same time */
    SR_FLOW_MGMT_LOCK(unit);

    /* Loop through RX flowsets and save to scache */
    flows = 0;
    scache = scache_ptr + sizeof(uint16) * 2;
    for (i = 0; i < bkinfo->max_flows[SR_FLOW_DIR_RX]; i++) {
        fset = &bkinfo->flowsets[SR_FLOW_DIR_RX][i];
        SR_FLOWSET_LOCK(fset);
        if (fset->num_flows != 0) {
            rxflow.flow_idx = i;
            rxflow.map_id = fset->map_id;
            sal_memcpy(&rxflow.config, &fset->config.rx, sizeof(rxflow.config));
            flows++;
            sal_memcpy(scache, &rxflow, sizeof(rxflow));
            scache += sizeof(rxflow);
        }
        SR_FLOWSET_UNLOCK(fset);
    }
    sal_memcpy(scache_ptr, &flows, sizeof(uint16)); /* Actual RX flowsets */

    /* And TX flowsets */
    flows = 0;
    for (i = 0; i < bkinfo->max_flows[SR_FLOW_DIR_TX]; i++) {
        fset = &bkinfo->flowsets[SR_FLOW_DIR_TX][i];
        SR_FLOWSET_LOCK(fset);
        if (fset->num_flows != 0) {
            txflow.flow_idx = i;
            txflow.map_id = fset->map_id;
            sal_memcpy(&txflow.config, &fset->config.tx, sizeof(txflow.config));
            flows++;
            sal_memcpy(scache, &txflow, sizeof(txflow));
            scache += sizeof(txflow);
        }
        SR_FLOWSET_UNLOCK(fset);
    }
    sal_memcpy(scache_ptr + sizeof(uint16), &flows, sizeof(uint16));

    /* Done */
    SR_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

/* SR flow management - reload flow management information from scache */
STATIC int
bcmi_esw_tsn_sr_flow_mgmt_reload(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_sr_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    tsn_sr_rx_flowset_wb_t rxflow;
    tsn_sr_tx_flowset_wb_t txflow;
    int stable_size = 0;
    tsn_sr_flowset_t *fset;
    tsn_sr_flowidx_entry_t *flow_ide;
    bcm_tsn_pri_map_config_t pmcfg;
    uint16 rxflows, txflows, i;
    int num_flows;
    int rv;
    int j, idx;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->sr_flows;
    if (!SR_FLOW_MGMT_DEVINFO(unit)->read_rx_flow ||
        !SR_FLOW_MGMT_DEVINFO(unit)->read_tx_flow) {
        return BCM_E_UNAVAIL;
    }

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_SR_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(rv);
    assert(scache_ptr != NULL);

    /* Avoid flowsets to be created/destroyed at the same time */
    SR_FLOW_MGMT_LOCK(unit);

    /* Get number of saved RX/TX flowsets */
    sal_memcpy(&rxflows, scache_ptr, sizeof(uint16));
    scache_ptr += sizeof(uint16);
    sal_memcpy(&txflows, scache_ptr, sizeof(uint16));
    scache_ptr += sizeof(uint16);

    /* Restore RX flowsets */
    for (i = 0; i < rxflows; i++) {
        /* Read from scache */
        sal_memcpy(&rxflow, scache_ptr, sizeof(rxflow));
        scache_ptr += sizeof(rxflow);

        /* Validate flow index */
        if (rxflow.flow_idx < 0 ||
            rxflow.flow_idx >= bkinfo->max_flows[SR_FLOW_DIR_RX]) {
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Validate priority map and get number of flows inside this flowset */
        num_flows = 1;
        if (rxflow.map_id != BCM_TSN_PRI_MAP_INVALID) {
            /* call API to get priority map configuration */
            rv = bcm_esw_tsn_pri_map_get(unit, rxflow.map_id, &pmcfg);
            if (BCM_FAILURE(rv) || pmcfg.map_type != bcmTsnPriMapTypeSrFlow) {
                SR_FLOW_MGMT_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            /* Number of flows is max flow offset + 1 */
            for (j = 0; j < pmcfg.num_map_entries; j++) {
                if ((pmcfg.map_entries[j].flags &
                     BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) &&
                    (pmcfg.map_entries[j].flow_offset > num_flows - 1)) {
                    num_flows = pmcfg.map_entries[j].flow_offset + 1;
                }
            }
        }

        /* Restore flow index entries */
        rv = bcmi_esw_tsn_sr_flow_idx_restore(
                unit, SR_FLOW_DIR_RX, rxflow.flow_idx, num_flows, &flow_ide);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Confirmed that this flowset can be restored */
        fset = &bkinfo->flowsets[SR_FLOW_DIR_RX][rxflow.flow_idx];
        SR_FLOWSET_LOCK(fset);

        /* Restore the flowset */
        fset->flow_ide = flow_ide;
        fset->map_id = rxflow.map_id;
        fset->num_flows = num_flows;
        fset->ref_count = 0;
        sal_memcpy(&fset->config.rx, &rxflow.config, sizeof(rxflow.config));

        /* Restore the individual flows */
        for (j = 0; j < num_flows; j++) {
            idx = rxflow.flow_idx + j;
            SR_FLOW_RX_LOCK(unit, idx);
            rv = SR_FLOW_MGMT_DEVINFO(unit)->read_rx_flow(
                    unit, idx, &bkinfo->flows_rx[idx]);
            if (BCM_FAILURE(rv)) {
                SR_FLOW_RX_UNLOCK(unit, idx);
                SR_FLOWSET_UNLOCK(fset);
                SR_FLOW_MGMT_UNLOCK(unit);
                return rv;
            }
            SR_FLOW_RX_UNLOCK(unit, idx);
        }

        /* Done restoring this flowset */
        SR_FLOWSET_UNLOCK(fset);
    }

    /* Restore TX flowsets */
    for (i = 0; i < txflows; i++) {
        /* Read from scache */
        sal_memcpy(&txflow, scache_ptr, sizeof(txflow));
        scache_ptr += sizeof(txflow);

        /* Validate flow index */
        if (txflow.flow_idx < 0 ||
            txflow.flow_idx >= bkinfo->max_flows[SR_FLOW_DIR_TX]) {
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Validate priority map and get number of flows inside this flowset */
        num_flows = 1;
        if (txflow.map_id != BCM_TSN_PRI_MAP_INVALID) {
            /* call API to get priority map configuration */
            rv = bcm_esw_tsn_pri_map_get(unit, txflow.map_id, &pmcfg);
            if (BCM_FAILURE(rv) || pmcfg.map_type != bcmTsnPriMapTypeSrFlow) {
                SR_FLOW_MGMT_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            /* Number of flows is max flow offset + 1 */
            for (j = 0; j < pmcfg.num_map_entries; j++) {
                if ((pmcfg.map_entries[j].flags &
                     BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) &&
                    (pmcfg.map_entries[j].flow_offset > num_flows - 1)) {
                    num_flows = pmcfg.map_entries[j].flow_offset + 1;
                }
            }
        }

        /* Restore flow index entries */
        rv = bcmi_esw_tsn_sr_flow_idx_restore(
                unit, SR_FLOW_DIR_TX, txflow.flow_idx, num_flows, &flow_ide);
        if (BCM_FAILURE(rv)) {
            SR_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Confirmed that this flowset can be restored */
        fset = &bkinfo->flowsets[SR_FLOW_DIR_TX][txflow.flow_idx];
        SR_FLOWSET_LOCK(fset);

        /* Restore the flowset */
        fset->flow_ide = flow_ide;
        fset->map_id = txflow.map_id;
        fset->num_flows = num_flows;
        fset->ref_count = 0;
        sal_memcpy(&fset->config.tx, &txflow.config, sizeof(txflow.config));

        /* Restore the individual flows */
        for (j = 0; j < num_flows; j++) {
            idx = txflow.flow_idx + j;
            SR_FLOW_TX_LOCK(unit, idx);
            rv = SR_FLOW_MGMT_DEVINFO(unit)->read_tx_flow(
                    unit, idx, &bkinfo->flows_tx[idx]);
            if (BCM_FAILURE(rv)) {
                SR_FLOW_TX_UNLOCK(unit, idx);
                SR_FLOWSET_UNLOCK(fset);
                SR_FLOW_MGMT_UNLOCK(unit);
                return rv;
            }
            SR_FLOW_TX_UNLOCK(unit, idx);
        }

        /* Done restoring this flowset */
        SR_FLOWSET_UNLOCK(fset);
    }

    /* Done */
    SR_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* SR flow management de-initialization */
STATIC int
bcmi_esw_tsn_sr_flow_mgmt_detach(int unit)
{
    tsn_sr_flows_bookkeeping_t *bkinfo;
    int dir, i;

    /* parameter check */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* check if it's already cleared */
    bkinfo = &tsn_bk_info[unit]->sr_flows;
    if (NULL == bkinfo) {
        return BCM_E_NONE;
    }

    /* de-allocate flow index pool */
    if (NULL != bkinfo->flowids_allocated) {
        sal_free(bkinfo->flowids_allocated);
        bkinfo->flowids_allocated = NULL;
    }

    /* de-allocate flow instances */
    if (NULL != bkinfo->flows_rx) {
        sal_free(bkinfo->flows_rx);
        bkinfo->flows_rx = NULL;
    }
    if (NULL != bkinfo->flows_tx) {
        sal_free(bkinfo->flows_tx);
        bkinfo->flows_tx = NULL;
    }

    /* de-allocate flow mutexes */
    if (NULL != bkinfo->flows_rx_mutex) {
        for (i = 0; i < bkinfo->max_flows[SR_FLOW_DIR_RX]; i++) {
            if (bkinfo->flows_rx_mutex[i] != NULL) {
                sal_mutex_destroy(bkinfo->flows_rx_mutex[i]);
                bkinfo->flows_rx_mutex[i] = NULL;
            }
        }
        sal_free(bkinfo->flows_rx_mutex);
        bkinfo->flows_rx_mutex = NULL;
    }
    if (NULL != bkinfo->flows_tx_mutex) {
        for (i = 0; i < bkinfo->max_flows[SR_FLOW_DIR_TX]; i++) {
            if (bkinfo->flows_tx_mutex[i] != NULL) {
                sal_mutex_destroy(bkinfo->flows_tx_mutex[i]);
                bkinfo->flows_tx_mutex[i] = NULL;
            }
        }
        sal_free(bkinfo->flows_tx_mutex);
        bkinfo->flows_tx_mutex = NULL;
    }

    /* De-allocate per direction */
    for (dir = SR_FLOW_DIR_RX; dir < SR_FLOW_DIR_COUNT; dir++) {

        /* clear flow index linked list */
        bkinfo->flowids_avail[dir] = NULL;

        /* de-allocate flowset instances */
        if (NULL != bkinfo->flowsets[dir]) {
            for (i = 0; i < bkinfo->max_flows[dir]; i++) {
                if (NULL != bkinfo->flowsets[dir][i].mutex) {
                    sal_mutex_destroy(bkinfo->flowsets[dir][i].mutex);
                    bkinfo->flowsets[dir][i].mutex = NULL;
                }
            }
            sal_free(bkinfo->flowsets[dir]);
            bkinfo->flowsets[dir] = NULL;
        }
    }

    /* destroy the flow management mutex */
    if (NULL != bkinfo->fmgmt_mutex) {
        sal_mutex_destroy(bkinfo->fmgmt_mutex);
        bkinfo->fmgmt_mutex = NULL;
    }

    return BCM_E_NONE;
}

/* SR flow management initialization */
STATIC int
bcmi_esw_tsn_sr_flow_mgmt_init(int unit)
{
    tsn_sr_flows_bookkeeping_t *bkinfo;
    tsn_sr_flowidx_entry_t *fid;
    int rxflows, txflows;
    int rv;
    int d, i;

    /* parameter check */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Retrieve number of TX/RX flows */
    bkinfo = &tsn_bk_info[unit]->sr_flows;
    if (SR_FLOW_MGMT_DEVINFO(unit) == NULL ||
        SR_FLOW_MGMT_DEVINFO(unit)->get_num_flows == NULL) {
        return BCM_E_UNAVAIL;
    }
    rv = SR_FLOW_MGMT_DEVINFO(unit)->get_num_flows(unit, &rxflows, &txflows);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (rxflows < 0) {
        rxflows = 0; /* Reserve negative value for future extension */
    }
    if (txflows < 0) {
        txflows = 0; /* Reserve negative value for future extension */
    }
    if (rxflows + txflows == 0) {
        /* No supported flows */
        return BCM_E_UNAVAIL;
    }
    bkinfo->max_flows[SR_FLOW_DIR_RX] = rxflows;
    bkinfo->max_flows[SR_FLOW_DIR_TX] = txflows;

    /*
     * We need to minimize the time taken for picking a free flow index.
     * To achieve this, we use linked list with pre-allocated entries so that
     * we can do it in O(1).
     */

    /* Allocate flow index entry pool (one pool for TX and RX) */
    bkinfo->flowids_allocated = (tsn_sr_flowidx_entry_t *)sal_alloc(
            sizeof(tsn_sr_flowidx_entry_t) * (rxflows + txflows),
            "tsn sr flow ids");
    if (NULL == bkinfo->flowids_allocated) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN SR: failed to allocate flow IDs\n")));
        (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }

    /* For RX and TX directions */
    fid = bkinfo->flowids_allocated;
    for (d = SR_FLOW_DIR_RX; d < SR_FLOW_DIR_COUNT; d++) {

        /* Prepare flow index entry linked list for quick ID allocation */
        for (i = 0; i < bkinfo->max_flows[d]; i++, fid++) {
            /*
             * Add an entry to the head of the linked list.
             * The flow index is going backward so the final list can be in
             * increasing order (some allocation requires continuous flows).
             */
            fid->flow_idx = bkinfo->max_flows[d] - i - 1;
            fid->next = bkinfo->flowids_avail[d];
            bkinfo->flowids_avail[d] = fid;
        }

        /* Allocate array of RX/TX flowset instances */
        if (bkinfo->max_flows[d] > 0) {
            /* The max number of flowsets should be the same as flows */
            bkinfo->flowsets[d] = (tsn_sr_flowset_t *)sal_alloc(
                sizeof(tsn_sr_flowset_t) * bkinfo->max_flows[d],
                "tsn sr rx flows");
            if (NULL == bkinfo->flowsets[d]) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META("TSN SR: failed to allocate flowsets\n")));
                (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(bkinfo->flowsets[d], 0,
                sizeof(tsn_sr_flowset_t) * bkinfo->max_flows[d]);
            /* Pre-create mutexes of all flowsets for performance */
            for (i = 0; i < bkinfo->max_flows[d]; i++) {
                bkinfo->flowsets[d][i].mutex =
                    sal_mutex_create("tsn sr flowset");
                if (NULL == bkinfo->flowsets[d][i].mutex) {
                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META(
                                 "TSN SR: failed to create flowset mutex\n")));
                    (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
                    return BCM_E_MEMORY;
                }
            }
        }
    }

    /* Allocate array of RX/TX flow instances and mutexes */
    if (rxflows > 0) {
        bkinfo->flows_rx = (bcmi_tsn_sr_rx_flow_t *)sal_alloc(
            sizeof(bcmi_tsn_sr_rx_flow_t) * rxflows, "tsn sr rx flows");
        if (NULL == bkinfo->flows_rx) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("TSN SR: failed to allocate RX flows\n")));
            (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->flows_rx, 0,
            sizeof(bcmi_tsn_sr_rx_flow_t) * rxflows);
        bkinfo->flows_rx_mutex = (sal_mutex_t *)sal_alloc(
            sizeof(sal_mutex_t) * rxflows, "tsn sr rx flows mutex");
        if (NULL == bkinfo->flows_rx_mutex) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META(
                          "TSN SR: failed to allocate RX flow mutexes\n")));
            (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        for (i = 0; i < rxflows; i++) {
            bkinfo->flows_rx_mutex[i] = sal_mutex_create("tsn sr flow mutex");
            if (NULL == bkinfo->flows_rx_mutex[i]) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META(
                              "TSN SR: failed to create flow mutex\n")));
                (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
                return BCM_E_MEMORY;
            }
        }
    }
    if (txflows > 0) {
        bkinfo->flows_tx = (bcmi_tsn_sr_tx_flow_t *)sal_alloc(
            sizeof(bcmi_tsn_sr_tx_flow_t) * txflows, "tsn sr tx flows");
        if (NULL == bkinfo->flows_tx) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("TSN SR: failed to allocate TX flows\n")));
            (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->flows_tx, 0,
            sizeof(bcmi_tsn_sr_tx_flow_t) * txflows);
        bkinfo->flows_tx_mutex = (sal_mutex_t *)sal_alloc(
            sizeof(sal_mutex_t) * txflows, "tsn sr tx flows mutex");
        if (NULL == bkinfo->flows_tx_mutex) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META(
                          "TSN SR: failed to allocate TX flow mutexes\n")));
            (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        for (i = 0; i < txflows; i++) {
            bkinfo->flows_tx_mutex[i] = sal_mutex_create("tsn sr flow mutex");
            if (NULL == bkinfo->flows_tx_mutex[i]) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META("TSN SR: failed to create flow mutex\n")));
                (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
                return BCM_E_MEMORY;
            }
        }
    }

    /* Create mutex for protecting flow management (allocate/free) */
    bkinfo->fmgmt_mutex = sal_mutex_create("tsn sr flow mgmt");
    if (NULL == bkinfo->fmgmt_mutex) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN SR: failed to create flow mgmt mutex\n")));
        (void)bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

 /*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_config_dump
 * Purpose:
 *   dump content of bcm_tsn_sr_auto_learn_config_t
 * Parameters:
 *   unit   - (IN) Unit number
 *   prefix - (IN) Character string to prefix output lines
 *   config - (IN) Auto Learn configuration
 * Returns:
 *   none
 */
STATIC void
bcmi_esw_tsn_sr_auto_learn_config_dump(
    int unit,
    char *prefix,
    bcm_tsn_sr_auto_learn_config_t *config)
{
    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == prefix) || (NULL == config)) {
        return;
    }

    if (bsl_check(bslLayerBcm, bslSourceTsn, bslSeverityDebug, unit)) {
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "bcm_tsn_sr_auto_learn_config_t:\n")));
        /* num_tx_flows & num_rx_flows */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "%s num_tx_flows %d num_rx_flows %d.\n"),
                              prefix,
                              config->num_tx_flows,
                              config->num_rx_flows));

        /* tx_flow_config */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "%s tx_flow_config:"
                              "flags 0x%x, seq_num %d, "
                              "ingress_mtu_profile %d.\n"),
                              prefix,
                              config->tx_flow_config.flags,
                              config->tx_flow_config.seq_num,
                              config->tx_flow_config.ingress_mtu_profile));
        /* rx_flow_config */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "%s rx_flow_config:"
                              "flags 0x%x, seqnum_accept_win_size %d, "
                              "seqnum_history_win_size %d, age_timeout %d, "
                              "ingress_mtu_profile %d\n"),
                              prefix,
                              config->rx_flow_config.flags,
                              config->rx_flow_config.seqnum_accept_win_size,
                              config->rx_flow_config.seqnum_history_win_size,
                              config->rx_flow_config.age_timeout,
                              config->rx_flow_config.ingress_mtu_profile));
    }

    TSN_FUNCTION_TRACE(unit, "OUT");
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_config_dump
 * Purpose:
 *   dump content of bcm_tsn_sr_auto_learn_group_config_t
 * Parameters:
 *   unit   - (IN) Unit number
 *   prefix - (IN) Character string to prefix output lines
 *   config - (IN) Auto Learn Group configuration
 * Returns:
 *   none
 */
STATIC void
bcmi_esw_tsn_sr_auto_learn_group_config_dump(
    int unit,
    char *prefix,
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
    bcm_port_t port;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == prefix) || (NULL == config)) {
        assert((prefix) && (config));
        return;
    }

    if (bsl_check(bslLayerBcm, bslSourceTsn, bslSeverityDebug, unit)) {
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "bcm_tsn_sr_auto_learn_group_config_t:\n")));

        /* flags */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "%s flags 0x%x\n"),
                              prefix,
                              config->flags));

        /* interlink_ports */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                             "%s interlink_ports:"),
                             prefix));

        PBMP_ITER(config->interlink_ports, port) {
            LOG_DEBUG(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "%2d "), port));
        }

        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "\n")));

        /* redundant_ports */
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "%s redundant_ports:"),
                              prefix));

        PBMP_ITER(config->redundant_ports, port) {
            LOG_DEBUG(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "%2d "), port));

        }

        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "\n")));
    }
    TSN_FUNCTION_TRACE(unit, "OUT");
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_config_cmp
 * Purpose:
 *   Check the Auto Learn entry is the same or not.
 * Parameters:
 *   unit           - (IN) Unit number
 *   config1        - (IN) Auto Learn Group config
 *   config2        - (IN) Auto Learn Group config
 * Returns:
 *   BCM_E_NONE     - not the same and no overlapped ports found
 *   BCM_E_PARAM    - NULL parameter
 *   BCM_E_EXISTS   - both config are the same
 *   BCM_E_BUSY     - not the same but overlapped ports found
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_config_cmp(
    int unit,
    bcm_tsn_sr_auto_learn_group_config_t *config1,
    bcm_tsn_sr_auto_learn_group_config_t *config2)
{
    int is_the_same = TRUE;
    bcm_pbmp_t tmp_pbmp;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == config1) || (NULL == config2)) {
        assert((config1) && ( config2));
        return BCM_E_PARAM;
    }

    /* Check if flags are different */
    if (config1->flags != config2->flags) {
        is_the_same = FALSE;
    }

    /* Check if any different found */
    if (BCM_PBMP_NEQ(config1->redundant_ports,
                     config2->redundant_ports)) {
        is_the_same = FALSE;
    }

    /* Check if any different found */
    if (BCM_PBMP_NEQ(config1->interlink_ports,
                     config2->interlink_ports)) {
        is_the_same = FALSE;
    }

    if (FALSE == is_the_same) {
        BCM_PBMP_ASSIGN(tmp_pbmp, config1->interlink_ports);
        BCM_PBMP_AND(tmp_pbmp, config2->interlink_ports);
        /* Check for overlapped ports found, return BCM_E_BUSY */
        if (BCM_PBMP_NOT_NULL(tmp_pbmp)) {
            return BCM_E_BUSY;
        }

        BCM_PBMP_ASSIGN(tmp_pbmp, config1->redundant_ports);
        BCM_PBMP_AND(tmp_pbmp, config2->redundant_ports);
        /* Check for overlapped ports found, return BCM_E_BUSY */
        if (BCM_PBMP_NOT_NULL(tmp_pbmp)) {
            return BCM_E_BUSY;
        }

        /* Not the same and no overlapped */
        return BCM_E_NONE;
    }

    TSN_FUNCTION_TRACE(unit, "OUT");
    /* Both config are the same, return BCM_E_EXISTS */
    return BCM_E_EXISTS;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_id_existed_check
 * Purpose:
 *   Check the Auto Learn Group ID exited or not.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 * Returns:
 *   BCM_E_NONE - Success.
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_id_existed_check(
    int unit,
    int group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    uint32 sw_idx = GROUP_ID_TO_SW_IDX(group_id);
    uint32 table_size;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);

    /* Check if the map_id and sw_idx is in the reasonable range */
    if (sw_idx >= table_size) {
        return BCM_E_PARAM;
    }

    if (AUTO_LEARN_INTF_USED_GET(unit, sw_idx)) {
        return BCM_E_NONE;
    }
    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_existed_check
 * Purpose:
 *   Check the Auto Learn Group configuration exited or not.
 * Parameters:
 *   unit           - (IN) Unit number
 *   config         - (IN) Current Group ID
 *   config         - (IN) Auto Learn Group configuration
 *   group_id       - (OUT) Group ID
 * Returns:
 *   BCM_E_NONE     - Success. Not the same and no overlapped ports found
 *   BCM_E_PARAM    - Fail. NULL parameter
 *   BCM_E_EXISTS   - Fail. Both configures are the same
 *   BCM_E_BUSY     - Fail. Not the same but overlapped ports found
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_existed_check(
    int unit,
    bcm_tsn_sr_auto_learn_group_config_t *config,
    int current_group_id,
    int *group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    bcm_tsn_sr_auto_learn_group_config_t *bk_config;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    uint32 sw_idx;
    uint32 table_size;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == config) || (NULL == group_id)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);
    assert(table_size > al_bk_info->valid_sw_idx_end);

    AUTO_LEARN_BK_LOCK(unit);

    /* Check if there is any the same configure in bk */
    for (sw_idx = (al_bk_info->valid_sw_idx_start);
         sw_idx <= (al_bk_info->valid_sw_idx_end);
         sw_idx++) {

        /* Not used, continue to check next one */
        if (AUTO_LEARN_INTF_USED_GET(unit, sw_idx) == 0) {
            continue;
        }

        bk_group_data = &(al_bk_info->group_data[sw_idx]);
        bk_config = &(bk_group_data->config);

        /* Compare if two configures are the same */
        rv = bcmi_esw_tsn_sr_auto_learn_group_config_cmp(
                unit, bk_config, config);
        if (BCM_SUCCESS(rv)) {
            /* Not the same and no overlapped port found */
            continue;
        } else if (BCM_E_EXISTS == rv) {
            /* The same configure found */
            *group_id = bk_group_data->group_id;
            AUTO_LEARN_BK_UNLOCK(unit);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            /* Overlapped ports found and Error handle for other */

            /* Change the config via set function, skip check itself */
            if (current_group_id == bk_group_data->group_id) {
                continue;
            }
            AUTO_LEARN_BK_UNLOCK(unit);
            return rv;
        }
    }
    AUTO_LEARN_BK_UNLOCK(unit);

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}


/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_get_empty_group_id
 * Purpose:
 *   Get a empty group_id
 * Parameters:
 *   unit       - (IN) Unit number
 *   group_id   - (OUT) Group ID
 * Returns:
 *   BCM_E_NONE - Success. Find a empty groud_id successfully
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_get_empty_group_id(
    int unit,
    int *group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    uint32 empty_sw_idx;
    uint32 table_size;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if (NULL == group_id) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);

    /* Full check */
    if (al_bk_info->valid_count == table_size) {
        return BCM_E_NOT_FOUND;
    }

    /* Check if it's in the reasonable range */
    if (al_bk_info->empty_sw_idx_end >= table_size) {
        return BCM_E_INTERNAL;
    }

    AUTO_LEARN_BK_LOCK(unit);

    /* Find an empty id */
    for (empty_sw_idx = al_bk_info->empty_sw_idx_start;
         empty_sw_idx <= al_bk_info->empty_sw_idx_end;
         empty_sw_idx++) {

        if (AUTO_LEARN_INTF_USED_GET(unit, empty_sw_idx) == 0) {
            /* Set bit map as used */
            AUTO_LEARN_INTF_USED_SET(unit, empty_sw_idx);

            /* Update valid start and end */
            if (al_bk_info->valid_count == 0) {
                al_bk_info->valid_sw_idx_start = empty_sw_idx;
                al_bk_info->valid_sw_idx_end = empty_sw_idx;
            } else if (empty_sw_idx < al_bk_info->valid_sw_idx_start) {
                al_bk_info->valid_sw_idx_start = empty_sw_idx;
            } else if (empty_sw_idx > al_bk_info->valid_sw_idx_end) {
                al_bk_info->valid_sw_idx_end = empty_sw_idx;
            }

            al_bk_info->valid_count++;
            *group_id = SW_IDX_TO_GROUP_ID(empty_sw_idx);

            /* Update the empty start, assume next one is real empty */
            al_bk_info->empty_sw_idx_start = empty_sw_idx + 1;
            AUTO_LEARN_BK_UNLOCK(unit);
            return BCM_E_NONE;
        }
        else {

            /* Update the empty start, assume next one is real empty */
            al_bk_info->empty_sw_idx_start = empty_sw_idx + 1;
        }
    }
    AUTO_LEARN_BK_UNLOCK(unit);
    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_destroy_group_id
 * Purpose:
 *   Free the resource of group_id
 * Parameters:
 *   unit       - (IN) Unit number
 *   group_id   - (IN) Group ID
 * Returns:
 *   BCM_E_NONE - Success. delete groud_id successfully
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_destroy_group_id(
    int unit,
    int group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    uint32 destroy_sw_idx = GROUP_ID_TO_SW_IDX(group_id);
    uint32 next_valid_sw_idx;
    uint32 table_size;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);

    /* Check if it's in the reasonable range */
    if (destroy_sw_idx >= table_size) {
        return BCM_E_INTERNAL;
    }
    AUTO_LEARN_BK_LOCK(unit);

    /* Destroy bitmap */
    AUTO_LEARN_INTF_USED_CLR(unit, destroy_sw_idx);

    /* Update empty start and end */
    if (destroy_sw_idx < (al_bk_info->empty_sw_idx_start)) {
        al_bk_info->empty_sw_idx_start = destroy_sw_idx;
    } else if (destroy_sw_idx > (al_bk_info->empty_sw_idx_end)) {
        al_bk_info->empty_sw_idx_end = destroy_sw_idx;
    }

    /* Update valid, find a next valid start or next valid end */
    next_valid_sw_idx = destroy_sw_idx;
    if ((al_bk_info->valid_count) > 1) {
        if (next_valid_sw_idx == (al_bk_info->valid_sw_idx_start)) {
            do {
                next_valid_sw_idx++;
                if (next_valid_sw_idx == table_size - 1) {
                    /* table_size -1 is the last one */
                    break;
                }
            } while (AUTO_LEARN_INTF_USED_GET(unit, next_valid_sw_idx) == 0);
            al_bk_info->valid_sw_idx_start = next_valid_sw_idx;
        } else if (next_valid_sw_idx == (al_bk_info->valid_sw_idx_end)) {
            do {
                next_valid_sw_idx--;
                if (next_valid_sw_idx == 0) {
                    /* 0 is the first one */
                    break;
                }
            } while (AUTO_LEARN_INTF_USED_GET(unit, next_valid_sw_idx) == 0);
            al_bk_info->valid_sw_idx_end = next_valid_sw_idx;
        }
    }

    if (0 < al_bk_info->valid_count) {
        al_bk_info->valid_count--;
    }

    if (0 == al_bk_info->valid_count) {
        al_bk_info->valid_sw_idx_start = 0;
        al_bk_info->valid_sw_idx_end = 0;
        al_bk_info->empty_sw_idx_start = 0;
        al_bk_info->empty_sw_idx_end = table_size - 1;
    }
    AUTO_LEARN_BK_UNLOCK(unit);

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_create_sr_multicast
 * Purpose:
 *   Create Multicast group for SR ports
 * Parameters:
 *   unit       - (IN) Unit
 *   port_pbmp  - (IN) port bitmap
 *   mcg_idx    - (OUT) multicast group index
 * Returns:
 *   BCM_E_NONE - Success.
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_create_sr_multicast(
    int unit,
    bcm_pbmp_t port_pbmp,
    int *mcg_idx)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcm_if_t encap_id;
    bcm_port_t port;
    bcm_gport_t gport;
    bcm_vlan_t vid = BCM_VLAN_INVALID;
    bcm_multicast_t mc_group;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if (NULL == mcg_idx) {
        return BCM_E_PARAM;
    }

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    rv = bcm_esw_multicast_create(unit, BCM_MULTICAST_TYPE_L2, &mc_group);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "bcm_multicast_create failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    PBMP_ITER(port_pbmp, port) {
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "port: %d, "), port));
        /* convert port to gport */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));

        /* Add HSR ports into the multicast group */
        rv = bcm_esw_multicast_l2_encap_get(unit,
                                            mc_group,
                                            gport,
                                            vid,
                                            &encap_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "bcm_esw_multicast_l2_encap_get"
                                  " failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        rv = bcm_esw_multicast_egress_add(unit,
                                          mc_group,
                                          gport,
                                          encap_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "bcm_esw_multicast_egress_add"
                                  " failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "mc_group(%d), port(%2d), "
                              "vlan(%d), encap_id(%d)\n"),
                              mc_group,
                              port,
                              vid,
                              encap_id));
    }
    *mcg_idx = _BCM_MULTICAST_ID_GET(mc_group);

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_delete_sr_multicast
 * Purpose:
 *   Delete Multicast group for SR ports
 * Parameters:
 *   unit       - (IN) Unit being delete.
 *   port_pbmp  - (IN) port bitmap being delete.
 *   mcg_idx   - (IN) multicast group index
 * Returns:
 *   BCM_E_NONE - Success.
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_delete_sr_multicast(
    int unit,
    bcm_pbmp_t port_pbmp,
    int mcg_idx)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcm_if_t encap_id;
    bcm_port_t port;
    bcm_gport_t gport;
    bcm_vlan_t vid = BCM_VLAN_INVALID;
    bcm_multicast_t mc_group;

    TSN_FUNCTION_TRACE(unit, "IN");

    _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_L2, mcg_idx);

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    PBMP_ITER(port_pbmp, port) {
        /* convert port to gport */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));

        /* Add HSR ports into the multicast group */
        rv = bcm_esw_multicast_l2_encap_get(unit,
                                            mc_group,
                                            gport,
                                            vid,
                                            &encap_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "bcm_multicast_l2_encap_get"
                                  " failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        rv = bcm_esw_multicast_egress_delete(unit,
                                             mc_group,
                                             gport,
                                             encap_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "bcm_esw_multicast_egress_delete"
                                  " failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "mc_group(%d), port(%2d), "
                              "vlan(%d), encap_id(%d)"),
                              mc_group,
                              port,
                              vid,
                              encap_id));
    }

    rv = bcm_esw_multicast_destroy(unit, mc_group);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "bcm_esw_multicast_destroy"
                              " mc_group(%d), failed(rv = %d)\n"),
                              mc_group,
                              rv));
        return rv;
    }
    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_hw_delete_all
 * Purpose:
 *   Delete all HW resource. Back to default status.
 * Parameters:
 *   unit       - (IN) Unit being delete.
 * Returns:
 *   BCM_E_NONE - Success.
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_hw_delete_all(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->group_port_reset_all);

    /* Reset all ports */
    rv = al_dev_info->group_port_reset_all(unit);
    BCM_IF_ERROR_RETURN(rv);
    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

STATIC int bcmi_esw_tsn_sr_auto_learn_lazy_init(int unit);
STATIC int bcmi_esw_tsn_sr_auto_learn_cleanup(int unit);

/* free the allocate group_id/l2mc entry & mac proxy entry */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
    int unit,
    int *group_id,
    int i_pro_idx,
    int r_pro_idx[bcmi_sr_auto_learn_group_count],
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Parameter NULL error handling */
    if (NULL == config) {
        return BCM_E_PARAM;
    }
    if (NULL != group_id) {
        /* Need to free the empty group_id from above */
        bcmi_esw_tsn_sr_auto_learn_destroy_group_id(unit, *group_id);
    }

    if (BCM_PBMP_NOT_NULL(config->redundant_ports)) {
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            r_pro_idx[bcmi_sr_auto_learn_group_l2mc]) {
            rv = bcmi_esw_tsn_sr_auto_learn_delete_sr_multicast(
                    unit,
                    config->redundant_ports,
                    r_pro_idx[bcmi_sr_auto_learn_group_l2mc]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "L2MC delete failed(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            r_pro_idx[bcmi_sr_auto_learn_group_macp]) {
            rv = bcmi_esw_tsn_sr_mac_proxy_delete(
                    unit,
                    r_pro_idx[bcmi_sr_auto_learn_group_macp]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "sr mac proxy profile delete failed"
                                      "(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
    }

    if (BCM_PBMP_NOT_NULL(config->interlink_ports)) {
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            i_pro_idx) {
            rv = bcmi_esw_tsn_sr_mac_proxy_delete(
                    unit,
                    i_pro_idx);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "sr mac proxy profile delete failed"
                                      "(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_create
 * Purpose:
 *   Add an Auto Learn config to hardware.
 * Parameters:
 *   unit       - (IN) Unit number
 *   config     - (IN) Auto Learn Group configuration
 *   group_id   - (OUT) Group ID
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_create(
    int unit,
    bcm_tsn_sr_auto_learn_group_config_t *config,
    int *group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    bcm_tsn_sr_auto_learn_group_config_t *bk_config;
    int pro_idx = BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    bcmi_tsn_sr_auto_learn_group_port_type_t group_ports_type;
    bcm_pbmp_t group_ports_pbmp;
    int new_i_pro_idx = BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    int new_r_pro_idx[bcmi_sr_auto_learn_group_count] =
        { BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID,
          BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID};

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == config) || (NULL == group_id) ||
        ((BCM_PBMP_IS_NULL(config->redundant_ports)) &&
         (BCM_PBMP_IS_NULL(config->interlink_ports))) ||
        ((0 != config->flags) &&
         ((~BCMI_TSN_SR_AUTO_LEARN_GROUP_VALID_FLAGS) & config->flags))) {
        return BCM_E_PARAM;
    }

    bcmi_esw_tsn_sr_auto_learn_group_config_dump(
        unit, "auto_learn_group_create", config);

    /* SR Auto Learn management initialization */
    rv = bcmi_esw_tsn_sr_auto_learn_lazy_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_sr_auto_learn_cleanup(unit);
        return (rv);
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->group_port_set);

    AUTO_LEARN_LOCK(unit);

    /* Check if the same configure and overlapped ports existed */
    rv = bcmi_esw_tsn_sr_auto_learn_group_existed_check(unit,
            config, BCMI_SR_AUTO_LEARN_GROUP_ID_INVALID, group_id);
    if (BCM_E_EXISTS == rv) {
        /* config is the same as config of existing group_id */
        AUTO_LEARN_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "config already existed, found group_id(%d)\n"),
                             *group_id));
        return BCM_E_EXISTS;
    } else if (BCM_E_BUSY == rv) {
        /* config is found on other group_id (overlapper ports) */
        AUTO_LEARN_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "find existed auto learn group failed"
                             "(rv = %d)\n"),
                             rv));
        return rv;
    } else if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        return rv;
    }

    /* Get a SW empty group_id */
    rv = bcmi_esw_tsn_sr_auto_learn_get_empty_group_id(unit, group_id);
    if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "no empty group_id(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Set SW bookkeeping database pointers */
    sw_idx = GROUP_ID_TO_SW_IDX(*group_id);
    bk_group_data = &(al_bk_info->group_data[sw_idx]);
    bk_config = &(bk_group_data->config);

    /* Check if overlapped */
    BCM_PBMP_ASSIGN(group_ports_pbmp, config->redundant_ports);
    BCM_PBMP_AND(group_ports_pbmp, config->interlink_ports);
    if (BCM_PBMP_NOT_NULL(group_ports_pbmp)) {
        (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
        return BCM_E_PARAM;
    }
    /*
     * Create auto learn group for redundant and interlink
     * 1. auto learn group bitmap with redundant ports
     * 2. auto learn group proxy bitmap with interlink & redundant ports
     */

    /* redundant ports */
    if (BCM_PBMP_NOT_NULL(config->redundant_ports)) {
        /* L2MC */
        group_ports_type = bcmi_sr_auto_learn_group_l2mc;
        BCM_PBMP_ASSIGN(group_ports_pbmp, config->redundant_ports);

        /* Set new auto learn group (config) */
        rv = bcmi_esw_tsn_sr_auto_learn_create_sr_multicast(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "L2MC create failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
        new_r_pro_idx[group_ports_type] = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }


        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;

        /* Set new auto learn group (config) */
        rv = bcmi_esw_tsn_sr_mac_proxy_insert(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "sr mac proxy profile insert failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
        new_r_pro_idx[group_ports_type] = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

    }

    /* interlink ports */
    if (BCM_PBMP_NOT_NULL(config->interlink_ports)) {
        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;
        BCM_PBMP_ASSIGN(group_ports_pbmp, config->interlink_ports);

        /* Set new auto learn group (config) */
        rv = bcmi_esw_tsn_sr_mac_proxy_insert(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "sr mac proxy profile insert failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
        new_i_pro_idx = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, group_id, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

    }

    /* Update SW bookkeeping database */
    AUTO_LEARN_BK_LOCK(unit);
    bk_group_data->interlink_pro_idx = new_i_pro_idx;
    bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_l2mc] =
        new_r_pro_idx[bcmi_sr_auto_learn_group_l2mc];
    bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_macp] =
        new_r_pro_idx[bcmi_sr_auto_learn_group_macp];
    bk_group_data->group_id = *group_id;
    sal_memcpy(bk_config,
               config,
               sizeof(bcm_tsn_sr_auto_learn_group_config_t));
    AUTO_LEARN_BK_UNLOCK(unit);

    AUTO_LEARN_UNLOCK(unit);

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_set
 * Purpose:
 *   Set Auto Learn config with the given group_id.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 *   config   - (IN) Auto Learn Group configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_set(
    int unit,
    int group_id,
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 sw_idx;
    int exisited_group_id;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    bcm_tsn_sr_auto_learn_group_config_t *bk_config;
    int pro_idx = BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    int new_i_pro_idx = BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    int new_r_pro_idx[bcmi_sr_auto_learn_group_count] =
        { BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID,
          BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID};
    bcmi_tsn_sr_auto_learn_group_port_type_t group_ports_type;
    bcm_pbmp_t group_ports_pbmp;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == config) ||
        (BCM_PBMP_IS_NULL(config->redundant_ports)) ||
        (BCM_PBMP_IS_NULL(config->interlink_ports)) ||
        ((0 != config->flags) &&
         ((~BCMI_TSN_SR_AUTO_LEARN_GROUP_VALID_FLAGS) & config->flags))) {
        return BCM_E_PARAM;
    }

    bcmi_esw_tsn_sr_auto_learn_group_config_dump(
        unit, "auto_learn_group_set", config);

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->group_port_delete);
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->group_port_set);

    AUTO_LEARN_LOCK(unit);

    /* Check if group id existed */
    rv = bcmi_esw_tsn_sr_auto_learn_group_id_existed_check(unit, group_id);
    if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "group_id(%d) doesn't exited\n"), group_id));
        return rv;
    }

    /* Check if the same configure existed or not */
    rv = bcmi_esw_tsn_sr_auto_learn_group_existed_check(unit,
            config, group_id, &exisited_group_id);
    if (BCM_E_EXISTS == rv) {
        if (exisited_group_id == group_id) {
            /* config is the same as config of current group_id */
            AUTO_LEARN_UNLOCK(unit);
            LOG_WARN(BSL_LS_BCM_TSN,
                     (BSL_META_U(unit,
                                 "config is the same as the config "
                                 "of current group_id(%d)\n"),
                                 group_id));
            /* config are the same, do nothing */
            return BCM_E_NONE;
        } else {
            /* the config is found on other group_id */
            AUTO_LEARN_UNLOCK(unit);
            LOG_WARN(BSL_LS_BCM_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, "
                                 "found group_id(%d)\n"),
                                 exisited_group_id));
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_BUSY == rv) {
        /* config is found on other group_id (overlapper ports) */
        AUTO_LEARN_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "find existed auto learn group failed"
                             "(rv = %d)\n"),
                             rv));
        return rv;
    } else if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        return rv;
    }

    /* Check if overlapped */
    BCM_PBMP_ASSIGN(group_ports_pbmp, config->redundant_ports);
    BCM_PBMP_AND(group_ports_pbmp, config->interlink_ports);
    if (BCM_PBMP_NOT_NULL(group_ports_pbmp)) {
        return BCM_E_PARAM;
    }

    /* Set SW bookkeeping database pointers */
    sw_idx = GROUP_ID_TO_SW_IDX(group_id);
    bk_group_data = &(al_bk_info->group_data[sw_idx]);
    bk_config = &(bk_group_data->config);

    /*
     * Delete bk auto learn group and then set new one
     * 1. auto learn group bitmap with redundant ports
     * 2. auto learn group proxy bitmap with interlink & redundant ports
     */

    /* Only add when new config is different from bookkeeping's */
    if (BCM_PBMP_NOT_NULL(bk_config->redundant_ports) &&
        BCM_PBMP_NEQ(bk_config->redundant_ports, config->redundant_ports)) {
        /* Set new auto learn group (config) */

        /* L2MC */
        group_ports_type = bcmi_sr_auto_learn_group_l2mc;
        BCM_PBMP_ASSIGN(group_ports_pbmp, config->redundant_ports);
        rv = bcmi_esw_tsn_sr_auto_learn_create_sr_multicast(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "L2MC create failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
        new_r_pro_idx[group_ports_type] = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, NULL, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }


        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;

        /* Set new auto learn group (config) */
        rv = bcmi_esw_tsn_sr_mac_proxy_insert(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, NULL, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "sr mac proxy profile insert failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }

        new_r_pro_idx[group_ports_type] = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, NULL, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* Only add when new config is different from bookkeeping's */
    if (BCM_PBMP_NOT_NULL(bk_config->interlink_ports) &&
        BCM_PBMP_NEQ(bk_config->interlink_ports, config->interlink_ports)) {
        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;

        /* Set new auto learn group (config) */
        BCM_PBMP_ASSIGN(group_ports_pbmp, config->interlink_ports);
        rv = bcmi_esw_tsn_sr_mac_proxy_insert(
                unit, group_ports_pbmp, &pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, NULL, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "sr mac proxy profile insert failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
        new_i_pro_idx = pro_idx;
        rv = al_dev_info->group_port_set(unit, group_ports_type,
                            group_ports_pbmp, pro_idx);

        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
                    unit, NULL, new_i_pro_idx, new_r_pro_idx, config);
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* delete redundant ports */
    if (BCM_PBMP_NOT_NULL(bk_config->redundant_ports) &&
        BCM_PBMP_NEQ(bk_config->redundant_ports, config->redundant_ports)) {
        /* L2MC */
        group_ports_type = bcmi_sr_auto_learn_group_l2mc;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->redundant_ports);
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            bk_group_data->redundant_pro_idx[group_ports_type]) {

            rv = bcmi_esw_tsn_sr_auto_learn_delete_sr_multicast(
                    unit, group_ports_pbmp,
                    bk_group_data->redundant_pro_idx[group_ports_type]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "L2MC delete failed(rv = %d)\n"),
                                      rv));
            }
        }
        /* Delete HW */

        /* should not delete the ports with new created */
        BCM_PBMP_REMOVE(group_ports_pbmp, config->redundant_ports);
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
        }

        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->redundant_ports);
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            bk_group_data->redundant_pro_idx[group_ports_type]) {
            rv = bcmi_esw_tsn_sr_mac_proxy_delete(
                    unit, bk_group_data->redundant_pro_idx[group_ports_type]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "sr mac proxy profile delete failed"
                                      "(rv = %d)\n"),
                                      rv));
            }
        }
        /* Delete HW */

        /* should not delete the ports overlapped with newly created */
        BCM_PBMP_REMOVE(group_ports_pbmp, config->interlink_ports);
        BCM_PBMP_REMOVE(group_ports_pbmp, config->redundant_ports);
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
        }
    }

    /* delete interlink ports */
    if (BCM_PBMP_NOT_NULL(bk_config->interlink_ports) &&
        BCM_PBMP_NEQ(bk_config->interlink_ports, config->interlink_ports)) {

        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->interlink_ports);
        if (BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID !=
            bk_group_data->redundant_pro_idx[group_ports_type]) {
            rv = bcmi_esw_tsn_sr_mac_proxy_delete(
                    unit, bk_group_data->interlink_pro_idx);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "sr mac proxy profile delete failed"
                                      "(rv = %d)\n"),
                                      rv));
            }
        }
        /* Delete HW */

        /* should not delete the ports overlapped with newly created */
        BCM_PBMP_REMOVE(group_ports_pbmp, config->interlink_ports);
        BCM_PBMP_REMOVE(group_ports_pbmp, config->redundant_ports);
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
        }
    }

    /* Update SW bookkeeping database */
    AUTO_LEARN_BK_LOCK(unit);
    if (BCM_PBMP_NOT_NULL(bk_config->interlink_ports) &&
        BCM_PBMP_NEQ(bk_config->interlink_ports, config->interlink_ports)) {
        bk_group_data->interlink_pro_idx = new_i_pro_idx;
    }
    if (BCM_PBMP_NOT_NULL(bk_config->redundant_ports) &&
        BCM_PBMP_NEQ(bk_config->redundant_ports, config->redundant_ports)) {
        bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_l2mc] =
            new_r_pro_idx[bcmi_sr_auto_learn_group_l2mc];
        bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_macp] =
            new_r_pro_idx[bcmi_sr_auto_learn_group_macp];
    }
    sal_memcpy(bk_config,
               config,
               sizeof(bcm_tsn_sr_auto_learn_group_config_t));
    AUTO_LEARN_BK_UNLOCK(unit);
    AUTO_LEARN_UNLOCK(unit);

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_get
 * Purpose:
 *   Get the Auto Learn config with the given group_id.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 *   config   - (OUT) Auto Learn Group configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_get(
    int unit,
    int group_id,
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    bcm_tsn_sr_auto_learn_group_config_t *bk_config;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    AUTO_LEARN_LOCK(unit);

    /* Check if group id existed */
    rv = bcmi_esw_tsn_sr_auto_learn_group_id_existed_check(unit, group_id);
    if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "group_id(%d) doesn't exited\n"), group_id));
        return rv;
    }

    /* Get the configure from SW bookkeeping database */
    sw_idx = GROUP_ID_TO_SW_IDX(group_id);
    bk_group_data = &(al_bk_info->group_data[sw_idx]);
    bk_config = &(bk_group_data->config);
    sal_memcpy(config,
               bk_config,
               sizeof(bcm_tsn_sr_auto_learn_group_config_t));

    AUTO_LEARN_UNLOCK(unit);

    bcmi_esw_tsn_sr_auto_learn_group_config_dump(
        unit, "auto_learn_group_get", config);

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_destroy
 * Purpose:
 *   Destroy an Auto Learn config with the given group_id by removing it
 *   from hardware and deleting the associated software state info.
 * Parameters:
 *   unit       - (IN) Unit number
 *   group_id   - (IN) Group ID
 * Returns:
 *   BCM_E_NONE - destroy successfully
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_destroy(
    int unit,
    int group_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    bcm_tsn_sr_auto_learn_group_config_t *bk_config;
    bcmi_tsn_sr_auto_learn_group_port_type_t group_ports_type;
    bcm_pbmp_t group_ports_pbmp;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->group_port_delete);

    /* Check if group id existed */
    rv = bcmi_esw_tsn_sr_auto_learn_group_id_existed_check(unit, group_id);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "group_id(%d) doesn't exited\n"), group_id));
        return BCM_E_NOT_FOUND;
    }
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AUTO_LEARN_LOCK(unit);

    /* Delete HW */
    if (BCM_FAILURE(rv)) {
        AUTO_LEARN_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "delete HW failed(rv = %d)\n"),
                              rv));

        return rv;
    }

    /* Set SW bookkeeping database pointers */
    sw_idx = GROUP_ID_TO_SW_IDX(group_id);
    bk_group_data = &(al_bk_info->group_data[sw_idx]);
    bk_config = &(bk_group_data->config);

    /*
     * Delete auto learn group for redundant and interlink
     * 1. auto learn group bitmap with redundant ports
     * 2. auto learn group proxy bitmap with interlink & redundant ports
     */
    AUTO_LEARN_BK_LOCK(unit);
    (void)bcmi_esw_tsn_sr_auto_learn_group_free_pro_idx(
            unit, &group_id,
            bk_group_data->interlink_pro_idx,
            bk_group_data->redundant_pro_idx,
            bk_config);
    AUTO_LEARN_BK_UNLOCK(unit);

    /* redundant ports */
    if (BCM_PBMP_NOT_NULL(bk_config->redundant_ports)) {
        /* L2MC */
        group_ports_type = bcmi_sr_auto_learn_group_l2mc;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->redundant_ports);

        /* Delete HW */
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "L2MC delete failed(rv = %d)\n"),
                                  rv));
            return rv;
        }

        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->redundant_ports);

        /* Delete HW */
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "MAC PROXY delete failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* interlink ports */
    if (BCM_PBMP_NOT_NULL(bk_config->interlink_ports)) {
        /* MAC PROXY */
        group_ports_type = bcmi_sr_auto_learn_group_macp;
        BCM_PBMP_ASSIGN(group_ports_pbmp, bk_config->interlink_ports);

        /* Delete HW */
        rv = al_dev_info->group_port_delete(unit, group_ports_type,
                            group_ports_pbmp);

        if (BCM_FAILURE(rv)) {
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* Clear SW bookkeeping database */
    AUTO_LEARN_BK_LOCK(unit);
    bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_l2mc] =
        BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    bk_group_data->redundant_pro_idx[bcmi_sr_auto_learn_group_macp] =
        BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    bk_group_data->interlink_pro_idx =
        BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;
    bk_group_data->group_id = BCMI_SR_AUTO_LEARN_GROUP_ID_INVALID;
    sal_memset(bk_config,
               0,
               sizeof(bcm_tsn_sr_auto_learn_group_config_t));
    AUTO_LEARN_BK_UNLOCK(unit);

    /* Clear HW status if there is no valid one.*/
    if (0 == al_bk_info->valid_count) {
        rv = bcmi_esw_tsn_sr_auto_learn_hw_delete_all(unit);
        if (BCM_FAILURE(rv)) {
            AUTO_LEARN_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "Delete all HW resource failed(rv = %d)\n"),
                                  rv));

            return rv;
        }
    }
    AUTO_LEARN_UNLOCK(unit);

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */

    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_group_traverse
 * Purpose:
 *   Traverse the Auto Learn config information.
 * Parameters:
 *   unit      - (IN) Unit number
 *   cb        - (IN) Traverse call back function
 *   user_data - (IN) User data
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_group_traverse(
    int unit,
    bcm_tsn_sr_auto_learn_group_traverse_cb cb,
    void *user_data)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    AUTO_LEARN_LOCK(unit);
    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    for (sw_idx = al_bk_info->valid_sw_idx_start;
         sw_idx <= al_bk_info->valid_sw_idx_end;
         sw_idx++) {
        if (AUTO_LEARN_INTF_USED_GET(unit, sw_idx)) {
            rv = cb(unit, SW_IDX_TO_GROUP_ID(sw_idx), user_data);
            if (BCM_FAILURE(rv)) {
                AUTO_LEARN_UNLOCK(unit);
                return rv;
            }
        }
    }

    AUTO_LEARN_UNLOCK(unit);

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}


/*
 * De-allocate a flowset list and underlying flows with flowset index
 */
STATIC void
bcmi_esw_tsn_sr_auto_learn_free_flowset(
    int unit,
    bcm_tsn_sr_flowset_t *flowset[bcmi_sr_auto_learn_flow_count],
    uint32 *flowset_cnt,
    int clear_hw)
{
    int i;
    bcmi_tsn_sr_auto_learn_flow_t flow_type;

    /* Parameter NULL error handling */
    if ((NULL == flowset) || (NULL == flowset_cnt)) {
        return;
    }

    for (flow_type = bcmi_sr_auto_learn_flow_tx;
         flow_type < bcmi_sr_auto_learn_flow_count;
         flow_type++) {
        if (TRUE == clear_hw) {
            for (i = 0; i < flowset_cnt[flow_type]; i++) {
                if (0 != flowset[flow_type][i]) {
                    (void)bcm_esw_tsn_sr_flowset_destroy(
                            unit, flowset[flow_type][i]);
                }
            }
        }
        if (flowset[flow_type] != NULL) {
            TSN_FREE(unit, flowset[flow_type], 0);
            flowset[flow_type] = NULL;
        }
        flowset_cnt[flow_type] = 0;
    }
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_enable
 * Purpose:
 *   Enable/disable SR flow auto learn subsystem
 * Parameters:
 *   unit   - (IN) Unit number
 *   enable - (IN) Enable/Disable
 *   config - (IN) Auto Learn configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_enable(
    int unit,
    int enable,
    bcm_tsn_sr_auto_learn_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t *bk_data = NULL;
    bcmi_tsn_sr_auto_learn_flow_t flow_type;
    bcm_tsn_sr_flowset_t flowset;
    bcm_tsn_sr_flow_t fid;
    uint32 hwfid;
    int i, prev = 0;
    uint32 ctrl_current_fid = 0, ctrl_last_fid = 0;
    uint32 ctrl_id_size[bcmi_sr_auto_learn_flow_count];

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((TRUE == enable) &&
        ((NULL == config) ||
         (NULL != config &&
          (0 == config->num_tx_flows || 0 == config->num_rx_flows)))) {
        return BCM_E_PARAM;
    }

    /* SR Auto Learn management initialization */
    rv = bcmi_esw_tsn_sr_auto_learn_lazy_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_sr_auto_learn_cleanup(unit);
        return (rv);
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    bcmi_esw_tsn_sr_auto_learn_config_dump(unit, "auto_learn_enable", config);

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->flow_id_pool_set);
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->flow_id_ctrl_set);
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->enable);

    /* Check Auto Learn is already enabled and config to enable is the same */
    if ((TRUE == enable) &&
        (TRUE == al_bk_info->data.enabled) &&
        (!sal_memcmp(config, &al_bk_info->data,
                      sizeof(bcm_tsn_sr_auto_learn_config_t)))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Auto Learn has already enabled, "
                              "please disable auto learn first.\n")));
        return BCM_E_BUSY;
    }

    AUTO_LEARN_LOCK(unit);
    AUTO_LEARN_BK_LOCK(unit);

    bk_data = &(al_bk_info->data);
    /*
     * Create N(num_tx/rx_flows) flows with tx/rx_flow_config for tx/rx flows
     * Each tx/rx flow is a linked list, begin at ctrl_current_fid and end
     * at ctrl_last_fid.
     */
    if (TRUE == enable) {
        /* Check if id sizes excess the max flow number */
        ctrl_id_size[bcmi_sr_auto_learn_flow_tx] = config->num_tx_flows;
        ctrl_id_size[bcmi_sr_auto_learn_flow_rx] = config->num_rx_flows;

        for (flow_type = bcmi_sr_auto_learn_flow_tx;
             flow_type < bcmi_sr_auto_learn_flow_count;
             flow_type++) {
            if (ctrl_id_size[flow_type] >
                (al_dev_info->max_flow_num[flow_type])) {
                AUTO_LEARN_BK_UNLOCK(unit);
                AUTO_LEARN_UNLOCK(unit);
                return BCM_E_PARAM;
            }
        }

        bcmi_esw_tsn_sr_auto_learn_free_flowset(
            unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);

        for (flow_type = bcmi_sr_auto_learn_flow_tx;
             flow_type < bcmi_sr_auto_learn_flow_count;
             flow_type++) {

            TSN_ALLOC(unit, bk_data->flowsets[flow_type],
                      bcm_tsn_sr_flowset_t,
                      sizeof(bcm_tsn_sr_flowset_t) * ctrl_id_size[flow_type],
                      "bk record Rx/Tx Flowset", 0, rv);
            if (BCM_FAILURE(rv)) {
                AUTO_LEARN_BK_UNLOCK(unit);
                AUTO_LEARN_UNLOCK(unit);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "failed to allocate"
                                        "Rx/Tx Flowset memory\n")));
                return rv;
            }
            /* Create N flowsets with tx/rx flow config */
            for (i = 0; i < ctrl_id_size[flow_type]; i++) {
                if (bcmi_sr_auto_learn_flow_tx == flow_type) {
                    rv = bcm_esw_tsn_sr_tx_flowset_create(
                            unit, BCM_TSN_PRI_MAP_INVALID,
                            &config->tx_flow_config, &flowset);
                } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
                    rv = bcm_esw_tsn_sr_rx_flowset_create(
                            unit, BCM_TSN_PRI_MAP_INVALID,
                            &config->rx_flow_config, &flowset);
                }

                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
                    AUTO_LEARN_BK_UNLOCK(unit);
                    AUTO_LEARN_UNLOCK(unit);
                    return rv;
                }
                bk_data->flowsets[flow_type][i] = flowset;
                bk_data->flowsets_cnt[flow_type] = i + 1;
                /* Get the flow_id from created flowset */
                rv = bcm_esw_tsn_sr_flowset_flow_get(
                        unit, flowset, 0, &fid);
                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
                    AUTO_LEARN_BK_UNLOCK(unit);
                    AUTO_LEARN_UNLOCK(unit);
                    return rv;
                }

                /* Convert software SR flow ID to hardware SR flow ID */
                rv = bcmi_esw_tsn_sr_hw_flow_id_get(
                        unit, fid, &hwfid);
                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
                    AUTO_LEARN_BK_UNLOCK(unit);
                    AUTO_LEARN_UNLOCK(unit);
                    return rv;
                }
                if (i == 0) {
                    ctrl_current_fid = hwfid;
                    prev = hwfid;
                    continue;
                }

                /* Write next fid (prev pointer to fid) */
                rv = al_dev_info->flow_id_pool_set(
                                    unit, flow_type, prev, hwfid);
                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
                    AUTO_LEARN_BK_UNLOCK(unit);
                    AUTO_LEARN_UNLOCK(unit);
                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META_U(unit,
                                          "Write to HW failed(rv = %d)\n"),
                                          rv));
                    return rv;
                }
                prev = hwfid;
            }
            ctrl_last_fid = hwfid;

            /* Set the config to HW */
            rv = al_dev_info->flow_id_ctrl_set(
                     unit,
                     flow_type,
                     ctrl_current_fid,
                     ctrl_last_fid,
                     ctrl_id_size[flow_type]);

            if (BCM_FAILURE(rv)) {
                bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
                AUTO_LEARN_BK_UNLOCK(unit);
                AUTO_LEARN_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "Write to HW failed(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
    }

    /* Turn ON / OFF HW learning */
    rv = al_dev_info->enable(unit, enable);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_sr_auto_learn_free_flowset(
            unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
        AUTO_LEARN_BK_UNLOCK(unit);
        AUTO_LEARN_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Write to HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }

    bk_data->enabled = enable;
    if (TRUE == enable) {
        sal_memcpy(&(bk_data->config),
                   config,
                   sizeof(bcm_tsn_sr_auto_learn_config_t));
    } else {
        /* Clear the config when disable */
        bcm_tsn_sr_auto_learn_config_t_init(&(bk_data->config));
        bcmi_esw_tsn_sr_auto_learn_free_flowset(
            unit, bk_data->flowsets, bk_data->flowsets_cnt, TRUE);
    }

    AUTO_LEARN_BK_UNLOCK(unit);
    AUTO_LEARN_UNLOCK(unit);

    bcmi_esw_tsn_sr_auto_learn_config_dump(
        unit, "SW bk config", &(bk_data->config));

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_enable_get
 * Purpose:
 *   Get enable/disable SR flow auto learn subsystem
 * Parameters:
 *   unit   - (IN) Unit number
 *   enable - (OUT) Enable/disable
 *   config - (OUT) Auto Learn configuration
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_enable_get(
    int unit,
    int *enabled,
    bcm_tsn_sr_auto_learn_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SR_SUPPORT)
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t *bk_data = NULL;

    TSN_FUNCTION_TRACE(unit, "IN");

    /* Parameter NULL error handling */
    if ((NULL == enabled) || (NULL == config)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    AUTO_LEARN_INIT(unit);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* Get enabled from SW BK */
    bk_data = &(al_bk_info->data);
    *enabled = bk_data->enabled;

    /* If it's enabled, copy bcm_tsn_sr_auto_learn_config_t */
    if (*enabled) {
        sal_memcpy(config,
                   &(bk_data->config),
                   sizeof(bcm_tsn_sr_auto_learn_config_t));

        bcmi_esw_tsn_sr_auto_learn_config_dump(
            unit, "auto_learn_enable_get", config);
    } else {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "SR flow auto learn subsystem isn't enabled\n")));
    }

    rv = BCM_E_NONE;
#endif /* BCM_TSN_SR_SUPPORT */
    TSN_FUNCTION_TRACE(unit, "OUT");
    return rv;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/* Auto Learn management - warmboot scache size allocation */
STATIC int
bcmi_esw_tsn_sr_auto_learn_wb_alloc(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    soc_scache_handle_t scache_handle;
    uint8 *auto_learn_scache = NULL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    tsn_sr_flows_bookkeeping_t *flow_bkinfo;
    uint32 table_size;
    int data_size = 0, group_data_size = 0;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* 1. Size of bcmi_esw_tsn_sr_auto_learn_bk_t.initialized */
    data_size = sizeof(int);

    /* Initialization check */
    AUTO_LEARN_DEV_INIT(unit);

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);

    /* 2. Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.intf_bitmap */
    data_size += SHR_BITALLOCSIZE(table_size);

    /* 3. Size of bcmi_esw_tsn_sr_auto_learn_bk_t.group_data */
    group_data_size =
        sizeof(bcmi_esw_tsn_sr_auto_learn_group_bk_data_t) * table_size;
    data_size += group_data_size;

    /* 4. Size of bcmi_esw_tsn_sr_auto_learn_bk_t.data */
    flow_bkinfo = &tsn_bk_info[unit]->sr_flows;
    data_size += sizeof(uint32) * 2;   /* flowsets_cnt */
    data_size +=                       /* Rx flowsets */
        flow_bkinfo->max_flows[SR_FLOW_DIR_RX] * sizeof(bcm_tsn_sr_flowset_t);
    data_size +=                       /* Tx flowsets */
        flow_bkinfo->max_flows[SR_FLOW_DIR_TX] * sizeof(bcm_tsn_sr_flowset_t);

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit,
                          BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_AUTO_LEARN);

    rv = _bcm_esw_scache_ptr_get(
            unit, scache_handle, TRUE, data_size,
            &auto_learn_scache, BCM_TSN_WB_DEFAULT_VERSION, NULL);

    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "Auto Learn SCACHE Memory not available\n")));
        return rv;
    }
    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_sync
 * Purpose:
 *   Record bcmi_esw_tsn_sr_auto_learn_group_bk_data_t for Warm Boot
 * Parameters:
 *   unit       - (IN) Unit number
 * Returns:
 *   BCM_E_NONE - Success.
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_sync(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t *bk_data = NULL;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    uint32 table_size;
    int data_size = 0, group_data_size = 0;
    uint8 *auto_learn_scache = NULL;
    soc_scache_handle_t scache_handle;

    TSN_FUNCTION_TRACE(unit, "IN");

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit,
                          BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_AUTO_LEARN);

    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
            &auto_learn_scache, BCM_TSN_WB_DEFAULT_VERSION, NULL));

    if (NULL == auto_learn_scache) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "Auto Learn SCACHE Memory not available\n")));
        return BCM_E_MEMORY;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* 1. Sync bcmi_esw_tsn_sr_auto_learn_bk_t.initialized */
    AUTO_LEARN_BK_LOCK(unit);
    data_size = sizeof(int);
    sal_memcpy(auto_learn_scache, &(al_bk_info->initialized), data_size);
    auto_learn_scache += data_size;
    AUTO_LEARN_BK_UNLOCK(unit);

    if (TRUE == al_bk_info->initialized) {
        /* Initialization check */
        AUTO_LEARN_INIT(unit);

        /* auto learn device info */
        dev_info = bk_info->tsn_dev_info;
        al_dev_info = dev_info->tsn_sr_auto_learn_info;

        /* Set SW bookkeeping database pointers */
        bk_group_data = al_bk_info->group_data;
        bk_data = &(al_bk_info->data);

        rv = al_dev_info->table_size_get(unit, &table_size);
        BCM_IF_ERROR_RETURN(rv);
        assert(table_size > 0);
        group_data_size =
            sizeof(bcmi_esw_tsn_sr_auto_learn_group_bk_data_t) * table_size;

        AUTO_LEARN_BK_LOCK(unit);
        /* 2. Sync bcmi_esw_tsn_sr_auto_learn_bk_t.intf_bitmap */
        data_size = SHR_BITALLOCSIZE(table_size);
        sal_memcpy(auto_learn_scache, al_bk_info->intf_bitmap, data_size);
        auto_learn_scache += data_size;

        /* 3. Sync bcmi_esw_tsn_sr_auto_learn_bk_t.group_data */
        data_size = group_data_size;
        sal_memcpy(auto_learn_scache, bk_group_data, data_size);
        auto_learn_scache += data_size;
        AUTO_LEARN_BK_UNLOCK(unit);

        if (TRUE == bk_data->enabled) {
            /* 4. Sync bcmi_esw_tsn_sr_auto_learn_bk_t.data */

            /* flowset_cnt */
            AUTO_LEARN_BK_LOCK(unit);
            data_size = sizeof(uint32);
            sal_memcpy(auto_learn_scache,
                       &(bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_tx]),
                       data_size);
            auto_learn_scache += data_size;

            data_size = sizeof(uint32);
            sal_memcpy(auto_learn_scache,
                       &(bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_rx]),
                       data_size);
            auto_learn_scache += data_size;

            /* Tx flowsets */
            data_size =
                bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_tx] *
                sizeof(bcm_tsn_sr_flowset_t);
            sal_memcpy(auto_learn_scache,
                       bk_data->flowsets[bcmi_sr_auto_learn_flow_tx],
                       data_size);
            auto_learn_scache += data_size;

            /* Rx flowsets */
            data_size =
                bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_rx] *
                sizeof(bcm_tsn_sr_flowset_t);
            sal_memcpy(auto_learn_scache,
                       bk_data->flowsets[bcmi_sr_auto_learn_flow_rx],
                       data_size);
            auto_learn_scache += data_size;
            AUTO_LEARN_BK_UNLOCK(unit);
        }
    }

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_reload
 * Purpose:
 *   Re-initialize the Auto Learn resource. Restore bookkeeping information.
 * Parameters:
 *   unit       - (IN) Unit number
 * Returns:
 *   BCM_E_NONE - Success. Warm boot successfully
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_reload(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    int enabled;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t *bk_data = NULL;
    bcmi_esw_tsn_sr_mac_proxy_bk_info_t *mp_bk_info = NULL;
    bcmi_esw_tsn_sr_mac_proxy_entry_t *mp_entries = NULL;
    bcmi_tsn_sr_auto_learn_flow_t flow_type;
    bcm_tsn_sr_flowset_t flowset;
    uint32 fid;
    bcmi_esw_tsn_sr_auto_learn_group_bk_data_t *bk_group_data;
    uint32 table_size;
    int data_size = 0;
    uint8 *auto_learn_scache = NULL;
    soc_scache_handle_t scache_handle;
    int i;
    bcmi_tsn_sr_auto_learn_group_port_type_t group_ports_type;
    int pro_idx = BCMI_SR_AUTO_LEARN_MPP_IDX_INVALID;

    TSN_FUNCTION_TRACE(unit, "IN");

    SOC_SCACHE_HANDLE_SET(scache_handle,
                          unit,
                          BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_AUTO_LEARN);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
            &auto_learn_scache, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        /* Proceed with level 1 warmboot */
        return BCM_E_NONE;
    }

    if (NULL == auto_learn_scache) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "SCACHE Memory not available \n")));
        return BCM_E_MEMORY;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* auto learn device info */
    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;

    /* Check if the chip specific function existed */
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->table_size_get);
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->enable_get);
    TSN_CHECK_NULL_DEV_INSTANCE(unit, al_dev_info->flow_id_ctrl_get);

    /* auto learn bk info */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* 1. Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.initialized */
    AUTO_LEARN_BK_LOCK(unit);
    data_size = sizeof(int);
    sal_memcpy(&(al_bk_info->initialized), auto_learn_scache, data_size);
    auto_learn_scache += data_size;
    AUTO_LEARN_BK_UNLOCK(unit);

    if (FALSE == al_bk_info->initialized) {
        /* 1st initialization hasn't been triggered */
        TSN_FUNCTION_TRACE(unit, "OUT : not initialized");
        return BCM_E_NONE;
    }

    /* Force to re-init again */
    al_bk_info->initialized = FALSE;

    /* SR Auto Learn management initialization */
    rv = bcmi_esw_tsn_sr_auto_learn_lazy_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_sr_auto_learn_cleanup(unit);
        return (rv);
    }

    /* Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.group_data : Start */
    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    assert(table_size > 0);

    /* Set SW bookkeeping database pointers */
    bk_data = &(al_bk_info->data);
    bk_group_data = al_bk_info->group_data;


    AUTO_LEARN_BK_LOCK(unit);

    /* 2. Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.intf_bitmap */
    data_size = SHR_BITALLOCSIZE(table_size);
    sal_memcpy(al_bk_info->intf_bitmap, auto_learn_scache, data_size);
    auto_learn_scache += data_size;

    /* Calculate the cnt/start/end */
    al_bk_info->valid_count = 0;
    al_bk_info->valid_sw_idx_start = 0;
    al_bk_info->valid_sw_idx_end = 0;
    al_bk_info->empty_sw_idx_start = 0;
    al_bk_info->empty_sw_idx_end = 0;

    for (i = 0; i < table_size; i++) {
        if (AUTO_LEARN_INTF_USED_GET(unit, i)) {
            al_bk_info->valid_count++;
        }

        if (AUTO_LEARN_INTF_USED_GET(unit, i) &&
            !AUTO_LEARN_INTF_USED_GET(unit, al_bk_info->valid_sw_idx_start)) {
            al_bk_info->valid_sw_idx_start = i;
        }

        if (AUTO_LEARN_INTF_USED_GET(unit, i) &&
            al_bk_info->valid_sw_idx_end < i) {
            al_bk_info->valid_sw_idx_end = i;
        }

        if (!AUTO_LEARN_INTF_USED_GET(unit, i) &&
            AUTO_LEARN_INTF_USED_GET(unit, al_bk_info->empty_sw_idx_start)) {
            al_bk_info->empty_sw_idx_start = i;
        }

        if (!AUTO_LEARN_INTF_USED_GET(unit, i) &&
            al_bk_info->empty_sw_idx_end < i) {
            al_bk_info->empty_sw_idx_end = i;
        }
    }

    /* 3. Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.group_data */
    data_size =
        sizeof(bcmi_esw_tsn_sr_auto_learn_group_bk_data_t) * table_size;
    sal_memcpy(bk_group_data, auto_learn_scache, data_size);
    auto_learn_scache += data_size;
    AUTO_LEARN_BK_UNLOCK(unit);

    /* Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.group_data : End */

    /* Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.data : Start */

    rv = al_dev_info->enable_get(unit, &enabled);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Read from HW failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    AUTO_LEARN_BK_LOCK(unit);
    bk_data->enabled = enabled;
    AUTO_LEARN_BK_UNLOCK(unit);

    if (TRUE == bk_data->enabled) {
        /* 3. Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.data */
        AUTO_LEARN_BK_LOCK(unit);
        data_size = sizeof(uint32);
        sal_memcpy(&(bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_tx]),
                   auto_learn_scache, data_size);
        auto_learn_scache += data_size;

        data_size = sizeof(uint32);
        sal_memcpy(&(bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_rx]),
                   auto_learn_scache, data_size);
        auto_learn_scache += data_size;
        AUTO_LEARN_BK_UNLOCK(unit);

        bcm_tsn_sr_auto_learn_config_t_init(&(bk_data->config));

        AUTO_LEARN_BK_LOCK(unit);
        bk_data->config.num_tx_flows =
            bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_tx];
        bk_data->config.num_rx_flows =
            bk_data->flowsets_cnt[bcmi_sr_auto_learn_flow_rx];
        AUTO_LEARN_BK_UNLOCK(unit);

        for (flow_type = bcmi_sr_auto_learn_flow_tx;
             flow_type <= bcmi_sr_auto_learn_flow_rx;
             flow_type++) {

            AUTO_LEARN_BK_LOCK(unit);
            data_size =
                bk_data->flowsets_cnt[flow_type] *
                sizeof(bcm_tsn_sr_flowset_t);
            TSN_ALLOC(
                unit, bk_data->flowsets[flow_type], bcm_tsn_sr_flowset_t,
                data_size, "bk record Rx/Tx Flowset", 0, rv);
            if (BCM_FAILURE(rv)) {
                AUTO_LEARN_BK_UNLOCK(unit);
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "bcmi_esw_tsn_sr_auto_learn_reload: "
                                        "failed to allocate memory\n")));
                return rv;
            }
            sal_memcpy(bk_data->flowsets[flow_type], auto_learn_scache,
                       data_size);
            auto_learn_scache += data_size;

            /* Take the 1st one for retrieving default flow config */
            flowset = bk_data->flowsets[flow_type][0];

            /* Get the flow_id from created flowset */
            rv = bcm_esw_tsn_sr_flowset_flow_get(
                    unit, flowset, 0, &fid);

            /* Get tx/rx_flow_config & num_tx/rx_flows */
            if (bcmi_sr_auto_learn_flow_tx == flow_type) {
                /* Get flow config with given software SR flow ID */
                rv = bcm_esw_tsn_sr_tx_flow_config_get(unit,
                        fid, &(bk_data->config.tx_flow_config));
                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, FALSE);
                    bcm_tsn_sr_auto_learn_config_t_init(&(bk_data->config));
                    AUTO_LEARN_BK_UNLOCK(unit);
                    return rv;
                }
            } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
                /* Get flow config with given software SR flow ID */
                rv = bcm_esw_tsn_sr_rx_flow_config_get(unit,
                        fid, &(bk_data->config.rx_flow_config));
                if (BCM_FAILURE(rv)) {
                    bcmi_esw_tsn_sr_auto_learn_free_flowset(
                        unit, bk_data->flowsets, bk_data->flowsets_cnt, FALSE);
                    bcm_tsn_sr_auto_learn_config_t_init(&(bk_data->config));
                    AUTO_LEARN_BK_UNLOCK(unit);
                    return rv;
                }
            }
            AUTO_LEARN_BK_UNLOCK(unit);
        }

    }
    /* Recovery bcmi_esw_tsn_sr_auto_learn_bk_t.data : End */

    /* Refresh MAC proxy ref_count for auto learn group usage */
    /* SR MAC Proxy bk info */
    mp_bk_info = &(bk_info->tsn_sr_mac_proxy_bk_info);
    mp_entries = mp_bk_info->sr_mp_entry;
    if (mp_entries == NULL) {
        return BCM_E_UNAVAIL;
    }

    SR_MAC_PROXY_LOCK(mp_bk_info);
    for (i = 0; i < table_size; i++) {
        if (AUTO_LEARN_INTF_USED_GET(unit, i)) {
            bk_group_data = &(al_bk_info->group_data[i]);

            /* Update ref_count for interlink ports */
            pro_idx = bk_group_data->interlink_pro_idx;
            mp_entries[pro_idx].ref_count++;

            /* Update ref_count for redundant ports */
            group_ports_type = bcmi_sr_auto_learn_group_macp;
            pro_idx = bk_group_data->redundant_pro_idx[group_ports_type];
            mp_entries[pro_idx].ref_count++;
        }
    }
    SR_MAC_PROXY_UNLOCK(mp_bk_info);

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *     bcmi_esw_tsn_sr_auto_learn_cleanup
 * Purpose:
 *     Internal function to clear the resource of the TSN module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_cleanup(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_data_t *bk_data = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_bk_info_cleanup\n")));

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &bk_info));

    /* Free and clear the SR Auto Learn bookkeeping info structure */
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    if (NULL != al_bk_info->intf_bitmap) {
        TSN_FREE(unit, al_bk_info->intf_bitmap, 0);
        al_bk_info->intf_bitmap = NULL;
    }
    if (NULL != al_bk_info->group_data) {
        TSN_FREE(unit, al_bk_info->group_data, 0);
        al_bk_info->group_data = NULL;
    }

    bk_data = &(al_bk_info->data);

    bcmi_esw_tsn_sr_auto_learn_free_flowset(
        unit, bk_data->flowsets, bk_data->flowsets_cnt, FALSE);

    /* Reset to default setting */
    al_bk_info->valid_count = 0;
    al_bk_info->valid_sw_idx_start = 0;
    al_bk_info->valid_sw_idx_end = 0;
    al_bk_info->empty_sw_idx_start = 0;
    al_bk_info->empty_sw_idx_end = 0;

    if (al_bk_info->mutex != NULL) {
        sal_mutex_destroy(al_bk_info->mutex);
        al_bk_info->mutex = NULL;
    }

    if (al_bk_info->bk_mutex != NULL) {
        sal_mutex_destroy(al_bk_info->bk_mutex);
        al_bk_info->bk_mutex = NULL;
    }

    al_bk_info->initialized = FALSE;
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_init
 * Purpose:
 *   create a default profile setting
 * Parameters:
 *   unit - (IN) Unit number
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_init(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /* Create mutex */
    al_bk_info->mutex = sal_mutex_create("auto_learn_mutex");
    if (al_bk_info->mutex == NULL) {
        return BCM_E_MEMORY;
    }

    /* Create mutex */
    al_bk_info->bk_mutex = sal_mutex_create("auto_learn_bk_mutex");
    if (al_bk_info->bk_mutex == NULL) {
        return BCM_E_MEMORY;
    }

    al_bk_info->initialized = FALSE;

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sr_auto_learn_lazy_init
 * Purpose:
 *   create a default profile setting
 * Parameters:
 *   unit - (IN) Unit number
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_sr_auto_learn_lazy_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_auto_learn_info_t *al_dev_info = NULL;
    bcmi_esw_tsn_sr_auto_learn_bk_t *al_bk_info = NULL;
    uint32 table_size, shr_bitdcl_size, bk_data_size;
    char *table_name;
    int i;

    TSN_FUNCTION_TRACE(unit, "IN");

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    dev_info = bk_info->tsn_dev_info;
    al_dev_info = dev_info->tsn_sr_auto_learn_info;
    al_bk_info = &(bk_info->tsn_sr_auto_learn_bk_info);

    /*
     * Lazy initialization,
     * 1st initializ should be performed in auto_learn_group_create
     */
    if (TRUE == al_bk_info->initialized) {
        /* 1st initialization has been triggered */
        TSN_FUNCTION_TRACE(unit, "OUT : initialized");
        return BCM_E_NONE;
    }

    /* Check chip device info instance is valid or not */
    if (NULL == al_dev_info) {
        return BCM_E_UNAVAIL;
    }

    table_name = al_dev_info->table_name;
    rv = al_dev_info->table_size_get(unit, &table_size);
    BCM_IF_ERROR_RETURN(rv);
    /* table_size should large than 0 */
    assert(table_size > 0);

    /* Allocate usage bitmap */
    shr_bitdcl_size = SHR_BITALLOCSIZE(table_size);
    TSN_ALLOC(unit, al_bk_info->intf_bitmap, SHR_BITDCL,
              shr_bitdcl_size,
              "SR Auto Learn bookkeeping intf_bitmap", 0, rv);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "bcmi_esw_tsn_init: failed to "
                                "allocate memory\n")));
        return rv;
    }
    sal_memset(al_bk_info->intf_bitmap, 0, shr_bitdcl_size);

    /* Allocate bcmi_esw_tsn_sr_auto_learn_group_bk_data_t */
    bk_data_size =
        sizeof(bcmi_esw_tsn_sr_auto_learn_group_bk_data_t) * table_size;
    TSN_ALLOC(unit, al_bk_info->group_data,
              bcmi_esw_tsn_sr_auto_learn_group_bk_data_t,
              bk_data_size,
              table_name, 0, rv);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "bcmi_esw_tsn_init: failed to "
                                "allocate memory\n")));
        return rv;
    }
    sal_memset(al_bk_info->group_data, 0, bk_data_size);

    /* Initialize default value */
    for (i = 0; i < table_size; i++) {
        al_bk_info->group_data[i].group_id =
            BCMI_SR_AUTO_LEARN_GROUP_ID_INVALID;
    }

    /* Initialize default valid & empty pointer */
    al_bk_info->valid_count = 0;
    al_bk_info->valid_sw_idx_start = 0;
    al_bk_info->valid_sw_idx_end = 0;
    al_bk_info->empty_sw_idx_start = 0;
    al_bk_info->empty_sw_idx_end = table_size - 1;

    al_bk_info->data.enabled = FALSE;
    bcm_tsn_sr_auto_learn_config_t_init(&(al_bk_info->data.config));

    /* For Lazy initialization */
    al_bk_info->initialized = TRUE;

    /* Clear HW status if there is no valid one.*/
    if (0 == al_bk_info->valid_count) {
        rv = bcmi_esw_tsn_sr_auto_learn_hw_delete_all(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "Delete all HW resource failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_port_config_set
 * Purpose:
 *     Set SR configuration on a port
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port to configure
 *     config           - (IN) port SR configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_port_config_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_sr_port_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_port_dev_info_t *sr_port_config = NULL;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    if (dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }
    sr_port_config = dev_info->sr_port_info;
    if ((sr_port_config == NULL) ||
        (sr_port_config->sr_port_config_set == NULL)) {
        return BCM_E_UNAVAIL;
    }

    return sr_port_config->sr_port_config_set(unit, port, config);
}

/*
 * Function:
 *     bcmi_esw_tsn_sr_port_config_get
 * Purpose:
 *     Get SR configuration from a port
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port
 *     config           - (OUT) port SR configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_sr_port_config_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_sr_port_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_sr_port_dev_info_t *sr_port_config = NULL;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    if (dev_info == NULL) {
        return BCM_E_UNAVAIL;
    }
    sr_port_config = dev_info->sr_port_info;
    if ((sr_port_config == NULL) ||
        (sr_port_config->sr_port_config_get == NULL)) {
        return BCM_E_UNAVAIL;
    }

    return sr_port_config->sr_port_config_get(unit, port, config);
}

#endif /* BCM_TSN_SR_SUPPORT */

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_dump_config
 * Purpose:
 *      dump the config of priority map
 * Parameters:
 *      unit - (IN) Unit number
 *      prefix - (IN) Prefix string for dump message
 *      config - (IN) Priority map config
 * Returns:
 *      none
 */
STATIC void
bcmi_esw_tsn_pri_map_dump_config(
    int unit,
    char *prefix,
    bcm_tsn_pri_map_config_t *config)
{
    int i;
    uint32 flags;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_dump_config\n")));

    if ((NULL == prefix) || (NULL == config)) {
        return;
    }

    if (bsl_check(bslLayerBcm, bslSourceTsn, bslSeverityDebug, unit)) {
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "bcmi_esw_tsn_pri_map_dump_config:\n")));
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                             "%s map_type %d, num_map_entries %d.\n"),
                             prefix,
                             config->map_type,
                             config->num_map_entries
                             ));

        for (i = 0; i < (config->num_map_entries); i++) {
            flags = (config->map_entries[i].flags);
            if (flags & BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) {
                LOG_DEBUG(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "%s map_entries[%d]: flow_offset %d.\n"),
                                      prefix,
                                      i,
                                      config->map_entries[i].flow_offset
                                      ));
            }

            if (flags & BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI) {
                LOG_DEBUG(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "%s map_entries[%d]: new_int_pri %d.\n"),
                                      prefix,
                                      i,
                                      config->map_entries[i].new_int_pri
                                      ));
            }

            if (flags & BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID) {
                LOG_DEBUG(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "%s map_entries[%d]: profile_id %d.\n"),
                                      prefix,
                                      i,
                                      config->map_entries[i].profile_id
                                      ));
            }
        }
    }
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_cmp
 * Purpose:
 *      Check the priority mapping entry is the same or not.
 * Parameters:
 *      unit - (IN) Unit number
 *      ent1 - (IN) priority mapping entry 1
 *      ent2 - (IN) priority mapping entry 2
  *     equal  - (OUT) identical flag.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Fail. Wrong parameter
 */
STATIC int
bcmi_esw_tsn_pri_map_cmp(
    int unit,
    bcm_tsn_pri_map_entry_t *ent1,
    bcm_tsn_pri_map_entry_t *ent2,
    uint8 *equal)
{
    int compare_flow, compare_prof, compare_intp, compare_egate, compare_pgate;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_cmp\n")));

    if ((NULL == ent1) || (NULL == ent2) ||
        (NULL == equal)) {
        return BCM_E_PARAM;
    }

    *equal = TRUE;

    /* If flags are different */
    if (ent1->flags != ent2->flags) {
        *equal = FALSE;
        return BCM_E_NONE;
    }

    /* Check which flags need to be compared */
    compare_flow =
        (ent1->flags & BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) ?
         TRUE : FALSE;
    compare_intp =
        (ent1->flags & BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID) ?
         TRUE : FALSE;
    compare_prof =
        (ent1->flags & BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI) ?
         TRUE : FALSE;
    compare_egate =
        (ent1->flags & BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_EXPRESS) ?
         TRUE : FALSE;
    compare_pgate =
        (ent1->flags & BCM_BCM_TSN_PRI_MAP_ENTRY_TAF_GATE_PREEMPT) ?
         TRUE : FALSE;

    /* If any different found */
    if ((compare_flow && (ent1->flow_offset != ent2->flow_offset)) ||
        (compare_intp && (ent1->new_int_pri != ent2->new_int_pri)) ||
        (compare_prof && (ent1->profile_id != ent2->profile_id)) ||
        (compare_egate && (ent1->taf_gate_express != ent2->taf_gate_express)) ||
        (compare_pgate && (ent1->taf_gate_preempt != ent2->taf_gate_preempt)))
    {
        *equal = FALSE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_id_existed_check
 * Purpose:
 *      Check the Priority Map ID exited or not.
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (OUT) Map ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_id_existed_check(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcm_tsn_pri_map_type_t map_type;
    uint32 sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_id_existed_check\n")));

    if (BCM_TSN_PRI_MAP_INVALID == map_id) {
        return BCM_E_CONFIG;
    }
    map_type = PRI_MAP_TYPE_GET(map_id);

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    /* Check if the map_id and sw_idx is in the reasonable range */
    assert(pm_dev_info->table_size > 0);
    if ((map_type >= bcmTsnPriMapTypeCount) ||
        (sw_idx >= pm_dev_info->table_size)) {
        return BCM_E_PARAM;
    }

    if (PRI_MAP_INTF_GET(unit, map_type, sw_idx)) {
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_existed_check
 * Purpose:
 *      Check the Priority Map configuration exited or not.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Priority Map configuration
 *      map_id - (OUT) Map ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_existed_check(
    int unit,
    bcm_tsn_pri_map_config_t *config,
    bcm_tsn_pri_map_t *map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    int ent_idx, is_the_same;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_config_t *bk_config = NULL;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    uint32 sw_idx;
    uint8 equal;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_existed_check\n")));

    if ((NULL == config) || (NULL == map_id)) {
        return BCM_E_PARAM;
    }
    map_type = config->map_type;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    if ((map_type >= bcmTsnPriMapTypeCount) ||
        (config->num_map_entries == 0) ||
        (config->num_map_entries >
         pm_dev_info->entry_size)) {
        return BCM_E_PARAM;
    }

    PRI_MAP_BK_LOCK(unit);
    for (sw_idx = 0;
         sw_idx < pm_dev_info->entry_size;
         sw_idx++) {
        is_the_same = TRUE;

        /* Non-used entry, return and continue to check new one */
        if (PRI_MAP_INTF_GET(unit, map_type, sw_idx) == 0) {
            continue;
        }

        bk_data = &(pm_bk_info->data[map_type][sw_idx]);
        bk_config = &(bk_data->config);

        /* Check if map_type is the same */
        if ((bk_config->num_map_entries == config->num_map_entries)) {
            for (ent_idx = 0; ent_idx < (config->num_map_entries); ent_idx++) {

                /* Compare two entries if they're the same */
                rv = bcmi_esw_tsn_pri_map_cmp(unit,
                        &(bk_config->map_entries[ent_idx]),
                        &(config->map_entries[ent_idx]),
                        &equal);
                if (BCM_FAILURE(rv)) {
                    PRI_MAP_BK_UNLOCK(unit);
                    return rv;
                }

                /* The two entries are different */
                if (FALSE == equal) {
                    is_the_same = FALSE;
                    break;
                }
            }
            if (TRUE == is_the_same) {
                *map_id = pm_bk_info->data[map_type][sw_idx].map_id;
                PRI_MAP_BK_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        }
    }
    PRI_MAP_BK_UNLOCK(unit);
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_hw_index_get
 * Purpose:
 *      Get HW memory base index with given map_id
 * Parameters:
 *      unit     - (IN) Unit number
 *      map_id   - (IN) Map ID
 *      map_type - (OUT) Priority Map Type
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_hw_index_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_type_t *map_type,
    uint32 *hw_index)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    uint32 sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_hw_index_get\n")));

    /* Parameter NULL error handling */
    if (NULL == hw_index || NULL == map_type) {
        return BCM_E_PARAM;
    }

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }
    *map_type = PRI_MAP_TYPE_GET(map_id);

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, *map_type);

    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[*map_type];
    if (NULL == pm_dev_info->tsn_pri_map_hw_index_get) {
        return BCM_E_UNAVAIL;
    }

    rv = pm_dev_info->tsn_pri_map_hw_index_get(unit, sw_idx, hw_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "HW operation failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_map_id_get
 * Purpose:
 *      Get map_id with given HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      map_type - (IN) Priority Map Type
 *      hw_index - (IN) HW memory base index
 *      map_id   - (OUT) Map ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_map_id_get(
    int unit,
    bcm_tsn_pri_map_type_t map_type,
    uint32 hw_index,
    bcm_tsn_pri_map_t *map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    uint32 sw_idx;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_map_id_get\n")));
    /* Input parameters check. */
    if (NULL == map_id) {
        return (BCM_E_PARAM);
    }

    if (tsn_bk_info[unit] == NULL) {
        /* Using device specific info that need no bk_info */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_noninit_dev_info_get(unit, &dev_info));
        if (NULL ==
                dev_info->tsn_pri_map_info[map_type]->tsn_pri_map_sw_idx_get) {
            return BCM_E_UNAVAIL;
        }
        rv = dev_info->tsn_pri_map_info[map_type]->tsn_pri_map_sw_idx_get(
                                                     unit, hw_index, &sw_idx);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "HW operation failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
        *map_id = SW_IDX_TO_MAP_ID(sw_idx, map_type);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];
    if (NULL == pm_dev_info->tsn_pri_map_sw_idx_get) {
        return BCM_E_UNAVAIL;
    }

    rv = pm_dev_info->tsn_pri_map_sw_idx_get(unit, hw_index, &sw_idx);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "HW operation failed(rv = %d)\n"),
                              rv));
        return rv;
    }
    *map_id = SW_IDX_TO_MAP_ID(sw_idx, map_type);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_ref_cnt_dec
 * Purpose:
 *      Decrease reference cnt by 1 with given map_id
 * Parameters:
 *      unit   - (IN) Unit number
 *      map_id - (IN) Map ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_ref_cnt_dec(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_type_t map_type = PRI_MAP_TYPE_GET(map_id);
    uint32 sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_ref_cnt_dec\n")));

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    PRI_MAP_BK_LOCK(unit);

    bk_data = &(pm_bk_info->data[map_type][sw_idx]);
    if (bk_data->ref_cnt == 0) {
        PRI_MAP_BK_UNLOCK(unit);
        return BCM_E_FAIL;
    }
    bk_data->ref_cnt -= 1;
    PRI_MAP_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_ref_cnt_inc
 * Purpose:
 *      Increase reference cnt by 1 with given map_id
 * Parameters:
 *      unit   - (IN) Unit number
 *      map_id - (IN) Map ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_ref_cnt_inc(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_type_t map_type = PRI_MAP_TYPE_GET(map_id);
    uint32 sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_ref_cnt_inc\n")));

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    PRI_MAP_BK_LOCK(unit);

    bk_data = &(pm_bk_info->data[map_type][sw_idx]);
    bk_data->ref_cnt += 1;

    PRI_MAP_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_ref_cnt_get
 * Purpose:
 *      Get current reference cnt with given map_id
 * Parameters:
 *      unit    - (IN) Unit number
 *      map_id  - (IN) Map ID
 *      ref_cnt - (OUT) Reference cnt
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_ref_cnt_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    uint32 *ref_cnt)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcm_tsn_pri_map_type_t map_type = PRI_MAP_TYPE_GET(map_id);
    uint32 sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_ref_cnt_get\n")));

    /* Parameter NULL error handling */
    if (NULL == ref_cnt) {
        return BCM_E_PARAM;
    }

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    *ref_cnt = pm_bk_info->data[map_type][sw_idx].ref_cnt;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_get_empty_map_id
 * Purpose:
 *      Get a empty map_id for specific map type
 * Parameters:
 *      unit - (IN) Unit number
 *      map_type - (IN) Priority Map Type
 *      map_id - (OUT) Map ID
 * Returns:
 *      BCM_E_NONE - Success on finding a empty map_id
 */
STATIC int
bcmi_esw_tsn_pri_map_get_empty_map_id(
    int unit,
    bcm_tsn_pri_map_type_t map_type,
    bcm_tsn_pri_map_t *map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    /* Empty id */
    uint32 e_sw_idx;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_get_empty_map_id\n")));

    if (NULL == map_id) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    PRI_MAP_BK_LOCK(unit);
    /* Already full, return E_RESOURCE */
    if (pm_bk_info->valid_count[map_type] == pm_dev_info->table_size) {
        PRI_MAP_BK_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    /* Try to find an empty id */
    for (e_sw_idx = pm_bk_info->empty_sw_idx_start[map_type];
         e_sw_idx <= pm_bk_info->empty_sw_idx_end[map_type];
         e_sw_idx++) {

        if (PRI_MAP_INTF_GET(unit, map_type, e_sw_idx) == 0) {
            /* An empty id found, set bit map as used */
            PRI_MAP_INTF_SET(unit, map_type, e_sw_idx);

            /* Update valid start and end */
            if (pm_bk_info->valid_count[map_type] == 0) {
                pm_bk_info->valid_sw_idx_start[map_type] = e_sw_idx;
                pm_bk_info->valid_sw_idx_end[map_type] = e_sw_idx;
            } else if (e_sw_idx < pm_bk_info->valid_sw_idx_start[map_type]) {
                pm_bk_info->valid_sw_idx_start[map_type] = e_sw_idx;
            } else if (e_sw_idx > pm_bk_info->valid_sw_idx_end[map_type]) {
                pm_bk_info->valid_sw_idx_end[map_type] = e_sw_idx;
            }

            pm_bk_info->valid_count[map_type]++;
            *map_id = SW_IDX_TO_MAP_ID(e_sw_idx, map_type);

            /* Update the empty start, assume next one is real empty */
            pm_bk_info->empty_sw_idx_start[map_type] = e_sw_idx + 1;

            /* An empty id found, break and return BCM_E_NONE */
            rv = BCM_E_NONE;
            break;
        } else {
            /* Not empty id */

            /* Update the empty start, assume next one is real empty */
            pm_bk_info->empty_sw_idx_start[map_type] = e_sw_idx + 1;
        }
    }
    PRI_MAP_BK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_destroy_map_id
 * Purpose:
 *      Free the resource of map_id
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Map ID
 * Returns:
 *      BCM_E_NONE - Success on destroying the given map_id
 */
STATIC int
bcmi_esw_tsn_pri_map_destroy_map_id(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcm_tsn_pri_map_type_t map_type = PRI_MAP_TYPE_GET(map_id);
    uint32 destroy_sw_idx = MAP_ID_TO_SW_IDX(map_id);
    uint32 n_valid_sw_idx;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_destroy_map_id\n")));

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    assert(pm_dev_info->table_size > 0);

    PRI_MAP_BK_LOCK(unit);
    /* Destroy bit map */
    PRI_MAP_INTF_CLR(unit, map_type, destroy_sw_idx);

    /* Update empty start and end */
    if (destroy_sw_idx < pm_bk_info->empty_sw_idx_start[map_type]) {
        pm_bk_info->empty_sw_idx_start[map_type] = destroy_sw_idx;
    } else if (destroy_sw_idx > pm_bk_info->empty_sw_idx_end[map_type]) {
        pm_bk_info->empty_sw_idx_end[map_type] = destroy_sw_idx;
    }

    /* Update valid, find a next valid start or next valid end */
    n_valid_sw_idx = destroy_sw_idx;
    if (pm_bk_info->valid_count[map_type] > 1) {
        if (n_valid_sw_idx == pm_bk_info->valid_sw_idx_start[map_type]) {
            do {
                n_valid_sw_idx++;
            } while (PRI_MAP_INTF_GET(unit, map_type, n_valid_sw_idx) == 0);
            pm_bk_info->valid_sw_idx_start[map_type] = n_valid_sw_idx;
        } else if (n_valid_sw_idx == pm_bk_info->valid_sw_idx_end[map_type]) {
            do {
                n_valid_sw_idx--;
            } while (PRI_MAP_INTF_GET(unit, map_type, n_valid_sw_idx) == 0);
            pm_bk_info->valid_sw_idx_end[map_type] = n_valid_sw_idx;
        }
    }
    pm_bk_info->valid_count[map_type]--;

    if (0 == pm_bk_info->valid_count[map_type]) {
        pm_bk_info->valid_sw_idx_start[map_type] = 0;
        pm_bk_info->valid_sw_idx_end[map_type] = 0;
        pm_bk_info->empty_sw_idx_start[map_type] = 0;
        pm_bk_info->empty_sw_idx_end[map_type] = pm_dev_info->table_size - 1;
    }
    PRI_MAP_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_pri_map_tsn_sr_coldboot_init
 * Purpose:
 *   Restore bookkeeping information of TSN & SR priority map resource.
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_pri_map_tsn_sr_coldboot_init(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_tsn_sr_coldboot_init\n")));

#if defined(BCM_WARM_BOOT_SUPPORT)
    /* Warm boot should be false in cold boot case */
    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    dev_info = bk_info->tsn_dev_info;

    /* Only init TSN & SR since there is only TSN and SR HW */
    for (map_type = bcmTsnPriMapTypeTsnFlow;
         map_type <= bcmTsnPriMapTypeSrFlow;
         map_type++) {

        pm_dev_info = dev_info->tsn_pri_map_info[map_type];
        if (NULL == pm_dev_info ||
            (map_type == bcmTsnPriMapTypeTsnFlow &&
             !soc_feature(unit, soc_feature_tsn)) ||
            (map_type == bcmTsnPriMapTypeSrFlow &&
             !soc_feature(unit, soc_feature_tsn_sr))) {
            /* Need to check all map_type */
            continue;
        }

        assert(pm_dev_info->table_size > 0);
        /*
         * Do not need to init 1st profile, thus sw_idx start from 1
         * 1st profile is always initialized in bcmi_esw_tsn_pri_map_init
         */
        for (sw_idx = 1;
             sw_idx < (pm_dev_info->table_size);
             sw_idx++) {
            if (NULL == pm_dev_info->tsn_pri_map_delete) {
                return BCM_E_UNAVAIL;
            }
            /* Use delete for reset HW to default value */
            rv = pm_dev_info->tsn_pri_map_delete(unit, sw_idx);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "HW operation failed(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
    }
    return BCM_E_NONE;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *   bcmi_esw_tsn_pri_map_tsn_sr_reload
 * Purpose:
 *   Restore bookkeeping information of TSN & SR priority map resource.
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_pri_map_tsn_sr_reload(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcm_tsn_pri_map_t map_id;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcm_tsn_pri_map_config_t config;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_tsn_sr_reload\n")));

    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    dev_info = bk_info->tsn_dev_info;

    /* only recovery TSN & SR & TAF
     * since MTU/STU wb is done by MTU/STU module
     */
    for (map_type = 0; map_type < bcmTsnPriMapTypeCount; map_type++) {

        if (map_type == bcmTsnPriMapTypeEgressMtuProfile ||
            map_type == bcmTsnPriMapTypeEgressStuProfile ) {
            continue;
        }

        pm_dev_info = dev_info->tsn_pri_map_info[map_type];
        if (NULL == pm_dev_info ||
            (map_type == bcmTsnPriMapTypeTsnFlow &&
             !soc_feature(unit, soc_feature_tsn)) ||
            (map_type == bcmTsnPriMapTypeSrFlow &&
             !soc_feature(unit, soc_feature_tsn_sr)) ||
            (map_type == bcmTsnPriMapTypeTafGate &&
             !soc_feature(unit, soc_feature_tsn_taf))) {
            /* Need to check all map_type */
            continue;
        }

        assert(pm_dev_info->table_size > 0);
        /*
         * Do not need to check 1st profile, thus sw_idx start from 1
         * 1st profile is always initialized in bcmi_esw_tsn_pri_map_init
         */
        for (sw_idx = 1;
             sw_idx < (pm_dev_info->table_size);
             sw_idx++) {
            PRI_MAP_BK_LOCK(unit);
            PRI_MAP_INTF_CLR(unit, map_type, sw_idx);
            PRI_MAP_BK_UNLOCK(unit);
            if (NULL == pm_dev_info->tsn_pri_map_wb_hw_existed ||
                NULL == pm_dev_info->tsn_pri_map_get) {
                return BCM_E_UNAVAIL;
            }

            PRI_MAP_LOCK(unit);
            rv = pm_dev_info->tsn_pri_map_wb_hw_existed(unit, sw_idx);
            PRI_MAP_UNLOCK(unit);
            if (BCM_SUCCESS(rv)) {
                /* rv = BCM_E_NONE means HW existed */

                /* Set bit map as used */
                map_id = SW_IDX_TO_MAP_ID(sw_idx, map_type);
                PRI_MAP_BK_LOCK(unit);
                PRI_MAP_INTF_SET(unit, map_type, sw_idx);
                PRI_MAP_BK_UNLOCK(unit);

                /* Restore SW database from HW */
                rv = pm_dev_info->tsn_pri_map_get(unit, sw_idx, &config);
                if (BCM_FAILURE(rv)) {
                    PRI_MAP_BK_LOCK(unit);
                    /* Clear SW usage bitmap */
                    PRI_MAP_INTF_CLR(unit, map_type, sw_idx);
                    PRI_MAP_BK_UNLOCK(unit);

                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META_U(unit,
                                          "Read from HW failed(rv = %d)\n"),
                                          rv));
                    return rv;
                }

                /* Write via bcm set API to make sure bk info is also updated */
                bcm_esw_tsn_pri_map_set(unit, map_id, &config);
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_mtu_stu_wb_set
 * Purpose:
 *      Warmboot function provided for mtu/stu module doing warmboot recovery
 *      MTU/STU modules should have config and map_id when doing warmboot
 *      and this API is for MTU/STU modules to do WB recovery.
 * Parameters:
 *      unit   - (IN) Unit number
 *      map_id - (IN) Map ID
 *      config - (IN) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_pri_map_mtu_stu_wb_set(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcm_tsn_pri_map_type_t map_type = PRI_MAP_TYPE_GET(map_id);
    uint32 wb_sw_idx = MAP_ID_TO_SW_IDX(map_id);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_mtu_stu_wb_set\n")));

    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter NULL error handling */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Parameter check */
    if (BCM_TSN_PRI_MAP_INVALID == map_id) {
        return BCM_E_CONFIG;
    }

    /* Check if the map_id encoding is MTU/STU */
    if (!(PRI_MAP_ID_IS_TYPE(map_id, bcmTsnPriMapTypeEgressMtuProfile) ||
        PRI_MAP_ID_IS_TYPE(map_id, bcmTsnPriMapTypeEgressStuProfile))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "This WB recovery API is only for MTU & STU"
                              "Error map_id(%d)\n"),
                              map_id));
        return BCM_E_PARAM;
    }

    /* Check if the config map_type is MTU/STU */
    if (!((config->map_type == bcmTsnPriMapTypeEgressMtuProfile) ||
        (config->map_type == bcmTsnPriMapTypeEgressStuProfile))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "This WB recovery API is only for MTU & STU"
                              "Error map_type(%d)\n"),
                              config->map_type));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    PRI_MAP_BK_LOCK(unit);

    /* Set bit map as used */
    PRI_MAP_INTF_SET(unit, map_type, wb_sw_idx);

    /* Update valid start and end */
    if (pm_bk_info->valid_count[map_type] == 0) {
        pm_bk_info->valid_sw_idx_start[map_type] = wb_sw_idx;
        pm_bk_info->valid_sw_idx_end[map_type] = wb_sw_idx;
    } else if (wb_sw_idx < pm_bk_info->valid_sw_idx_start[map_type]) {
        pm_bk_info->valid_sw_idx_start[map_type] = wb_sw_idx;
    } else if (wb_sw_idx > pm_bk_info->valid_sw_idx_end[map_type]) {
        pm_bk_info->valid_sw_idx_end[map_type] = wb_sw_idx;
    }
    pm_bk_info->valid_count[map_type]++;

    /* Update empty start and end */
    if (pm_bk_info->empty_sw_idx_start[map_type] ==
        pm_bk_info->empty_sw_idx_end[map_type]) {
        /* Do nothing */
    } else if (wb_sw_idx == pm_bk_info->empty_sw_idx_start[map_type]) {
        /* Assume next one is real empty */
        pm_bk_info->empty_sw_idx_start[map_type]++;
    } else if (wb_sw_idx == pm_bk_info->empty_sw_idx_end[map_type]) {
        /* Assume previous one is real empty */
        pm_bk_info->empty_sw_idx_end[map_type]--;
    }
    PRI_MAP_BK_UNLOCK(unit);

    /* Write via bcm set API to make sure bk info is also updated */
    bcm_esw_tsn_pri_map_set(unit, map_id, config);

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_create
 * Purpose:
 *      Add a priority map config to hardware.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Priority Map configuration
 *      map_id - (OUT) Map ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_create(
    int unit,
    bcm_tsn_pri_map_config_t *config,
    bcm_tsn_pri_map_t *map_id)
{
    bcm_error_t rv, rv2;
    int i;
    uint32 sw_idx;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_config_t *bk_config = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_create\n")));

    /* Parameter NULL error handling */
    if ((NULL == config) || (NULL == map_id)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, config->map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[config->map_type];

    if ((config->map_type < bcmTsnPriMapTypeTsnFlow) ||
        (config->map_type >= bcmTsnPriMapTypeCount) ||
        (config->num_map_entries != (pm_dev_info->entry_size))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Error map_type(%d), num_map_entries(%d)\n"),
                              config->map_type,
                              config->num_map_entries));
        return BCM_E_PARAM;
    }

    PRI_MAP_LOCK(unit);

    /* Check if the same pri_map existed or not, return EXISTS if yes. */
    rv = bcmi_esw_tsn_pri_map_existed_check(unit, config, map_id);
    if (BCM_E_EXISTS == rv) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "config already existed, found map_id(%d)\n"),
                             *map_id));
        PRI_MAP_UNLOCK(unit);
        return BCM_E_EXISTS;
    } else if (BCM_E_NOT_FOUND != rv) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "find existed Priority Map config failed"
                             "(rv = %d)\n"),
                             rv));
        PRI_MAP_UNLOCK(unit);
        return rv;
    }

    /* Get a SW empty map_id */
    rv = bcmi_esw_tsn_pri_map_get_empty_map_id(
            unit,
            config->map_type,
            map_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "no empty map_id(rv = %d)\n"),
                              rv));
        PRI_MAP_UNLOCK(unit);
        return rv;
    }

    /* Set the config to HW */
    sw_idx = MAP_ID_TO_SW_IDX(*map_id);
    if ((config->map_type == bcmTsnPriMapTypeEgressMtuProfile) ||
        (config->map_type == bcmTsnPriMapTypeEgressStuProfile)) {
        /*
         * There is no HW operations for MTU/STU
         * MTU/STU are stored in bookkeeping bcmi_esw_tsn_pri_map_bk_info_t
         */
    } else if (NULL == pm_dev_info->tsn_pri_map_set) {
        /* Besides MTU/STU, instance should not be NULL */
        PRI_MAP_UNLOCK(unit);
        return BCM_E_UNAVAIL;
    } else {
        rv = pm_dev_info->tsn_pri_map_set(unit, sw_idx, config);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));

            /* Destroy the map_id got from above */
            rv2 = bcmi_esw_tsn_pri_map_destroy_map_id(unit, *map_id);
            if (BCM_FAILURE(rv2)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "clear usage bitmap failed(rv = %d)\n"),
                                      rv2));
                PRI_MAP_UNLOCK(unit);
                return rv2;
            }

            PRI_MAP_UNLOCK(unit);
            return rv;
        }
    }

    /* Increase reference cnt of MTU/STU profile_id */
    if (config->map_type == bcmTsnPriMapTypeEgressMtuProfile ||
        config->map_type == bcmTsnPriMapTypeEgressStuProfile) {
        for (i = 0;
             i < config->num_map_entries;
             i++) {
            if ((config->map_type == bcmTsnPriMapTypeEgressMtuProfile) &&
                (config->map_entries[i].flags &
                 BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID)) {
                /* Increase reference cnt of MTU profile_id */
                rv = bcmi_esw_tsn_mtu_profile_ref_inc(
                        unit, config->map_entries[i].profile_id);
            } else if (
                (config->map_type == bcmTsnPriMapTypeEgressStuProfile) &&
                (config->map_entries[i].flags &
                 BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID)) {
                /* Increase reference cnt of STU profile_id */
                rv = bcmi_esw_tsn_stu_profile_ref_inc(
                        unit, config->map_entries[i].profile_id);
            }
            if (BCM_FAILURE(rv)) {
                PRI_MAP_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "Increase MTU / STU reference cnt failed"
                                      "(rv = %d)\n"),
                                      rv));
                return rv;
            }

        }
    }

    PRI_MAP_BK_LOCK(unit);
    /* Update SW bookkeeping database */
    bk_data = &(pm_bk_info->data[config->map_type][sw_idx]);
    bk_data->map_id = *map_id;
    bk_data->ref_cnt = 0;
    bk_config = &(bk_data->config);
    sal_memcpy(bk_config,
               config,
               sizeof(bcm_tsn_pri_map_config_t));
    PRI_MAP_BK_UNLOCK(unit);
    PRI_MAP_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_set
 * Purpose:
 *      Set priority map config with the given map_id.
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Map ID
 *      config - (IN) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_set(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 ref_cnt;
    uint32 sw_idx;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcm_tsn_pri_map_t exisited_map_id;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_config_t *bk_config = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_set\n")));

    /* Parameter NULL error handling */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Parameter check */
    if (BCM_TSN_PRI_MAP_INVALID == map_id) {
        return BCM_E_CONFIG;
    }

    bcmi_esw_tsn_pri_map_dump_config(unit, "config set", config);

    /* If parameter config->map_type is the same as map_type of map_id. */
    map_type = PRI_MAP_TYPE_GET(map_id);
    if ((config->map_type != map_type)) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "set wrong config to map_id(%d)\n"
                             "map_type doesn't match\n"), map_id));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    if ((config->map_type < bcmTsnPriMapTypeTsnFlow) ||
        (config->map_type >= bcmTsnPriMapTypeCount) ||
        (config->num_map_entries != (pm_dev_info->entry_size))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Error map_type(%d), num_map_entries(%d)\n"),
                              config->map_type,
                              config->num_map_entries));
        return BCM_E_PARAM;
    }

    PRI_MAP_LOCK(unit);

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        PRI_MAP_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }

    /* Check if the same pri_map existed or not */
    rv = bcmi_esw_tsn_pri_map_existed_check(unit, config, &exisited_map_id);
    if (BCM_E_EXISTS == rv) {
        if (exisited_map_id == map_id) {
            /* config is the same as config of current map_id */
            LOG_WARN(BSL_LS_BCM_TSN,
                     (BSL_META_U(unit,
                                 "config is the same as the config "
                                 "of current map_id(%d)\n"),
                                 map_id));
            PRI_MAP_UNLOCK(unit);
            return BCM_E_NONE;
        } else {
            /* config is found on other map_id */
            LOG_WARN(BSL_LS_BCM_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, found map_id(%d)\n"),
                                 exisited_map_id));
            PRI_MAP_UNLOCK(unit);
            return BCM_E_EXISTS;
        }
    }

    /* Check if there is any module using this pri_map id. */
    rv = bcmi_esw_tsn_pri_map_ref_cnt_get(unit, map_id, &ref_cnt);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "get reference count error(rv = %d)\n"),
                              rv));
        PRI_MAP_UNLOCK(unit);
        return rv;
    }

    /* There is still other module using the pri_map id, return E_BUSY. */
    if (ref_cnt != 0) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "unable to change the config of map_id(%d)\n"
                             "is still used by other (%d) component.\n"),
                             map_id,
                             ref_cnt));
        PRI_MAP_UNLOCK(unit);
        return BCM_E_BUSY;
    }

    /* Set the config to HW */
    sw_idx = MAP_ID_TO_SW_IDX(map_id);
    if ((map_type == bcmTsnPriMapTypeEgressMtuProfile) ||
        (map_type == bcmTsnPriMapTypeEgressStuProfile)) {
        /*
         * There is no HW operations for MTU/STU
         * MTU/STU are stored in bookkeeping bcmi_esw_tsn_pri_map_bk_info_t
         */
    } else if (NULL == pm_dev_info->tsn_pri_map_set) {
        /* Besides MTU/STU, instance should not be NULL */
        PRI_MAP_UNLOCK(unit);
        return BCM_E_UNAVAIL;
    } else {
        rv = pm_dev_info->tsn_pri_map_set(unit, sw_idx, config);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "write to HW failed(rv = %d)\n"),
                                  rv));
            PRI_MAP_UNLOCK(unit);
            return rv;
        }
    }

    PRI_MAP_BK_LOCK(unit);
    /* Update SW bookkeeping database */
    bk_data = &(pm_bk_info->data[map_type][sw_idx]);
    bk_config = &(bk_data->config);
    sal_memcpy(bk_config,
               config,
               sizeof(bcm_tsn_pri_map_config_t));
    PRI_MAP_BK_UNLOCK(unit);
    PRI_MAP_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_get
 * Purpose:
 *      Get the priority map config with the given map_id.
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Map ID
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 sw_idx;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_config_t *bk_config = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_get\n")));

    /* Parameter NULL error handling */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Parameter check */
    if (BCM_TSN_PRI_MAP_INVALID == map_id) {
        return BCM_E_CONFIG;
    }

    PRI_MAP_LOCK(unit);

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        PRI_MAP_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    /* Get the config from SW bookkeeping database */
    sw_idx = MAP_ID_TO_SW_IDX(map_id);
    map_type = PRI_MAP_TYPE_GET(map_id);

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    PRI_MAP_BK_LOCK(unit);
    bk_data = &(pm_bk_info->data[map_type][sw_idx]);
    bk_config = &(bk_data->config);
    sal_memcpy(config,
               bk_config,
               sizeof(bcm_tsn_pri_map_config_t));
    PRI_MAP_BK_UNLOCK(unit);
    PRI_MAP_UNLOCK(unit);

    bcmi_esw_tsn_pri_map_dump_config(unit, "config get", config);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_destroy
 * Purpose:
 *      Free the resource of map_id
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Priority Map ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_destroy(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    int i;
    uint32 ref_cnt;
    uint32 sw_idx;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_data_t *bk_data = NULL;
    bcm_tsn_pri_map_config_t *bk_config = NULL;

    /* Parameter check */
    if (BCM_TSN_PRI_MAP_INVALID == map_id) {
        return BCM_E_CONFIG;
    }

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_destroy\n")));

    PRI_MAP_LOCK(unit);

    /* Check if pri_map id existed */
    rv = bcmi_esw_tsn_pri_map_id_existed_check(unit, map_id);
    if (BCM_FAILURE(rv)) {
        PRI_MAP_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "map_id(%d) doesn't exited, rv(%d)\n"),
                             map_id,
                             rv));
        return rv;
    }
    map_type = PRI_MAP_TYPE_GET(map_id);

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Initialization check */
    PRI_MAP_INIT(unit, map_type);

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];

    /* Check if there is any module using this pri_map id. */
    rv = bcmi_esw_tsn_pri_map_ref_cnt_get(unit, map_id, &ref_cnt);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "get reference count error(rv = %d)\n"),
                              rv));
        PRI_MAP_UNLOCK(unit);
        return rv;
    }

    /* There is still other module using the pri_map id, return E_BUSY. */
    if (ref_cnt != 0) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "unable to delete map_id(%d)\n"
                             "is still used by other (%d) component.\n"),
                             map_id,
                             ref_cnt));
        PRI_MAP_UNLOCK(unit);
        return BCM_E_BUSY;
    }
    sw_idx = MAP_ID_TO_SW_IDX(map_id);

    /* Delete HW */
    if ((map_type == bcmTsnPriMapTypeEgressMtuProfile) ||
        (map_type == bcmTsnPriMapTypeEgressStuProfile)) {
        /*
         * There is no HW operations for MTU/STU
         * MTU/STU are stored in bookkeeping bcmi_esw_tsn_pri_map_bk_info_t
         */
    } else if (NULL == pm_dev_info->tsn_pri_map_delete) {
        /* Besides MTU/STU, instance should not be NULL */
        PRI_MAP_UNLOCK(unit);
        return BCM_E_UNAVAIL;
    } else {
        if (sw_idx == 0) {
            /* cannot delete 1st default setting */
            return BCM_E_NONE;
        }
        rv = pm_dev_info->tsn_pri_map_delete(unit, sw_idx);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "Delete HW failed(rv = %d)\n"),
                                  rv));
            PRI_MAP_UNLOCK(unit);
            return rv;
        }
    }

    /* Clear SW usage bitmap */
    rv = bcmi_esw_tsn_pri_map_destroy_map_id(unit, map_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "clear usage bitmap failed(rv = %d)\n"),
                              rv));
        PRI_MAP_UNLOCK(unit);
        return rv;
    }

    bk_data = &(pm_bk_info->data[map_type][sw_idx]);
    bk_config = &(bk_data->config);
    /* Decrease reference cnt of MTU/STU profile_id */
    if (map_type == bcmTsnPriMapTypeEgressMtuProfile ||
        map_type == bcmTsnPriMapTypeEgressStuProfile) {
        for (i = 0;
             i < bk_config->num_map_entries;
             i++) {
            if ((map_type == bcmTsnPriMapTypeEgressMtuProfile) &&
                (bk_config->map_entries[i].flags &
                 BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID)) {
                /* Decrease reference cnt of MTU profile_id */
                rv = bcmi_esw_tsn_mtu_profile_ref_dec(
                        unit, bk_config->map_entries[i].profile_id);
            } else if ((map_type == bcmTsnPriMapTypeEgressStuProfile) &&
                       (bk_config->map_entries[i].flags &
                        BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID)) {
                /* Decrease reference cnt of STU profile_id */
                rv = bcmi_esw_tsn_stu_profile_ref_dec(
                        unit, bk_config->map_entries[i].profile_id);
            }
            if (BCM_FAILURE(rv)) {
                PRI_MAP_UNLOCK(unit);
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "Decrease MTU / STU reference cnt failed"
                                      "(rv = %d)\n"),
                                      rv));
                return rv;
            }
        }
    }
    PRI_MAP_BK_LOCK(unit);

    /* Update SW bookkeeping database */
    bk_data->map_id = BCM_TSN_PRI_MAP_INVALID;
    bk_data->ref_cnt = 0;
    sal_memset(bk_config,
               0,
               sizeof(bcm_tsn_pri_map_config_t));
    PRI_MAP_BK_UNLOCK(unit);
    PRI_MAP_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_pri_map_traverse
 * Purpose:
 *      Traverse the priority map config information.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_traverse(
    int unit,
    bcm_tsn_pri_map_traverse_cb cb,
    void *user_data)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    uint32 sw_idx;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_traverse\n")));

    if (NULL == cb) {
        return BCM_E_PARAM;
    }

    for (map_type = bcmTsnPriMapTypeTsnFlow;
         map_type < bcmTsnPriMapTypeCount;
         map_type++) {

        /* Initialization check */
        PRI_MAP_INIT(unit, map_type);
        PRI_MAP_LOCK(unit);
        for (sw_idx = PRI_MAP_BK_INFO(unit)->valid_sw_idx_start[map_type];
             sw_idx <= PRI_MAP_BK_INFO(unit)->valid_sw_idx_end[map_type];
             sw_idx++) {

            if (PRI_MAP_INTF_GET(unit, map_type, sw_idx)) {
                /* Doing the user cb function with config */
                rv = cb(unit, SW_IDX_TO_MAP_ID(sw_idx, map_type), user_data);
                if (BCM_FAILURE(rv)) {
                    PRI_MAP_UNLOCK(unit);
                    return rv;
                }
            }
        }
        PRI_MAP_UNLOCK(unit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_pri_map_cleanup
 * Purpose:
 *     Internal function to clear the resource of the TSN module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_pri_map_cleanup(int unit)
{
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_cleanup\n")));

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    /* Free and clear the priority map bookkeeping info structure */
    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);

    for (map_type = bcmTsnPriMapTypeTsnFlow;
         map_type < bcmTsnPriMapTypeCount;
         map_type++) {
        if (pm_bk_info->intf_bitmap[map_type] != NULL) {
            TSN_FREE(unit, pm_bk_info->intf_bitmap[map_type], 0);
            pm_bk_info->intf_bitmap[map_type] = NULL;
        }
        if (pm_bk_info->data[map_type] != NULL) {
            TSN_FREE(unit, pm_bk_info->data[map_type], 0);
            pm_bk_info->data[map_type] = NULL;
        }

        /* Reset to default setting */
        pm_bk_info->valid_count[map_type] = 0;
        pm_bk_info->valid_sw_idx_start[map_type] = 0;
        pm_bk_info->valid_sw_idx_end[map_type] = 0;
        pm_bk_info->empty_sw_idx_start[map_type] = 0;
        pm_bk_info->empty_sw_idx_end[map_type] = 0;
    }

    if (pm_bk_info->mutex != NULL) {
        sal_mutex_destroy(pm_bk_info->mutex);
        pm_bk_info->mutex = NULL;
    }

    if (pm_bk_info->bk_mutex != NULL) {
        sal_mutex_destroy(pm_bk_info->bk_mutex);
        pm_bk_info->bk_mutex = NULL;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_tsn_pri_map_init
 * Purpose:
 *      create a default profile setting
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_pri_map_init(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_pri_map_info_t *pm_dev_info = NULL;
    bcmi_esw_tsn_pri_map_bk_info_t *pm_bk_info = NULL;
    int i, pri_map_id;
    bcm_tsn_pri_map_type_t map_type = bcmTsnPriMapTypeCount;
    bcm_tsn_pri_map_config_t config;
    uint32 table_size, shr_bitdcl_size, bk_data_size;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_init\n")));

    /* error check when bit is not enough for bcm_tsn_pri_map_type */
    assert((1 << (32 - BCMI_TSN_PRI_MAP_TYPE_SHIFT - 1)) >=
            bcmTsnPriMapTypeCount);

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));

    pm_bk_info = &(bk_info->tsn_pri_map_bk_info);
    dev_info = bk_info->tsn_dev_info;

    /* Check if all device info instances of Priority Map are NULL */
    for (map_type = bcmTsnPriMapTypeTsnFlow;
         map_type < bcmTsnPriMapTypeCount;
         map_type++) {
        pm_dev_info = dev_info->tsn_pri_map_info[map_type];
        if (NULL != pm_dev_info) {
            break;
        }
    }

    /* All device info instances of Priority Map are NULL */
    if (map_type == bcmTsnPriMapTypeCount) {
        LOG_WARN(BSL_LS_BCM_SWITCH,
                 (BSL_META_U(unit,
                             "Priority Map not supported:\n"
                             "platform initialized, but none"
                             "Priority Map services are available.\n")));
        return BCM_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_pri_map_init: Priority Map\n")));

    /* Allocate Priority Map bookkeeping resource, also do initialization */
    for (map_type = bcmTsnPriMapTypeTsnFlow;
         map_type < bcmTsnPriMapTypeCount;
         map_type++) {

        pm_dev_info = dev_info->tsn_pri_map_info[map_type];
        if (NULL == pm_dev_info ||
            (map_type == bcmTsnPriMapTypeSrFlow &&
             !soc_feature(unit, soc_feature_tsn_sr)) ||
            (map_type == bcmTsnPriMapTypeEgressMtuProfile &&
             !soc_feature(unit, soc_feature_tsn_mtu_stu)) ||
            (map_type == bcmTsnPriMapTypeEgressStuProfile &&
             !soc_feature(unit, soc_feature_tsn_mtu_stu)) ||
            (map_type == bcmTsnPriMapTypeTafGate &&
             !soc_feature(unit, soc_feature_tsn_taf))) {
            /* Some chips only have some PriMapType */
            continue;
        }

        /* table_size should large than 0 */
        assert(pm_dev_info->table_size > 0);
        table_size = pm_dev_info->table_size;

        /* Allocate usage bitmap */
        shr_bitdcl_size = SHR_BITALLOCSIZE(table_size);
        TSN_ALLOC(unit, pm_bk_info->intf_bitmap[map_type], SHR_BITDCL,
                  shr_bitdcl_size,
                  "Priority bookkeeping intf_bitmap", 0, rv);
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "bcmi_esw_tsn_pri_map_init: failed to "
                                    "allocate memory\n")));
            return rv;
        }
        sal_memset(pm_bk_info->intf_bitmap[map_type], 0, shr_bitdcl_size);

        /* Allocate data */
        bk_data_size =
            sizeof(bcmi_esw_tsn_pri_map_bk_info_data_t) * table_size;
        TSN_ALLOC(unit, pm_bk_info->data[map_type],
                  bcmi_esw_tsn_pri_map_bk_info_data_t,
                  bk_data_size,
                  pm_dev_info->table_name, 0, rv);
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "bcmi_esw_tsn_pri_map_init: failed to "
                                    "allocate memory\n")));
            return rv;
        }
        sal_memset(pm_bk_info->data[map_type], 0, bk_data_size);

        /* Initialize default value */
        for (i = 0; i < table_size; i++) {
            pm_bk_info->data[map_type][i].map_id = BCM_TSN_PRI_MAP_INVALID;
        }

        /* Initialize default valid & empty pointer */
        pm_bk_info->valid_count[map_type] = 0;
        pm_bk_info->valid_sw_idx_start[map_type] = 0;
        pm_bk_info->valid_sw_idx_end[map_type] = 0;
        pm_bk_info->empty_sw_idx_start[map_type] = 0;
        pm_bk_info->empty_sw_idx_end[map_type] = table_size - 1;
    }

    /* Create mutex */
    pm_bk_info->mutex = sal_mutex_create("priority_map_mutex");
    if (pm_bk_info->mutex == NULL) {
        return BCM_E_MEMORY;
    }

    /* Create bk_mutex */
    pm_bk_info->bk_mutex = sal_mutex_create("priority_map_bk_mutex");
    if (pm_bk_info->bk_mutex == NULL) {
        return BCM_E_MEMORY;
    }

    /* Create 1st default TSN profile */
    map_type = bcmTsnPriMapTypeTsnFlow;
    pm_dev_info = dev_info->tsn_pri_map_info[map_type];
    bcm_tsn_pri_map_config_t_init(&config);
    config.map_type = map_type;
    config.num_map_entries = pm_dev_info->entry_size;
    for (i = 0; i < config.num_map_entries; i++) {
        bcm_tsn_pri_map_entry_t_init(&config.map_entries[i]);
        config.map_entries[i].flags = BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET;
        config.map_entries[i].flags |= BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI;
        config.map_entries[i].flow_offset = 0;
        config.map_entries[i].new_int_pri = i;
    }

    rv = bcmi_esw_tsn_pri_map_create(unit, &config, &pri_map_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "create default TSN profile failed(rv = %d)\n"),
                              rv));
        return rv;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Create 1st default SR profile */
        map_type = bcmTsnPriMapTypeSrFlow;
        pm_dev_info = dev_info->tsn_pri_map_info[map_type];
        bcm_tsn_pri_map_config_t_init(&config);
        config.map_type = map_type;
        config.num_map_entries = pm_dev_info->entry_size;
        for (i = 0; i < config.num_map_entries; i++) {
            bcm_tsn_pri_map_entry_t_init(&config.map_entries[i]);
            config.map_entries[i].flags = BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET;
            config.map_entries[i].flags |= BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI;
            config.map_entries[i].flow_offset = 0;
            config.map_entries[i].new_int_pri = i;
        }

        rv = bcmi_esw_tsn_pri_map_create(unit, &config, &pri_map_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "create default SR profile failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    if (soc_feature(unit, soc_feature_tsn_taf)) {
        /* reserve default taf profile at index 0 */
        PRI_MAP_INTF_SET(unit, bcmTsnPriMapTypeTafGate, 0);
    }
    return BCM_E_NONE;
}

/* TSN flow: free a chain of one or more continuous flow indexes */
STATIC int
bcmi_esw_tsn_flow_idx_free(int unit, tsn_flowidx_entry_t *entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowidx_entry_t *fid, *prev, *last;
    int eidx;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (entry == NULL) {
        return BCM_E_PARAM;
    }

    /* Protect access to the flow indexes */
    TSN_FLOW_MGMT_LOCK(unit);

    /* Traverse the list and insert the chain into the list */
    bkinfo = &tsn_bk->tsn_flows;
    eidx = entry->flow_idx;
    prev = NULL;
    for (fid = bkinfo->flowids_avail; fid != NULL; fid = fid->next) {

        /* If the index is smaller than current, insert before current */
        if (eidx < fid->flow_idx) {
            /* Find the last item in the supplied chain */
            for (last = entry; last->next != NULL; last = last->next);
            /* Connect remaining items to the chain */
            last->next = fid;
            /* Insert the chain to the list */
            if (NULL == prev) {
                bkinfo->flowids_avail = entry;
            } else {
                prev->next = entry;
            }
            TSN_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_NONE;
        }

        /* all indexes should be different */
        assert(eidx != fid->flow_idx);

        /* record the previous item */
        prev = fid;
    }

    /* The index is larger than all items in the list, append it at the end */
    if (NULL == prev) {
        bkinfo->flowids_avail = entry;
    } else {
        prev->next = entry;
    }
    TSN_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

/* TSN flow: allocate one or more continuous flow idx(s) */
STATIC int
bcmi_esw_tsn_flow_idx_allocate(
    int unit,
    int count,
    tsn_flowidx_entry_t **entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowidx_entry_t *fid, *begin, *prev, *bprev;
    int idx, bidx;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (count <= 0 || entry == NULL) {
        return BCM_E_PARAM;
    }

    /* Protect access to the flow indexes */
    TSN_FLOW_MGMT_LOCK(unit);

    /* Check if any index is available */
    bkinfo = &tsn_bk->tsn_flows;
    if (NULL == bkinfo->flowids_avail) {
        TSN_FLOW_MGMT_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    /*
     * Quick retrieval for typical case: count == 1.
     */
    if (1 == count) {
        /* Get one entry from the head */
        fid = bkinfo->flowids_avail;
        bkinfo->flowids_avail = fid->next;
        fid->next = NULL;
        *entry = fid;
        TSN_FLOW_MGMT_UNLOCK(unit);
        return BCM_E_NONE;
    }

    /* Traverse the list and search for enough continuous items to allocate */
    idx = bidx = bkinfo->max_flows; /* invalid current index */
    prev = bprev = begin = NULL;
    for (fid = bkinfo->flowids_avail; fid != NULL; fid = fid->next) {

        /* Check if it's continuous */
        if (idx + 1 != fid->flow_idx) {
            /* Index not continuous, restart the searching */
            idx = fid->flow_idx;
            bidx = idx;
            begin = fid;
            bprev = prev;
        } else if (fid->flow_idx - bidx + 1 == count) {

            /*
             * Found enough continuous flow indexes: update and return
             */

            /* Update the linked list by removing the chain from the list */
            if (NULL == bprev) {
                /* The chain is the head */
                bkinfo->flowids_avail = fid->next;
            } else {
                /* Connect previous one to the next of the chain */
                bprev->next = fid->next;
            }

            /* Terminate this chain and return */
            fid->next = NULL;
            *entry = begin;
            TSN_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_NONE;

        } else {
            /* continuous but not yet reached the count */
            idx++;
        }

        /* record the previous one */
        prev = fid;
    }

    /* Cannot find enough continuous entries */
    TSN_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_RESOURCE;
}

/*
 * De-allocate a TSN flowset and underlying flows with flowset index
 * This function should be called with flowset mutex locked (if made public)
 * and ref_count = 0.
 */
STATIC int
bcmi_esw_tsn_flowset_free(int unit, int fsid)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int rv = BCM_E_NONE;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fset = &bkinfo->flowsets[fsid];

    /* Free flow indexes */
    rv = bcmi_esw_tsn_flow_idx_free(unit, fset->flow_ide);

    /* Clear flowset data */
    fset->flow_ide = NULL;
    fset->num_flows = 0;

    return rv;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_destroy
 * Purpose:
 *     Destroy a TSN flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_destroy(int unit, bcm_tsn_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    if (fset->num_flows == 0) {
        /* Not allocated */
        return BCM_E_NOT_FOUND;
    }

    /* Check if it's still in use */
    TSN_FLOWSET_LOCK(fset);
    if (fset->ref_count != 0) {
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_BUSY;
    }

    /* Handle per-flow specific clean up */
    for (i = 0; i < fset->num_flows; i++) {
        TSN_FLOW_LOCK(unit, fsid + i);
        if (TSN_FLOW_MGMT_DEVINFO(unit)->clear_flow) {
            (void)TSN_FLOW_MGMT_DEVINFO(unit)->clear_flow(unit, fsid + i);
        }
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            (void)bcmi_esw_tsn_mtu_profile_ref_dec(
                unit, bkinfo->flows[fsid + i].ingress_mtu_profile);
        }
        TSN_FLOW_UNLOCK(unit, fsid + i);
    }

    /* De-allocate the flowset (note that mutex is always there) */
    rv = bcmi_esw_tsn_flowset_free(unit, fsid);
    TSN_FLOWSET_UNLOCK(fset);
    return rv;
}

/* Allocate a TSN flowset */
STATIC int
bcmi_esw_tsn_flowset_alloc(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    int *pfsid)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    bcm_tsn_pri_map_config_t pmcfg;
    tsn_flowidx_entry_t *fids;
    tsn_flowset_t *fset;
    int fsid;
    int num_flows;
    int rv;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (pfsid == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_flows;

    /* Get number of flows from this priority map */
    num_flows = 1;
    if (pri_map != BCM_TSN_PRI_MAP_INVALID) {
        /* call API to get priority map configuration */
        rv = bcm_esw_tsn_pri_map_get(unit, pri_map, &pmcfg);
        if (BCM_FAILURE(rv) || pmcfg.map_type != bcmTsnPriMapTypeTsnFlow) {
            return BCM_E_PARAM;
        }
        /* Number of flows is max flow offset + 1 */
        for (i = 0; i < pmcfg.num_map_entries; i++) {
            if ((pmcfg.map_entries[i].flags &
                 BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) &&
                (pmcfg.map_entries[i].flow_offset > num_flows - 1)) {
                num_flows = pmcfg.map_entries[i].flow_offset + 1;
            }
        }
    }

    /* Allocate flow indexes */
    rv = bcmi_esw_tsn_flow_idx_allocate(unit, num_flows, &fids);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * For efficiency and convenience, the flowset index is the base (first)
     * flow index so that we don't need to have another set of flowset
     * allocation/free mechanism which would take more time in this time
     * critical operation.
     */
    fsid = fids->flow_idx;
    assert(fsid >= 0 && fsid + num_flows <= bkinfo->max_flows);

    /* Write to the flowset */
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    assert(fset->num_flows == 0);
    fset->map_id = pri_map;
    fset->flow_ide = fids;
    fset->ref_count = 0;
    fset->num_flows = num_flows;
    TSN_FLOWSET_UNLOCK(fset);

    /* Return the allocated flowset index */
    *pfsid = fsid;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_create
 * Purpose:
 *     Create a TSN flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_flow_config_t *def_cfg,
    bcm_tsn_flowset_t *flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    bcm_tsn_flowset_t flowset_id;
    bcm_tsn_flow_config_t *flow;
    int fsid;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (def_cfg == NULL || flowset == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_flows;

    /* sanity check for platform specific routines */
    if (!TSN_FLOW_MGMT_DEVINFO(unit)->check_flow_config ||
        !TSN_FLOW_MGMT_DEVINFO(unit)->write_flow) {
        return BCM_E_UNAVAIL;
    }

    /* Validate flow configuration */
    BCM_IF_ERROR_RETURN(
        TSN_FLOW_MGMT_DEVINFO(unit)->check_flow_config(unit, def_cfg));

    /* Allocate flows and flowset. Basic init check is also done inside. */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_flowset_alloc(unit, pri_map, &fsid));
    flowset_id = TSN_FLOWSET_ID_FROM_IDX(fsid);

    /* Write to the flowset software copy */
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    sal_memcpy(&fset->config, def_cfg, sizeof(fset->config));

    /* Write to individual flows */
    for (i = 0; i < fset->num_flows; i++) {
        TSN_FLOW_LOCK(unit, fsid + i);
        flow = &bkinfo->flows[fsid + i];

        /* Store the flow configuration */
        sal_memcpy(flow, def_cfg, sizeof(*flow));

        /* Write to HW */
        rv = TSN_FLOW_MGMT_DEVINFO(unit)->write_flow(unit, fsid + i, flow);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_esw_tsn_flowset_destroy(unit, flowset_id);
            TSN_FLOW_UNLOCK(unit, fsid + i);
            TSN_FLOWSET_UNLOCK(fset);
            return rv;
        }
        TSN_FLOW_UNLOCK(unit, fsid + i);
    }
    TSN_FLOWSET_UNLOCK(fset);

    /* Convert to user flowset ID */
    *flowset = flowset_id;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_config_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;

    /* parameter check */
    if (pri_map == NULL || config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Return the value */
    *pri_map = fset->map_id;
    sal_memcpy(config, &fset->config, sizeof(fset->config));
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_status_get
 * Purpose:
 *     Get current status of specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     status           - (OUT) status structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_status_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    bcm_tsn_flowset_status_t *status)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;

    /* parameter check */
    if (status == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Return the value */
    status->active = fset->ref_count > 0 ?  1 : 0;
    status->count = fset->num_flows;
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_ref_inc
 * Purpose:
 *     Increase reference count for the specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_flowset_ref_inc(int unit, bcm_tsn_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Update the reference count */
    fset->ref_count++;
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_ref_dec
 * Purpose:
 *     Decrease reference count for the specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_flowset_ref_dec(int unit, bcm_tsn_flowset_t flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }

    /* Update the reference count */
    if (fset->ref_count > 0) {
        fset->ref_count--;
    }
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_traverse
 * Purpose:
 *     Traverse all created TSN flowsets
 * Parameters:
 *     unit             - (IN) unit number
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data supplied to the callback function
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_traverse(
    int unit,
    bcm_tsn_flowset_traverse_cb cb,
    void *user_data)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int rv = BCM_E_NONE;
    int i;

    /* parameter check */
    if (cb == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    /* Loop through the flowset array */
    bkinfo = &tsn_bk->tsn_flows;
    for (i = 0; i < bkinfo->max_flows; i++) {
        fset = &bkinfo->flowsets[i];
        TSN_FLOWSET_LOCK(fset);
        if (fset->num_flows != 0) {
            rv = cb(unit, TSN_FLOWSET_ID_FROM_IDX(i), user_data);
        }
        TSN_FLOWSET_UNLOCK(fset);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_flow_get
 * Purpose:
 *     Retrieve an individual TSN flow based on the flow offset from a flow set.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     index            - (IN) index of the flow
 *     flow_id          - (OUT) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flowset_flow_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    int index,
    bcm_tsn_flow_t *flow_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fsid;

    /* parameter check */
    if (flow_id == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fsid = TSN_FLOWSET_ID_TO_IDX(flowset);
    if (fsid < 0 || fsid >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }
    fset = &bkinfo->flowsets[fsid];
    TSN_FLOWSET_LOCK(fset);
    if (fset->num_flows == 0) {
        /* Not allocated */
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_NOT_FOUND;
    }
    if (index < 0 || index >= fset->num_flows) {
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_PARAM;
    }
    assert(fsid + index < bkinfo->max_flows);

    /* Got SW flow ID */
    *flow_id = (bcm_tsn_flow_t)TSN_FLOW_ID_FROM_IDX(fsid + index);
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flow_config_get
 * Purpose:
 *     Get configuration for the specified TSN flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flow_config_get(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    int fidx;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fidx = TSN_FLOW_ID_TO_IDX((int)flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[i].num_flows != 0) {
            if (bkinfo->flowsets[i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (i == -1) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Return the configuration */
    TSN_FLOW_LOCK(unit, fidx);
    sal_memcpy(config, &bkinfo->flows[fidx], sizeof(*config));
    TSN_FLOW_UNLOCK(unit, fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flow_config_set
 * Purpose:
 *     Set configuration for the specified TSN flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_flow_config_set(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flow_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowset_t *fset;
    int fidx;
    int rv;
    int i;

    /* parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    fidx = TSN_FLOW_ID_TO_IDX((int)flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }

    /* Validate if it's from a flowset */
    fset = NULL;
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[i].num_flows != 0) {
            fset = &bkinfo->flowsets[i];
            if (bkinfo->flowsets[i].num_flows > (fidx - i)) {
                break;
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
    }
    if (fset == NULL) {
        /* No valid flowsets */
        return BCM_E_NOT_FOUND;
    }

    /* Validate flow configuration */
    if (TSN_FLOW_MGMT_DEVINFO(unit)->check_flow_config) {
        rv = TSN_FLOW_MGMT_DEVINFO(unit)->check_flow_config(unit, config);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    /*
     * Configure the flow using the supplied configuration
     */
    TSN_FLOWSET_LOCK(fset);
    TSN_FLOW_LOCK(unit, fidx);
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Clear the old MTU profile id */
        if (TSN_FLOW_MGMT_DEVINFO(unit)->clear_flow_mtu) {
            rv = TSN_FLOW_MGMT_DEVINFO(unit)->clear_flow_mtu(
                    unit, fidx);
            if (BCM_FAILURE(rv)) {
                TSN_FLOW_UNLOCK(unit, fidx);
                TSN_FLOWSET_UNLOCK(fset);
                return rv;
            }
        } else {
            TSN_FLOW_UNLOCK(unit, fidx);
            TSN_FLOWSET_UNLOCK(fset);
            return BCM_E_UNAVAIL;
        }
        (void)bcmi_esw_tsn_mtu_profile_ref_dec(
            unit, bkinfo->flows[fidx].ingress_mtu_profile);
    }
    /* Copy to SW flow info */
    sal_memcpy(&bkinfo->flows[fidx], config, sizeof(*config));
    /* Write to HW */
    if (TSN_FLOW_MGMT_DEVINFO(unit)->write_flow) {
        rv = TSN_FLOW_MGMT_DEVINFO(unit)->write_flow(
                unit, fidx, &bkinfo->flows[fidx]);
        if (BCM_FAILURE(rv)) {
            TSN_FLOW_UNLOCK(unit, fidx);
            TSN_FLOWSET_UNLOCK(fset);
            return rv;
        }
    } else {
        TSN_FLOW_UNLOCK(unit, fidx);
        TSN_FLOWSET_UNLOCK(fset);
        return BCM_E_UNAVAIL;
    }
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        (void)bcmi_esw_tsn_mtu_profile_ref_inc(
            unit, bkinfo->flows[fidx].ingress_mtu_profile);
    }
    TSN_FLOW_UNLOCK(unit, fidx);
    TSN_FLOWSET_UNLOCK(fset);
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_esw_tsn_hw_flow_id_get
 * Purpose:
 *     Convert software TSN flow ID to hardware TSN flow ID
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) SW flow ID
 *     hw_id            - (OUT) HW flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_hw_flow_id_get(int unit, bcm_tsn_flow_t flow_id, uint32 *hw_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    int fidx;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (hw_id == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_flows;
    fidx = TSN_FLOW_ID_TO_IDX((int)flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }

    /* Call platform specific routine to convert */
    if (!TSN_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id) {
        return BCM_E_UNAVAIL;
    }
    return TSN_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id(unit, fidx, hw_id);
}

/*
 * Function:
 *     bcmi_esw_tsn_sw_flow_id_get
 * Purpose:
 *     Convert hardware TSN flow ID to software TSN flow ID
 * Parameters:
 *     unit             - (IN) unit number
 *     hw_id            - (IN) HW flow ID
 *     flow_id          - (OUT) SW flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_sw_flow_id_get(int unit, uint32 hw_id, bcm_tsn_flow_t *flow_id)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    const bcmi_esw_tsn_dev_info_t *dev_info;
    int fidx;

    /* parameter check */
    if (flow_id == NULL) {
        return BCM_E_PARAM;
    }

    if (tsn_bk_info[unit] == NULL) {
        /* Using device specific info that need no bk_info */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_noninit_dev_info_get(unit, &dev_info));
        if (!dev_info->tsn_flows_info->get_sw_flow_idx) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN(dev_info->tsn_flows_info->get_sw_flow_idx(
                                                        unit, hw_id, &fidx));
        *flow_id = TSN_FLOW_ID_FROM_IDX(fidx);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;

    /* Call platform specific routine to convert */
    if (!TSN_FLOW_MGMT_DEVINFO(unit)->get_sw_flow_idx) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(
        TSN_FLOW_MGMT_DEVINFO(unit)->get_sw_flow_idx(unit, hw_id, &fidx));

    /* Validate converted index */
    if (fidx < 0 || fidx >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }

    /* Done */
    *flow_id = TSN_FLOW_ID_FROM_IDX(fidx);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_flowset_identify
 * Purpose:
 *     TSN flow to flowset converstion (identify the flowset based on a flow)
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_flowset_identify(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flowset_t *flowset)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    int fidx;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (flowset == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_flows;
    fidx = TSN_FLOW_ID_TO_IDX((int)flow_id);
    if (fidx < 0 || fidx >= bkinfo->max_flows) {
        return BCM_E_PARAM;
    }

    /* Find the flowset */
    for (i = fidx; i >= 0; i--) {
        if (bkinfo->flowsets[i].num_flows != 0) {
            if (bkinfo->flowsets[i].num_flows > (fidx - i)) {
                /* Convert to user flowset ID */
                *flowset = TSN_FLOWSET_ID_FROM_IDX(i);
                return BCM_E_NONE;
            } else {
                /* The flow is not within this flowset */
                return BCM_E_NOT_FOUND;
            }
        }
    }

    /* No flowset found with index less then the flow */
    return BCM_E_NOT_FOUND;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* TSN flow: allocate specified flow idx(s) for warmboot restore */
STATIC int
bcmi_esw_tsn_flow_idx_restore(
    int unit,
    int index,
    int count,
    tsn_flowidx_entry_t **entry)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowidx_entry_t *fid, *prev, *bprev, *begin;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (count <= 0 || entry == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;

    /* Protect access to the flow indexes */
    TSN_FLOW_MGMT_LOCK(unit);

    /* Traverse the list and search for the specified items */
    begin = NULL;
    prev = bprev = NULL;
    for (fid = bkinfo->flowids_avail; fid != NULL; fid = fid->next) {
        if (fid->flow_idx == index) {
            if (begin == NULL) {
                /* Found the head */
                begin = fid;
                bprev = prev;
            }

            /* Check if we have got all the items */
            count--;
            if (count == 0) {
                /* Done. Removing the chain from the list */
                if (NULL == bprev) {
                    /* The chain was the head */
                    bkinfo->flowids_avail = fid->next;
                } else {
                    /* Connect previous one to the next of the chain */
                    bprev->next = fid->next;
                }

                /* Terminate this chain and return */
                fid->next = NULL;
                *entry = begin;
                TSN_FLOW_MGMT_UNLOCK(unit);
                return BCM_E_NONE;
            }
            index++;
        } else if (fid->flow_idx > index) {
            /* Not found in this sorted list */
            break;
        }

        /* record the previous one */
        prev = fid;
    }

    /* Cannot find the specified item or enough continuous entries */
    TSN_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_INTERNAL;
}

/* TSN flow management - warmboot scache size allocation */
STATIC int
bcmi_esw_tsn_flow_mgmt_wb_alloc(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    int stable_size = 0;
    int alloc_size;
    int rv;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* Calculate allocate size */
    alloc_size = 0;
    alloc_size += sizeof(uint16);       /* Number of actual flows */
    alloc_size +=                       /* Allocated flows */
        sizeof(tsn_flowset_wb_t) * bkinfo->max_flows;

    /* Allocate scache for warmboot */
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_TSN_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        /* Proceed with level 1 warmboot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/* TSN flow management - sync information to scache */
STATIC int
bcmi_esw_tsn_flow_mgmt_sync(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL, *scache;
    tsn_flowset_wb_t tsnflow;
    int stable_size = 0;
    tsn_flowset_t *fset;
    uint16 flows;
    int rv;
    int i;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_TSN_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(rv);
    assert(scache_ptr != NULL);

    /* Avoid flowsets to be created/destroyed at the same time */
    TSN_FLOW_MGMT_LOCK(unit);

    /* Loop through flowsets and save to scache */
    flows = 0;
    scache = scache_ptr + sizeof(uint16);
    for (i = 0; i < bkinfo->max_flows; i++) {
        fset = &bkinfo->flowsets[i];
        TSN_FLOWSET_LOCK(fset);
        if (fset->num_flows != 0) {
            tsnflow.flow_idx = i;
            tsnflow.map_id = fset->map_id;
            sal_memcpy(&tsnflow.config, &fset->config, sizeof(tsnflow.config));
            flows++;
            sal_memcpy(scache, &tsnflow, sizeof(tsnflow));
            scache += sizeof(tsnflow);
        }
        TSN_FLOWSET_UNLOCK(fset);
    }
    sal_memcpy(scache_ptr, &flows, sizeof(uint16)); /* Actual flowsets */

    /* Done */
    TSN_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

/* TSN flow management - reload flow management information from scache */
STATIC int
bcmi_esw_tsn_flow_mgmt_reload(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    tsn_flowset_wb_t tsnflow;
    int stable_size = 0;
    tsn_flowset_t *fset;
    tsn_flowidx_entry_t *flow_ide;
    bcm_tsn_pri_map_config_t pmcfg;
    uint16 flows, i;
    int num_flows;
    int rv;
    int j, idx;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_flows;
    if (!TSN_FLOW_MGMT_DEVINFO(unit)->read_flow) {
        return BCM_E_UNAVAIL;
    }

    /* Requires extended scache support level-2 warmboot */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TSN, BCM_TSN_WB_SCACHE_PART_TSN_FLOWS);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(rv);
    assert(scache_ptr != NULL);

    /* Avoid flowsets to be created/destroyed at the same time */
    TSN_FLOW_MGMT_LOCK(unit);

    /* Get number of saved flowsets */
    sal_memcpy(&flows, scache_ptr, sizeof(uint16));
    scache_ptr += sizeof(uint16);

    /* Restore flowsets */
    for (i = 0; i < flows; i++) {
        /* Read from scache */
        sal_memcpy(&tsnflow, scache_ptr, sizeof(tsnflow));
        scache_ptr += sizeof(tsnflow);

        /* Validate flow index */
        if (tsnflow.flow_idx < 0 ||
            tsnflow.flow_idx >= bkinfo->max_flows) {
            TSN_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Validate priority map and get number of flows inside this flowset */
        num_flows = 1;
        if (tsnflow.map_id != BCM_TSN_PRI_MAP_INVALID) {
            /* call API to get priority map configuration */
            rv = bcm_esw_tsn_pri_map_get(unit, tsnflow.map_id, &pmcfg);
            if (BCM_FAILURE(rv) || pmcfg.map_type != bcmTsnPriMapTypeTsnFlow) {
                TSN_FLOW_MGMT_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            /* Number of flows is max flow offset + 1 */
            for (j = 0; j < pmcfg.num_map_entries; j++) {
                if ((pmcfg.map_entries[j].flags &
                     BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET) &&
                    (pmcfg.map_entries[j].flow_offset > num_flows - 1)) {
                    num_flows = pmcfg.map_entries[j].flow_offset + 1;
                }
            }
        }

        /* Restore flow index entries */
        rv = bcmi_esw_tsn_flow_idx_restore(
                unit, tsnflow.flow_idx, num_flows, &flow_ide);
        if (BCM_FAILURE(rv)) {
            TSN_FLOW_MGMT_UNLOCK(unit);
            return BCM_E_INTERNAL;
        }

        /* Confirmed that this flowset can be restored */
        fset = &bkinfo->flowsets[tsnflow.flow_idx];
        TSN_FLOWSET_LOCK(fset);

        /* Restore the flowset */
        fset->flow_ide = flow_ide;
        fset->map_id = tsnflow.map_id;
        fset->num_flows = num_flows;
        fset->ref_count = 0;
        sal_memcpy(&fset->config, &tsnflow.config, sizeof(tsnflow.config));

        /* Restore the individual flows */
        for (j = 0; j < num_flows; j++) {
            idx = tsnflow.flow_idx + j;
            TSN_FLOW_LOCK(unit, idx);
            rv = TSN_FLOW_MGMT_DEVINFO(unit)->read_flow(
                    unit, idx, &bkinfo->flows[idx]);
            if (BCM_FAILURE(rv)) {
                TSN_FLOW_UNLOCK(unit, idx);
                TSN_FLOWSET_UNLOCK(fset);
                TSN_FLOW_MGMT_UNLOCK(unit);
                return rv;
            }
            TSN_FLOW_UNLOCK(unit, idx);
        }

        /* Done restoring this flowset */
        TSN_FLOWSET_UNLOCK(fset);
    }

    /* Done */
    TSN_FLOW_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* TSN flow management de-initialization */
STATIC int
bcmi_esw_tsn_flow_mgmt_detach(int unit)
{
    tsn_flows_bookkeeping_t *bkinfo;
    int i;

    /* parameter check */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* check if it's already cleared */
    bkinfo = &tsn_bk_info[unit]->tsn_flows;
    if (NULL == bkinfo) {
        return BCM_E_NONE;
    }

    /* de-allocate flow index pool */
    if (NULL != bkinfo->flowids_allocated) {
        sal_free(bkinfo->flowids_allocated);
        bkinfo->flowids_allocated = NULL;
    }

    /* de-allocate flow instances */
    if (NULL != bkinfo->flows) {
        sal_free(bkinfo->flows);
        bkinfo->flows = NULL;
    }

    /* de-allocate flow mutexes */
    if (NULL != bkinfo->flows_mutex) {
        for (i = 0; i < bkinfo->max_flows; i++) {
            if (bkinfo->flows_mutex[i] != NULL) {
                sal_mutex_destroy(bkinfo->flows_mutex[i]);
                bkinfo->flows_mutex[i] = NULL;
            }
        }
        sal_free(bkinfo->flows_mutex);
        bkinfo->flows_mutex = NULL;
    }

    /* clear flow index linked list */
    bkinfo->flowids_avail = NULL;

    /* de-allocate flowset instances */
    if (NULL != bkinfo->flowsets) {
        for (i = 0; i < bkinfo->max_flows; i++) {
            if (NULL != bkinfo->flowsets[i].mutex) {
                sal_mutex_destroy(bkinfo->flowsets[i].mutex);
                bkinfo->flowsets[i].mutex = NULL;
            }
        }
        sal_free(bkinfo->flowsets);
        bkinfo->flowsets = NULL;
    }

    /* destroy the flow management mutex */
    if (NULL != bkinfo->fmgmt_mutex) {
        sal_mutex_destroy(bkinfo->fmgmt_mutex);
        bkinfo->fmgmt_mutex = NULL;
    }

    return BCM_E_NONE;
}

/* TSN flow management initialization */
STATIC int
bcmi_esw_tsn_flow_mgmt_init(int unit)
{
    bcmi_esw_tsn_bk_info_t *tsn_bk;
    tsn_flows_bookkeeping_t *bkinfo;
    tsn_flowidx_entry_t *fid;
    int maxflows;
    int rv;
    int i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    /* Retrieve number of flows */
    bkinfo = &tsn_bk->tsn_flows;
    if (TSN_FLOW_MGMT_DEVINFO(unit) == NULL ||
        TSN_FLOW_MGMT_DEVINFO(unit)->get_num_flows == NULL) {
        return BCM_E_UNAVAIL;
    }
    rv = TSN_FLOW_MGMT_DEVINFO(unit)->get_num_flows(unit, &maxflows);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (maxflows <= 0) {
        /* No supported flows */
        return BCM_E_UNAVAIL;
    }
    bkinfo->max_flows = maxflows;

    /*
     * We need to minimize the time taken for picking a free flow index.
     * To achieve this, we use linked list with pre-allocated entries so that
     * we can do it in O(1).
     */

    /* Allocate flow index entry pool */
    bkinfo->flowids_allocated = (tsn_flowidx_entry_t *)sal_alloc(
            sizeof(tsn_flowidx_entry_t) * maxflows,
            "tsn flow ids");
    if (NULL == bkinfo->flowids_allocated) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN: failed to allocate flow IDs\n")));
        (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }

    /* Initialize flow ID list */
    fid = bkinfo->flowids_allocated;
    /* Prepare flow index entry linked list for quick ID allocation */
    for (i = 0; i < bkinfo->max_flows; i++, fid++) {
        /*
         * Add an entry to the head of the linked list.
         * The flow index is going backward so the final list can be in
         * increasing order (some allocation requires continuous flows).
         */
        fid->flow_idx = bkinfo->max_flows - i - 1;
        fid->next = bkinfo->flowids_avail;
        bkinfo->flowids_avail = fid;
    }

    /* Allocate array of flowset and flow instances */
    if (bkinfo->max_flows > 0) {
        /* The max number of flowsets should be the same as flows */
        bkinfo->flowsets = (tsn_flowset_t *)sal_alloc(
            sizeof(tsn_flowset_t) * bkinfo->max_flows, "tsn flowsets");
        if (NULL == bkinfo->flowsets) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("TSN: failed to allocate flowsets\n")));
            (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->flowsets, 0,
            sizeof(tsn_flowset_t) * bkinfo->max_flows);
        /* Pre-create mutexes of all flowsets for performance */
        for (i = 0; i < bkinfo->max_flows; i++) {
            bkinfo->flowsets[i].mutex = sal_mutex_create("tsn flowset");
            if (NULL == bkinfo->flowsets[i].mutex) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META("TSN: failed to create flowset mutex\n")));
                (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
                return BCM_E_MEMORY;
            }
        }

        /* Allocate array of flow instances and mutexes */
        bkinfo->flows = (bcm_tsn_flow_config_t *)sal_alloc(
            sizeof(bcm_tsn_flow_config_t) * maxflows, "tsn flows");
        if (NULL == bkinfo->flows) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("TSN: failed to allocate flows\n")));
            (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->flows, 0, sizeof(bcm_tsn_flow_config_t) * maxflows);
        bkinfo->flows_mutex = (sal_mutex_t *)sal_alloc(
            sizeof(sal_mutex_t) * maxflows, "tsn flows mutex");
        if (NULL == bkinfo->flows_mutex) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("TSN: failed to allocate flow mutexes\n")));
            (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        for (i = 0; i < maxflows; i++) {
            bkinfo->flows_mutex[i] = sal_mutex_create("tsn flow mutex");
            if (NULL == bkinfo->flows_mutex[i]) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META("TSN: failed to create flow mutex\n")));
                (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
                return BCM_E_MEMORY;
            }
        }
    }

    /* Create mutex for protecting flow management (allocate/free) */
    bkinfo->fmgmt_mutex = sal_mutex_create("tsn flow mgmt");
    if (NULL == bkinfo->fmgmt_mutex) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN: failed to create flow mgmt mutex\n")));
        (void)bcmi_esw_tsn_flow_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_ref_inc
 * Purpose:
 *      Increase the reference count for the
 *      specific mtu profile ID
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) profile ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_ref_inc(
    int unit,
    int mtu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int index, rv, min, max;
    bcm_tsn_mtu_profile_type_t type;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = dev_info->tsn_mtu_info;

    /* this function also check the validity of parameter profile id */
    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }

    MTU_BK_INIT(unit, type);
    ctrl_info->mtu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    bk_info->tsn_mtu_bk_info.mtu_refcount[type][index]++;
    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_ref_dec
 * Purpose:
 *      Decrease the reference count of the
 *      specific mtu profile ID
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) profile ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_ref_dec(
    int unit,
    int mtu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int index, rv, min, max;
    bcm_tsn_mtu_profile_type_t type;

    /* Check whether the module is being detached */
    if (tsn_bk_info[unit] == NULL && tsn_bk_info_initialized == 1) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = dev_info->tsn_mtu_info;

    /* this function also check the validity of parameter profile id */
    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }

    MTU_BK_INIT(unit, type);
    /* Cannot minus the reference count that is already 0 */
    ctrl_info->mtu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_mtu_bk_info.mtu_refcount[type][index] == 0) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    bk_info->tsn_mtu_bk_info.mtu_refcount[type][index]--;
    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_ref_inc
 * Purpose:
 *      Increase the reference count for the
 *      specific stu profile ID
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) profile ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_ref_inc(
    int unit,
    int stu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int index, rv, min, max;
    bcm_tsn_stu_profile_type_t type;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = dev_info->tsn_stu_info;

    /* this function also check the validity of parameter profile id */
    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        STU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }

    STU_BK_INIT(unit, type);
    ctrl_info->stu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        STU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    bk_info->tsn_stu_bk_info.stu_refcount[type][index]++;
    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_ref_dec
 * Purpose:
 *      Decrease the reference count for the
 *      specific stu profile ID
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) profile ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_ref_dec(
    int unit,
    int stu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int index, rv, min, max;
    bcm_tsn_stu_profile_type_t type;

    /* Check whether the module is being detached */
    if (tsn_bk_info[unit] == NULL && tsn_bk_info_initialized == 1) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = dev_info->tsn_stu_info;

    /* this function also check the validity of parameter profile id */
    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        STU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }

    STU_BK_INIT(unit, type);
    /* Cannot minus the reference count that is already 0 */
    ctrl_info->stu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        STU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_stu_bk_info.stu_refcount[type][index] == 0) {
        STU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    bk_info->tsn_stu_bk_info.stu_refcount[type][index]--;
    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_mtu_hw_profile_id_get
 * Purpose:
 *     Convert software mtu profile ID to hardware mtu profile ID
 * Parameters:
 *     unit - (IN) unit number
 *     mtu_profile_id - (IN) SW profile ID
 *     hw_profile_id - (OUT) HW profile ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_mtu_hw_profile_id_get(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    int *hw_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    int rv;

    if (hw_profile_id == NULL || type == NULL) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;
    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id,
                                       type, hw_profile_id);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    MTU_BK_UNLOCK(unit);

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcmi_esw_tsn_mtu_sw_profile_id_get
 * Purpose:
 *     Convert hardware mtu profile ID to software mtu profile ID
 * Parameters:
 *     unit - (IN) unit number
 *     hw_profile_id - (IN) HW profile ID
 *     mtu_profile_id - (OUT) SW profile ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_mtu_sw_profile_id_get(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    int hw_profile_id,
    int *mtu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info;
    int rv;

    if (mtu_profile_id == NULL) {
        return BCM_E_UNAVAIL;
    }

    if (type != bcmTsnMtuProfileTypeIngress &&
        type != bcmTsnMtuProfileTypeEgress) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_UNAVAIL;
    }

    if (tsn_bk_info[unit] == NULL) {
        /* Using device specific info that need no bk_info */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_noninit_dev_info_get(unit, &dev_info));
        if (NULL == dev_info->tsn_mtu_info->mtu_profile_encode) {
            return BCM_E_UNAVAIL;
        }
        rv = dev_info->tsn_mtu_info->mtu_profile_encode(unit, hw_profile_id,
                                                        type, mtu_profile_id);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_encode(unit, hw_profile_id,
                                       type, mtu_profile_id);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    MTU_BK_UNLOCK(unit);

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcmi_esw_tsn_stu_hw_profile_id_get
 * Purpose:
 *     Convert software stu profile ID to hardware stu profile ID
 * Parameters:
 *     unit - (IN) unit number
 *     stu_profile_id - (IN) SW profile ID
 *     hw_profile_id - (OUT) HW profile ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stu_hw_profile_id_get(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_profile_type_t *type,
    int *hw_profile_id)

{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    int rv;

    if (hw_profile_id == NULL || type == NULL) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;
    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id,
                                       type, hw_profile_id);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    STU_BK_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_stu_sw_profile_id_get
 * Purpose:
 *     Convert hardware stu profile ID to software stu profile ID
 * Parameters:
 *     unit - (IN) unit number
 *     hw_profile_id - (IN) HW profile ID
 *     stu_profile_id - (OUT) SW profile ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stu_sw_profile_id_get(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    int hw_profile_id,
    int *stu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info;
    int rv;

    if (stu_profile_id == NULL) {
        return BCM_E_UNAVAIL;
    }

    if (type != bcmTsnStuProfileTypeIngress &&
        type != bcmTsnStuProfileTypeEgress) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_UNAVAIL;
    }

    if (tsn_bk_info[unit] == NULL) {
        /* Using device specific info that need no nk_info */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_noninit_dev_info_get(unit, &dev_info));
        if (NULL == dev_info->tsn_stu_info->stu_profile_encode) {
            return BCM_E_UNAVAIL;
        }
        rv = dev_info->tsn_stu_info->stu_profile_encode(unit, hw_profile_id,
                                                        type, stu_profile_id);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_encode(unit, hw_profile_id,
                                       type, stu_profile_id);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    STU_BK_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_get
 * Purpose:
 *      Get mtu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 *      type - (OUT) Indicates ingress or egress mtu profile
 *      config - (OUT) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_get(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    bcm_tsn_mtu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int index, rv;

    if (type == NULL || config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;
    MTU_BK_LOCK(unit);
    /* this function also check the validity of parameter profile id */
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id, type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->mtu_profile_get(unit, index, *type, config);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_set
 * Purpose:
 *      Set mtu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 *      config - (IN) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_set(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int index, ref_count, rv, min, max;
    bcm_tsn_mtu_profile_type_t type;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (config->size <= 0) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Invalid MTU size.\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    MTU_BK_LOCK(unit);
    /* this function also check the validity of parameter profile id */
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(
                  unit, "Profile ID 0 is the default, can't modify it.\n")));
        return BCM_E_PARAM;
    }

    rv = ctrl_info->mtu_profile_mem_refcnt_get(unit, type, index, &ref_count);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    /* ref_count > 0, then the entry is created */
    if (ref_count <= 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "NTU Profile entry is not created.\n")));
        return BCM_E_PARAM;
    }

    /* the profile entry is referenced by other flows */
    MTU_BK_INIT(unit, type);
    ctrl_info->mtu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_mtu_bk_info.mtu_refcount[type][index] > 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "Flow is in use of Ingress MTU Profile\n")));
        return BCM_E_BUSY;
    }

    rv = ctrl_info->mtu_profile_set(unit, index, type, config);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_destroy
 * Purpose:
 *      Clear the mtu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_destroy(
    int unit,
    int mtu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    bcm_tsn_mtu_profile_type_t type;
    int index, rv, min, max;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* this function also check the validity of parameter profile id */
    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_decode(unit, mtu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(
                  unit, "Profile ID 0 is the default, can't modify it.\n")));
        return BCM_E_PARAM;
    }

    rv = ctrl_info->mtu_profile_mem_idx_get(unit, type, &min, &max);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    MTU_BK_INIT(unit, type);
    /* the profile entry is referenced bt other flows */
    if (index < min || index > max) {
        MTU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_mtu_bk_info.mtu_refcount[type][index] != 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "Flow is in use of Ingress MTU Profile\n")));
        return BCM_E_EXISTS;
    }

    rv = ctrl_info->mtu_profile_destroy(unit, type, index);
    MTU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_create
 * Purpose:
 *      Create mtu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress mtu profile
 *      config - (IN) configuration of the mtu profile
 *      mtu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_create(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    bcm_tsn_mtu_config_t *config,
    int *mtu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int ref_count, rv, rv1;
    uint32 index;

    if (config == NULL || mtu_profile_id == NULL) {
        return BCM_E_PARAM;
    }

    if (config->size <= 0) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Invalid MTU size.\n")));
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* Check the validity of the type */
    if (type != bcmTsnMtuProfileTypeIngress &&
        type != bcmTsnMtuProfileTypeEgress) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_PARAM;
    }

    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_create(unit, type, config, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->mtu_profile_mem_refcnt_get(unit, type, index, &ref_count);
    if (BCM_FAILURE(rv)) {
        rv1 = ctrl_info->mtu_profile_destroy(unit, type, index);
        if (BCM_FAILURE(rv1)) {
            MTU_BK_UNLOCK(unit);
            return rv1;
        }
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* Encode the mtu profile id */
    rv = ctrl_info->mtu_profile_encode(unit, index, type, mtu_profile_id);
    if (BCM_FAILURE(rv)) {
        rv1 = ctrl_info->mtu_profile_destroy(unit, type, index);
        if (BCM_FAILURE(rv1)) {
            MTU_BK_UNLOCK(unit);
            return rv1;
        }
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    /* ref_count > 1 indicates replicated mtu profile is created */
    if (ref_count > 1) {
        if (type == bcmTsnMtuProfileTypeIngress) {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, "
                                 "found ingress mtu profile id(%d)\n"),
                                 *mtu_profile_id));
        } else {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, "
                                 "found egress mtu profile id(%d)\n"),
                                 *mtu_profile_id));
        }
    }

    MTU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_mtu_profile_traverse(
    int unit,
    bcm_tsn_mtu_profile_traverse_cb cb,
    void *user_data)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_traverse(unit, cb, user_data);
    MTU_BK_UNLOCK(unit);

    return rv;

}


/*
 * Function:
 *      bcmi_esw_tsn_ingress_mtu_config_set
 * Purpose:
 *      Set the priority of source ingress
 *      mtu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the mtu profile priority
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_ingress_mtu_config_set(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    MTU_BK_LOCK(unit);
    rv = ctrl_info->ingress_mtu_config_set(unit, config);
    MTU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_ingress_mtu_config_get
 * Purpose:
 *      Get the priority of source ingress
 *      mtu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the mtu profile priority
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_ingress_mtu_config_get(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    int rv;

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    MTU_BK_LOCK(unit);
    rv = ctrl_info->ingress_mtu_config_get(unit, config);
    MTU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_tsn_port_control_egress_mtu_set
 * Purpose:
 *      Setup HW memory with pri_map_id and port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      pri_map_id - (IN) priority map id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_egress_mtu_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_t pri_map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    bcm_tsn_pri_map_t pre_pri_map_id;
    bcm_tsn_pri_map_config_t config;
    int rv;
    uint32 hw_index;
    bcm_tsn_pri_map_type_t type;
    bcm_port_t port;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }

    /* use pri_map_invalid to detach the pri_map_id from port*/
    if (pri_map_id == BCM_TSN_PRI_MAP_INVALID) {
        MTU_BK_LOCK(unit);
        pre_pri_map_id = bk_info->tsn_mtu_bk_info.pri_map_port[port];
        if (pre_pri_map_id != BCM_TSN_PRI_MAP_INVALID) {
            (void)bcmi_esw_tsn_pri_map_ref_cnt_dec(unit, pre_pri_map_id);
        }
        bk_info->tsn_mtu_bk_info.pri_map_port[port] = pri_map_id;
        MTU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }
    /* Check if map id match to the bcmTsnControlEgressMtuPriorityMap */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_pri_map_hw_index_get(unit, pri_map_id, &type, &hw_index));
    if (type != bcmTsnPriMapTypeEgressMtuProfile) {
        return BCM_E_PARAM;
    }
    rv = bcm_esw_tsn_pri_map_get(unit, pri_map_id, &config);
    BCM_IF_ERROR_RETURN(rv);

    if (bk_info->tsn_mtu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }

    MTU_BK_LOCK(unit);
    /*
     * Store map id into SW egress_mtu_pri_map with the corresponding
     * port number
     */
    /* Increase Priority Map Reference Count */
    rv = bcmi_esw_tsn_pri_map_ref_cnt_inc(unit, pri_map_id);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    pre_pri_map_id = bk_info->tsn_mtu_bk_info.pri_map_port[port];
    if (pre_pri_map_id != BCM_TSN_PRI_MAP_INVALID) {
       (void)bcmi_esw_tsn_pri_map_ref_cnt_dec(unit, pre_pri_map_id);
    }
    bk_info->tsn_mtu_bk_info.pri_map_port[port] = pri_map_id;

    /*
     * Write into HW memory to store config with the
     * corresponding port number
     */
    rv = ctrl_info->egress_mtu_port_control_set(unit, gport, &config);
    MTU_BK_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *      bcmi_tsn_port_control_egress_mtu_get
 * Purpose:
 *      get pri_map_id from HW memory with port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      pri_map_id - (OUT) priority map id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_egress_mtu_get(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_t *pri_map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    bcm_port_t port;

    if (pri_map_id == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_mtu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }
    /* retrieve priority map id from the prio_map_port sw data structure */
    MTU_BK_LOCK(unit);
    *pri_map_id = bk_info->tsn_mtu_bk_info.pri_map_port[port];
    MTU_BK_UNLOCK(unit);
    /* the priority map is not store in the specific port number */
    if (*pri_map_id == BCM_TSN_PRI_MAP_INVALID) {
        return BCM_E_EMPTY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_tsn_port_control_ingress_mtu_set
 * Purpose:
 *      Setup HW memory with profile id and port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      profile_id - (IN) ingress mtu profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_ingress_mtu_set(
    int unit,
    bcm_gport_t gport,
    int profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;

    int rv, index;
    bcm_tsn_mtu_profile_type_t type;
    bcm_port_t port;
    uint32 pre_profile_id;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }
    MTU_BK_LOCK(unit);
    rv = ctrl_info->mtu_profile_decode(unit, profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }

    if (type != bcmTsnMtuProfileTypeIngress) {
        MTU_BK_UNLOCK(unit);
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_PARAM;
    }
    /* detach the previous mtu_profile_id*/
    if (profile_id == 0) {
        rv = bcm_esw_tsn_port_control_get(unit, gport,
                                          bcmTsnControlIngressMtuProfile,
                                          &pre_profile_id);
        if (BCM_FAILURE(rv)) {
            MTU_BK_UNLOCK(unit);
            return rv;
        }

        rv = ctrl_info->ingress_mtu_port_control_set(unit, gport, 0);
        if (BCM_FAILURE(rv)) {
            MTU_BK_UNLOCK(unit);
            return rv;
        }
        (void)bcmi_esw_tsn_mtu_profile_ref_dec(unit, pre_profile_id);
        MTU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }
    rv = ctrl_info->ingress_mtu_port_control_set(unit, gport, index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    (void)bcmi_esw_tsn_mtu_profile_ref_inc(unit, profile_id);
    MTU_BK_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *      bcmi_tsn_port_control_ingress_mtu_get
 * Purpose:
 *      get profile id from HW memory with port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      profile id - (OUT) ingress mtu profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_ingress_mtu_get(
    int unit,
    bcm_gport_t gport,
    int *profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_mtu_info_t *ctrl_info = NULL;
    bcm_port_t port;

    int index, rv;
    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    MTU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }
    MTU_BK_LOCK(unit);
    rv = ctrl_info->ingress_mtu_port_control_get(unit, gport, &index);
    if (BCM_FAILURE(rv)) {
        MTU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->mtu_profile_encode(
            unit, index, bcmTsnMtuProfileTypeIngress, profile_id);
    MTU_BK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_get
 * Purpose:
 *      Get stu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 *      type - (OUT) Indicates ingress or egress stu profile
 *      config - (OUT) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_get(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_profile_type_t *type,
    bcm_tsn_stu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int index, rv;

    if (type == NULL || config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;
    STU_BK_LOCK(unit);
    /* this function also check the validity of parameter profile id */
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id, type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->stu_profile_get(unit, index, *type, config);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_set
 * Purpose:
 *      Set stu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 *      config - (IN) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_set(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int index, ref_count, rv, min, max;
    bcm_tsn_stu_profile_type_t type;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (config->size <= 0) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Invalid STU size.\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    STU_BK_LOCK(unit);
    /* this function also check the validity of parameter profile id */
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(
                  unit, "Profile ID 0 is the default, can't modify it.\n")));
        return BCM_E_PARAM;
    }

    rv = ctrl_info->stu_profile_mem_refcnt_get(unit, type, index, &ref_count);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    /* ref_count > 0, then the entry is created */
    if (ref_count <= 0) {
        STU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "MTU Profile entry is not created.\n")));
        return BCM_E_PARAM;
    }

    /* the profile entry is referenced by other flows */
    STU_BK_INIT(unit, type);
    ctrl_info->stu_profile_mem_idx_get(unit, type, &min, &max);
    if (index < min || index > max) {
        STU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_stu_bk_info.stu_refcount[type][index] > 0) {
        STU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "Flow is in use of Ingress STU Profile\n")));
        return BCM_E_BUSY;
    }

    rv = ctrl_info->stu_profile_set(unit, index, type, config);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_destroy
 * Purpose:
 *      Clear the stu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_destroy(
    int unit,
    int stu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    bcm_tsn_stu_profile_type_t type;
    int index, rv, min, max;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* this function also check the validity of parameter profile id */
    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_decode(unit, stu_profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* index 0 in profile memory is the default value */
    if (index == 0) {
        MTU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(
                  unit, "Profile ID 0 is the default, can't modify it.\n")));
        return BCM_E_PARAM;
    }

    rv = ctrl_info->stu_profile_mem_idx_get(unit, type, &min, &max);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    STU_BK_INIT(unit, type);
    /* the profile entry is referenced bt other flows */
    if (index < min || index > max) {
        STU_BK_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    if (bk_info->tsn_stu_bk_info.stu_refcount[type][index] != 0) {
        STU_BK_UNLOCK(unit);
        LOG_WARN(BSL_LS_BCMAPI_TSN,
                (BSL_META_U(unit,
                            "Flow is in use of Ingress STU Profile\n")));
        return BCM_E_EXISTS;
    }

    rv = ctrl_info->stu_profile_destroy(unit, type, index);
    STU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_create
 * Purpose:
 *      Create stu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress stu profile
 *      config - (IN) configuration of the stu profile
 *      stu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_create(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    bcm_tsn_stu_config_t *config,
    int *stu_profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int ref_count, rv, rv1;
    uint32 index;

    if (config == NULL || stu_profile_id == NULL) {
        return BCM_E_PARAM;
    }

    if (config->size <= 0) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Invalid STU size.\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* Check the validity of the type */
    if (type != bcmTsnStuProfileTypeIngress &&
        type != bcmTsnStuProfileTypeEgress) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_PARAM;
    }

    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_create(unit, type, config, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->stu_profile_mem_refcnt_get(unit, type, index, &ref_count);
    if (BCM_FAILURE(rv)) {
        rv1 = ctrl_info->stu_profile_destroy(unit, type, index);
        if (BCM_FAILURE(rv1)) {
            STU_BK_UNLOCK(unit);
            return rv1;
        }
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* Encode the stu profile id */
    rv = ctrl_info->stu_profile_encode(unit, index, type, stu_profile_id);
    if (BCM_FAILURE(rv)) {
        rv1 = ctrl_info->stu_profile_destroy(unit, type, index);
        if (BCM_FAILURE(rv1)) {
            STU_BK_UNLOCK(unit);
            return rv1;
        }
        STU_BK_UNLOCK(unit);
        return rv;
    }

    /* ref_count > 1 indicates replicated stu profile is created */
    if (ref_count > 1) {
        if (type == bcmTsnStuProfileTypeIngress) {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, "
                                 "found ingress stu profile id(%d)\n"),
                                 *stu_profile_id));
        } else {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                     (BSL_META_U(unit,
                                 "config already existed, "
                                 "found egress stu profile id(%d)\n"),
                                 *stu_profile_id));
        }
    }

    STU_BK_UNLOCK(unit);
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_stu_profile_traverse(
    int unit,
    bcm_tsn_stu_profile_traverse_cb cb,
    void *user_data)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_traverse(unit, cb, user_data);
    STU_BK_UNLOCK(unit);

    return rv;

}


/*
 * Function:
 *      bcmi_esw_tsn_ingress_stu_config_set
 * Purpose:
 *      Set the priority of source ingress
 *      stu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the stu profile priority
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_ingress_stu_config_set(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    STU_BK_LOCK(unit);
    rv = ctrl_info->ingress_stu_config_set(unit, config);
    STU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_ingress_stu_config_get
 * Purpose:
 *      Get the priority of source ingress
 *      stu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the stu profile priority
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_esw_tsn_ingress_stu_config_get(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    int rv;

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    STU_BK_LOCK(unit);
    rv = ctrl_info->ingress_stu_config_get(unit, config);
    STU_BK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_tsn_port_control_egress_stu_set
 * Purpose:
 *      Setup HW memory with pri_map_id and port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      pri_map_id - (IN) priority map id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_egress_stu_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_t pri_map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    bcm_tsn_pri_map_t pre_pri_map_id;
    bcm_tsn_pri_map_config_t config;
    int rv;
    uint32 hw_index;
    bcm_tsn_pri_map_type_t type;
    bcm_port_t port;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }

    /* use pri_map_invalid to detach the pri_map_id from port*/
    if (pri_map_id == BCM_TSN_PRI_MAP_INVALID) {
        STU_BK_LOCK(unit);
        pre_pri_map_id = bk_info->tsn_stu_bk_info.pri_map_port[port];
        if (pre_pri_map_id != BCM_TSN_PRI_MAP_INVALID) {
            (void)bcmi_esw_tsn_pri_map_ref_cnt_dec(unit, pre_pri_map_id);
        }
        bk_info->tsn_stu_bk_info.pri_map_port[port] = pri_map_id;
        STU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }
    /* Check if map id match to the bcmTsnControlEgressStuPriorityMap */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_pri_map_hw_index_get(unit, pri_map_id, &type, &hw_index));
    if (type != bcmTsnPriMapTypeEgressStuProfile) {
        return BCM_E_PARAM;
    }
    rv = bcm_esw_tsn_pri_map_get(unit, pri_map_id, &config);
    BCM_IF_ERROR_RETURN(rv);

    if (bk_info->tsn_stu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }

    STU_BK_LOCK(unit);
    /*
     * Store map id into SW egress_stu_pri_map with the corresponding
     * port number
     */
    /* Increase Priority Map Reference Count */
    rv = bcmi_esw_tsn_pri_map_ref_cnt_inc(unit, pri_map_id);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    pre_pri_map_id = bk_info->tsn_stu_bk_info.pri_map_port[port];
    if (pre_pri_map_id != BCM_TSN_PRI_MAP_INVALID) {
       (void)bcmi_esw_tsn_pri_map_ref_cnt_dec(unit, pre_pri_map_id);
    }
    bk_info->tsn_stu_bk_info.pri_map_port[port] = pri_map_id;

    /*
     * Write into HW memory to store config with the
     * corresponding port number
     */
    rv = ctrl_info->egress_stu_port_control_set(unit, port, &config);
    STU_BK_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *      bcmi_tsn_port_control_egress_stu_get
 * Purpose:
 *      get pri_map_id from HW memory with port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      pri_map_id - (OUT) priority map id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_egress_stu_get(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_t *pri_map_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    bcm_port_t port;

    if (pri_map_id == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));

        return BCM_E_PARAM;
    }
    if (bk_info->tsn_stu_bk_info.pri_map_port == NULL) {
        return BCM_E_INTERNAL;
    }
    /* retrieve priority map id from the prio_map_port sw data structure */
    STU_BK_LOCK(unit);
    *pri_map_id = bk_info->tsn_stu_bk_info.pri_map_port[port];
    STU_BK_UNLOCK(unit);
    /* the priority map is not store in the specific port number */
    if (*pri_map_id == BCM_TSN_PRI_MAP_INVALID) {
        return BCM_E_EMPTY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_tsn_port_control_ingress_stu_set
 * Purpose:
 *      Setup HW memory with profile id and port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      profile_id - (IN) ingress stu profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_ingress_stu_set(
    int unit,
    bcm_gport_t gport,
    int profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;

    int rv, index;
    bcm_tsn_stu_profile_type_t type;
    bcm_port_t port;
    uint32 pre_profile_id;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }
    STU_BK_LOCK(unit);
    rv = ctrl_info->stu_profile_decode(unit, profile_id, &type, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }

    if (type != bcmTsnStuProfileTypeIngress) {
        STU_BK_UNLOCK(unit);
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Wrong Profile Type.\n")));
        return BCM_E_PARAM;
    }
    /* detach the previous mtu_profile_id*/
    if (profile_id == 0) {
        rv = bcm_esw_tsn_port_control_get(unit, gport,
                                          bcmTsnControlIngressStuProfile,
                                          &pre_profile_id);
        if (BCM_FAILURE(rv)) {
            STU_BK_UNLOCK(unit);
            return rv;
        }

        rv = ctrl_info->ingress_stu_port_control_set(unit, gport, 0);
        if (BCM_FAILURE(rv)) {
            STU_BK_UNLOCK(unit);
            return rv;
        }
        (void)bcmi_esw_tsn_stu_profile_ref_dec(unit, pre_profile_id);
        STU_BK_UNLOCK(unit);
        return BCM_E_NONE;
    }
    rv = ctrl_info->ingress_stu_port_control_set(unit, gport, index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    (void)bcmi_esw_tsn_stu_profile_ref_inc(unit, profile_id);
    STU_BK_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *      bcmi_tsn_port_control_ingress_stu_get
 * Purpose:
 *      get profile id from HW memory with port information
 * Parameters:
 *      unit - (IN) Unit number
 *      port - (IN) port number
 *      profile id - (OUT) ingress stu profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_tsn_port_control_ingress_stu_get(
    int unit,
    bcm_gport_t gport,
    int *profile_id)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const tsn_stu_info_t *ctrl_info = NULL;
    bcm_port_t port;

    int index, rv;
    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    /* Use this init macro after the bk info instance get */
    STU_DEV_INIT(unit);
    ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= ctrl_info->logical_port_num) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Invalid Port Range, should be"
                                " %d\n"), ctrl_info->logical_port_num));
        return BCM_E_PARAM;
    }
    STU_BK_LOCK(unit);
    rv = ctrl_info->ingress_stu_port_control_get(unit, gport, &index);
    if (BCM_FAILURE(rv)) {
        STU_BK_UNLOCK(unit);
        return rv;
    }
    rv = ctrl_info->stu_profile_encode(
            unit, index, bcmTsnStuProfileTypeIngress, profile_id);
    STU_BK_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *     bcmi_esw_tsn_port_control_set
 * Description:
 *     Internal function for configure port-based operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Port number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (IN) argument to be set
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_port_control_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_control_t type,
    uint32 arg)
{
    if (type == bcmTsnControlEgressMtuPriorityMap) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_egress_mtu_set(unit, port, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlIngressMtuProfile) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_ingress_mtu_set(unit, port, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlEgressStuPriorityMap) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_egress_stu_set(unit, port, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlIngressStuProfile) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_ingress_stu_set(unit, port, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlTafGatePriMap) {
        if (soc_feature(unit, soc_feature_tsn_taf)) {
            uint32 hw_index;
            bcm_tsn_pri_map_type_t map;

            if (arg == (uint32)BCM_TSN_PRI_MAP_INVALID) {
                hw_index = 0;
            } else {
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_hw_index_get(unit, arg, &map,
                                                      &hw_index));
                if (map != bcmTsnPriMapTypeTafGate) {
                    return BCM_E_PARAM;
                }
            }
            return bcmi_esw_tsn_taf_port_control_set(unit, port, type,
                                                     hw_index);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlTafEnable ||
               type == bcmTsnControlTafGatePriMapL2Select) {
        if (soc_feature(unit, soc_feature_tsn_taf)) {
            return bcmi_esw_tsn_taf_port_control_set(unit, port, type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmi_esw_tsn_port_control_get
 * Description:
 *     Internal function for get the configure of port-based operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Port number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (OUT) argument got
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_port_control_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_control_t type,
    uint32 *arg)
{
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (type == bcmTsnControlEgressMtuPriorityMap) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_egress_mtu_get(
                       unit, port, (bcm_tsn_pri_map_t *)arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlIngressMtuProfile) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_ingress_mtu_get(
                       unit, port, (int *)arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlEgressStuPriorityMap) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_egress_stu_get(
                       unit, port, (bcm_tsn_pri_map_t *)arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlIngressStuProfile) {
        if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
            return bcmi_tsn_port_control_ingress_stu_get(
                       unit, port, (int *)arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlTafGatePriMap) {
        if (soc_feature(unit, soc_feature_tsn_taf)) {
            uint32 hw_index;
            bcm_tsn_pri_map_t sw_index;

            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_taf_port_control_get(unit, port, type, &hw_index));

            if (0 == hw_index) {
                sw_index = BCM_TSN_PRI_MAP_INVALID;
            } else {
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_pri_map_map_id_get(
                        unit, bcmTsnPriMapTypeTafGate, hw_index, &sw_index));
            }
            *arg = (uint32)sw_index;
            return BCM_E_NONE;
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (type == bcmTsnControlTafEnable ||
               type == bcmTsnControlTafGatePriMapL2Select) {
        if (soc_feature(unit, soc_feature_tsn_taf)) {
            return bcmi_esw_tsn_taf_port_control_get(unit, port, type, arg);
        } else {
            return BCM_E_UNAVAIL;
        }
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmi_esw_tsn_mtu_cleanup
 * Purpose:
 *     Internal function to clear the resource of the TSN module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_mtu_cleanup(int unit)
{

    bcm_tsn_mtu_profile_type_t type;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_mtu_cleanup\n")));

    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
        /* Free and clear the mtu bookkeeping info structure */
        if (bk_info->tsn_mtu_bk_info.mutex != NULL) {
            sal_mutex_destroy(bk_info->tsn_mtu_bk_info.mutex);
            bk_info->tsn_mtu_bk_info.mutex = NULL;
        }
        for (type = bcmTsnMtuProfileTypeIngress;
             type < bcmTsnMtuProfileTypeCount; type++) {
            if (bk_info->tsn_mtu_bk_info.mtu_refcount[type] != NULL) {
                TSN_FREE(unit,
                         bk_info->tsn_mtu_bk_info.mtu_refcount[type], 0);
                bk_info->tsn_mtu_bk_info.mtu_refcount[type] = NULL;
            }
        }
        if (bk_info->tsn_mtu_bk_info.pri_map_port != NULL) {
            TSN_FREE(unit,
                     bk_info->tsn_mtu_bk_info.pri_map_port, 0);
            bk_info->tsn_mtu_bk_info.pri_map_port = NULL;
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_esw_tsn_mtu_init
 * Purpose:
 *      create a default profile setting
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_mtu_init(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    int rv = BCM_E_NONE;
    int i, min, max, alloc_size;
    const tsn_mtu_info_t *ctrl_info = NULL;
    bcm_tsn_mtu_profile_type_t type;

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &bk_info));
    /* Allocate MTU bookkeeping structure */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        MTU_DEV_INIT(unit);
        ctrl_info = bk_info->tsn_dev_info->tsn_mtu_info;
        for (type = bcmTsnMtuProfileTypeIngress;
             type < bcmTsnMtuProfileTypeCount; type++) {
            if (bk_info->tsn_mtu_bk_info.mtu_refcount[type] == NULL) {
                ctrl_info->mtu_profile_mem_idx_get(unit, type, &min, &max);
                alloc_size = (max - min + 1) * sizeof(int);
                TSN_ALLOC(unit, bk_info->tsn_mtu_bk_info.mtu_refcount[type],
                          int, alloc_size, "Ingress MTU Reference Count",
                          0, rv);
            }
            if (BCM_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "bcmi_esw_tsn_init: failed to "
                                        "allocate memory\n")));
                return rv;
            }
        }
        if (bk_info->tsn_mtu_bk_info.pri_map_port == NULL) {
            alloc_size =
                ctrl_info->logical_port_num * sizeof(bcm_tsn_pri_map_t);
            TSN_ALLOC(unit, bk_info->tsn_mtu_bk_info.pri_map_port,
                      bcm_tsn_pri_map_t, alloc_size,
                      "MTU Priority Map Port Array", 0, rv);
        }
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "bcmi_esw_tsn_init: failed to "
                                    "allocate memory\n")));
            return rv;
        }
        /* Initialize pri_map_port array */
        for (i = 0; i < ctrl_info->logical_port_num; i++) {
            bk_info->tsn_mtu_bk_info.pri_map_port[i] = BCM_TSN_PRI_MAP_INVALID;
        }
        /* Create MTU mutex */
        bk_info->tsn_mtu_bk_info.mutex = sal_mutex_create("mtu_mutex");
        if (bk_info->tsn_mtu_bk_info.mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_stu_cleanup
 * Purpose:
 *     Internal function to clear the resource of the TSN module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stu_cleanup(int unit)
{

    bcm_tsn_stu_profile_type_t type;
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_stu_cleanup\n")));

    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
        /* Free and clear the stu bookkeeping info structure */
        if (bk_info->tsn_stu_bk_info.mutex != NULL) {
            sal_mutex_destroy(bk_info->tsn_stu_bk_info.mutex);
            bk_info->tsn_stu_bk_info.mutex = NULL;
        }
        for (type = bcmTsnStuProfileTypeIngress;
             type < bcmTsnStuProfileTypeCount; type++) {
            if (bk_info->tsn_stu_bk_info.stu_refcount[type] != NULL) {
                TSN_FREE(unit,
                         bk_info->tsn_stu_bk_info.stu_refcount[type], 0);
                bk_info->tsn_stu_bk_info.stu_refcount[type] = NULL;
            }
        }
        if (bk_info->tsn_stu_bk_info.pri_map_port != NULL) {
            TSN_FREE(unit,
                     bk_info->tsn_stu_bk_info.pri_map_port, 0);
            bk_info->tsn_stu_bk_info.pri_map_port = NULL;
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_esw_tsn_stu_init
 * Purpose:
 *      create a default profile setting
 * Parameters:
 *      unit - (IN) Unit number
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_esw_tsn_stu_init(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    int rv = BCM_E_NONE;
    int i, min, max, alloc_size;
    const tsn_stu_info_t *ctrl_info = NULL;
    bcm_tsn_stu_profile_type_t type;

    BCM_IF_ERROR_RETURN(
        tsn_bk_info_instance_get(unit, &bk_info));
    /* Allocate STU bookkeeping structure */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        STU_DEV_INIT(unit);
        ctrl_info = bk_info->tsn_dev_info->tsn_stu_info;
        for (type = bcmTsnStuProfileTypeIngress;
             type < bcmTsnStuProfileTypeCount; type++) {
            if (bk_info->tsn_stu_bk_info.stu_refcount[type] == NULL) {
                ctrl_info->stu_profile_mem_idx_get(unit, type, &min, &max);
                alloc_size = (max - min + 1) * sizeof(int);
                TSN_ALLOC(unit, bk_info->tsn_stu_bk_info.stu_refcount[type],
                          int, alloc_size, "Ingress STU Reference Count",
                          0, rv);
            }
            if (BCM_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_TSN,
                            (BSL_META_U(unit,
                                        "bcmi_esw_tsn_init: failed to "
                                        "allocate memory\n")));
                return rv;
            }
        }
        if (bk_info->tsn_stu_bk_info.pri_map_port == NULL) {
            alloc_size =
                ctrl_info->logical_port_num * sizeof(bcm_tsn_pri_map_t);
            TSN_ALLOC(unit, bk_info->tsn_stu_bk_info.pri_map_port,
                      bcm_tsn_pri_map_t, alloc_size,
                      "STU Priority Map Port Array", 0, rv);
        }
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "bcmi_esw_tsn_init: failed to "
                                    "allocate memory\n")));
            return rv;
        }
        /* Initialize pri_map_port array */
        for (i = 0; i < ctrl_info->logical_port_num; i++) {
            bk_info->tsn_stu_bk_info.pri_map_port[i] = BCM_TSN_PRI_MAP_INVALID;
        }
        /* Create STU mutex */
        bk_info->tsn_stu_bk_info.mutex = sal_mutex_create("stu_mutex");
        if (bk_info->tsn_stu_bk_info.mutex == NULL) {
            return BCM_E_MEMORY;
        }
    }
    return BCM_E_NONE;
}


/* Start of tsn event functions */
/*
 * Event instance information
 * NOTE: index order should be the same with bcmi_tsn_evt_inst_type_t
 */
STATIC const tsn_evt_inst_alloc_t tsn_evt_inst_info[bcmiTsnEvtCount] = {
    {bcmiTsnEvtCESys,
     bcmTsnEventTypeCounterExceeded, BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM,
     TSN_EVENT_SYSTEM_CB_MAX},
    {bcmiTsnEvtCEFlow,
     bcmTsnEventTypeCounterExceeded, BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW,
     TSN_EVENT_FLOW_CB_MAX},
    {bcmiTsnEvtCEPort,
     bcmTsnEventTypeCounterExceeded, BCM_TSN_EVENT_SOURCE_TYPE_PORT,
     TSN_EVENT_PORT_CB_MAX},
    {bcmiTsnEvtWLASys,
     bcmTsnEventTypeSrWrongLanA, BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM,
     TSN_EVENT_SYSTEM_CB_MAX},
    {bcmiTsnEvtWLAFlow,
     bcmTsnEventTypeSrWrongLanA, BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW,
     TSN_EVENT_FLOW_CB_MAX},
    {bcmiTsnEvtWLBSys,
     bcmTsnEventTypeSrWrongLanB, BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM,
     TSN_EVENT_SYSTEM_CB_MAX},
    {bcmiTsnEvtWLBFlow,
     bcmTsnEventTypeSrWrongLanB, BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW,
     TSN_EVENT_FLOW_CB_MAX},
    {bcmiTsnEvtSNSys,
     bcmTsnEventTypeSrSeqNumWindowAgeOut, BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM,
     TSN_EVENT_SYSTEM_CB_MAX},
    {bcmiTsnEvtSNFlow,
     bcmTsnEventTypeSrSeqNumWindowAgeOut, BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW,
     TSN_EVENT_FLOW_CB_MAX}
};

#if defined(BCM_TSN_SR_SUPPORT)
/*
 * Invoked from bcmi_esw_tsn_sr_rx_flowset_create to maintain the active rx flow
 * database in device.
 */
STATIC int
bcmi_esw_tsn_event_rx_flow_add(int unit, int fidx)
{
    uint32 hw_id;

    SR_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id(
        unit, 0, fidx, &hw_id);
    if (TSN_EVENT_DEVINFO(unit)->update_rx_flow) {
        BCM_IF_ERROR_RETURN(
            TSN_EVENT_DEVINFO(unit)->update_rx_flow(unit, -1, hw_id, 1));
    }
    return BCM_E_UNAVAIL;
}

STATIC int
bcmi_esw_tsn_event_rx_flow_delete(int unit, int fidx)
{
    uint32 hw_id;

    SR_FLOW_MGMT_DEVINFO(unit)->get_hw_flow_id(
        unit, 0, fidx, &hw_id);
    if (TSN_EVENT_DEVINFO(unit)->update_rx_flow) {
        BCM_IF_ERROR_RETURN(
            TSN_EVENT_DEVINFO(unit)->update_rx_flow(unit, -1, hw_id, 0));
    }
    return BCM_E_UNAVAIL;
}

/*
 * Handle flow based event
 * bcmTsnEventTypeSrWrongLanA, bcmTsnEventTypeSrWrongLanB,
 * bcmTsnEventTypeSrSeqNumWindowAgeOut
 */
STATIC int
bcmi_esw_tsn_flow_event_notify(
    int unit,
    bcm_tsn_event_type_t event,
    int multiple,
    int hw_flow_id)
{
    bcmi_esw_tsn_bk_info_t      *tsn_bk;
    tsn_event_bk_t              *bkinfo;
    int                         rv = BCM_E_NONE, i;
    tsn_evt_stat_t              *evt_stat;
    tsn_evt_regist_t            *er;
    tsn_evt_inst_t              *einst;
    bcm_tsn_event_source_t      src;
    bcmi_tsn_evt_inst_type_t    evt_inst_type;
    bcm_tsn_sr_flow_t           flow_id;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_events;

    TSN_EVENT_LOCK(unit);
    evt_stat = &bkinfo->evt_stat[event];
    SHR_BITCLR_RANGE(evt_stat->fbmp, 0, _SHR_BITDCLSIZE(bkinfo->max_flows));
    SHR_BITCLR_RANGE(evt_stat->ffbmp, 0, _SHR_BITDCLSIZE(bkinfo->max_flows));

    if ((event == bcmTsnEventTypeSrWrongLanA) ||
        (event == bcmTsnEventTypeSrWrongLanB) ||
        (event == bcmTsnEventTypeSrSeqNumWindowAgeOut)) {
        if (TSN_EVENT_DEVINFO(unit)->event_flow_stat_get) {
            rv = TSN_EVENT_DEVINFO(unit)->
                 event_flow_stat_get(unit, event, evt_stat->fbmp,
                                     evt_stat->ffbmp,
                                     bkinfo->max_flows);
            if (BCM_FAILURE(rv)) {
                TSN_EVENT_UNLOCK(unit);
                return rv;
            }
        }
        if (!multiple) {
            SHR_BITCLR_RANGE(evt_stat->fbmp, 0,
                             _SHR_BITDCLSIZE(bkinfo->max_flows));
            SHR_BITSET(evt_stat->fbmp, hw_flow_id);
        }

        /* Iter the flow id which indicate the wrong lan */
        SHR_BIT_ITER(evt_stat->fbmp, bkinfo->max_flows, i) {
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_sr_sw_flow_id_get(unit, i, &flow_id));
            if (SHR_BITGET(evt_stat->ffbmp, i)) {
                /* Wrong lan event, registered as per flow */
                if (event == bcmTsnEventTypeSrWrongLanA) {
                    evt_inst_type = bcmiTsnEvtWLAFlow;
                } else if (event == bcmTsnEventTypeSrWrongLanB) {
                    evt_inst_type = bcmiTsnEvtWLBFlow;
                } else {
                    evt_inst_type = bcmiTsnEvtSNFlow;
                }

                er = &bkinfo->eregist[evt_inst_type];
                TSN_REVT_LOCK(er);
                for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
                    /* matched the flow id with registerd source */
                    if (einst->sr_flow == flow_id) {
                        if (NULL != einst->cb) {
                            /* invoke registerd callback */
                            bcm_tsn_event_source_t_init(&src);
                            src.source_type = BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW;
                            src.sr_flow = einst->sr_flow;
                            einst->cb(unit, event, &src, einst->user_data);
                        }
                    }
                }
                TSN_REVT_UNLOCK(er);
            }

            /* Wrong lan event, registered as per system */
            if (event == bcmTsnEventTypeSrWrongLanA) {
                evt_inst_type = bcmiTsnEvtWLASys;
            } else if (event == bcmTsnEventTypeSrWrongLanB) {
                evt_inst_type = bcmiTsnEvtWLBSys;
            } else {
                evt_inst_type = bcmiTsnEvtSNSys;
            }
            er = &bkinfo->eregist[evt_inst_type];
            TSN_REVT_LOCK(er);
            for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
                /* For src == system, callback with flow_id information */
                if (NULL != einst->cb) {
                    bcm_tsn_event_source_t_init(&src);
                    src.source_type = BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW;
                    src.sr_flow = flow_id;
                    einst->cb(unit, event, &src, einst->user_data);
                }
            }
            TSN_REVT_UNLOCK(er);
        }
        /* Clear the event stat information */
        if (TSN_EVENT_DEVINFO(unit)->event_flow_stat_clear) {
                rv = TSN_EVENT_DEVINFO(unit)->
                     event_flow_stat_clear(unit, event, evt_stat->fbmp,
                                           bkinfo->max_flows);
        }
    }
    TSN_EVENT_UNLOCK(unit);
    return rv;
}
#endif /* BCM_TSN_SR_SUPPORT */

/*
 * Handle counter exceed event
 */
STATIC int
bcmi_esw_tsn_cnt_exceed_notify(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_source_t th_src,
    int multiple,
    int index)
{
    bcm_tsn_stat_threshold_config_t config;
    bcm_pbmp_t                      pbmp;
    bcm_port_t                      p;
    bcm_gport_t                     gport;
    uint64                          hw_value, sw_value, th_value;
    bcmi_esw_tsn_bk_info_t          *tsn_bk;
    tsn_event_bk_t                  *bkinfo;
    tsn_evt_regist_t                *er;
    tsn_evt_inst_t                  *einst;
    bcm_tsn_event_source_t          src;
#if defined(BCM_TSN_SR_SUPPORT)
    tsn_evt_stat_t                  *evt_stat;
    int                             i;
    bcm_tsn_sr_flow_t               flow_id;
#endif


    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_events;

    /* Get the threshold value */
    bcm_tsn_stat_threshold_config_t_init(&config);
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_threshold_get(unit, th_src, stat, &config));
    COMPILER_64_SET(th_value, 0, config.threshold);

    if (th_src == bcmTsnStatThresholdSourcePort) {
        BCM_PBMP_CLEAR(pbmp);
        /* sync the port stat */
        BCM_IF_ERROR_RETURN(
            bcmi_esw_tsn_stat_portcnt_stat_sync(unit, stat));

        if (multiple) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else {
            BCM_PBMP_PORT_SET(pbmp, index);
        }
        /* Iterate valid port to compare the threshold value */
        BCM_PBMP_ITER(pbmp, p) {
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_stat_portcnt_hw_sw_get(unit, stat, p,
                                                    &hw_value, &sw_value));
            BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, p, &gport));
            if (COMPILER_64_GT(hw_value, th_value)) {
                /* event registered with src = port */
                er = &bkinfo->eregist[bcmiTsnEvtCEPort];
                TSN_REVT_LOCK(er);
                for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
                    if (einst->port == p) {
                        if (NULL != einst->cb) {
                            bcm_tsn_event_source_t_init(&src);
                            src.source_type = BCM_TSN_EVENT_SOURCE_TYPE_PORT;
                            src.port = gport;
                            einst->cb(unit, bcmTsnEventTypeCounterExceeded,
                                      &src, einst->user_data);
                        }
                    }
                }
                TSN_REVT_UNLOCK(er);
                /* event registered with src = system */
                er = &bkinfo->eregist[bcmiTsnEvtCESys];
                TSN_REVT_LOCK(er);
                for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
                    if (NULL != einst->cb) {
                        bcm_tsn_event_source_t_init(&src);
                        src.source_type = BCM_TSN_EVENT_SOURCE_TYPE_PORT;
                        src.port = gport;
                        einst->cb(unit, bcmTsnEventTypeCounterExceeded,
                                  &src, einst->user_data);
                    }
                }
                TSN_REVT_UNLOCK(er);

                /* Clear the HW value to avoid continuous threshold event */
                COMPILER_64_ZERO(hw_value);
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_stat_portcnt_hw_set(unit, stat, p, hw_value));
            } /* end of COMPILER_64_GT */
        } /* end of valid port iter */
    }
    if (th_src == bcmTsnStatThresholdSourceSRFlow) {
#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            /* sync the flow stat */
            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_stat_flowcnt_stat_sync(unit, stat));

            evt_stat = &bkinfo->evt_stat[bcmTsnEventTypeCounterExceeded];
            SHR_BITCLR_RANGE(evt_stat->fbmp, 0,
                             _SHR_BITDCLSIZE(bkinfo->max_flows));
            if (multiple) {
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_stat_flowcnt_fbmp_get(
                        unit, stat, evt_stat->fbmp,
                        _SHR_BITDCLSIZE(bkinfo->max_flows)));
            } else {
                SHR_BITSET(evt_stat->fbmp, index);
            }
            /* Iter the flows which support the stat */
            SHR_BIT_ITER(evt_stat->fbmp, bkinfo->max_flows, i) {
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_sr_sw_flow_id_get(unit, i, &flow_id));
                BCM_IF_ERROR_RETURN(
                    bcmi_esw_tsn_stat_flowcnt_hw_sw_get(unit, stat, flow_id,
                                                        &hw_value, &sw_value));
                if (COMPILER_64_GT(hw_value, th_value)) {
                    er = &bkinfo->eregist[bcmiTsnEvtCEFlow];
                    TSN_REVT_LOCK(er);
                    for (einst = er->evt_inst; einst != NULL;
                         einst = einst->next) {
                        /* matched the flow id with registerd source */
                        if (einst->sr_flow == flow_id) {
                            if (NULL != einst->cb) {
                                /* invoke registerd callback */
                                bcm_tsn_event_source_t_init(&src);
                                src.source_type =
                                    BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW;
                                src.sr_flow = einst->sr_flow;
                                einst->cb(unit, bcmTsnEventTypeCounterExceeded,
                                          &src, einst->user_data);
                            }
                        }
                    }
                    TSN_REVT_UNLOCK(er);
                    er = &bkinfo->eregist[bcmiTsnEvtCESys];
                    TSN_REVT_LOCK(er);
                    for (einst = er->evt_inst; einst != NULL;
                         einst = einst->next) {
                        /*
                         * For src == system,
                         * callback with flow_id information
                         */
                        if (NULL != einst->cb) {
                            bcm_tsn_event_source_t_init(&src);
                            src.source_type = BCM_TSN_EVENT_SOURCE_TYPE_SYSTEM;
                            src.sr_flow = flow_id;
                            einst->cb(unit, bcmTsnEventTypeCounterExceeded,
                                      &src, einst->user_data);
                        }
                    }
                    TSN_REVT_UNLOCK(er);
                    /* Clear the HW value to avoid continuous threshold event */
                    COMPILER_64_ZERO(hw_value);
                    BCM_IF_ERROR_RETURN(
                        bcmi_esw_tsn_stat_flowcnt_hw_set(unit, stat, flow_id,
                                                         hw_value));
                }
            }
        } else
#endif /* BCM_TSN_SR_SUPPORT */
        {
            /* Do nothing */
            return BCM_E_NONE;
        }
    }
    return BCM_E_NONE;
}

void bcmi_tsn_event_poll(void *unit_vp)
{
    int                     unit = PTR_TO_INT(unit_vp);
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;
    sal_usecs_t             interval;
    int                     rv;

    /* parameter check */
    rv = tsn_bk_info_instance_get(unit, &tsn_bk);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN EVT: failed to get the tsn bkinfo\n")));
        return;
    }

    bkinfo = &tsn_bk->tsn_events;

    while (bkinfo->poll_enable) {
        if (bkinfo->poll_interval != 0) {
            interval = (bkinfo->poll_interval) * MILLISECOND_USEC;
        } else {
            interval = TSN_EVENT_POLL_INTERVAL_DEFAULT;
        }

        (void)sal_sem_take(bkinfo->poll_notify, interval);

        if (bkinfo->poll_enable == 0) {
            break;
        }

        /* invoke device poll function */
        if (TSN_EVENT_DEVINFO(unit)->poll) {
            (void)TSN_EVENT_DEVINFO(unit)->poll(unit);
        }

    }
    bkinfo->poll_tid = SAL_THREAD_ERROR;
    bkinfo->poll_interval = 0;
    bkinfo->poll_enable = 0;

    sal_thread_exit(0);
}

STATIC int
bcmi_esw_tsn_evt_poll_config(int unit, int enable)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;
    soc_timeout_t           to;
    int                     rv = BCM_E_NONE;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_events;

    if (enable) {
        if (bkinfo->poll_tid && bkinfo->poll_tid != SAL_THREAD_ERROR) {
            /* Thead existed and running. invoke poll function */
            sal_sem_give(bkinfo->poll_notify);
            return BCM_E_NONE;
        }
        TSN_EVENT_LOCK(unit);
        if ((bkinfo->poll_tid == NULL) ||
            (bkinfo->poll_tid == SAL_THREAD_ERROR)) {
            bkinfo->poll_tid = sal_thread_create("bcmTSN", SAL_THREAD_STKSZ, 50,
                                                 bcmi_tsn_event_poll,
                                                 INT_TO_PTR(unit));
            if (bkinfo->poll_tid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "TSN EVT: failed to create thread\n")));
                TSN_EVENT_UNLOCK(unit);
                return BCM_E_MEMORY;
            }
        }
        TSN_EVENT_UNLOCK(unit);
    } else {
        TSN_EVENT_LOCK(unit);
        bkinfo->poll_interval = 0;
        bkinfo->poll_enable = 0;
        TSN_EVENT_UNLOCK(unit);
        if (bkinfo->poll_tid && bkinfo->poll_tid != SAL_THREAD_ERROR) {
            sal_sem_give(bkinfo->poll_notify);
            if (SAL_BOOT_SIMULATION) {
                soc_timeout_init(&to, 300 * 1000000, 0);
            } else {
                soc_timeout_init(&to, 60 * 1000000, 0);
            }

            while (bkinfo->poll_tid != SAL_THREAD_ERROR) {
                if (soc_timeout_check(&to)) {
                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META_U(unit,
                                          "TSN thread will not exit\n")));
                    rv = BCM_E_INTERNAL;
                    break;
                }
            }
        }
    }
    return rv;
}

/* Internal function invoke when  tsnControl change */
STATIC void
bcmi_esw_tsn_evt_ageout_config(int unit, bcm_tsn_control_t type, uint32 arg)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;
    uint32                  enable, interval = 0, rmode;
    uint8                   change = 0, rmode_change = 0;

    /* parameter check */
    if (BCM_FAILURE(tsn_bk_info_instance_get(unit, &tsn_bk))) {
        return;
    }
    bkinfo = &tsn_bk->tsn_events;

    TSN_EVENT_LOCK(unit);

    /*
     * Interested in bcmTsnControlSrAgeTickInterval,
     * bcmTsnControlSrAgeOutEnable
     * bcmTsnControlSrSeqNumWindowResetMode
     */
    enable = bkinfo->pre_enable;
    if (type == bcmTsnControlSrAgeTickInterval) {
        interval = arg;
        if (bkinfo->pre_interval != interval) {
            change = 1;
        }
        bkinfo->pre_interval = interval;
    } else if (type == bcmTsnControlSrAgeOutEnable) {
        enable = arg;
        if (bkinfo->pre_enable != enable) {
            change = 1;
        }
        bkinfo->pre_enable = enable;
    } else if (type == bcmTsnControlSrSeqNumWindowResetMode) {
        rmode = arg;
        if ((bkinfo->pre_rmode != rmode) &&
            (rmode == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW ||
             bkinfo->pre_rmode ==
             BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW)) {
            rmode_change = 1;
        }
        bkinfo->pre_rmode = rmode;
    } else {
        TSN_EVENT_UNLOCK(unit);
        return;
    }

    /* update the poll thread parameter */
    bkinfo->poll_interval = bkinfo->pre_interval;

    bkinfo->poll_enable = enable;

    TSN_EVENT_UNLOCK(unit);

    if ((change) || (rmode_change)) {
        bcmi_esw_tsn_evt_poll_config(unit, enable);
    }
    return;
}

/* Free event instance resource */
STATIC int
bcmi_esw_tsn_evt_inst_free(
    int unit,
    bcmi_tsn_evt_inst_type_t inst_type,
    tsn_evt_inst_t  *entry)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (inst_type < 0 || inst_type >= bcmiTsnEvtCount || entry == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_events;
    TSN_EVENT_LOCK(unit);
    /* Put it back to the head */
    entry->next = bkinfo->einst_avail[inst_type];
    bkinfo->einst_avail[inst_type] = entry;
    TSN_EVENT_UNLOCK(unit);

    return BCM_E_NONE;
}

/* Allocate event instance */
STATIC int
bcmi_esw_tsn_evt_inst_allocate(
    int unit,
    bcmi_tsn_evt_inst_type_t inst_type,
    tsn_evt_inst_t  **entry)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;
    tsn_evt_inst_t          *einst;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    if (inst_type < 0 || inst_type >= bcmiTsnEvtCount || entry == NULL) {
        return BCM_E_PARAM;
    }
    bkinfo = &tsn_bk->tsn_events;

    TSN_EVENT_LOCK(unit);
    /* Get one entry from the head */
    einst = bkinfo->einst_avail[inst_type];
    if (einst == NULL) {
        TSN_EVENT_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }
    bkinfo->einst_avail[inst_type] = einst->next;
    einst->next = NULL;
    *entry = einst;
    TSN_EVENT_UNLOCK(unit);

    return BCM_E_NONE;
}

STATIC int
bcmi_esw_tsn_event_mgmt_detach(int unit)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;
    tsn_evt_regist_t        *eregist;
    int                     i;

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    bkinfo = &tsn_bk->tsn_events;

    if (bkinfo->poll_notify) {
        if (bkinfo->poll_enable) {
            BCM_IF_ERROR_RETURN(bcmi_esw_tsn_evt_poll_config(unit, 0));
        }
        sal_sem_destroy(bkinfo->poll_notify);
        bkinfo->poll_notify = NULL;
    }
    /* de-allocate event instance pool */
    if (NULL != bkinfo->einst_allocated) {
        sal_free(bkinfo->einst_allocated);
        bkinfo->einst_allocated = NULL;
    }
    /* de-allocate event mgmt mutex */
    if (NULL != bkinfo->evt_mutex) {
        sal_mutex_destroy(bkinfo->evt_mutex);
        bkinfo->evt_mutex = NULL;
    }

    for (i = 0; i < bcmiTsnEvtCount; i++) {
        /* clear linked list */
        bkinfo->einst_avail[i] = NULL;
        /* destroy mutex */
        eregist = &bkinfo->eregist[i];
        if (NULL != eregist->mutex) {
            sal_mutex_destroy(eregist->mutex);
            eregist->mutex = NULL;
        }
    }
    for (i = 0; i < bcmTsnEventTypeCount; i++) {
        /* de-allocate register event instance */
        if (NULL != bkinfo->evt_stat[i].port_ref) {
            sal_free(bkinfo->evt_stat[i].port_ref);
            bkinfo->evt_stat[i].port_ref = NULL;
        }
        if (NULL != bkinfo->evt_stat[i].flow_ref) {
            sal_free(bkinfo->evt_stat[i].flow_ref);
            bkinfo->evt_stat[i].flow_ref = NULL;
        }
        if (NULL != bkinfo->evt_stat[i].pbmp) {
            sal_free(bkinfo->evt_stat[i].pbmp);
            bkinfo->evt_stat[i].pbmp = NULL;
        }
        if (NULL != bkinfo->evt_stat[i].fbmp) {
            sal_free(bkinfo->evt_stat[i].fbmp);
            bkinfo->evt_stat[i].fbmp = NULL;
        }
        if (NULL != bkinfo->evt_stat[i].ffbmp) {
            sal_free(bkinfo->evt_stat[i].ffbmp);
            bkinfo->evt_stat[i].ffbmp = NULL;
        }
    }
    return BCM_E_NONE;
}

/* event management initialization */
STATIC int
bcmi_esw_tsn_event_mgmt_init(int unit)
{
    bcmi_esw_tsn_bk_info_t      *tsn_bk;
    tsn_event_bk_t              *bkinfo;
    tsn_evt_inst_t              *einst;
    int                         i, j, total_insts, maxflows = 0, maxports;
    tsn_evt_regist_t            *eregist;
    tsn_evt_stat_t              *evt_stat;
    bcm_pbmp_t                  all_pbmp;
#if defined(BCM_TSN_SR_SUPPORT)
    tsn_sr_flows_bookkeeping_t  *sr_flow_bkinfo;
#endif

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    bkinfo = &tsn_bk->tsn_events;
    if (TSN_EVENT_DEVINFO(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    /* Allocate event cb link list */
    total_insts = 0;
    for (i = 0; i < bcmiTsnEvtCount; i++) {
        total_insts += tsn_evt_inst_info[i].inst_limits;
    }

    bkinfo->einst_allocated = (tsn_evt_inst_t *)sal_alloc(
        sizeof(tsn_evt_inst_t) * total_insts,
        "tsn event instances");
    if (NULL == bkinfo->einst_allocated) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN EVNT: failed to alloc event instance\n")));
        (void)bcmi_esw_tsn_event_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(bkinfo->einst_allocated, 0,
               sizeof(tsn_evt_inst_t) * total_insts);

     /* Retrieve number of flows */
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
         sr_flow_bkinfo = &tsn_bk->sr_flows;
        if (sr_flow_bkinfo != NULL &&
            sr_flow_bkinfo->max_flows[SR_FLOW_DIR_TX] != 0 &&
            sr_flow_bkinfo->max_flows[SR_FLOW_DIR_RX] != 0) {
            maxflows = sr_flow_bkinfo->max_flows[SR_FLOW_DIR_TX] +
                       sr_flow_bkinfo->max_flows[SR_FLOW_DIR_RX];
            bkinfo->max_rx_flows = sr_flow_bkinfo->max_flows[SR_FLOW_DIR_RX];
        } else {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                          "TSN EVNT: failed to get sr flow information.\n")));
            return BCM_E_INTERNAL;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */
    bkinfo->max_flows = maxflows;

    einst = bkinfo->einst_allocated;
    for (i = 0; i < bcmiTsnEvtCount; i++) {
        for (j = 0; j < tsn_evt_inst_info[i].inst_limits; j++, einst++) {
            /* Initialize available linked list */
            einst->next = bkinfo->einst_avail[i];
            bkinfo->einst_avail[i] = einst;
        }

        /* Initialize registered events instance */
        sal_memset(&bkinfo->eregist[i], 0, sizeof(tsn_evt_regist_t));
        eregist = &bkinfo->eregist[i];
        eregist->source_type = tsn_evt_inst_info[i].source_type;
        eregist->inst_type = tsn_evt_inst_info[i].inst_type;
        eregist->evt_type = tsn_evt_inst_info[i].evt_type;

        /* Create mutex */
        eregist->mutex = sal_mutex_create("tsn evt register mutex");
        if (NULL == eregist->mutex) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: "
                                  "failed to create register mutex\n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    /* Get the max port in the system */
    maxports = 0;
    BCM_PBMP_CLEAR(all_pbmp);
    /* Get the all valid port map */
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
    /* Iterate the bitmap to get the maximum port number */
    BCM_PBMP_ITER(all_pbmp, i) {
        maxports = i;
    }
    /* Total port counts should be (maximum port number + 1) */
    bkinfo->max_ports = maxports + 1;
    for (i = 0; i < bcmTsnEventTypeCount; i++) {
        /* Allocate and initialize the event stat  */
        evt_stat = &bkinfo->evt_stat[i];
        evt_stat->event_ref = 0;
        evt_stat->port_ref = sal_alloc(bkinfo->max_ports * sizeof(int),
                                       "alloc evt port basis stat");
        if (NULL == evt_stat->port_ref) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: failed to create evt stat \n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(evt_stat->port_ref, 0, bkinfo->max_ports * sizeof(int));

        evt_stat->flow_ref = sal_alloc(maxflows * sizeof(int),
                                       "alloc evt flow basis stat");
        if (NULL == evt_stat->flow_ref) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: failed to create evt stat \n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(evt_stat->flow_ref, 0, bkinfo->max_ports * sizeof(int));

        evt_stat->pbmp = sal_alloc(SHR_BITALLOCSIZE(bkinfo->max_ports),
                                   "alloc evt stat pbmp");
        if (NULL == evt_stat->pbmp) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: failed to alloc evt stat pbmp\n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(evt_stat->pbmp, 0, SHR_BITALLOCSIZE(bkinfo->max_ports));
        evt_stat->fbmp = sal_alloc(SHR_BITALLOCSIZE(maxflows),
                                    "alloc evt stat fbmp");
        if (NULL == evt_stat->fbmp) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: failed to alloc evt stat fbmp\n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(evt_stat->fbmp, 0, SHR_BITALLOCSIZE(maxflows));
        evt_stat->ffbmp = sal_alloc(SHR_BITALLOCSIZE(maxflows),
                                    "alloc evt stat ffbmp");
        if (NULL == evt_stat->ffbmp) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN EVT: "
                                  "failed to alloc evt stat ffbmp\n")));
            (void)bcmi_esw_tsn_event_mgmt_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(evt_stat->ffbmp, 0, SHR_BITALLOCSIZE(maxflows));
   }

    /* Create mutex for protecting event management (allocate/free) */
    bkinfo->evt_mutex = sal_mutex_create("tsn evt mgmt mutex");
    if (NULL == bkinfo->evt_mutex) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN EVT: failed to create event mgmt mutex\n")));
        (void)bcmi_esw_tsn_event_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }

    /* Create poll notify */
    bkinfo->poll_notify = sal_sem_create("TSN EVT poll", sal_sem_BINARY, 0);
    if (NULL == bkinfo->poll_notify) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN EVT: failed to create event poll "
                              "notify\n")));
        (void)bcmi_esw_tsn_event_mgmt_detach(unit);
        return BCM_E_MEMORY;
    }
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Register the event notify callback */
        if (TSN_EVENT_DEVINFO(unit)->register_flow_event_cb) {
            (void)TSN_EVENT_DEVINFO(unit)->register_flow_event_cb(
                unit, bcmi_esw_tsn_flow_event_notify);
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */
    if (TSN_EVENT_DEVINFO(unit)->register_cnt_exceed_cb) {
        (void)TSN_EVENT_DEVINFO(unit)->register_cnt_exceed_cb(
            unit, bcmi_esw_tsn_cnt_exceed_notify);
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_esw_tsn_event_validate(
    int unit,
    bcmi_tsn_evt_inst_type_t inst_type,
    bcm_tsn_event_source_t *src)
{
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    bcm_port_t              port;
#if defined(BCM_TSN_SR_SUPPORT)
    tsn_sr_flows_bookkeeping_t *sr_flow_bkinfo;
    bcm_tsn_sr_flow_t flow_id = src->sr_flow;
    int fidx;
    int dir;
#endif

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_PORT) {
        if (BCM_GPORT_IS_SET(src->port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, src->port, &port));
        } else {
            port = src->port;
        }
        if (!SOC_PORT_VALID(unit, port) ||
            !SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            return BCM_E_PARAM;
        }
    }
    if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW) {
#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            sr_flow_bkinfo = &tsn_bk->sr_flows;
            dir = SR_FLOW_ID_TO_DIR(flow_id);
            fidx = SR_FLOW_ID_TO_IDX(flow_id);
            if (fidx < 0 ||
                fidx >= sr_flow_bkinfo->max_flows[dir]) {
                return BCM_E_PARAM;
            }
        } else
#endif /* BCM_TSN_SR_SUPPORT */
        {
            return BCM_E_PARAM;
        }
    }

    if (TSN_EVENT_DEVINFO(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }
    if (TSN_EVENT_DEVINFO(unit)->validate_event) {
        return TSN_EVENT_DEVINFO(unit)->validate_event(unit, inst_type);
    }
    return BCM_E_UNAVAIL;
}

/* Enable /disable the event mask */
STATIC int
bcmi_esw_tsn_event_mask_set(int unit, bcm_tsn_event_type_t event, int enable)
{
    if (TSN_EVENT_DEVINFO(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }
    if (TSN_EVENT_DEVINFO(unit)->event_mask_set) {
        return TSN_EVENT_DEVINFO(unit)->event_mask_set(unit, event, enable);
    }
    return BCM_E_UNAVAIL;
}

STATIC int
bcmi_esw_tsn_event_register(
    int unit,
    bcm_tsn_event_type_t event,
    bcm_tsn_event_source_t *src,
    bcm_tsn_event_cb cb,
    void *user_data)
{
    int                         i, rv = BCM_E_NONE, event_count;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32                      idx;
#endif
    bcmi_tsn_evt_inst_type_t    inst_type;
    tsn_evt_regist_t            *er;
    tsn_evt_inst_t              *einst, *einst_alloc;
    bcmi_esw_tsn_bk_info_t      *tsn_bk;
    tsn_event_bk_t              *bkinfo;
    tsn_evt_stat_t              *evt_stat;
    bcm_port_t                  port;

    if ((cb == NULL) || (src == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    bkinfo = &tsn_bk->tsn_events;
    if (TSN_EVENT_DEVINFO(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    /* get the internal instance type */
    inst_type = bcmiTsnEvtCount;
    for (i = 0; i < bcmiTsnEvtCount; i++) {
        if ((tsn_evt_inst_info[i].source_type == src->source_type) &&
            (tsn_evt_inst_info[i].evt_type == event)) {
            inst_type = tsn_evt_inst_info[i].inst_type;
            break;
        }
    }
    if (inst_type == bcmiTsnEvtCount) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_event_validate(unit, inst_type, src));

    if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_PORT) {
        if (BCM_GPORT_IS_SET(src->port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, src->port, &port));
        } else {
            port = src->port;
        }
    } else {
        port = 0;
    }
    TSN_EVENT_LOCK(unit);
    er = &bkinfo->eregist[inst_type];
    TSN_REVT_LOCK(er);
    if (NULL == er->evt_inst) {
        /* registered instance type array is empty */
        /* allocate the event instance from pool */
        rv = bcmi_esw_tsn_evt_inst_allocate(unit, inst_type, &einst_alloc);
        if (BCM_FAILURE(rv)) {
            TSN_REVT_UNLOCK(er);
            TSN_EVENT_UNLOCK(unit);
            return rv;
        }
        /* Put it on the head of registered event instance list */
        einst_alloc->cb = cb;
        einst_alloc->user_data = user_data;
        einst_alloc->port = port;
        einst_alloc->sr_flow = src->sr_flow;
        einst_alloc->next = NULL;
        er->evt_inst = einst_alloc;
    } else {
        /* registered instance type array is available */
        for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
            if ((einst->cb == cb) && (einst->port == port) &&
                (einst->sr_flow == src->sr_flow)) {
                /* matched the callback and event source */
                TSN_REVT_UNLOCK(er);
                TSN_EVENT_UNLOCK(unit);
                return BCM_E_EXISTS;
            } else {
                /* not exactly matched instance, allocate a new one */
                rv = bcmi_esw_tsn_evt_inst_allocate(unit, inst_type,
                                                    &einst_alloc);
                if (BCM_FAILURE(rv)) {
                    TSN_REVT_UNLOCK(er);
                    TSN_EVENT_UNLOCK(unit);
                    return rv;
                }
                /* Put it on the head of registered event instance list */
                einst_alloc->cb = cb;
                einst_alloc->user_data = user_data;
                einst_alloc->port = port;
                einst_alloc->sr_flow = src->sr_flow;
                einst_alloc->next = er->evt_inst;
                er->evt_inst = einst_alloc;
            }
        }
    }

    /* update the event stat */
    evt_stat = &bkinfo->evt_stat[event];
    if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_PORT) {
        evt_stat->port_ref[port]++;
    } else if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW) {
#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            rv = bcmi_esw_tsn_sr_hw_flow_id_get(unit, src->sr_flow, &idx);
            if (BCM_FAILURE(rv)) {
                einst = er->evt_inst;
                er->evt_inst = einst->next;
                (void)bcmi_esw_tsn_evt_inst_free(unit, inst_type, einst);
                TSN_REVT_UNLOCK(er);
                TSN_EVENT_UNLOCK(unit);
                return rv;
            }
            evt_stat->flow_ref[idx]++;
            if ((SR_FLOW_ID_TO_DIR(src->sr_flow) == SR_FLOW_DIR_RX) &&
                (evt_stat->flow_ref[idx] == 1)) {
                if (TSN_EVENT_DEVINFO(unit)->update_rx_flow) {
                    rv = TSN_EVENT_DEVINFO(unit)->update_rx_flow(unit,
                                                                 (int)event,
                                                                 idx,
                                                                 1);
                    if (BCM_FAILURE(rv)) {
                        einst = er->evt_inst;
                        er->evt_inst = einst->next;
                        (void)bcmi_esw_tsn_evt_inst_free(unit,
                                                         inst_type,
                                                         einst);
                        TSN_REVT_UNLOCK(er);
                        TSN_EVENT_UNLOCK(unit);
                        return rv;
                    }
                }
            }
        } else
#endif /* BCM_TSN_SR_SUPPORT */
        {
            einst = er->evt_inst;
            er->evt_inst = einst->next;
            (void)bcmi_esw_tsn_evt_inst_free(unit,
                                             inst_type,
                                             einst);
            TSN_REVT_UNLOCK(er);
            TSN_EVENT_UNLOCK(unit);
            return BCM_E_UNAVAIL;
        }
    }
    event_count = evt_stat->event_ref;
    evt_stat->event_ref++;

    if (event_count == 0) {
        rv = bcmi_esw_tsn_event_mask_set(unit, event, 1);
        if (BCM_FAILURE(rv)) {
            einst = er->evt_inst;
            er->evt_inst = einst->next;
            (void)bcmi_esw_tsn_evt_inst_free(unit, inst_type, einst);
        }
    }
    TSN_REVT_UNLOCK(er);
    TSN_EVENT_UNLOCK(unit);
    return BCM_E_NONE;
}

STATIC int
bcmi_esw_tsn_event_unregister(
    int unit,
    bcm_tsn_event_type_t event,
    bcm_tsn_event_source_t *src,
    bcm_tsn_event_cb cb)
{
    int                         i, rv = BCM_E_NOT_FOUND, event_count;
#if defined(BCM_TSN_SR_SUPPORT)
    uint32                      idx;
#endif
    bcmi_tsn_evt_inst_type_t    inst_type;
    tsn_evt_regist_t            *er;
    tsn_evt_inst_t              *einst, *prev;
    bcmi_esw_tsn_bk_info_t      *tsn_bk;
    tsn_event_bk_t              *bkinfo;
    tsn_evt_stat_t              *evt_stat;
    bcm_port_t                  port;

    if ((cb == NULL) || (src == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));
    bkinfo = &tsn_bk->tsn_events;

    /* get the internal instance type */
    inst_type = bcmiTsnEvtCount;
    for (i = 0; i < bcmiTsnEvtCount; i++) {
        if ((tsn_evt_inst_info[i].source_type == src->source_type) &&
            (tsn_evt_inst_info[i].evt_type == event)) {
            inst_type = tsn_evt_inst_info[i].inst_type;
            break;
        }
    }
    if (inst_type == bcmiTsnEvtCount) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_event_validate(unit, inst_type, src));

    if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_PORT) {
        if (BCM_GPORT_IS_SET(src->port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, src->port, &port));
        } else {
            port = src->port;
        }
    } else {
        port = 0;
    }
    TSN_EVENT_LOCK(unit);
    er = &bkinfo->eregist[inst_type];
    TSN_REVT_LOCK(er);
    if (NULL == er->evt_inst) {
        /* registered instance type array is empty. */
        TSN_REVT_UNLOCK(er);
        TSN_EVENT_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
        /* registered instance type array is available */
        prev = NULL;
        for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
            if ((einst->cb == cb) && (einst->port == port) &&
                (einst->sr_flow == src->sr_flow)) {
                /* matched the callback and event source */
                /* remove it from the er list */
                if (prev == NULL) {
                    er->evt_inst = einst->next;
                } else {
                    prev->next = einst->next;
                }
                /* Free to inst pool */
                (void)bcmi_esw_tsn_evt_inst_free(unit, inst_type, einst);
                rv = BCM_E_NONE;
                break;
            }
            prev = einst;
        }
        TSN_REVT_UNLOCK(er);
        if (BCM_SUCCESS(rv)) {
            /* update the bkinfo->event_count */
            evt_stat = &bkinfo->evt_stat[event];
            if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_PORT) {
                if (evt_stat->port_ref[port] > 0) {
                    evt_stat->port_ref[port]--;
                }
            } else if (src->source_type == BCM_TSN_EVENT_SOURCE_TYPE_SR_FLOW) {
#if defined(BCM_TSN_SR_SUPPORT)
                if (soc_feature(unit, soc_feature_tsn_sr)) {
                    rv = bcmi_esw_tsn_sr_hw_flow_id_get(unit, src->sr_flow,
                                                        &idx);
                    if (BCM_FAILURE(rv)) {
                        TSN_EVENT_UNLOCK(unit);
                        return rv;
                    }
                    if (evt_stat->flow_ref[idx] > 0) {
                        evt_stat->flow_ref[idx]--;
                    }
                    if ((SR_FLOW_ID_TO_DIR(src->sr_flow) == SR_FLOW_DIR_RX) &&
                        (evt_stat->flow_ref[idx] == 0)) {
                        if (TSN_EVENT_DEVINFO(unit)->update_rx_flow) {
                            rv = TSN_EVENT_DEVINFO(unit)->update_rx_flow(
                                 unit, (int)event, idx, 0);
                            if (BCM_FAILURE(rv)) {
                                TSN_EVENT_UNLOCK(unit);
                                return rv;
                            }
                        }
                    }
                } else
#endif /* BCM_TSN_SR_SUPPORT */
                {
                    TSN_EVENT_UNLOCK(unit);
                    return BCM_E_UNAVAIL;
                }
            }
            evt_stat->event_ref--;
            event_count = evt_stat->event_ref;
            if (event_count == 0) {
                rv = bcmi_esw_tsn_event_mask_set(unit, event, 0);
                if (BCM_FAILURE(rv)) {
                    TSN_EVENT_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }
    TSN_EVENT_UNLOCK(unit);
    return rv;
}

STATIC int
bcmi_esw_tsn_event_notification_traverse(
    int unit,
    bcm_tsn_event_notification_traverse_cb cb,
    void *user_data)
{
    bcm_tsn_event_type_t    event;
    bcm_tsn_event_source_t  src;
    bcm_tsn_event_cb        evt_cb;
    void                    *evt_cb_user_data;
    int                     i, rv;
    tsn_evt_regist_t        *er;
    tsn_evt_inst_t          *einst;
    bcmi_esw_tsn_bk_info_t  *tsn_bk;
    tsn_event_bk_t          *bkinfo;

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    /* parameter check */
    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &tsn_bk));

    bkinfo = &tsn_bk->tsn_events;

    for (i = 0; i < bcmiTsnEvtCount; i++) {
        er = &bkinfo->eregist[i];
        TSN_REVT_LOCK(er);
        for (einst = er->evt_inst; einst != NULL; einst = einst->next) {
            event = er->evt_type;
            bcm_tsn_event_source_t_init(&src);
            src.source_type = er->source_type;
            src.port = einst->port;
            src.sr_flow = einst->sr_flow;
            evt_cb = einst->cb;
            evt_cb_user_data = einst->user_data;
            rv = cb(unit, event, &src, evt_cb, evt_cb_user_data, user_data);
            if (BCM_FAILURE(rv)) {
                TSN_REVT_UNLOCK(er);
                return rv;
            }
        }
        TSN_REVT_UNLOCK(er);
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *   bcmi_esw_tsn_reinit
 * Purpose:
 *   Restore bookkeeping information.
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_reinit(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /*
         * Load TSN SR MAC Proxy info from hardware
         * into software data structures.
         */
        rv = bcmi_esw_tsn_sr_mac_proxy_reload(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Doing TSN TAF resource reinit */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        rv = bcmi_esw_tsn_taf_reload(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN TAF Reload Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* Doing TSN and SR priority map resource reinit */
    rv = bcmi_esw_tsn_pri_map_tsn_sr_reload(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Priority Map Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Doing SR flows resource reinint */
        rv = bcmi_esw_tsn_sr_flow_mgmt_reload(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN SR flows warm boot reload failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Doing TSN flows resource reinint */
    rv = bcmi_esw_tsn_flow_mgmt_reload(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN flows warm boot reload failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        /*
         * Load TSN SR Auto Learn info from hardware
         * into software data structures.
         */
        rv = bcmi_esw_tsn_sr_auto_learn_reload(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "SR Auto Learn Warm boot failed(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Doing TSN MTU & STU resource reinit */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        rv = bcmi_esw_tsn_egress_mtu_reload(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN MTU Reload Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
        rv = bcmi_esw_tsn_egress_stu_reload(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN STU Reload Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* Doing TSN PRP-Forwarding resource reinit */
    rv = bcmi_esw_tsn_sr_port_prp_forwarding_reload(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN PRP-Forwarding Reload Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Doing TSN Portcnt Stat/Port bitmap resource reinit */
    rv = bcmi_esw_tsn_stat_portcnt_reload(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Stat Portcnt Reload Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Doing TSN Flowcnt Stat resource reinit */
    rv = bcmi_esw_tsn_stat_flowcnt_reload(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Stat Flowcnt Reload Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_esw_tsn_sync
 * Purpose:
 *   Store bookkeeping information for
 *   Warmboot recovery.
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_sync(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* Doing TSN MTU & STU resource sync */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        rv = bcmi_esw_tsn_egress_mtu_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN MTU Sync Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
        rv = bcmi_esw_tsn_egress_stu_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN STU Sync Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    /* Sync TSN flows to scache */
    rv = bcmi_esw_tsn_flow_mgmt_sync(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN flows warm boot sync failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Sync SR flows to scache */
        rv = bcmi_esw_tsn_sr_flow_mgmt_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN SR flows warm boot sync failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }

        rv = bcmi_esw_tsn_sr_port_prp_forwarding_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN PRP Forwarding Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        rv = bcmi_esw_tsn_sr_auto_learn_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN SR Auto Learn Sync Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Doing TSN Portcnt Stat/Port bitmap resource sync */
    rv = bcmi_esw_tsn_stat_portcnt_sync(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Stat Portcnt Sync Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

     /* Doing TSN Flowtcnt Stat resource sync */
    rv = bcmi_esw_tsn_stat_flowcnt_sync(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Stat Flowcnt Sync Warm boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }
 
    /* Doing TSN TAF resource sync */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        rv = bcmi_esw_tsn_taf_sync(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN TAF Sync Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *   bcmi_esw_tsn_coldboot_init
 * Purpose:
 *   Restore bookkeeping information.
 * Parameters:
 *   unit - (IN) unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_coldboot_init(int unit)
{
    bcm_error_t rv = BCM_E_UNAVAIL;

#if defined(BCM_WARM_BOOT_SUPPORT)
    /* Warm boot should be false in cold boot case */
    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Allocate scache for SR flows */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_sr_flow_mgmt_wb_alloc(unit));
    }
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        /* Allocate scache for SR Auto Learn management */
        BCM_IF_ERROR_RETURN(bcmi_esw_tsn_sr_auto_learn_wb_alloc(unit));
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Allocate scache for TSN flows */
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_flow_mgmt_wb_alloc(unit));

#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Doing TSN and SR priority map resource cold boot init */
    rv = bcmi_esw_tsn_pri_map_tsn_sr_coldboot_init(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN Priority Map Cold boot failed"
                              "(rv = %d)\n"),
                              rv));
        return rv;
    }

    /* Doing TAF resource cold boot init */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        rv = bcmi_esw_tsn_taf_coldboot_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "TSN TAF Cold boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_bk_info_cleanup
 * Purpose:
 *     Internal function to clear the resource of the TSN module.
 * Parameters:
 *     unit             - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_bk_info_cleanup(int unit)
{
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_bk_info_cleanup\n")));

    /* Free and clear the bookkeeping info structure */
    if (tsn_bk_info[unit] != NULL) {
        /* Event management clean up */
        bcmi_esw_tsn_event_mgmt_detach(unit);

#if defined(BCM_TSN_SR_SUPPORT)
        if (soc_feature(unit, soc_feature_tsn_sr)) {
            /* SR Auto Learn management clean up */
            bcmi_esw_tsn_sr_auto_learn_cleanup(unit);
            /* SR MAC proxy management clean up */
            bcmi_esw_tsn_sr_mac_proxy_cleanup(unit);
            /* SR flow management clean up */
            bcmi_esw_tsn_sr_flow_mgmt_detach(unit);
        }
#endif /* BCM_TSN_SR_SUPPORT */

        /* TSN priority map management clean up */
        bcmi_esw_tsn_pri_map_cleanup(unit);

        /* MTU & STU management clean up */
        bcmi_esw_tsn_mtu_cleanup(unit);
        bcmi_esw_tsn_stu_cleanup(unit);

        /* TSN flow management clean up */
        bcmi_esw_tsn_flow_mgmt_detach(unit);

        TSN_FREE(unit, tsn_bk_info[unit], 0);
        tsn_bk_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_detach
 * Purpose:
 *     Clear the resource of the TSN module.
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_detach(int unit)
{
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* clean up TSN Stat counters info resource */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_cleanup(unit));

    /* clean up TAF resource */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        bcmi_esw_tsn_taf_cleanup(unit);
    }

    /* clean up bookkeeping info resource */
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_bk_info_cleanup(unit));

#ifdef BCM_GREYHOUND2_SUPPORT
    /* chip specific detach */
    if (SOC_IS_GREYHOUND2(unit)) {
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_info_detach(unit));
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_init
 * Purpose:
 *     Internal function to initialize the resource and setup device info.
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_init(int unit)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    int rv = BCM_E_NONE;
    int i;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_init\n")));

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* Initialize structure */
    if (!tsn_bk_info_initialized) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            tsn_bk_info[i] = NULL;
        }
        tsn_bk_info_initialized = 1;
    }

    /* Clean up the bookkeeping resource */
    if (tsn_bk_info[unit] != NULL) {
        rv = bcmi_esw_tsn_bk_info_cleanup(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Allocate TSN bookkeeping resource for this unit. */
    TSN_ALLOC(unit, bk_info, bcmi_esw_tsn_bk_info_t,
              sizeof(bcmi_esw_tsn_bk_info_t),
              "TSN bookkeeping info", 0, rv);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "bcmi_esw_tsn_init: failed to "
                                "allocate memory\n")));
        return rv;
    }

    /* Get the chip dev_info structure. */
    tsn_bk_info[unit] = bk_info;
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_tsn_info_init(unit, &bk_info->tsn_dev_info);
    } else
#endif
    {
        rv = BCM_E_UNAVAIL;
    }

    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv) || bk_info->tsn_dev_info == NULL) {
        bcmi_esw_tsn_bk_info_cleanup(unit);
        if (BCM_SUCCESS(rv)) {
            return BCM_E_UNAVAIL;
        }
        return (rv);
    }

    /* Initialize TAF */
    if (soc_feature(unit, soc_feature_tsn_taf)) {
        rv = bcmi_esw_tsn_taf_init(unit);
        /* Release resource if any error occurred. */
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            return rv;
        }
    }

    /* Allocate TSN Priority Map bookkeeping resource for this unit */
    rv = bcmi_esw_tsn_pri_map_init(unit);

    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_bk_info_cleanup(unit);
        return rv;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Allocate TSN SR Mac Proxy bookkeeping resource for this unit */
        rv = bcmi_esw_tsn_sr_mac_proxy_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            return rv;
        }

        /* SR flow management initialization */
        rv = bcmi_esw_tsn_sr_flow_mgmt_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            return (rv);
        }

    }
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        /* Initialize SR Auto Learn management */
        rv = bcmi_esw_tsn_sr_auto_learn_init(unit);
        /* Release resource if any error occurred. */
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* TSN flow management initialization */
    rv = bcmi_esw_tsn_flow_mgmt_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_detach(unit);
        return (rv);
    }

    /* Allocate MTU /STU bookkeeping resource for this unit */
    rv = bcmi_esw_tsn_mtu_init(unit);
    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_bk_info_cleanup(unit);
        return rv;
    }

    rv = bcmi_esw_tsn_stu_init(unit);
    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_bk_info_cleanup(unit);
        return rv;
    }

    /* Initialize TSN Stat counters */
    rv = bcmi_esw_tsn_stat_init(unit);
    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_detach(unit);
        return rv;
    }

    /* Initialize TSN event management */
    rv = bcmi_esw_tsn_event_mgmt_init(unit);
    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_detach(unit);
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Warm Boot recovery */
    if (SOC_WARM_BOOT(unit)) {
        rv = bcmi_esw_tsn_reinit(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "Warm boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        /* Cold Boot */
        rv = bcmi_esw_tsn_coldboot_init(unit);
        if (BCM_FAILURE(rv)) {
            bcmi_esw_tsn_detach(unit);
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "Cold boot failed"
                                  "(rv = %d)\n"),
                                  rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_esw_tsn_control_set
 * Description:
 *     Internal function for configure device-based operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (IN) argument to be set
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_control_set(int unit, bcm_tsn_control_t type, uint32 arg)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_chip_ctrl_info_t *ctrl_info = NULL;
    int rv;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;

    ctrl_info = dev_info->tsn_chip_ctrl_info;
    if ((ctrl_info != NULL) &&
        (ctrl_info->tsn_chip_ctrl_set != NULL)) {
        rv = ctrl_info->tsn_chip_ctrl_set(unit, type, arg);
        if (BCM_SUCCESS(rv)) {
            bcmi_esw_tsn_evt_ageout_config(unit, type, arg);
        }
        return rv;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmi_esw_tsn_control_get
 * Description:
 *     Internal function for get the configure of device-based operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (OUT) argument got
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_control_get(int unit, bcm_tsn_control_t type, uint32 *arg)
{
    bcmi_esw_tsn_bk_info_t *bk_info = NULL;
    const bcmi_esw_tsn_dev_info_t *dev_info = NULL;
    const tsn_chip_ctrl_info_t *ctrl_info = NULL;

    BCM_IF_ERROR_RETURN(tsn_bk_info_instance_get(unit, &bk_info));
    dev_info = bk_info->tsn_dev_info;

    ctrl_info = dev_info->tsn_chip_ctrl_info;
    if ((ctrl_info != NULL) &&
        (ctrl_info->tsn_chip_ctrl_get != NULL)) {
        return ctrl_info->tsn_chip_ctrl_get(unit, type, arg);
    }

    return BCM_E_UNAVAIL;
}

#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_init
 * Purpose:
 *     Initialize the TSN module.
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_init(int unit)
{
#if defined(BCM_TSN_SUPPORT)
    /* initialize TSN info */
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_init(unit));

    return BCM_E_NONE;
#else /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_TSN_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_tsn_detach
 * Purpose:
 *     Clear the resource of the TSN module.
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_detach(int unit)
{
#if defined(BCM_TSN_SUPPORT)
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_detach(unit));

    return BCM_E_NONE;
#else /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_TSN_SUPPORT */
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *     bcm_esw_tsn_sync
 * Purpose:
 *   Store bookkeeping information for
 *   Warmboot recovery.
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_sync(int unit)
{
#if defined(BCM_TSN_SUPPORT)
    /* Store bookkeeping information */
    BCM_IF_ERROR_RETURN(bcmi_esw_tsn_sync(unit));

    return BCM_E_NONE;
#else /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_TSN_SUPPORT */
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcm_esw_tsn_sr_port_config_set
 * Purpose:
 *     Set SR configuration on a port
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port to configure
 *     config           - (IN) port SR configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_port_config_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_sr_port_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_port_config_set(unit, port, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_port_config_get
 * Purpose:
 *     Get SR configuration from a port
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port
 *     config           - (OUT) port SR configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_port_config_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_sr_port_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_port_config_get(unit, port, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_pri_map_create
 * Purpose:
 *      Add a priority map config to hardware.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Priority Map configuration
 *      map_id - (OUT) Map ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_pri_map_create(
    int unit,
    bcm_tsn_pri_map_config_t *config,
    bcm_tsn_pri_map_t *map_id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_pri_map_create(unit, config, map_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_pri_map_set
 * Purpose:
 *      Set priority map config with the given map_id.
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Map ID
 *      config - (IN) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_pri_map_set(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_pri_map_set(unit, map_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_pri_map_get
 * Purpose:
 *      Get the priority map config with the given map_id.
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Map ID
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_pri_map_get(
    int unit,
    bcm_tsn_pri_map_t map_id,
    bcm_tsn_pri_map_config_t *config)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_pri_map_get(unit, map_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_esw_tsn_pri_map_destroy
 * Purpose:
 *      Free the resource of map_id
 * Parameters:
 *      unit - (IN) Unit number
 *      map_id - (IN) Priority Map ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_pri_map_destroy(
    int unit,
    bcm_tsn_pri_map_t map_id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_pri_map_destroy(unit, map_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_pri_map_traverse
 * Purpose:
 *      Traverse the priority map config information.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_pri_map_traverse(
    int unit,
    bcm_tsn_pri_map_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_pri_map_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_tx_flowset_create
 * Purpose:
 *     Create an SR TX flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_tx_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_sr_tx_flow_config_t *def_cfg,
    bcm_tsn_sr_flowset_t *flowset)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return
            bcmi_esw_tsn_sr_tx_flowset_create(unit, pri_map, def_cfg, flowset);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_tx_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flowset, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_tx_flowset_config_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_sr_tx_flow_config_t *default_config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_tx_flowset_config_get(
                   unit, flowset, pri_map, default_config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flowset_create
 * Purpose:
 *     Create an SR RX flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_rx_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_sr_rx_flow_config_t *def_cfg,
    bcm_tsn_sr_flowset_t *flowset)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return
            bcmi_esw_tsn_sr_rx_flowset_create(unit, pri_map, def_cfg, flowset);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flowset, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flowset_config_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_sr_rx_flow_config_t *default_config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flowset_config_get(
                   unit, flowset, pri_map, default_config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_flowset_status_get
 * Purpose:
 *     Get current status of specified SR flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     status           - (OUT) status structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flowset_status_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    bcm_tsn_sr_flowset_status_t *status)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_flowset_status_get(unit, flowset, status);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_flowset_traverse
 * Purpose:
 *     Traverse all created SR flowsets
 * Parameters:
 *     unit             - (IN) unit number
 *     is_rx            - (IN) 1 for RX flowsets; 0 for TX flowsets
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data supplied to the callback function
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flowset_traverse(
    int unit,
    int is_rx,
    bcm_tsn_sr_flowset_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_flowset_traverse(unit, is_rx, cb, user_data);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_flowset_destroy
 * Purpose:
 *     Destroy an SR (TX or RX) flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flowset_destroy(int unit, bcm_tsn_sr_flowset_t flowset)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_flowset_destroy(unit, flowset);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_flowset_flow_get
 * Purpose:
 *     Retrieve an individual SR flow based on the flow offset from a flow set.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     index            - (IN) index of the flow
 *     flow_id          - (OUT) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flowset_flow_get(
    int unit,
    bcm_tsn_sr_flowset_t flowset,
    int index,
    bcm_tsn_sr_flow_t *flow_id)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_flowset_flow_get(unit, flowset, index, flow_id);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_tx_flow_config_get
 * Purpose:
 *     Get configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_tx_flow_config_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_tx_flow_config_get(unit, flow_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_tx_flow_config_set
 * Purpose:
 *     Set configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_tx_flow_config_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_tx_flow_config_set(unit, flow_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_tx_flow_status_get
 * Purpose:
 *     Get status for the specified flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     status           - (OUT) flow status
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a RX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_tx_flow_status_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_tx_flow_status_t *status)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_tx_flow_status_get(unit, flow_id, status);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_config_get
 * Purpose:
 *     Get configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_config_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_config_get(unit, flow_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_config_set
 * Purpose:
 *     Set configuration for the specified SR flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_config_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_config_set(unit, flow_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_status_get
 * Purpose:
 *     Get status for the specified flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     status           - (OUT) flow status
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_status_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_sr_rx_flow_status_t *status)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_status_get(unit, flow_id, status);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_reset
 * Purpose:
 *     Reset SR RX flow status
 * Parameters:
 *     unit             - (IN) unit number
 *     flags            - (IN) flags to select items to reset
 *     flow_id          - (IN) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_reset(int unit, uint32 flags, bcm_tsn_sr_flow_t flow_id)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_reset(unit, flags, flow_id);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_seqnum_history_set
 * Purpose:
 *     Set the sequence number history bits (one bit for one sequence number)
 *     in the RX flow.
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     offset_in_bits   - (IN) Offset (in bits) in the seqnum history window
 *     size_in_bits     - (IN) Size (in bits) to write
 *     history_bits     - (IN) Bit buffer to write
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_seqnum_history_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    int offset_in_bits,
    int size_in_bits,
    uint8 *history_bits)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_seqnum_history_set(
                unit, flow_id, offset_in_bits, size_in_bits, history_bits);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_sr_rx_flow_seqnum_history_get
 * Purpose:
 *     Get the sequence number history bits (one bit for one sequence number)
 *     in the RX flow.
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     offset_in_bits   - (IN) Offset (in bits) in the seqnum history window
 *     max_size_in_bits - (IN) Size (in bits) of the buffer
 *     history_bits     - (OUT) Bit buffer to read
 *     actual_size      - (OUT) Actual size (in bits) read to the buffer
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If it's called with a TX flow, BCM_E_BADID is returned
 */
int
bcm_esw_tsn_sr_rx_flow_seqnum_history_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    int offset_in_bits,
    int max_size_in_bits,
    uint8 *history_bits,
    int *actual_size)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_sr_rx_flow_seqnum_history_get(
            unit, flow_id, offset_in_bits, max_size_in_bits,
            history_bits, actual_size);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flowset_create
 * Purpose:
 *     Create a TSN flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     pri_map          - (IN) TSN priority map ID
 *     def_cfg          - (IN) default flow configuration
 *     flowset          - (OUT) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_create(
    int unit,
    bcm_tsn_pri_map_t pri_map,
    bcm_tsn_flow_config_t *def_cfg,
    bcm_tsn_flowset_t *flowset)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_create(unit, pri_map, def_cfg, flowset);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flowset_status_get
 * Purpose:
 *     Get current status of specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     status           - (OUT) status structure
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_status_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    bcm_tsn_flowset_status_t *status)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_status_get(unit, flowset, status);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flowset_flow_get
 * Purpose:
 *     Retrieve an individual TSN flow based on the flow offset from a flow set.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     index            - (IN) index of the flow
 *     flow_id          - (OUT) flow ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_flow_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    int index,
    bcm_tsn_flow_t *flow_id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_flow_get(unit, flowset, index, flow_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flowset_config_get
 * Purpose:
 *     Get default configuration for the specified TSN flowset
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 *     pri_map          - (OUT) TSN priority map ID
 *     default_config   - (OUT) default flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_config_get(
    int unit,
    bcm_tsn_flowset_t flowset,
    bcm_tsn_pri_map_t *pri_map,
    bcm_tsn_flow_config_t *default_config)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_config_get(unit, flowset,
                                               pri_map, default_config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flowset_traverse
 * Purpose:
 *     Traverse all created TSN flowsets
 * Parameters:
 *     unit             - (IN) unit number
 *     cb               - (IN) callback function
 *     user_data        - (IN) user data supplied to the callback function
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_traverse(
    int unit,
    bcm_tsn_flowset_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *     bcm_esw_tsn_flowset_destroy
 * Purpose:
 *     Destroy a TSN flowset.
 * Parameters:
 *     unit             - (IN) unit number
 *     flowset          - (IN) flowset ID
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flowset_destroy(int unit, bcm_tsn_flowset_t flowset)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flowset_destroy(unit, flowset);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flow_config_get
 * Purpose:
 *     Get configuration for the specified TSN flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (OUT) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flow_config_get(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flow_config_t *config)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flow_config_get(unit, flow_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_flow_config_set
 * Purpose:
 *     Set configuration for the specified TSN flow
 * Parameters:
 *     unit             - (IN) unit number
 *     flow_id          - (IN) flow ID
 *     config           - (IN) flow configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_flow_config_set(
    int unit,
    bcm_tsn_flow_t flow_id,
    bcm_tsn_flow_config_t *config)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_flow_config_set(unit, flow_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_group_create
 * Purpose:
 *   Add an Auto Learn config to hardware.
 * Parameters:
 *   unit     - (IN) Unit number
 *   config   - (IN) Auto Learn Group configuration
 *   group_id - (OUT) Group ID
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_group_create(
    int unit,
    bcm_tsn_sr_auto_learn_group_config_t *config,
    int *group_id)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_group_create(unit, config, group_id);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_group_get
 * Purpose:
 *   Get the Auto Learn config with the given group_id.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 *   config   - (OUT) Auto Learn Group configuration
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_group_get(
    int unit,
    int group_id,
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_group_get(unit, group_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_group_set
 * Purpose:
 *   Set Auto Learn config with the given group_id.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 *   config   - (IN) Auto Learn Group configuration
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_group_set(
    int unit,
    int group_id,
    bcm_tsn_sr_auto_learn_group_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_group_set(unit, group_id, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_group_destroy
 * Purpose:
 *   Destroy an Auto Learn config with the given group_id by removing it
 *   from hardware and deleting the associated software state info.
 * Parameters:
 *   unit     - (IN) Unit number
 *   group_id - (IN) Group ID
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_group_destroy(
    int unit,
    int group_id)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_group_destroy(unit, group_id);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_group_traverse
 * Purpose:
 *   Traverse the Auto Learn config information.
 * Parameters:
 *   unit      - (IN) Unit number
 *   cb        - (IN) Traverse call back function
 *   user_data - (IN) User data
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_group_traverse(
    int unit,
    bcm_tsn_sr_auto_learn_group_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_group_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_enable
 * Purpose:
 *   Enable/disable SR flow auto learn subsystem
 * Parameters:
 *   unit   - (IN) Unit number
 *   enable - (IN) Enable/Disable
 *   config - (IN) Auto Learn configuration
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_enable(
    int unit,
    int enable,
    bcm_tsn_sr_auto_learn_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_enable(unit, enable, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *   bcm_esw_tsn_sr_auto_learn_enable_get
 * Purpose:
 *   Get enable/disable SR flow auto learn subsystem
 * Parameters:
 *   unit   - (IN) Unit number
 *   enable - (OUT) Enable/disable
 *   config - (OUT) Auto Learn configuration
 * Returns:
 *   BCM_E_xxx
 */
int
bcm_esw_tsn_sr_auto_learn_enable_get(
    int unit,
    int *enabled,
    bcm_tsn_sr_auto_learn_config_t *config)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr) &&
        soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return bcmi_esw_tsn_sr_auto_learn_enable_get(unit, enabled, config);
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_control_set
 * Description:
 *     Configure device operation modes for TSN.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (IN) argument to be set
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_control_set(int unit, bcm_tsn_control_t type, uint32 arg)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_control_set(unit, type, arg);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_control_get
 * Description:
 *     Get the configure of device TSN operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (OUT) argument got
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_control_get(int unit, bcm_tsn_control_t type, uint32 *arg)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_control_get(unit, type, arg);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_port_control_set
 * Description:
 *     Configure port operation modes for TSN.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Port number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (IN) argument to be set
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_esw_tsn_port_control_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_control_t type,
    uint32 arg)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_port_control_set(unit, port, type, arg);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcm_esw_tsn_port_control_get
 * Description:
 *     Get the configure of port TSN operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     port - (IN) Port number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (OUT) argument got
 * Returns:
 *     BCM_E_XXX
 */
 int bcm_esw_tsn_port_control_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_control_t type,
    uint32 *arg)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_port_control_get(unit, port, type, arg);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_set
 * Description:
 *    Set 64-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (IN) 64-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    /* Write hardware and software accumulated counters */
    if (soc_feature(unit, soc_feature_tsn)) {
        rv = bcmi_esw_tsn_stat_portcnt_set(unit, port, stat, val);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_set32
 * Description:
 *    Set lower 32-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (IN) 32-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 *    Same as bcm_esw_tsn_port_stat_set, except the counter value is 32-bit.
 */
int
bcm_esw_tsn_port_stat_set32(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint32 val)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    uint64 val64;

    if (soc_feature(unit, soc_feature_tsn)) {
        COMPILER_64_SET(val64, 0, val);
        rv = bcm_esw_tsn_port_stat_set(unit, port, stat, val64);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_get
 * Description:
 *    Get 64-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (OUT) 64-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

    /* Input parameter check. */
    if (NULL == val) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    /* Read software accumulated counters */
    if (soc_feature(unit, soc_feature_tsn)) {
        rv = bcmi_esw_tsn_stat_portcnt_get(unit, port, 0, stat, val);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_get32
 * Description:
 *    Get lower 32-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (OUT) 32-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 *    Same as bcm_esw_tsn_port_stat_get, except converts result to 32-bit.
 */
int
bcm_esw_tsn_port_stat_get32(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    uint64 val64;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (NULL == val) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        COMPILER_64_ZERO(val64);
        rv = bcm_esw_tsn_port_stat_get(unit, port, stat, &val64);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        COMPILER_64_TO_32_LO(*val, val64);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_multi_set
 * Purpose:
 *    Set 64-bit counter value for multiple TSN statistic types.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (IN) Set statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_multi_set(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_set(unit, port, stat_arr[stix],
                                           value_arr[stix]);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_multi_set32
 * Purpose:
 *    Set lower 32-bit counter value for multiple TSN statistic types.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (IN) Set 32-bit statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_multi_set32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_set32(unit, port, stat_arr[stix],
                                             value_arr[stix]);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_multi_get
 * Purpose:
 *    Get 64-bit counter value for multiple TSN statistic types.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (OUT) Collected statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_multi_get(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_get(unit, port, stat_arr[stix],
                                           &(value_arr[stix]));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_multi_get32
 * Purpose:
 *    Get lower 32-bit counter value for multiple TSN statistic types.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (OUT) Collected 32-bit statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_multi_get32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_get32(unit, port, stat_arr[stix],
                                             &(value_arr[stix]));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_sync_get
 * Description:
 *    Get the specified hardware statistics (64-bit) from the device.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (OUT) 64-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_sync_get(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TSN_SUPPORT)
    /* Read software accumulated counters */
    if (soc_feature(unit, soc_feature_tsn)) {
        rv = bcmi_esw_tsn_stat_portcnt_get(unit, port, 1, stat, val);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_sync_get32
 * Description:
 *    Get the specified hardware statistics (32-bit) from the device.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *    val  - (OUT) 32-bit counter value.
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 *    Same as bcm_esw_tsn_port_stat_sync_get, except converts result to 32-bit.
 */
int
bcm_esw_tsn_port_stat_sync_get32(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    uint64 val64;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (NULL == val) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        COMPILER_64_ZERO(val64);
        rv = bcm_esw_tsn_port_stat_sync_get(unit, port, stat, &val64);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        COMPILER_64_TO_32_LO(*val, val64);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_sync_multi_get
 * Purpose:
 *    Get multiple hardware statistics (64-bit) from the device.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (OUT) Collected statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_sync_multi_get(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */

    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_sync_get(unit, port, stat_arr[stix],
                                                &(value_arr[stix]));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_port_stat_sync_multi_get32
 * Purpose:
 *    Get multiple hardware statistics (32-bit) from the device.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    nstat - (IN) Number of elements in stat array
 *    stat_arr - (IN) Array of TSN statistics types defined in bcm_tsn_stat_t
 *    value_arr - (OUT) Collected 32-bit statistics values
 * Returns:
 *    BCM_E_NONE - Success.
 *    BCM_E_PARAM - Illegal parameter.
 *    BCM_E_BADID - Illegal port number.
 *    BCM_E_INTERNAL - Chip access failure.
 *    BCM_E_UNAVAIL - Counter/variable is not implemented on this current chip.
 * Notes:
 */
int
bcm_esw_tsn_port_stat_sync_multi_get32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TSN_SUPPORT)
    int stix;
#endif /* BCM_TSN_SUPPORT */
    /* Input parameter check. */
    if (nstat <= 0) {
        return BCM_E_PARAM;
    }
    if ((NULL == stat_arr) || (NULL == value_arr)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        for (stix = 0; stix < nstat; stix++) {
            rv = bcm_esw_tsn_port_stat_sync_get32(unit, port, stat_arr[stix],
                                                  &(value_arr[stix]));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }

    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_tsn_stat_group_create
 * Description:
 *      Create a TSN stat group by picking stat types (in an array) to be
 *      included for counting
 * Parameters:
 *      unit             - (IN) unit number
 *      group_type       - (IN) TSN statistic group type
 *      count            - (IN) the number of elements in stat_arr
 *      stat_arr         - (IN) array for tsn stat types
 *      id               - (OUT) tsn statistic group id
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_stat_group_create(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    int count,
    bcm_tsn_stat_t *stat_arr,
    bcm_tsn_stat_group_t *id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_group_create(unit, group_type, count,
                                                      stat_arr, id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_stat_group_get
 * Description:
 *      Get configuration of the TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      id               - (IN) TSN statistic group id
 *      group_type       - (OUT) TSN statistic group type
 *      max              - (IN) the size of stat_arr
 *      stat_arr         - (OUT) array for returned TSN stat types
 *      count            - (OUT) actual number of TSN stat types
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_stat_group_get(
    int unit,
    bcm_tsn_stat_group_t id,
    bcm_tsn_stat_group_type_t *group_type,
    int max,
    bcm_tsn_stat_t *stat_arr,
    int *count)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_group_get(unit, id, group_type, max,
                                                   stat_arr, count);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_stat_group_set
 * Description:
 *      Set configuration of the TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      id               - (IN) TSN statistic group id
 *      count            - (IN) the number of elements in stat_arr
 *      stat_arr         - (IN) array for TSN stat types
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_stat_group_set(
    int unit,
    bcm_tsn_stat_group_t id,
    int count,
    bcm_tsn_stat_t *stat_arr)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_group_set(unit, id, count, stat_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_stat_group_destroy
 * Description:
 *      Destroy a TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      id               - (IN) TSN statistic group id
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_stat_group_destroy(int unit, bcm_tsn_stat_group_t id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_group_destroy(unit, id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_stat_group_traverse
 * Description:
 *      Traverse TSN stat groups
 * Parameters:
 *      unit             - (IN) unit number
 *      cb               - (IN) User provided call back function
 *      user_data        - (IN) User provided data
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_stat_group_traverse(
    int unit,
    bcm_tsn_stat_group_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_group_traverse(unit, cb, user_data);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_group_set
 * Description:
 *      Attach ingress/egress stat group to this flow and enable counting for
 *      the stat group. To disable counting for a specific group type, set
 *      BCM_TSN_STAT_GROUP_INVALID as stat_group
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      group_type       - (IN) TSN statistic group type
 *      stat_group       - (IN) TSN statistic group
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_group_set(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_group_type_t group_type,
    bcm_tsn_stat_group_t stat_group)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_flow_set(unit, flow, group_type,
                                                  stat_group);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_group_get
 * Description:
 *      Get the stat group for this flow with specified group type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      group_type       - (IN) TSN statistic group type
 *      stat_group       - (OUT)TSN statistic group
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_group_get(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_group_type_t group_type,
    bcm_tsn_stat_group_t *stat_group)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_stat_flowcnt_flow_get(unit, flow, group_type,
                                                  stat_group);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_set
 * Description:
 *      Set 64-bit counter value for specified TSN statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_set(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint64 val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_set(
                   unit, flow, 1, &stat, &val);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_set32
 * Description:
 *      Set lower 32-bit counter value for specified TSN statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_set32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint32 val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 value64;

        COMPILER_64_SET(value64, 0, val);
        return bcmi_esw_tsn_stat_flowcnt_set(
                   unit, flow, 1, &stat, &value64);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_get
 * Description:
 *      Get 64-bit counter value for specified TSN statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_get(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint64 *val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_get(
                   unit, flow, 0, 1, &stat, val);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_get32
 * Description:
 *      Get lower 32-bit counter value for specified TSN statistic type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_get32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint32 *val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 value64;
        int rv;

        if (NULL == val) {
            return BCM_E_PARAM;
        }
        rv = bcmi_esw_tsn_stat_flowcnt_get(
                 unit, flow, 0, 1, &stat, &value64);
        if (BCM_SUCCESS(rv)) {
            *val = COMPILER_64_LO(value64);
        }
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_multi_set
 * Description:
 *      Set 64-bit counter value for multiple TSN statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) number of element in stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_multi_set(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_set(
                   unit, flow, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_multi_set32
 * Description:
 *      Set lower 32-bit counter value for multiple TSN statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) number of element in stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (IN) counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_multi_set32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 *value64_arr = NULL;
        int i, rv;

        if (NULL == value_arr) {
            return BCM_E_PARAM;
        }
        value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
        if (NULL == value64_arr) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < nstat ; i++) {
            COMPILER_64_SET(value64_arr[i], 0, value_arr[i]);
        }
        rv = bcmi_esw_tsn_stat_flowcnt_set(
                 unit, flow, nstat, stat_arr, value64_arr);
        sal_free(value64_arr);
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_multi_get
 * Description:
 *      Get 64-bit counter value for multiple TSN statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_multi_get(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_get(
                   unit, flow, 0, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_multi_get32
 * Description:
 *      Get lower 32-bit counter value for multiple TSN statistic types.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_multi_get32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 *value64_arr = NULL;
        int i, rv;

        if (NULL == value_arr) {
            return BCM_E_PARAM;
        }
        value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
        if (NULL == value64_arr) {
            return BCM_E_MEMORY;
        }
        rv = bcmi_esw_tsn_stat_flowcnt_get(
                 unit, flow, 0, nstat, stat_arr, value64_arr);
        if (BCM_SUCCESS(rv)) {
            for (i = 0; i < nstat ; i++) {
                value_arr[i] = COMPILER_64_LO(value64_arr[i]);
            }
        }
        sal_free(value64_arr);
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_sync_get
 * Description:
 *      Get the specified hardware statistics (64-bit) from the device.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_sync_get(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint64 *val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_get(
                   unit, flow, 1, 1, &stat, val);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_sync_get32
 * Description:
 *      Get the specified hardware statistics (32-bit) from the device.
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      stat             - (IN) TSN stat types
 *      val              - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_sync_get32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    bcm_tsn_stat_t stat,
    uint32 *val)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 value64;
        int rv;

        if (NULL == val) {
            return BCM_E_PARAM;
        }
        rv = bcmi_esw_tsn_stat_flowcnt_get(
                 unit, flow, 1, 1, &stat, &value64);
        if (BCM_SUCCESS(rv)) {
            *val = COMPILER_64_LO(value64);
        }
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_sync_multi_get
 * Description:
 *      Get multiple hardware statistics (64-bit) from the device
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_sync_multi_get(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint64 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        return bcmi_esw_tsn_stat_flowcnt_get(
                   unit, flow, 1, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_tsn_sr_flow_stat_sync_multi_get32
 * Description:
 *      Get multiple hardware statistics (32-bit) from the device
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      nstat            - (IN) size of stat_arr and value_arr
 *      stat_arr         - (IN) TSN stat types
 *      value_arr        - (OUT)counter value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_sr_flow_stat_sync_multi_get32(
    int unit,
    bcm_tsn_sr_flow_t flow,
    int nstat,
    bcm_tsn_stat_t *stat_arr,
    uint32 *value_arr)
{
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        uint64 *value64_arr = NULL;
        int i, rv;

        if (NULL == value_arr) {
            return BCM_E_PARAM;
        }
        value64_arr = sal_alloc(nstat * sizeof(uint64), "value64_arr");
        if (NULL == value64_arr) {
            return BCM_E_MEMORY;
        }
        rv = bcmi_esw_tsn_stat_flowcnt_get(
                 unit, flow, 1, nstat, stat_arr, value64_arr);
        if (BCM_SUCCESS(rv)) {
            for (i = 0; i < nstat ; i++) {
                value_arr[i] = COMPILER_64_LO(value64_arr[i]);
            }
        }
        sal_free(value64_arr);
        return rv;
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *    bcm_esw_tsn_stat_threshold_set
 * Purpose:
 *    Configure threshold for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (IN) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_stat_threshold_set(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    int rv = BCM_E_UNAVAIL;

    /* Input parameter check */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        rv = bcmi_esw_tsn_stat_threshold_set(unit, source, stat, config);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *    bcm_esw_tsn_stat_threshold_get
 * Purpose:
 *    Get the threshold configuration for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (OUT) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_esw_tsn_stat_threshold_get(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    int rv = BCM_E_UNAVAIL;

    /* Input parameter check */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        rv = bcmi_esw_tsn_stat_threshold_get(unit, source, stat, config);
    }
#endif /* BCM_TSN_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_tsn_event_register
 * Purpose:
 *     The callback function registration to handle the TSN event on
 *     specified event source.
 * Parameters:
 *     unit             - (IN) unit number
 *     event            - (IN) TSN event
 *     src              - (IN) source triggeres the event
 *     cb               - (IN) callback function
 *     user_data        - (IN) Pointer to user data to supply in the callback.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_event_register(
    int unit,
    bcm_tsn_event_type_t event,
    bcm_tsn_event_source_t *src,
    bcm_tsn_event_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_event_register(unit, event, src, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_event_unregister
 * Purpose:
 *     The callback function unregistration of the TSN event on
 *     specified event source.
 * Parameters:
 *     unit             - (IN) unit number
 *     event            - (IN) TSN event
 *     src              - (IN) source triggeres the event
 *     cb               - (IN) callback function
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_event_unregister(
    int unit,
    bcm_tsn_event_type_t event,
    bcm_tsn_event_source_t *src,
    bcm_tsn_event_cb cb)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_event_unregister(unit, event, src, cb);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_tsn_event_notification_traverse
 * Purpose:
 *     Traverse all registered event notification callback
 * Parameters:
 *     unit             - (IN) unit number
 *     cb               - (IN) callback function
 *     user_data        - (IN) Pointer to user data to supply in the callback
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_esw_tsn_event_notification_traverse(
    int unit,
    bcm_tsn_event_notification_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn)) {
        return bcmi_esw_tsn_event_notification_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_mtu_profile_create
 * Purpose:
 *      Create mtu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress mtu profile
 *      config - (IN) configuration of the mtu profile
 *      mtu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_mtu_profile_create(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    bcm_tsn_mtu_config_t *config,
    int *mtu_profile_id)
{
    if (config == NULL || mtu_profile_id == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_mtu_profile_create(unit, type, config,
                                               mtu_profile_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_mtu_profile_get
 * Purpose:
 *      Get mtu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 *      type - (OUT) Indicates ingress or egress mtu profile
 *      config - (OUT) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_mtu_profile_get(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    bcm_tsn_mtu_config_t *config)
{
    if (config == NULL || type == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_mtu_profile_get(unit, mtu_profile_id,
                                            type, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_esw_tsn_mtu_profile_set
 * Purpose:
 *      Set mtu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 *      config - (IN) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_mtu_profile_set(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_mtu_profile_set(unit, mtu_profile_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_mtu_profile_destroy
 * Purpose:
 *      Clear the mtu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) Indicated the mtu profile entry
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_mtu_profile_destroy(
    int unit,
    int mtu_profile_id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_mtu_profile_destroy(unit, mtu_profile_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tsn_mtu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_mtu_profile_traverse(
    int unit,
    bcm_tsn_mtu_profile_traverse_cb cb,
    void *user_data)
{
    if (user_data == NULL || cb == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_mtu_profile_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_ingress_mtu_config_set
 * Purpose:
 *      Set ingress mtu profile source priority
 *      with the config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_ingress_mtu_config_set(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_ingress_mtu_config_set(unit, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_ingress_mtu_config_get
 * Purpose:
 *      Get ingress mtu profile source priority
 *      with the config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_ingress_mtu_config_get(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_ingress_mtu_config_get(unit, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_tsn_stu_profile_create
 * Purpose:
 *      Create stu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress stu profile
 *      config - (IN) configuration of the stu profile
 *      stu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_stu_profile_create(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    bcm_tsn_stu_config_t *config,
    int *stu_profile_id)
{
    if (config == NULL || stu_profile_id == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_stu_profile_create(unit, type, config,
                                               stu_profile_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_stu_profile_get
 * Purpose:
 *      Get stu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 *      type - (OUT) Indicates ingress or egress stu profile
 *      config - (OUT) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_stu_profile_get(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_profile_type_t *type,
    bcm_tsn_stu_config_t *config)
{
    if (config == NULL || type == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_stu_profile_get(unit, stu_profile_id,
                                            type, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_esw_tsn_stu_profile_set
 * Purpose:
 *      Set stu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 *      config - (IN) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_stu_profile_set(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_stu_profile_set(unit, stu_profile_id, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_stu_profile_destroy
 * Purpose:
 *      Clear the stu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) Indicated the stu profile entry
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_stu_profile_destroy(
    int unit,
    int stu_profile_id)
{
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_stu_profile_destroy(unit, stu_profile_id);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tsn_stu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_stu_profile_traverse(
    int unit,
    bcm_tsn_stu_profile_traverse_cb cb,
    void *user_data)
{
    if (user_data == NULL || cb == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_stu_profile_traverse(unit, cb, user_data);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_ingress_stu_config_set
 * Purpose:
 *      Set ingress stu profile source priority
 *      with the config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_ingress_stu_config_set(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_ingress_stu_config_set(unit, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tsn_ingress_stu_config_get
 * Purpose:
 *      Get ingress stu profile source priority
 *      with the config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_tsn_ingress_stu_config_get(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    if (config == NULL) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_esw_tsn_ingress_stu_config_get(unit, config);
    }
#endif /* BCM_TSN_SUPPORT */
    return BCM_E_UNAVAIL;
}

