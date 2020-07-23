/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Manage chip specific functionality for TSN implementation on GH2.
 */

#include <shared/bsl.h>
#include <sal/core/time.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/scache.h>
#include <bcm/tsn.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/hurricane3.h>
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw_dispatch.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(BCM_TSN_SUPPORT)

#if defined(BCM_TSN_SR_SUPPORT)
/*
 * SR Auto Learn management: HW platform specific constants
 */

/* TX & RX flow index 0 is invalid, thus only 4095 are available */
#define GH2_SR_AUTO_LEARN_TX_FLOW_SIZE        (4095)
#define GH2_SR_AUTO_LEARN_RX_FLOW_SIZE        (4095)
#ifdef BCM_FIRELIGHT_SUPPORT
#define FL_SR_AUTO_LEARN_TX_FLOW_SIZE        (255)
#define FL_SR_AUTO_LEARN_RX_FLOW_SIZE        (255)
#endif /* BCM_FIRELIGHT_SUPPORT */
/*
 * Default value (reset value) for Auto Learn Group Ports
 * Note:
 * 0 is not a INVALID value of L2MC
 * but there are two HW fields ING_SR_ENABLE & ING_SR_PORT_ROLE to make
 * sure the port is SR redudant port.
 */
#define GH2_SR_AUTO_LEARN_GROUP_PORTS_L2MC_DEFAULT_VALUE   (0)
#define GH2_SR_AUTO_LEARN_GROUP_PORTS_MACP_DEFAULT_VALUE   (0)

/*
 * SR flow management: HW platform specific constants
 */
#define TSN_SRFLOWS_RX_ACCEPT_SIZE_MIN      512
#define TSN_SRFLOWS_RX_ACCEPT_SIZE_MAX      65536
#define TSN_SRFLOWS_RX_ACCEPT_POWER_MIN     9
#define TSN_SRFLOWS_RX_ACCEPT_POWER_MAX     16
#define TSN_SRFLOWS_RX_WINDOW_SIZE_MIN      64
#define TSN_SRFLOWS_RX_WINDOW_SIZE_MAX      4096
#define TSN_SRFLOWS_RX_WINDOW_POWER_MIN     6
#define TSN_SRFLOWS_RX_WINDOW_POWER_MAX     12
#define TSN_SRFLOWS_RX_SNPOOL_SIZE_MIN      64
#define TSN_SRFLOWS_RX_SNPOOL_SIZE_MAX      1024
#define TSN_SRFLOWS_RX_SNPOOL_POWER_MIN     6
#define TSN_SRFLOWS_RX_SNPOOL_POWER_MAX     10
#define TSN_SRFLOWS_RX_SNPOOL_SIZE_COUNT    \
    (TSN_SRFLOWS_RX_SNPOOL_POWER_MAX - TSN_SRFLOWS_RX_SNPOOL_POWER_MIN + 1)
#define TSN_SRFLOWS_RX_SNPOOL_IDX_MAX       \
    (TSN_SRFLOWS_RX_SNPOOL_SIZE_COUNT - 1)
#define TSN_SRFLOWS_RX_SN_HIST_POOLS        4
#define TSN_SRFLOWS_RX_SN_HIST_POOL_SIZE    1024
#define TSN_SRFLOWS_RX_SN_HIST_POOL_POWER   10
#define TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE     256
#define TSN_SRFLOWS_RX_SN_HIST_MEM_POWER    8
#define TSN_SRFLOWS_RX_SN_HIST_SUB_INDEX    \
    (TSN_SRFLOWS_RX_SN_HIST_POOL_POWER - TSN_SRFLOWS_RX_SN_HIST_MEM_POWER)
#define TSN_SRFLOWS_RX_SN_HIST_SUB_POOLS    4
#define TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE   64
#define TSN_SRFLOWS_RX_AGE_TIME_MAX         32768
#define TSN_SRFLOWS_SEQNUM_VALUE_MAX        65535
#define TSN_SRFLOWS_FLOWID_TX_MASK          0x1000
#ifdef BCM_FIRELIGHT_SUPPORT
#define FL_TSN_SRFLOWS_FLOWID_TX_MASK          0x0100
#endif
#define TSN_SRFLOWS_DEFAULT_NEXT_SN         0

/* SR flow management: sequence number slice entry */
typedef struct tsn_sr_seqnum_slice_s {
    int index;                              /* Pool index */
    int entry;                              /* Entry in the pool */
    int offset;                             /* Offset in the entry */
    int power;                              /* size in power of bits */
    struct tsn_sr_seqnum_slice_s *next;
} tsn_sr_seqnum_slice_t;

/* SR flow management bookkeeping */
typedef struct gh2_tsn_sr_flows_bkinfo_s {
    /* Max number of flows (for efficiency) */
    int max_flows_rx;
    int max_flows_tx;

    /* seqnum history slice pools */
    tsn_sr_seqnum_slice_t *sn_pools[TSN_SRFLOWS_RX_SN_HIST_POOLS]
                                   [TSN_SRFLOWS_RX_SNPOOL_SIZE_COUNT];
    tsn_sr_seqnum_slice_t *sn_entries_avail;
    tsn_sr_seqnum_slice_t *sn_entries_allocated;
    sal_mutex_t snpool_mutex;

} gh2_tsn_sr_flows_bkinfo_t;

#endif /* BCM_TSN_SR_SUPPORT */

/* Structure definition for GH2 MTU profile memory bookkeeping information */
typedef struct gh2_tsn_mtu_bk_info_s {
    soc_profile_mem_t *mtu_profile[bcmTsnMtuProfileTypeCount];
    sal_mutex_t mutex;
} gh2_tsn_mtu_bk_info_t;

/* Structure definition for GH2 STU profile memory bookkeeping information */
typedef struct gh2_tsn_stu_bk_info_s {
    soc_profile_mem_t *stu_profile[bcmTsnStuProfileTypeCount];
    sal_mutex_t mutex;
} gh2_tsn_stu_bk_info_t;

/* Structure definition for GH2 PRP-forwarding bookkeeping information */
typedef struct gh2_tsn_prp_forwarding_bk_info_s {
    int prp_forwarding_enable;
    bcm_field_group_t prp_redirect_group;
    bcm_port_t  prp_redirect_port;
    bcm_gport_t prp_redirect_gport;
    int switch_init;
    sal_mutex_t mutex;
} gh2_tsn_prp_forwarding_bk_info_t;

typedef struct gh2_tsn_events_bkinfo_s {
    bcmi_esw_tsn_flow_event_notify_cb  gh2_flow_event_notify;
    bcmi_esw_tsn_cnt_exceed_notify_cb  gh2_cnt_exceed_notify;
    sal_mutex_t event_mutex;
    int         max_rx_flows;
    SHR_BITDCL  *active_rx_fbmp;    /* Current active rx flows */
    int         active_rx_fbmp_min;
    int         active_rx_fbmp_max;
    SHR_BITDCL  *wrong_lana_fbmp;   /* Watched flows for wrong lan events */
    SHR_BITDCL  *wrong_lanb_fbmp;   /* Watched flows for wrong lan events */
    SHR_BITDCL  *sn_reset_fbmp;     /* Watched flows for sn window reset */
    int         wrong_lana_min_cd;  /* Min count down value of wrong lan a */
    int         wrong_lanb_min_cd; /* Min count down value of wrong lan b */
    int         sn_reset_min_cd; /* Min count down value of sn reset */
    uint32      window_reset_mode;
    uint32      ageout_tick_ms;
    uint32      age_enable;
    sal_usecs_t ptime; /* previous time */
    int         wla_tick;
    int         wlb_tick;
    int         reset_tick;
    int         wla_mask;
    int         wlb_mask;
} gh2_tsn_events_bkinfo_t;

/*
 * TSN flow management: HW platform specific constants
 */
#define TSN_FLOWS_MAX_NUM_OF_FLOWS                  4095

/* Initialization flag for GH2 TSN bookkeeping info structure */
static int gh2_tsn_bkinfo_initialized = 0;

/* Structure definition for GH2 TSN bookkeeping info */
typedef struct gh2_tsn_bkinfo_s {
    gh2_tsn_mtu_bk_info_t gh2_mtu_bk_info;
    gh2_tsn_stu_bk_info_t gh2_stu_bk_info;
    gh2_tsn_prp_forwarding_bk_info_t gh2_prp_forwarding_bk_info;
#if defined(BCM_TSN_SR_SUPPORT)
    gh2_tsn_sr_flows_bkinfo_t sr_flows;
#endif /* BCM_TSN_SR_SUPPORT */
    gh2_tsn_events_bkinfo_t tsn_events;
} gh2_tsn_bkinfo_t;

/* GH2 TSN bookkeeping info */
STATIC gh2_tsn_bkinfo_t *gh2_tsn_bkinfo[BCM_MAX_NUM_UNITS];

/* Check if TSN bookkeeping info structure intialized */
#define TSN_BKINFO_IS_INIT(_unit_)                                        \
    do {                                                                  \
        if (!SOC_UNIT_VALID(_unit_)) {                                    \
            return BCM_E_UNIT;                                            \
        }                                                                 \
        if (gh2_tsn_bkinfo[(_unit_)] == NULL) {                           \
            LOG_ERROR(BSL_LS_BCM_TSN,                                     \
                      (BSL_META_U((_unit_),                               \
                                  "TSN Error: chip bookkeeping info not " \
                                  "initialized\n")));                     \
            return (BCM_E_INIT);                                          \
        }                                                                 \
    } while (0)

/* Function Enter/Leave Tracing */
#define GH2_TSN_FUNCTION_TRACE(_u_, _str_)                                    \
    do {                                                                      \
        LOG_DEBUG(BSL_LS_BCM_TSN,                                             \
                    (BSL_META_U((_u_),                                        \
                                "%s: " _str_ "\n"), FUNCTION_NAME()));        \
    } while (0)

/* For TSN bookkeeping access */
#define TSN_BKINFO(_u_) gh2_tsn_bkinfo[_u_]

/* Check if TSN MTU bookkeeping info structure initialized */
#define TSN_MTU_BKINFO_IS_INIT(_unit_, _type_)                                \
    do {                                                                      \
        TSN_BKINFO_IS_INIT(_unit_);                                           \
        if ((_type_) != bcmTsnMtuProfileTypeIngress &&                        \
            (_type_) != bcmTsnMtuProfileTypeEgress) {                         \
            return BCM_E_PARAM;                                               \
        }                                                                     \
        if ((NULL ==                                                          \
            gh2_tsn_bkinfo[(_unit_)]->gh2_mtu_bk_info.mtu_profile[_type_])) { \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_unit_),                                   \
                                  "TSN Error: MTU bookkeeping info not "      \
                                  "initialized\n")));                         \
            return BCM_E_INIT;                                                \
        }                                                                     \
    } while (0)


/* TSN event management: bookkeeping access */
#define EVENTS_BKINFO(_u_)         (&TSN_BKINFO(_u_)->tsn_events)

/* TSN event management: mutex  */
#define EVENT_MGMT_LOCK(_u_)                                            \
    sal_mutex_take(EVENTS_BKINFO(unit)->event_mutex, sal_mutex_FOREVER)
#define EVENT_MGMT_UNLOCK(_u_)                                          \
    sal_mutex_give(EVENTS_BKINFO(unit)->event_mutex)

/* MTU Bookkeeping Information lock/unlock */
#define MTU_BKINFO_LOCK(_u_) \
               sal_mutex_take(gh2_tsn_bkinfo[(_u_)]->gh2_mtu_bk_info.mutex, \
                   sal_mutex_FOREVER)

#define MTU_BKINFO_UNLOCK(_u_) \
               sal_mutex_give(gh2_tsn_bkinfo[(_u_)]->gh2_mtu_bk_info.mutex)

/* Check if TSN STU bookkeeping info structure initialized */
#define TSN_STU_BKINFO_IS_INIT(_unit_, _type_)                                \
    do {                                                                      \
        TSN_BKINFO_IS_INIT(_unit_);                                           \
        if ((_type_) != bcmTsnStuProfileTypeIngress &&                        \
            (_type_) != bcmTsnStuProfileTypeEgress) {                         \
            return BCM_E_PARAM;                                               \
        }                                                                     \
        if ((NULL ==                                                          \
            gh2_tsn_bkinfo[(_unit_)]->gh2_stu_bk_info.stu_profile[_type_])) { \
            LOG_ERROR(BSL_LS_BCM_TSN,                                         \
                      (BSL_META_U((_unit_),                                   \
                                  "TSN Error: STU bookkeeping info not "      \
                                  "initialized\n")));                         \
            return BCM_E_INIT;                                                \
        }                                                                     \
    } while (0)

/* STU Bookkeeping Information lock/unlock */
#define STU_BKINFO_LOCK(_u_) \
               sal_mutex_take(gh2_tsn_bkinfo[(_u_)]->gh2_stu_bk_info.mutex, \
                   sal_mutex_FOREVER)

#define STU_BKINFO_UNLOCK(_u_) \
               sal_mutex_give(gh2_tsn_bkinfo[(_u_)]->gh2_stu_bk_info.mutex)

/* GH2 support 8 internal priority for MTU/STU profile ID */
#define EGRESS_MTU_STU_INT_PRI_NUM 8

/* Egress MTU & STU profile ID encode value */
#define EGR_MTU_STU_PROFILE_ID_ENCODE 0x01000000

/* Refer to GH2 EGR_MTU_CHECK/EGR_STU_CHECK (66 ports) */
#define GH2_LOGIC_PORT_MAX_NUM 66

/* MTU profile memory information */
STATIC const soc_mem_t gh2_mtu_profile_mem[bcmTsnMtuProfileTypeCount] =
                           {MTU_PROFILEm,
                            EGR_MTU_PROFILEm};
/* STU profile memory information */
STATIC const soc_mem_t gh2_stu_profile_mem[bcmTsnStuProfileTypeCount] =
                           {STU_PROFILEm,
                            EGR_STU_PROFILEm};

/*
 * Definitions for Modified-PRP port's custom header(32 bits)
 *  +-------------------------------+
 *  |      Ethertype (16 bits)      |
 *  +--------------+----------------+
 *  | Non_PRP Flag | Port (15 bits) |
 *  +--------------+----------------+
 *
 *  Field location :
 *  - Port : b0~b14
 *  - Non_PRP Flag : b15
 *  - Ethertype : b16~b31
 *
 *  Note :
 *  - Macros defined below for the term of "CH" means "CustomHeader"
 */
#define PRP_FORWARDING_CH_HEADER_MASK        0xffffffff
#define PRP_FORWARDING_CH_ETHERTYPE_MASK     0xffff
#define PRP_FORWARDING_CH_ETHERTYPE_OFFSET   16
#define PRP_FORWARDING_CH_NON_PRP_OFFSET     15
#define PRP_FORWARDING_CH_PORT_MASK          0x7fff
#define PRP_FORWARDING_CH_PORT_OFFSET        0

/* SW defined a non-common value for Modified-PRP(loopback) port */
#define PRP_FORWARDING_CH_ETHERTYPE          0x10bc

#define PRP_FORWARDING_CH_ENCAP_VALUE(_non_prp_, _port_)                      \
        ((PRP_FORWARDING_CH_ETHERTYPE << PRP_FORWARDING_CH_ETHERTYPE_OFFSET) |\
         (((_non_prp_) ? 1 : 0) << PRP_FORWARDING_CH_NON_PRP_OFFSET) |        \
         (((_port_) & PRP_FORWARDING_CH_PORT_MASK) <<                         \
          PRP_FORWARDING_CH_PORT_OFFSET))

/* PRP-forwarding Bookkeeping Information lock/unlock */
#define PRP_FORWARDING_BKINFO_LOCK(_u_)                             \
               sal_mutex_take(gh2_tsn_bkinfo[(_u_)]->               \
               gh2_prp_forwarding_bk_info.mutex, sal_mutex_FOREVER)

#define PRP_FORWARDING_BKINFO_UNLOCK(_u_)                           \
               sal_mutex_give(gh2_tsn_bkinfo[(_u_)]->               \
               gh2_prp_forwarding_bk_info.mutex)

#define PRP_FORWARDING_NEED_PRP_SETTING(_config_)           \
        (((bcmTsnSrPortTypeNone != (_config_).port_type) && \
          (bcmTsnSrPortTypePrp == (_config_).port_type ||   \
           bcmTsnSrPortModeInterworkingPrp == (_config_).port_mode)) ? 1 : 0)

#define PRP_FORWARDING_NEED_EGRESS_MASK_SETTING(_config_)            \
        ((bcmTsnSrPortTypeNone != (_config_).port_type &&            \
          bcmTsnSrPortTypePrp != (_config_).port_type &&             \
          bcmTsnSrPortTypeDot1cb != (_config_).port_type &&          \
          bcmTsnSrPortModeInterworkingPrp != (_config_).port_mode && \
          0 == (_config_).mac_da_flows) ? 1 : 0)

#define PRP_FORWARDING_REDIRECT_GROUP_INVALID    (0)

#define PRP_FORWARDING_ENABLE(_u_)\
        (TSN_BKINFO(_u_)->gh2_prp_forwarding_bk_info.prp_forwarding_enable)
#define PRP_FORWARDING_SWITCH_INIT(_u_)\
        (TSN_BKINFO(_u_)->gh2_prp_forwarding_bk_info.switch_init)
#define PRP_FORWARDING_REDIRECT_GROUP(_u_)\
        (TSN_BKINFO(_u_)->gh2_prp_forwarding_bk_info.prp_redirect_group)
#define PRP_FORWARDING_REDIRECT_PORT(_u_)\
        (TSN_BKINFO(_u_)->gh2_prp_forwarding_bk_info.prp_redirect_port)
#define PRP_FORWARDING_REDIRECT_GPORT(_u_)\
        (TSN_BKINFO(_u_)->gh2_prp_forwarding_bk_info.prp_redirect_gport)

#define PRP_FORWARDING_ERROR_RETURN(_op_)                      \
        do {                                                   \
            int _rv_ = (_op_);                                 \
            if (BCM_FAILURE(_rv_)) {                           \
                return (_rv_);                                 \
            }                                                  \
        } while (0)

#define PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(_op_, _extra_clean_)       \
        do {                                                              \
            int _rv_ = (_op_);                                            \
            if (BCM_FAILURE(_rv_)) {                                      \
                (_extra_clean_);                                          \
                return (_rv_);                                            \
            }                                                             \
        } while (0)

#define PRP_FORWARDING_CLEAN_ERROR_RETURN(_op_)                           \
        do {                                                              \
            int _rv_ = (_op_);                                            \
            if (BCM_FAILURE(_rv_)) {                                      \
                LOG_WARN(BSL_LS_BCMAPI_TSN,                               \
                         (BSL_META_U(                                     \
                              unit,                                       \
                              "prp_forwarding clean failed\n")));         \
                return;                                                   \
            }                                                             \
        } while (0)

#define PRP_FORWARDING_CREATE_ERROR_RETURN(_op_)                              \
        do {                                                                  \
            int _rv_ = (_op_);                                                \
            if (BCM_FAILURE(_rv_)) {                                          \
                bcmi_gh2_tsn_sr_port_prp_forwarding_destroy(unit, prp_port);  \
                return (_rv_);                                                \
            }                                                                 \
        } while (0)

#define PRP_FORWARDING_DESTROY_ERROR_RETURN(_op_)                         \
        do {                                                              \
            int _rv_ = (_op_);                                            \
            if (BCM_FAILURE(_rv_)) {                                      \
                LOG_WARN(BSL_LS_BCMAPI_TSN,                               \
                         (BSL_META_U(                                     \
                              unit,                                       \
                              "prp_forwarding destroy port %d failed\n"), \
                              prp_port));                                 \
                return;                                                   \
            }                                                             \
        } while (0)

#define PRP_FORWARDING_DESTROY_ERROR_RETURN_WITH_CLEAN(_op_, _extra_clean_) \
        do {                                                                \
            int _rv_ = (_op_);                                              \
            if (BCM_FAILURE(_rv_)) {                                        \
                (_extra_clean_);                                            \
                LOG_WARN(BSL_LS_BCMAPI_TSN,                                 \
                         (BSL_META_U(                                       \
                              unit,                                         \
                              "prp_forwarding destroy port %d failed\n"),   \
                              prp_port));                                   \
                return;                                                     \
            }                                                               \
        } while (0)

#if defined(BCM_TSN_SR_SUPPORT)

/* SR flow management: quick array to get power of 2 */
static const int tsn_math_powers[] = {
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
    8192, 16384, 32768, 65536
};

/* SR flow management: SR RX flow seqnum history field names */
static const soc_field_t tsn_flow_sr_hist_indexs[] = {
    SN_HISTORY_INDEX_1f, SN_HISTORY_INDEX_2f,
    SN_HISTORY_INDEX_3f, SN_HISTORY_INDEX_4f
};
static const soc_field_t tsn_flow_sr_hist_offsets[] = {
    SN_HISTORY_OFFSET_1f, SN_HISTORY_OFFSET_2f,
    SN_HISTORY_OFFSET_3f, SN_HISTORY_OFFSET_4f
};
static const soc_field_t tsn_flow_sr_hist_sizes[] = {
    SN_HISTORY_SIZE_1f, SN_HISTORY_SIZE_2f,
    SN_HISTORY_SIZE_3f, SN_HISTORY_SIZE_4f
};

/* SR flow management: SR RX seqnum history pool table names */
static const soc_mem_t tsn_flow_sr_hist_pools[] = {
    SR_SN_HISTORY_0m,
    SR_SN_HISTORY_1m,
    SR_SN_HISTORY_2m,
    SR_SN_HISTORY_3m,
    SR_SN_HISTORY_4m,
    SR_SN_HISTORY_5m,
    SR_SN_HISTORY_6m,
    SR_SN_HISTORY_7m,
    SR_SN_HISTORY_8m,
    SR_SN_HISTORY_9m,
    SR_SN_HISTORY_10m,
    SR_SN_HISTORY_11m,
    SR_SN_HISTORY_12m,
    SR_SN_HISTORY_13m,
    SR_SN_HISTORY_14m,
    SR_SN_HISTORY_15m
};

/* SR flow management: helper macros for flow ID conversion */
#define TSN_SRFLOWS_IDX_TO_HW_ID(_tx_, _idx_)                               \
    (((_idx_) + 1) | ((_tx_) ? TSN_SRFLOWS_FLOWID_TX_MASK : 0))
#define TSN_SRFLOWS_HW_ID_IS_TX(_id_)                                       \
    (((_id_) & TSN_SRFLOWS_FLOWID_TX_MASK) ? 1 : 0)
#define TSN_SRFLOWS_HW_ID_TO_IDX(_id_)                                      \
    (((_id_) & (~TSN_SRFLOWS_FLOWID_TX_MASK)) - 1)

#ifdef BCM_FIRELIGHT_SUPPORT
#define FL_TSN_SRFLOWS_IDX_TO_HW_ID(_tx_, _idx_)                               \
    (((_idx_) + 1) | ((_tx_) ? FL_TSN_SRFLOWS_FLOWID_TX_MASK : 0))
#define FL_TSN_SRFLOWS_HW_ID_IS_TX(_id_)                                       \
    (((_id_) & FL_TSN_SRFLOWS_FLOWID_TX_MASK) ? 1 : 0)
#define FL_TSN_SRFLOWS_HW_ID_TO_IDX(_id_)                                      \
    (((_id_) & (~FL_TSN_SRFLOWS_FLOWID_TX_MASK)) - 1)
#endif

/* SR flow management: bookkeeping access (for easier porting) */
#define SRFLOWS_BKINFO(_u_)         (TSN_BKINFO(_u_)->sr_flows)

/* SR flow management: convenient macro for seqnum pool protecting */
#define SRFLOWS_SNPOOL_LOCK(_u_)                                            \
    sal_mutex_take(SRFLOWS_BKINFO(unit).snpool_mutex, sal_mutex_FOREVER)
#define SRFLOWS_SNPOOL_UNLOCK(_u_)                                          \
    sal_mutex_give(SRFLOWS_BKINFO(unit).snpool_mutex)

/* SR flow management: convenient macro to access seqnum slice pools */
#define SR_SNPOOL(_idx, _pow)       SRFLOWS_BKINFO(unit).sn_pools[_idx][_pow]

/* SR flow management: helper macro to get a slice from pool */
#define TSN_SRFLOW_GET_SN_SLICE(_sn, _idx, _pow)                            \
    do {                                                                    \
        (_sn) = SR_SNPOOL((_idx), (_pow));                                  \
        if ((_sn) != NULL) {                                                \
            SR_SNPOOL((_idx), (_pow)) = (_sn)->next;                        \
            (_sn)->next = NULL;                                             \
        }                                                                   \
    } while (0)

/* Convenient macro: check whether a number is power of 2 */
#define TSN_IS_POWER_OF_2(_n_)              (!((_n_) & ((_n_) - 1)))

/* Convenient macro: get log2 for a non-zero power-of-2 number */
#define TSN_GET_LOG2(_n_, _power_)                                          \
    do {                                                                    \
        unsigned int _num = (unsigned int)(_n_);                            \
        for ((_power_) = -1; _num; (_power_)++) {                           \
            _num >>= 1;                                                     \
        }                                                                   \
    } while (0)

/*
 * SR Auto Learn :
 * flow ID is 13-bit (1st lead bit is global for TX and RX)
 * Doing 12-bit mask for NEXT_FLOW_ID
 */
#define AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(_idx_)                        \
        ((_idx_) & 0x0FFF)
#define AUTO_LEARN_HW_FLOW_ID_TO_FLOW_ID(_type_, _idx_)                \
        (((_type_) == bcmi_sr_auto_learn_flow_tx ? 0x1000 : 0x0) |     \
         ((_idx_) & 0x0FFF))
#ifdef BCM_FIRELIGHT_SUPPORT
#define FL_AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(_idx_)                        \
        ((_idx_) & 0x00FF)
#define FL_AUTO_LEARN_HW_FLOW_ID_TO_FLOW_ID(_type_, _idx_)                \
        (((_type_) == bcmi_sr_auto_learn_flow_tx ? 0x0100 : 0x0) |     \
         ((_idx_) & 0x00FF))
#endif
#endif /* BCM_TSN_SR_SUPPORT */

/* TSN flow management: helper macros for flow ID conversion */
#define TSN_FLOWS_IDX_TO_HW_ID(_idx_)               ((_idx_) + 1)
#define TSN_FLOWS_HW_ID_TO_IDX(_id_)                ((_id_) - 1)

STATIC int
bcmi_gh2_tsn_mtu_profile_decode(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    int *index);

STATIC int
bcmi_gh2_tsn_mtu_profile_encode(
    int unit,
    int index,
    bcm_tsn_mtu_profile_type_t type,
    int *mtu_profile_id);

#if defined(BCM_TSN_SR_SUPPORT)

/* SR flow mgmt - free seqnum history pool */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slice_free(
    int unit,
    bcmi_tsn_sr_seqnum_slice_t slice)
{
    tsn_sr_seqnum_slice_t *org = (tsn_sr_seqnum_slice_t *)slice;
    tsn_sr_seqnum_slice_t *sn, *sn2, *prev;
    int entries;
    int pow;
    int offset;
    int merged;
    int i;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == slice) {
        return BCM_E_PARAM;
    }

    /* validate it first before putting back into the pools */
    i = 0;
    entries = soc_mem_index_count(unit, SR_SN_HISTORY_0m);
    for (sn = org; sn != NULL; sn = sn->next) {
        /* The total number of chained slices < number of pools */
        i++;
        if (i > TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }

        /* Validate item values */
        if (sn->index < 0 || sn->index >= TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }
        if (sn->power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
            sn->power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
            return BCM_E_PARAM;
        }
        if (sn->offset < 0 || sn->offset >= TSN_SRFLOWS_RX_SNPOOL_SIZE_MAX) {
            return BCM_E_PARAM;
        }
        if (sn->offset % tsn_math_powers[sn->power]) {
            /* offset should be multiples of the slice size */
            return BCM_E_PARAM;
        }
        if (sn->entry < 0 || sn->entry >= entries) {
            return BCM_E_PARAM;
        }
    }

    /* Put back to the pools one by one */
    while (org != NULL) {
        /* Handle one slice at a time */
        sn = org;
        org = sn->next;
        sn->next = NULL;

        /* Start putting back the slice */
        SRFLOWS_SNPOOL_LOCK(unit);

        /* See if we can find another half and merge into a bigger one */
        pow = sn->power - TSN_SRFLOWS_RX_SNPOOL_POWER_MIN;
        for (i = pow; i < TSN_SRFLOWS_RX_SNPOOL_IDX_MAX; i++) {
            /*
             * Calculate the offset value for the slice of another half
             * Another offset = this_offset XOR slice_size
             * (offset must be multiples of the slice_size)
             */
            offset = sn->offset ^ tsn_math_powers[sn->power];

            /* Find the slice with the offset within the same entry */
            merged = 0;
            prev = NULL;
            for (sn2 = SR_SNPOOL(sn->index, i); sn2 != NULL; sn2 = sn2->next) {
                if (sn2->offset == offset) {
                    if (sn2->entry == sn->entry) {

                        /* Found. Remove it from the pool */
                        if (prev == NULL) {
                            SR_SNPOOL(sn->index, i) = sn2->next;
                        } else {
                            prev->next = sn2->next;
                        }

                        /* Change it to be next size with offset corrected */
                        sn2->power++;
                        sn2->offset &= ~tsn_math_powers[sn->power];
                        sn2->next = NULL;

                        /* Free the original one */
                        sn->next = SRFLOWS_BKINFO(unit).sn_entries_avail;
                        SRFLOWS_BKINFO(unit).sn_entries_avail = sn;

                        /* This becomes the one to return to the pool */
                        sn = sn2;
                        pow = sn2->power - TSN_SRFLOWS_RX_SNPOOL_POWER_MIN;
                        merged = 1;
                        break;
                    }
                }
                prev = sn2;
            }
            /*
             * If no merge happens, just return it to the pool.
             * Otherwise try to merge in the next size level.
             */
            if (!merged) {
                break;
            }
        }

        /* Return the slice to the pool */
        sn->next = SR_SNPOOL(sn->index, pow);
        SR_SNPOOL(sn->index, pow) = sn;

        /* Done this one */
        SRFLOWS_SNPOOL_UNLOCK(unit);
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - allocate seqnum history pool */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slice_alloc(
    int unit,
    int winsize,
    bcmi_tsn_sr_seqnum_slice_t *slice)
{
    tsn_sr_seqnum_slice_t *sn, *sn2, *sn3, *sn4;
    int power, pow;
    int i, j, k, m;

    /* For convenience */
    static const int num_pools = TSN_SRFLOWS_RX_SN_HIST_POOLS;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == slice || winsize <= 0 || !TSN_IS_POWER_OF_2(winsize)) {
        return BCM_E_PARAM;
    }

    /* Validate and correct size */
    TSN_GET_LOG2(winsize, power);
    if (power == 0) {
        /* Special case: use 64-bit slice for 1-bit window size */
        power = TSN_SRFLOWS_RX_WINDOW_POWER_MIN;
    }
    if (power < TSN_SRFLOWS_RX_WINDOW_POWER_MIN ||
        power > TSN_SRFLOWS_RX_WINDOW_POWER_MAX) {
        return BCM_E_PARAM;
    }
    pow = power - TSN_SRFLOWS_RX_WINDOW_POWER_MIN; /* pool slot index */

    /* Start finding matching slices */
    SRFLOWS_SNPOOL_LOCK(unit);

    /*
     * 1. Check for exact match
     *    Only applies when the window size is one of the slice size
     */
    if (pow <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX) {
        for (i = 0; i < num_pools; i++) {
            if (SR_SNPOOL(i, pow) != NULL) {
                /* Got one exact match slice */
                TSN_SRFLOW_GET_SN_SLICE(sn, i, pow);
                *slice = (bcmi_tsn_sr_seqnum_slice_t)sn;
                SRFLOWS_SNPOOL_UNLOCK(unit);
                return BCM_E_NONE;
            }
        }
    }

    /*
     * 2. Check if the following conditions can meet
     *    - 1/2 + 1/2
     *    - 1/2 + 1/4 + 1/4
     *    - 1/2 + 1/4 + 1/8 + 1/8
     *    This only applies to
     *    - numbers that can be broken down to half (so power >= 1)
     *    - numbers that are at most twice of the max slice size
     *      (so pow <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX + 1)
     */
    if (pow >= 1 && pow <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX + 1) {
        /* Find the first 1/2 slice */
        for (i = 0; i < num_pools; i++) {
            if (SR_SNPOOL(i, pow - 1) != NULL) {
                /* Found the first 1/2 slice, now find the 2nd 1/2 slice */
                for (j = 0; j < num_pools; j++) {
                    if (j == i) {
                        continue;
                    }
                    if (SR_SNPOOL(j, pow - 1) != NULL) {
                        /* Found 1/2 + 1/2 slice, chain and return */
                        TSN_SRFLOW_GET_SN_SLICE(sn, i, pow - 1);
                        assert(sn != NULL);
                        TSN_SRFLOW_GET_SN_SLICE(sn2, j, pow - 1);
                        sn->next = sn2;
                        *slice = (bcmi_tsn_sr_seqnum_slice_t)sn;
                        SRFLOWS_SNPOOL_UNLOCK(unit);
                        return BCM_E_NONE;
                    }
                }

                /* Only one 1/2 slice available, so try to find 1/4 slices */
                if (pow >= 2) {
                    for (j = 0; j < num_pools; j++) {
                        if (j == i) {
                            continue;
                        }
                        if (SR_SNPOOL(j, pow - 2) != NULL) {
                            /* Found first 1/4, now find another 1/4 slice */
                            for (k = 0; k < num_pools; k++) {
                                if (k == i || k == j) {
                                    continue;
                                }
                                if (SR_SNPOOL(k, pow - 2) != NULL) {
                                    /* Found 1/2 + 1/4 + 1/4 */
                                    TSN_SRFLOW_GET_SN_SLICE(sn, i, pow - 1);
                                    assert(sn != NULL);
                                    TSN_SRFLOW_GET_SN_SLICE(sn2, j, pow - 2);
                                    assert(sn2 != NULL);
                                    TSN_SRFLOW_GET_SN_SLICE(sn3, k, pow - 2);
                                    sn2->next = sn3;
                                    sn->next = sn2;
                                    *slice = (bcmi_tsn_sr_seqnum_slice_t)sn;
                                    SRFLOWS_SNPOOL_UNLOCK(unit);
                                    return BCM_E_NONE;
                                }
                            }

                            /* Try to find remaining 1/8 + 1/8 */
                            if (pow >= 3) {
                                m = -1;
                                for (k = 0; k < num_pools; k++) {
                                    if (k == i || k == j) {
                                        continue;
                                    }
                                    if (SR_SNPOOL(k, pow - 3) != NULL) {
                                        if (m == -1) {
                                            /* Found first 1/8, record it */
                                            m = k;
                                        } else {
                                            /* Found 1/2 + 1/4 + 1/8 x 2 */
                                            TSN_SRFLOW_GET_SN_SLICE(
                                                sn, i, pow - 1);
                                            assert(sn != NULL);
                                            TSN_SRFLOW_GET_SN_SLICE(
                                                sn2, j, pow - 2);
                                            assert(sn2 != NULL);
                                            TSN_SRFLOW_GET_SN_SLICE(
                                                sn3, m, pow - 3);
                                            assert(sn3 != NULL);
                                            TSN_SRFLOW_GET_SN_SLICE(
                                                sn4, k, pow - 3);
                                            sn3->next = sn4;
                                            sn2->next = sn3;
                                            sn->next = sn2;
                                            *slice =
                                                (bcmi_tsn_sr_seqnum_slice_t)sn;
                                            SRFLOWS_SNPOOL_UNLOCK(unit);
                                            return BCM_E_NONE;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /*
     * 3. Check if we can find 1/4 + 1/4 + 1/4 + 1/4
     *    - numbers that can be broken down to 1/4 (so power >= 2)
     *    - numbers that are at most 4 times of the max slice size
     *      (so pow <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX + 2)
     *    Note this algorithm is optimized for 4 pools.
     */
    if (pow >= 2 && pow <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX + 2) {
        /* Check if all pools have 1/4 slices */
        for (i = 0; i < num_pools; i++) {
            if (SR_SNPOOL(i, pow - 2) == NULL) {
                break;
            }
        }
        if (i == num_pools) {
            /* All pools has 1/4 slices, chain them and return */
            sn = NULL;
            for (j = 0; j < num_pools; j++) {
                TSN_SRFLOW_GET_SN_SLICE(sn2, j, pow - 2);
                assert(sn2 != NULL);
                if (sn == NULL) {
                    sn = sn2;
                } else {
                    sn2->next = sn;
                    sn = sn2;
                }
            }
            *slice = (bcmi_tsn_sr_seqnum_slice_t)sn;
            SRFLOWS_SNPOOL_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /*
     * 4. Cannot find smaller slices for combining.
     *    Try to break down bigger slices for it.
     */
    if (pow >= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX) {
        /* No bigger slice than this */
        SRFLOWS_SNPOOL_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }
    /* Find bigger slices to break, starting from next size */
    for (m = pow + 1; m <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX; m++) {
        /* Scan through all pools for this size */
        for (i = 0; i < num_pools; i++) {
            if (SR_SNPOOL(i, m) != NULL) {
                /* break it until we get the required size */
                for (j = m; j > pow; j--) {

                    /* Take this one out of the pool */
                    sn = SR_SNPOOL(i, j);
                    assert(sn != NULL);
                    SR_SNPOOL(i, j) = sn->next;

                    /* Break size into half */
                    sn->power--;

                    /* Get another slice instance */
                    sn2 = SRFLOWS_BKINFO(unit).sn_entries_avail;
                    if (sn2 == NULL) {
                        assert(0); /* shouldn't happen as we allocated max */
                        SRFLOWS_SNPOOL_UNLOCK(unit);
                        return BCM_E_RESOURCE;
                    }
                    SRFLOWS_BKINFO(unit).sn_entries_avail = sn2->next;

                    /* Fill the new slice */
                    sn2->index = sn->index;
                    sn2->entry = sn->entry;
                    sn2->power = sn->power;
                    sn2->next = NULL;

                    /* Calculate new offset */
                    assert(sn->power < sizeof(tsn_math_powers) / sizeof(int));
                    sn2->offset = sn->offset + tsn_math_powers[sn->power];

                    /* Chain them together */
                    sn->next = sn2;

                    /* Put into the pool of the new size */
                    sn2->next = SR_SNPOOL(i, j - 1);
                    SR_SNPOOL(i, j - 1) = sn;
                }

                /* Now we should have available slices */
                sn = SR_SNPOOL(i, pow);
                assert(sn != NULL);
                SR_SNPOOL(i, pow) = sn->next;
                sn->next = NULL;
                *slice = (bcmi_tsn_sr_seqnum_slice_t)sn;
                SRFLOWS_SNPOOL_UNLOCK(unit);
                return BCM_E_NONE;
            }
        }
    }

    /* Cannot find slices to fulfill the request */
    SRFLOWS_SNPOOL_UNLOCK(unit);
    return BCM_E_RESOURCE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* SR flow mgmt - break slice until specified slice found (called with lock) */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slice_wb_break(
    int unit,
    tsn_sr_seqnum_slice_t *slice,
    int offset,
    int power,
    tsn_sr_seqnum_slice_t **pslice)
{
    tsn_sr_seqnum_slice_t *sn, *sn2, *sn_break, *sn_put;
    int diff;
    int pow;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (slice == NULL || pslice == NULL ||
        offset < 0 || offset >= TSN_SRFLOWS_RX_SNPOOL_SIZE_MAX ||
        power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
        power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
        return BCM_E_PARAM;
    }

    /* This routine is for warm boot only */
    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Validate supplied slice supplied by internal caller */
    assert(slice->entry >= 0);
    assert(slice->entry < (uint32)soc_mem_index_count(unit, SR_SN_HISTORY_0m));
    assert(slice->offset >= 0);
    assert(slice->offset < TSN_SRFLOWS_RX_SN_HIST_POOL_SIZE);
    assert(slice->power >= TSN_SRFLOWS_RX_SNPOOL_POWER_MIN);
    assert(slice->power <= TSN_SRFLOWS_RX_SNPOOL_POWER_MAX);
    assert(slice->power > power);

    /* break it until we get the specified slice */
    sn = slice;
    for (;;) {

        /* Break size into half */
        sn->power--;
        assert(sn->power >= power);

        /* Get another slice instance */
        sn2 = SRFLOWS_BKINFO(unit).sn_entries_avail;
        if (sn2 == NULL) {
            assert(0); /* shouldn't happen as we allocated max */
            SRFLOWS_SNPOOL_UNLOCK(unit);
            return BCM_E_RESOURCE;
        }
        SRFLOWS_BKINFO(unit).sn_entries_avail = sn2->next;

        /* Fill the new slice */
        sn2->index = sn->index;
        sn2->entry = sn->entry;
        sn2->power = sn->power;
        sn2->next = NULL;

        /* Calculate new offset */
        assert(sn->power < sizeof(tsn_math_powers) / sizeof(int));
        diff = tsn_math_powers[sn->power];
        sn2->offset = sn->offset + diff;

        /* Check which one to break and which one to put back */
        if (offset & diff) {
            sn_break = sn2;
            sn_put = sn;
        } else {
            sn_break = sn;
            sn_put = sn2;
        }

        /* Put back unused slice into the pool */
        pow = sn->power - TSN_SRFLOWS_RX_WINDOW_POWER_MIN;
        sn_put->next = SR_SNPOOL(sn->index, pow);
        SR_SNPOOL(sn->index, pow) = sn_put;

        /* Return if we have found it */
        if (sn_break->power == power) {
            assert(sn_break->offset == offset);
            *pslice = sn_break;
            break;
        }

        /* Not yet, break it further */
        sn = sn_break;
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - get specified SN slice from the pools */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slice_wb_restore(
    int unit,
    int index,
    int entry,
    int offset,
    int power,
    tsn_sr_seqnum_slice_t **pslice)
{
    tsn_sr_seqnum_slice_t *sn, *prev;
    int offset2;
    int power2;
    int pow;
    int m;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (pslice == NULL ||
        index < 0 || index >= TSN_SRFLOWS_RX_SN_HIST_POOLS ||
        entry < 0 ||
        entry >= (uint32)soc_mem_index_count(unit, SR_SN_HISTORY_0m) ||
        offset < 0 || offset >= TSN_SRFLOWS_RX_SNPOOL_SIZE_MAX ||
        power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
        power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
        return BCM_E_PARAM;
    }

    /* This routine is for warm boot only */
    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Start finding the specified slice */
    pow = power - TSN_SRFLOWS_RX_WINDOW_POWER_MIN; /* pool slot index */
    SRFLOWS_SNPOOL_LOCK(unit);

    /* Check if the exact slice exists */
    prev = NULL;
    for (sn = SR_SNPOOL(index, pow); sn != NULL; prev = sn, sn = sn->next) {
        if (sn->entry == entry && sn->offset == offset) {
            if (prev == NULL) {
                SR_SNPOOL(index, pow) = sn->next;
            } else {
                prev->next = sn->next;
            }
            *pslice = sn;
            SRFLOWS_SNPOOL_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /* Need to break down bigger slice to get what we want */
    if (pow >= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX) {
        /* No bigger slice than this */
        SRFLOWS_SNPOOL_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }
    /* Find bigger slices to break, starting from next size */
    for (m = pow + 1; m <= TSN_SRFLOWS_RX_SNPOOL_IDX_MAX; m++) {
        /* Find the slice containing the specified window */
        if (SR_SNPOOL(index, m) != NULL) {
            power2 = m + TSN_SRFLOWS_RX_WINDOW_POWER_MIN;
            offset2 = offset & (~(tsn_math_powers[power2] - 1));
            prev = NULL;
            for (sn = SR_SNPOOL(index, m); sn != NULL; sn = sn->next) {
                if (sn->entry == entry && sn->offset == offset2) {
                    if (prev == NULL) {
                        SR_SNPOOL(index, m) = sn->next;
                    } else {
                        prev->next = sn->next;
                    }
                    rv = bcmi_gh2_tsn_sr_flows_seqnum_slice_wb_break(
                            unit, sn, offset, power, pslice);
                    SRFLOWS_SNPOOL_UNLOCK(unit);
                    return rv;
                }
                prev = sn;
            }
        }
    }

    SRFLOWS_SNPOOL_UNLOCK(unit);
    return BCM_E_RESOURCE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* SR flow mgmt - read seqnum history pool for a flow */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_hist_read(
    int unit,
    int idx,
    int winsize,
    bcmi_tsn_sr_seqnum_slice_t slice,
    int offset,
    int max_size,
    uint8 *history,
    int *actual)
{
    tsn_sr_seqnum_slice_t *sn, *slices[TSN_SRFLOWS_RX_SN_HIST_POOLS];
    int left;       /* bits left to read */
    int off;        /* current offset to read for this flow */
    int pos;        /* starting position within the flow for a slice */
    int sz;         /* size of the slice */
    int poff;       /* current offset within a pool */
    int pbits;      /* bits to read within a pool */
    int sidx;       /* sub pool (per memory access) index */
    int soff;       /* current offset within a sub pool */
    int sbits;      /* bits to read within a sub pool */
    int boff;       /* buffer offset in bits */
    soc_mem_t mem;
    sr_sn_history_0_entry_t hist_entry;
    uint32 fldbuf[TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE / sizeof(uint32)];
    int rv;
    int i, j;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == history || NULL == slice ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx ||
        (winsize < TSN_SRFLOWS_RX_WINDOW_SIZE_MIN && winsize != 1) ||
        winsize > TSN_SRFLOWS_RX_WINDOW_SIZE_MAX) {
        return BCM_E_PARAM;
    }

    /* Put slice in the order of pool index and validate */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        slices[i] = NULL;
    }
    pos = 0;
    for (sn = (tsn_sr_seqnum_slice_t *)slice; sn != NULL; sn = sn->next) {
        if (sn->index < 0 || sn->index >= TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }
        if (slices[sn->index] != NULL) {
            return BCM_E_PARAM;
        }
        slices[sn->index] = sn;
        if (sn->power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
            sn->power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
            return BCM_E_PARAM;
        }
        pos += tsn_math_powers[sn->power];
    }
    if (pos != winsize) {
        if (pos != TSN_SRFLOWS_RX_SNPOOL_SIZE_MIN || winsize != 1) {
            /* The only case that pos != winsize is that winsize = 1 */
            return BCM_E_PARAM;
        }
    }

    /* Calculate bits to read */
    max_size = (max_size > (winsize - offset)) ? (winsize - offset) : max_size;

    /* Try to read it pool by pool */
    off = offset;           /* Offset of this flow */
    left = max_size;        /* Remaining bits to read */
    pos = 0;                /* Slice starting position of this flow */
    boff = 0;               /* Buffer offset */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        if (slices[i] == NULL) {
            continue;
        }
        sn = slices[i];
        sz = tsn_math_powers[sn->power];    /* Size of this slice */

        /* Check if our target is within this slice */
        if (off < pos || off >= pos + sz) {
            pos += sz;
            continue;
        }

        /* Calculate starting offset within this pool */
        poff = sn->offset + (off - pos);

        /* Calculate bits to read within this pool */
        pbits = sz - (off - pos);
        pbits = (pbits > left) ? left : pbits;

        /* Loop through sub pools */
        for (j = 0; pbits > 0 && j < TSN_SRFLOWS_RX_SN_HIST_SUB_POOLS; j++) {
            /* Check if we need to read this sub pool */
            if (poff < j * TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE ||
                poff >= (j + 1) * TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE) {
                continue;
            }

            /* Calculate actual HW sub pool index */
            sidx = (i << TSN_SRFLOWS_RX_SN_HIST_SUB_INDEX) |
                   (poff >> TSN_SRFLOWS_RX_SN_HIST_MEM_POWER);

            /* Read the entry and the field from the sub pool */
            assert(sidx < sizeof(tsn_flow_sr_hist_pools) / sizeof(soc_mem_t));
            mem = tsn_flow_sr_hist_pools[sidx];
            sal_memset(&hist_entry, 0, sizeof(hist_entry));
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, sn->entry, &hist_entry);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to read mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                return rv;
            }
            soc_mem_field_get(
                unit, mem, (uint32 *)&hist_entry, SN_WINDOWf, fldbuf);

            /* Calculate starting offset within this sub pool */
            soff = poff & (TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE - 1);

            /* Calculate bits to read within this sub pool */
            sbits = TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE - soff;
            sbits = (sbits > pbits) ? pbits : sbits;

            /* Copy bits from field to buffer */
            SHR_BITCOPY_RANGE(
                (SHR_BITDCL *)history, boff, (SHR_BITDCL *)fldbuf, soff, sbits);

            /* Advance offsets and remaining bits */
            poff += sbits;
            off += sbits;
            pbits -= sbits;
            boff += sbits;
            left -= sbits;
        }

        /* Update position for next slice */
        pos += sz;
    }

    *actual = max_size;
    return BCM_E_NONE;
}

/* SR flow mgmt - write seqnum history pool for a flow */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_hist_write(
    int unit,
    int idx,
    int winsize,
    bcmi_tsn_sr_seqnum_slice_t slice,
    int offset,
    int size,
    uint8 *history)
{
    tsn_sr_seqnum_slice_t *sn, *slices[TSN_SRFLOWS_RX_SN_HIST_POOLS];
    int left;       /* bits left to write */
    int off;        /* current offset to write for this flow */
    int pos;        /* starting position within the flow for a slice */
    int sz;         /* size of the slice */
    int poff;       /* current offset within a pool */
    int pbits;      /* bits to write within a pool */
    int sidx;       /* sub pool (per memory access) index */
    int soff;       /* current offset within a sub pool */
    int sbits;      /* bits to write within a sub pool */
    int boff;       /* buffer offset in bits */
    uint32 bsel;    /* seqnum sub pool block select */
    soc_mem_t mem;
    sr_sn_history_0_entry_t hist_entry;
    uint32 fldbuf[TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE / sizeof(uint32)];
    int rv;
    int i, j, k;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == history || NULL == slice ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx ||
        (winsize < TSN_SRFLOWS_RX_WINDOW_SIZE_MIN && winsize != 1) ||
        winsize > TSN_SRFLOWS_RX_WINDOW_SIZE_MAX) {
        return BCM_E_PARAM;
    }

    /* Put slice in the order of pool index and validate */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        slices[i] = NULL;
    }
    pos = 0;
    for (sn = (tsn_sr_seqnum_slice_t *)slice; sn != NULL; sn = sn->next) {
        if (sn->index < 0 || sn->index >= TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }
        if (slices[sn->index] != NULL) {
            return BCM_E_PARAM;
        }
        slices[sn->index] = sn;
        if (sn->power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
            sn->power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
            return BCM_E_PARAM;
        }
        pos += tsn_math_powers[sn->power];
    }
    if (pos != winsize) {
        if (pos != TSN_SRFLOWS_RX_SNPOOL_SIZE_MIN || winsize != 1) {
            /* The only case that pos != winsize is that winsize = 1 */
            return BCM_E_PARAM;
        }
    }

    /* Writing across winsize is not allowed */
    if (size > (winsize - offset)) {
        return BCM_E_PARAM;
    }

    /* Try to write it pool by pool */
    off = offset;           /* Offset of this flow */
    left = size;            /* Remaining bits to write */
    pos = 0;                /* Slice starting position of this flow */
    boff = 0;               /* Buffer offset */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        if (slices[i] == NULL) {
            continue;
        }
        sn = slices[i];
        sz = tsn_math_powers[sn->power];    /* Size of this slice */

        /* Check if our target is within this slice */
        if (off < pos || off >= pos + sz) {
            pos += sz;
            continue;
        }

        /* Calculate starting offset within this pool */
        poff = sn->offset + (off - pos);

        /* Calculate bits to write within this pool */
        pbits = sz - (off - pos);
        pbits = (pbits > left) ? left : pbits;

        /* Loop through sub pools */
        for (j = 0; pbits > 0 && j < TSN_SRFLOWS_RX_SN_HIST_SUB_POOLS; j++) {
            /* Check if we need to write this sub pool */
            if (poff < j * TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE ||
                poff >= (j + 1) * TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE) {
                continue;
            }

            /* Calculate actual HW sub pool index */
            sidx = (i << TSN_SRFLOWS_RX_SN_HIST_SUB_INDEX) |
                   (poff >> TSN_SRFLOWS_RX_SN_HIST_MEM_POWER);

            /* read the entry and the field from the sub pool */
            assert(sidx < sizeof(tsn_flow_sr_hist_pools) / sizeof(soc_mem_t));
            mem = tsn_flow_sr_hist_pools[sidx];
            sal_memset(&hist_entry, 0, sizeof(hist_entry));
            MEM_LOCK(unit, mem);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, sn->entry, &hist_entry);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to read mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            soc_mem_field_get(
                unit, mem, (uint32 *)&hist_entry, SN_WINDOWf, fldbuf);

            /* Calculate starting offset within this sub pool */
            soff = poff & (TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE - 1);

            /* Calculate bits to write within this sub pool */
            sbits = TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE - soff;
            sbits = (sbits > pbits) ? pbits : sbits;

            /* Fill block selections */
            bsel = 0;
            for (k = 0;
                 k < TSN_SRFLOWS_RX_SN_HIST_MEM_SIZE /
                     TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE;
                 k++) {
                if (soff < (k + 1) * TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE &&
                    soff + sbits > k * TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE) {
                    bsel |= 1 << k;
                }
            }
            soc_mem_field32_set(unit, mem, &hist_entry, BLOCK_SELECTf, bsel);

            /* Copy bits from buffer to field */
            SHR_BITCOPY_RANGE(
                (SHR_BITDCL *)fldbuf, soff, (SHR_BITDCL *)history, boff, sbits);
            soc_mem_field_set(
                unit, mem, (uint32 *)&hist_entry, SN_WINDOWf, fldbuf);

            /* Write to HW */
            rv = soc_mem_write(
                unit, mem, MEM_BLOCK_ALL, sn->entry, &hist_entry);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to write mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            MEM_UNLOCK(unit, mem);

            /* Advance offsets and remaining bits */
            poff += sbits;
            off += sbits;
            pbits -= sbits;
            boff += sbits;
            left -= sbits;
        }

        /* Update position for next slice */
        pos += sz;
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - reset seqnum history pool for a flow */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_hist_reset(
    int unit,
    int idx,
    int winsize,
    bcmi_tsn_sr_seqnum_slice_t slice,
    uint32 reset_mode)
{
    uint8 history[BITS2BYTES(TSN_SRFLOWS_RX_WINDOW_SIZE_MAX)];

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == slice ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx ||
        (winsize < TSN_SRFLOWS_RX_WINDOW_SIZE_MIN && winsize != 1) ||
        winsize > TSN_SRFLOWS_RX_WINDOW_SIZE_MAX) {
        return BCM_E_PARAM;
    }

    /* Prepare seqnum values */
    if (reset_mode == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1 ||
        reset_mode == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW) {
        /* All zeroes */
        sal_memset(history, 0, BITS2BYTES(winsize));
    } else if (reset_mode == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW2) {
        /* All 1's except the last bit */
        if (winsize == 1) {
            /* Special case: 1 bit (it's the first bit in this byte) */
            history[0] = 0;
        } else {
            sal_memset(history, 0xff, BITS2BYTES(winsize));
            history[BITS2BYTES(winsize) - 1] &= ~0x80;
        }
    } else {
        /* Reset mode not supported */
        return BCM_E_UNAVAIL;
    }

    /* Call write routine to write the values */
    return bcmi_gh2_tsn_sr_flows_seqnum_hist_write(
                unit, idx, winsize, slice, 0, winsize, history);
}

/* SR flow mgmt - clean up seqnum history slice pools */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slices_cleanup(int unit)
{
    int i, j;

    /* Free resources */
    TSN_BKINFO_IS_INIT(unit);
    if (SRFLOWS_BKINFO(unit).sn_entries_allocated != NULL) {
        sal_free(SRFLOWS_BKINFO(unit).sn_entries_allocated);
        SRFLOWS_BKINFO(unit).sn_entries_allocated = NULL;
    }
    if (SRFLOWS_BKINFO(unit).snpool_mutex != NULL) {
        sal_mutex_destroy(SRFLOWS_BKINFO(unit).snpool_mutex);
        SRFLOWS_BKINFO(unit).snpool_mutex = NULL;
    }

    /* Clear values */
    SRFLOWS_BKINFO(unit).sn_entries_avail = NULL;
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        for (j = 0; j < TSN_SRFLOWS_RX_SNPOOL_SIZE_COUNT; j++) {
            SR_SNPOOL(i, j) = NULL;
        }
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - intiailize seqnum history slice pools */
STATIC int
bcmi_gh2_tsn_sr_flows_seqnum_slices_init(int unit)
{
    tsn_sr_seqnum_slice_t *slice;
    uint32 entries;
    uint32 i, j;

    TSN_BKINFO_IS_INIT(unit);

    /* Calculate max entries to be pre-allcoated */
    entries = (uint32)soc_mem_index_count(unit, SR_SN_HISTORY_0m);
    j = entries *                                       /* entries per pool */
        TSN_SRFLOWS_RX_SN_HIST_POOL_SIZE *              /* size per entry */
        TSN_SRFLOWS_RX_SN_HIST_POOLS                    /* pools */
        / TSN_SRFLOWS_RX_SNPOOL_SIZE_MIN;               /* min slice size */

    /* Pre-allocate slice entries for efficiency */
    SRFLOWS_BKINFO(unit).sn_entries_allocated = (tsn_sr_seqnum_slice_t *)
        sal_alloc(sizeof(tsn_sr_seqnum_slice_t) * j, "tsn sr sn slices");
    if (SRFLOWS_BKINFO(unit).sn_entries_allocated == NULL) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN SR: failed to allocate SN pool slices\n")));
        return BCM_E_MEMORY;
    }

    /* Initialize the free slice entries list by hooking them together */
    slice = SRFLOWS_BKINFO(unit).sn_entries_allocated;
    for (i = 0; i < j - 1; i++) {
        slice[i].next = &slice[i + 1];
    }
    slice[j - 1].next = NULL;
    SRFLOWS_BKINFO(unit).sn_entries_avail = &slice[0];

    /* Initialize slices linked list for all sizes */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        for (j = 0; j < TSN_SRFLOWS_RX_SNPOOL_SIZE_COUNT; j++) {
            SR_SNPOOL(i, j) = NULL;
        }
    }

    /* Create mutex for protecting seqnum slices */
    SRFLOWS_BKINFO(unit).snpool_mutex = sal_mutex_create("tsn sr sn pool");
    if (NULL == SRFLOWS_BKINFO(unit).snpool_mutex) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("TSN SR: failed to create SN pool mutex\n")));
        (void)bcmi_gh2_tsn_sr_flows_seqnum_slices_cleanup(unit);
        return BCM_E_MEMORY;
    }

    /* Prepare the entries with max size for seqnum allocation */
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        /* Add max-size slices */
        for (j = 0; j < entries; j++) {

            /* Get one empty entry */
            slice = SRFLOWS_BKINFO(unit).sn_entries_avail;
            SRFLOWS_BKINFO(unit).sn_entries_avail = slice->next;

            /* Fill with entry info and max size */
            slice->index = i;
            slice->entry = entries - j - 1;
            slice->offset = 0;
            slice->power = TSN_SRFLOWS_RX_SNPOOL_POWER_MAX;

            /* Hook to it */
            slice->next = SR_SNPOOL(i, TSN_SRFLOWS_RX_SNPOOL_IDX_MAX);
            SR_SNPOOL(i, TSN_SRFLOWS_RX_SNPOOL_IDX_MAX) = slice;
        }
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - get number of RX/TX flows */
STATIC int
bcmi_gh2_tsn_sr_flows_get_num_flows(int unit, int *rx_flows, int *tx_flows)
{
    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == rx_flows || NULL == tx_flows) {
        return BCM_E_PARAM;
    }

    /* Get the flows based on number of entries in HW memory (if not yet) */
    if (SRFLOWS_BKINFO(unit).max_flows_rx == 0) {

        /* The actual valid flow count - 1 since HW flow 0 is invalid */
        SRFLOWS_BKINFO(unit).max_flows_rx =
            soc_mem_index_count(unit, SR_RXm) - 1;
        SRFLOWS_BKINFO(unit).max_flows_tx =
            soc_mem_index_count(unit, SR_TXm) - 1;
    }

    /* Return the cached values */
    *rx_flows = SRFLOWS_BKINFO(unit).max_flows_rx;
    *tx_flows = SRFLOWS_BKINFO(unit).max_flows_tx;

    return BCM_E_NONE;
}

/* SR flow mgmt - convert flow index to HW flow ID */
STATIC int
bcmi_gh2_tsn_sr_flows_get_hw_flow_id(
    int unit,
    int is_tx,
    int fidx,
    uint32 *hw_id)
{
    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == hw_id || fidx < 0) {
        return BCM_E_PARAM;
    }

    /* Check if it's valid */
    if (is_tx) {
        if (fidx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
            return BCM_E_PARAM;
        }
    } else {
        if (fidx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
            return BCM_E_PARAM;
        }
    }

    /* Convert it */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        *hw_id = (uint32)FL_TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, fidx);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        *hw_id = (uint32)TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, fidx);
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - convert flow index to HW flow ID */
STATIC int
bcmi_gh2_tsn_sr_flows_get_sw_flow_idx(
    int unit,
    uint32 hw_id,
    int *is_tx,
    int *fidx)
{
    int tx;
    int idx;

    /* parameter check */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == is_tx || NULL == fidx) {
        return BCM_E_PARAM;
    }

    /* Convert it */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        tx = FL_TSN_SRFLOWS_HW_ID_IS_TX((int)hw_id);
        idx = FL_TSN_SRFLOWS_HW_ID_TO_IDX((int)hw_id);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        tx = TSN_SRFLOWS_HW_ID_IS_TX((int)hw_id);
        idx = TSN_SRFLOWS_HW_ID_TO_IDX((int)hw_id);
    }

    /* Check if it's valid */
    if (TSN_BKINFO(unit) != NULL) {
        TSN_BKINFO_IS_INIT(unit);
        if (tx) {
            if (idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
                return BCM_E_PARAM;
            }
        } else {
            if (idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
                return BCM_E_PARAM;
            }
        }
    }

    *is_tx = tx;
    *fidx = idx;
    return BCM_E_NONE;
}

/* SR flow mgmt - check flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_check_rx_flow_config(
    int unit,
    bcm_tsn_sr_rx_flow_config_t *cfg)
{
    int index;
    bcm_tsn_mtu_profile_type_t type;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == cfg) {
        return BCM_E_PARAM;
    }

    /* Check min/max window sizes (special case: history size can be 1) */
    if ((cfg->seqnum_history_win_size != 1 &&
         cfg->seqnum_history_win_size < TSN_SRFLOWS_RX_WINDOW_SIZE_MIN) ||
         cfg->seqnum_history_win_size > TSN_SRFLOWS_RX_WINDOW_SIZE_MAX ||
         cfg->seqnum_accept_win_size < TSN_SRFLOWS_RX_ACCEPT_SIZE_MIN ||
         cfg->seqnum_accept_win_size > TSN_SRFLOWS_RX_ACCEPT_SIZE_MAX) {
        return BCM_E_PARAM;
    }

    /* Check if they're power of 2 */
    if (!TSN_IS_POWER_OF_2(cfg->seqnum_accept_win_size) ||
        !TSN_IS_POWER_OF_2(cfg->seqnum_history_win_size)) {
        return BCM_E_PARAM;
    }

    /* History window size should be smaller than acceptance window size */
    if (cfg->seqnum_history_win_size >= cfg->seqnum_accept_win_size) {
        return BCM_E_PARAM;
    }

    /* Check age out time (for GH2, it must be power of 2) */
    if (!TSN_IS_POWER_OF_2(cfg->age_timeout) ||
        cfg->age_timeout > TSN_SRFLOWS_RX_AGE_TIME_MAX) {
        return BCM_E_PARAM;
    }

    /* Check flags */
    if (cfg->flags &
        ~(BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_DUPLICATES |
          BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_PKT_IN_DROP_WINDOW |
          BCM_TSN_SR_RX_FLOW_CONFIG_DROP_OUT_OF_ORDER |
          BCM_TSN_SR_RX_FLOW_CONFIG_DO_NOT_CUT_THROUGH)) {
          /* Found unsupported flags */
          return BCM_E_UNAVAIL;
    }

    /* Check MTU profile id */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Using MTU decode function to do profile id validation */
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_mtu_profile_decode(
                              unit, cfg->ingress_mtu_profile, &type, &index));
        if (type == bcmTsnMtuProfileTypeEgress) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - check flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_check_tx_flow_config(
    int unit,
    bcm_tsn_sr_tx_flow_config_t *cfg)
{
    int index;
    bcm_tsn_mtu_profile_type_t type;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == cfg) {
        return BCM_E_PARAM;
    }

    /* check sequence number */
    if (cfg->seq_num > TSN_SRFLOWS_SEQNUM_VALUE_MAX) {
        return BCM_E_PARAM;
    }

    /* Check MTU profile id */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Using MTU decode function to do profile id validation */
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_mtu_profile_decode(
                              unit, cfg->ingress_mtu_profile, &type, &index));
        if (type == bcmTsnMtuProfileTypeEgress) {
            return BCM_E_PARAM;
        }

    }

    return BCM_E_NONE;
}

/* SR flow mgmt - clear flow config - mtu */
STATIC int
bcmi_gh2_tsn_sr_flows_clear_rx_flow_mtu(int unit, int idx)
{
    soc_mem_t mem;
    ing_mtu_check_2_entry_t mtu_ent;
    int rv;
    int is_tx = 0;
    uint32 hw_id;

    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        hw_id = (uint32)FL_TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        hw_id = (uint32)TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
    }
    mem = ING_MTU_CHECK_2m;
    sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field32_set(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf, 0);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    MEM_UNLOCK(unit, mem);
    return BCM_E_NONE;
}

/* SR flow mgmt - clear flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_clear_rx_flow(int unit, int idx)
{
    soc_mem_t mem;
    sr_rx_entry_t entry;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
        return BCM_E_PARAM;
    }

    /* Clear the entry with HW index idx+1 since entry 0 is not visible */
    mem = SR_RXm;
    sal_memset(&entry, 0, sizeof(entry));
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /* Clear MTU profile id HW */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        rv = bcmi_gh2_tsn_sr_flows_clear_rx_flow_mtu(unit, idx);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to clear mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - clear flow config - mtu */
STATIC int
bcmi_gh2_tsn_sr_flows_clear_tx_flow_mtu(int unit, int idx)
{
    soc_mem_t mem;
    ing_mtu_check_2_entry_t mtu_ent;
    int rv;
    int is_tx = 1;
    uint32 hw_id;

    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        hw_id = (uint32)FL_TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        hw_id = (uint32)TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
    }
    mem = ING_MTU_CHECK_2m;
    sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field32_set(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf, 0);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }

    MEM_UNLOCK(unit, mem);
    return BCM_E_NONE;
}

/* SR flow mgmt - clear flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_clear_tx_flow(int unit, int idx)
{
    soc_mem_t mem;
    sr_tx_entry_t entry;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
        return BCM_E_PARAM;
    }

    /* Write to HW with HW index idx+1 since entry 0 is not visible to SW */
    mem = SR_TXm;
    sal_memset(&entry, 0, sizeof(entry));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field32_set(
        unit, mem, &entry, NEXT_SNf, TSN_SRFLOWS_DEFAULT_NEXT_SN);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }

    /* Clear MTU profile id HW */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        rv = bcmi_gh2_tsn_sr_flows_clear_tx_flow_mtu(unit, idx);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to clear mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
    }
    MEM_UNLOCK(unit, mem);

    return BCM_E_NONE;
}

/* SR flow mgmt - reset RX flow */
STATIC int
bcmi_gh2_tsn_sr_flows_reset_rx_flow(int unit, int idx, uint32 flags)
{
    soc_mem_t mem;
    sr_rx_entry_t entry;
    uint32 v;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
        return BCM_E_PARAM;
    }

    /* Write to HW with HW index idx+1 since entry 0 is not visible to SW */
    mem = SR_RXm;
    sal_memset(&entry, 0, sizeof(entry));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }

    /* seqnum age count down and wrong LAN clear count down */
    v = soc_mem_field32_get(unit, mem, &entry, SN_AGE_OUT_ENABLEf);
    if (v) {
        /* Calculate the countdown if aging enabled, otherwise fill with 0 */
        v = 1 << soc_mem_field32_get(unit, mem, &entry, AGE_OUT_MULTIPLIERf);
    }
    if (flags & BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_AGE_COUNTDOWN) {
        soc_mem_field32_set(
            unit, mem, &entry, SN_AGE_OUT_COUNT_DOWNf, v);
    }
    if (flags & BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_A_AGE_COUNTDOWN) {
        /* Reset the value only if the indicator bit is set */
        if (soc_mem_field32_get(unit, mem, &entry, WRONG_LAN_Af)) {
            soc_mem_field32_set(
                unit, mem, &entry, WRONG_LAN_A_AGE_OUT_COUNT_DOWNf, v);
        }
    }
    if (flags & BCM_TSN_SR_RX_FLOW_RESET_WRONGLAN_B_AGE_COUNTDOWN) {
        /* Reset the value only if the indicator bit is set */
        if (soc_mem_field32_get(unit, mem, &entry, WRONG_LAN_Bf)) {
            soc_mem_field32_set(
                unit, mem, &entry, WRONG_LAN_B_AGE_OUT_COUNT_DOWNf, v);
        }
    }

    /* seqnum reset state */
    if (flags & BCM_TSN_SR_RX_FLOW_RESET_SEQNUM_WIN_STATE) {
        soc_mem_field32_set(unit, mem, &entry, SN_RESET_STATEf, 1);
    }

    /* Do final writing */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    MEM_UNLOCK(unit, mem);

    return BCM_E_NONE;
}

/* SR flow mgmt - write flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_write_rx_flow(
    int unit,
    int idx,
    bcmi_tsn_sr_rx_flow_t *config)
{
    soc_mem_t mem;
    sr_rx_entry_t entry;
    cut_through_attribute_entry_t ct_entry;
    ing_mtu_check_2_entry_t mtu_ent;
    bcm_tsn_sr_rx_flow_config_t *cfg;
    tsn_sr_seqnum_slice_t *sn;
    int entries;
    uint32 v;
    int power;
    int rv;
    int i;
    int is_tx = 0;
    uint32 hw_id;
    int mtu_hw_index;
    bcm_tsn_mtu_profile_type_t type;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == config || idx < 0 ||
        idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
        return BCM_E_PARAM;
    }

    /* Double check the configuration is correct */
    cfg = &config->config;
    BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_sr_flows_check_rx_flow_config(unit, cfg));
    if (config->sn_slice == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate slice before doing anything */
    i = 0;
    entries = soc_mem_index_count(unit, SR_SN_HISTORY_0m);
    sn = (tsn_sr_seqnum_slice_t *)config->sn_slice;
    for (; sn != NULL; sn = sn->next) {
        /* The total number of chained slices < number of pools */
        i++;
        if (i > TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }

        /* Validate item values */
        if (sn->index < 0 || sn->index >= TSN_SRFLOWS_RX_SN_HIST_POOLS) {
            return BCM_E_PARAM;
        }
        if (sn->power < TSN_SRFLOWS_RX_SNPOOL_POWER_MIN ||
            sn->power > TSN_SRFLOWS_RX_SNPOOL_POWER_MAX) {
            return BCM_E_PARAM;
        }
        if (sn->offset < 0 || sn->offset > TSN_SRFLOWS_RX_SNPOOL_SIZE_MAX) {
            return BCM_E_PARAM;
        }
        if (sn->offset % tsn_math_powers[sn->power]) {
            /* offset should be multiples of the slice size */
            return BCM_E_PARAM;
        }
        if (sn->entry < 0 || sn->entry >= entries) {
            return BCM_E_PARAM;
        }
    }

    /*
     * Per flow disable cut-through
     * Write to HW with HW index (idx+flow_type_offset)+1
     * since entry 0 is not visible to SW
     */
    if (soc_feature(unit, soc_feature_tsn_sr_flow_no_asf_select)) {
        mem = CUT_THROUGH_ATTRIBUTEm;
        sal_memset(&ct_entry, 0, sizeof(ct_entry));
        v = (cfg->flags & BCM_TSN_SR_RX_FLOW_CONFIG_DO_NOT_CUT_THROUGH) ? 1 : 0;
        soc_mem_field32_set(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf, v);
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               FL_TSN_SRFLOWS_IDX_TO_HW_ID(0, idx),
                               &ct_entry);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               TSN_SRFLOWS_IDX_TO_HW_ID(0, idx),
                               &ct_entry);
        }

        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    } else {
        if (cfg->flags & BCM_TSN_SR_RX_FLOW_CONFIG_DO_NOT_CUT_THROUGH) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "SR RX flow disable cut-through "
                                    "is not configurable\n")));
            return BCM_E_PARAM;
        }
    }

    /* Write to HW with HW index idx+1 since entry 0 is not visible to SW */
    mem = SR_RXm;
    sal_memset(&entry, 0, sizeof(entry));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }

    /* Flags */
    v = (cfg->flags & BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_DUPLICATES) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &entry, ACCEPT_DUPLICATE_PACKETf, v);
    v = (cfg->flags & BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_PKT_IN_DROP_WINDOW) ?
        1 : 0;
    soc_mem_field32_set(unit, mem, &entry, ACCEPT_IN_DROP_WINDOWf, v);
    v = (cfg->flags & BCM_TSN_SR_RX_FLOW_CONFIG_DROP_OUT_OF_ORDER) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &entry, DROP_OUT_OF_ORDERf, v);

    /* Acceptance window (already validated as a valid power-of-2 number) */
    TSN_GET_LOG2(cfg->seqnum_accept_win_size, power);
    assert(power >= TSN_SRFLOWS_RX_ACCEPT_POWER_MIN &&
           power <= TSN_SRFLOWS_RX_ACCEPT_POWER_MAX);
    v = (uint32)TSN_SRFLOWS_RX_ACCEPT_POWER_MAX - power;
    soc_mem_field32_set(unit, mem, &entry, ACCEPTANCE_WINDOW_SIZEf, v);

    /* Age out time interval */
    if (cfg->age_timeout == 0) {
        /* Disable aging */
        soc_mem_field32_set(unit, mem, &entry, AGE_OUT_MULTIPLIERf, 0);
        soc_mem_field32_set(unit, mem, &entry, SN_AGE_OUT_ENABLEf, 0);
    } else {
        /* The actual multiplier is 2^N */
        TSN_GET_LOG2(cfg->age_timeout, v);
        soc_mem_field32_set(unit, mem, &entry, AGE_OUT_MULTIPLIERf, v);
        /* Enable aging */
        soc_mem_field32_set(unit, mem, &entry, SN_AGE_OUT_ENABLEf, 1);
    }

    /* Sequence number history window size (also already validated) */
    if (cfg->seqnum_history_win_size == 1) {
        /* Special case: 1-bit */
        v = 0;
    } else {
        TSN_GET_LOG2(cfg->seqnum_history_win_size, power);
        assert(power >= TSN_SRFLOWS_RX_WINDOW_POWER_MIN &&
               power <= TSN_SRFLOWS_RX_WINDOW_POWER_MAX);
        v = (uint32)power - TSN_SRFLOWS_RX_WINDOW_POWER_MIN + 1;
    }
    soc_mem_field32_set(unit, mem, &entry, SN_WINDOW_SIZEf, v);

    /* Clear seqnum pool sizes to 0 (disabled by default) */
    for (i = 0; i < sizeof(tsn_flow_sr_hist_sizes) / sizeof(soc_field_t); i++) {
        soc_mem_field32_set(unit, mem, &entry, tsn_flow_sr_hist_sizes[i], 0);
    }

    /* Write indexes to the seqnum pools */
    sn = (tsn_sr_seqnum_slice_t *)config->sn_slice;
    for (; sn != NULL; sn = sn->next) {
        soc_mem_field32_set(
            unit, mem, &entry, tsn_flow_sr_hist_indexs[sn->index], sn->entry);
        soc_mem_field32_set(
            unit, mem, &entry, tsn_flow_sr_hist_offsets[sn->index],
            sn->offset / TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE);
        soc_mem_field32_set(
            unit, mem, &entry, tsn_flow_sr_hist_sizes[sn->index],
            sn->power - TSN_SRFLOWS_RX_SNPOOL_POWER_MIN + 2);
    }

    /* Do final writing */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    /* Unlock memory  SR_RXm */
    MEM_UNLOCK(unit, mem);

    /* MTU profile id HW writing */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            hw_id = (uint32)FL_TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            hw_id = (uint32)TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
        }
        mem = ING_MTU_CHECK_2m;
        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
        MEM_LOCK(unit, mem);
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to read mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        type = bcmTsnMtuProfileTypeIngress;
        rv = bcmi_gh2_tsn_mtu_profile_decode(
                              unit, cfg->ingress_mtu_profile,
                              &type, &mtu_hw_index);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to decode profile id[%d]"),
                                  cfg->ingress_mtu_profile));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        soc_mem_field32_set(unit, mem, &mtu_ent,
                            MTU_PROFILE_INDEXf, mtu_hw_index);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        MEM_UNLOCK(unit, mem);
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - write flow config */
STATIC int
bcmi_gh2_tsn_sr_flows_write_tx_flow(
    int unit,
    int idx,
    bcmi_tsn_sr_tx_flow_t *config)
{
    soc_mem_t mem;
    sr_tx_entry_t entry;
    cut_through_attribute_entry_t ct_entry;
    ing_mtu_check_2_entry_t mtu_ent;
    int rv;
    uint32 v = 0;
    int is_tx = 1;
    uint32 hw_id;
    int mtu_hw_index;
    bcm_tsn_mtu_profile_type_t type;
    bcm_tsn_sr_tx_flow_config_t *cfg;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == config || idx < 0 ||
        idx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
        return BCM_E_PARAM;
    }

    /* Double check the configuration is correct */
    cfg = &config->config;
    BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_sr_flows_check_tx_flow_config(unit, cfg));

    /*
     * Per flow disable cut-through
     * Write to HW with HW index (idx+flow_type_offset)+1
     * since entry 0 is not visible to SW
     */
    if (soc_feature(unit, soc_feature_tsn_sr_flow_no_asf_select)) {
        mem = CUT_THROUGH_ATTRIBUTEm;
        sal_memset(&ct_entry, 0, sizeof(ct_entry));
        v = (cfg->flags & BCM_TSN_SR_TX_FLOW_CONFIG_DO_NOT_CUT_THROUGH) ?
                1 : 0;
        soc_mem_field32_set(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf, v);
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               FL_TSN_SRFLOWS_IDX_TO_HW_ID(1, idx),
                               &ct_entry);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                               TSN_SRFLOWS_IDX_TO_HW_ID(1, idx),
                               &ct_entry);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    } else {
        if (cfg->flags & BCM_TSN_SR_TX_FLOW_CONFIG_DO_NOT_CUT_THROUGH) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "SR TX flow disable cut-through "
                                    "is not configurable\n")));
            return BCM_E_PARAM;
        }
    }

    /* Write to HW with HW index idx+1 since entry 0 is not visible to SW */
    mem = SR_TXm;
    sal_memset(&entry, 0, sizeof(entry));
    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field32_set(unit, mem, &entry, NEXT_SNf, config->config.seq_num);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    /* Unlock memory  SR_TXm */
    MEM_UNLOCK(unit, mem);

    /* MTU profile id HW writing */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            hw_id = (uint32)FL_TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            hw_id = (uint32)TSN_SRFLOWS_IDX_TO_HW_ID(is_tx, idx);
        }
        mem = ING_MTU_CHECK_2m;
        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
        MEM_LOCK(unit, mem);
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to read mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        type = bcmTsnMtuProfileTypeIngress;
        rv = bcmi_gh2_tsn_mtu_profile_decode(
                              unit, config->config.ingress_mtu_profile,
                              &type, &mtu_hw_index);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to decode profile id[%d]"),
                                  config->config.ingress_mtu_profile));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        soc_mem_field32_set(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf,
                            mtu_hw_index);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        /* Unlock memory  ING_MTU_CHECK_2m */
        MEM_UNLOCK(unit, mem);
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* SR flow mgmt - read flow config (for warm boot) */
STATIC int
bcmi_gh2_tsn_sr_flows_read_rx_flow(
    int unit,
    int idx,
    bcmi_tsn_sr_rx_flow_t *config)
{
    soc_mem_t mem;
    sr_rx_entry_t entry;
    cut_through_attribute_entry_t ct_entry;
    ing_mtu_check_2_entry_t mtu_ent;
    bcm_tsn_sr_rx_flow_config_t *cfg;
    tsn_sr_seqnum_slice_t *slice, *sn;
    uint32 power;
    uint32 v;
    int mtu_idx, mtu_id;
    int eidx;
    int offs;
    int rv;
    int i;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == config ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
        return BCM_E_PARAM;
    }

    /* This routine is for warm boot only */
    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Get config from HW with HW index idx+1 */
    mem = SR_RXm;
    sal_memset(&entry, 0, sizeof(entry));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /* Start compiling the configuration from HW values */
    cfg = &config->config;

    /* Flags */
    cfg->flags = 0;
    if (soc_mem_field32_get(unit, mem, &entry, ACCEPT_DUPLICATE_PACKETf)) {
        cfg->flags |= BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_DUPLICATES;
    }
    if (soc_mem_field32_get(unit, mem, &entry, ACCEPT_IN_DROP_WINDOWf)) {
        cfg->flags |= BCM_TSN_SR_RX_FLOW_CONFIG_ACCEPT_PKT_IN_DROP_WINDOW;
    }
    if (soc_mem_field32_get(unit, mem, &entry, DROP_OUT_OF_ORDERf)) {
        cfg->flags |= BCM_TSN_SR_RX_FLOW_CONFIG_DROP_OUT_OF_ORDER;
    }

    /* Acceptance window */
    v = soc_mem_field32_get(unit, mem, &entry, ACCEPTANCE_WINDOW_SIZEf);
    power = TSN_SRFLOWS_RX_ACCEPT_POWER_MAX - v;
    if (power < TSN_SRFLOWS_RX_ACCEPT_POWER_MIN ||
        power > TSN_SRFLOWS_RX_ACCEPT_POWER_MAX) {
        return BCM_E_INTERNAL;
    }
    assert(power < sizeof(tsn_math_powers) / sizeof(int));
    cfg->seqnum_accept_win_size = (uint32)tsn_math_powers[power];

    /* Sequence number history window size */
    v = soc_mem_field32_get(unit, mem, &entry, SN_WINDOW_SIZEf);
    if (v == 0) {
        cfg->seqnum_history_win_size = 1;
    } else {
        power = v + TSN_SRFLOWS_RX_WINDOW_POWER_MIN - 1;
        if (power < TSN_SRFLOWS_RX_WINDOW_POWER_MIN ||
            power > TSN_SRFLOWS_RX_WINDOW_POWER_MAX) {
            return BCM_E_INTERNAL;
        }
        assert(power < sizeof(tsn_math_powers) / sizeof(int));
        cfg->seqnum_history_win_size = (uint32)tsn_math_powers[power];
    }

    /* Age out time interval */
    v = soc_mem_field32_get(unit, mem, &entry, SN_AGE_OUT_ENABLEf);
    if (v == 0) {
        cfg->age_timeout = 0;
    } else {
        power = soc_mem_field32_get(unit, mem, &entry, AGE_OUT_MULTIPLIERf);
        assert(power < sizeof(tsn_math_powers) / sizeof(int));
        cfg->age_timeout = (uint32)tsn_math_powers[power];
    }

    /* Re-build SN slices */
    sn = NULL;
    for (i = 0; i < TSN_SRFLOWS_RX_SN_HIST_POOLS; i++) {
        v = soc_mem_field32_get(unit, mem, &entry, tsn_flow_sr_hist_sizes[i]);
        if (v != 0 && v != 1) {
            eidx = (int)soc_mem_field32_get(
                            unit, mem, &entry, tsn_flow_sr_hist_indexs[i]);
            offs = (int)soc_mem_field32_get(
                            unit, mem, &entry, tsn_flow_sr_hist_offsets[i]);
            rv = bcmi_gh2_tsn_sr_flows_seqnum_slice_wb_restore(
                    unit,
                    i,
                    eidx,
                    offs * TSN_SRFLOWS_RX_SN_HIST_BLOCK_SIZE,
                    v - 2 + TSN_SRFLOWS_RX_SNPOOL_POWER_MIN,
                    &slice
                 );
            if (rv != BCM_E_NONE) {
                if (sn != NULL) {
                    (void)bcmi_gh2_tsn_sr_flows_seqnum_slice_free(
                            unit, (bcmi_tsn_sr_seqnum_slice_t)sn);
                }
                return rv;
            }
            if (slice != NULL) {
                slice->next = sn;
                sn = slice;
            } else {
                assert(0);
                return BCM_E_INTERNAL;
            }
        }
    }
    config->sn_slice = (bcmi_tsn_sr_seqnum_slice_t)sn;

    /* Get flag from hardware  */
    mem = CUT_THROUGH_ATTRIBUTEm;
    sal_memset(&ct_entry, 0, sizeof(ct_entry));
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                          FL_TSN_SRFLOWS_IDX_TO_HW_ID(0, idx), &ct_entry);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                          TSN_SRFLOWS_IDX_TO_HW_ID(0, idx), &ct_entry);
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    if (soc_mem_field32_get(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf)) {
        cfg->flags |= BCM_TSN_SR_RX_FLOW_CONFIG_DO_NOT_CUT_THROUGH;
    }
    /* MTU SR rx flow warm boot read */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        mem = ING_MTU_CHECK_2m;

        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              FL_TSN_SRFLOWS_IDX_TO_HW_ID(0, idx), &mtu_ent);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              TSN_SRFLOWS_IDX_TO_HW_ID(0, idx), &mtu_ent);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Failed to read mem[%s]"),
                                SOC_MEM_NAME(unit, mem)));
            return rv;
        }
        mtu_idx = soc_mem_field32_get(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf);
        rv = bcmi_gh2_tsn_mtu_profile_encode(unit, mtu_idx,
                                             bcmTsnMtuProfileTypeIngress,
                                             &mtu_id);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to encode mtu profile"
                                  " from idx = %d"), mtu_idx));
            return rv;
        }
        cfg->ingress_mtu_profile = mtu_id;
    }

    return BCM_E_NONE;
}

/* SR flow mgmt - read flow config (for warm boot) */
STATIC int
bcmi_gh2_tsn_sr_flows_read_tx_flow(
    int unit,
    int idx,
    bcmi_tsn_sr_tx_flow_t *config)
{
    soc_mem_t mem;
    sr_tx_entry_t entry;
    cut_through_attribute_entry_t ct_entry;
    ing_mtu_check_2_entry_t mtu_ent;
    int mtu_idx, mtu_id;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == config ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
        return BCM_E_PARAM;
    }

    /* This routine is for warm boot only */
    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Get config from HW with HW index idx+1 */
    mem = SR_TXm;
    sal_memset(&entry, 0, sizeof(entry));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    config->config.seq_num = soc_mem_field32_get(unit, mem, &entry, NEXT_SNf);

    /* Get config from hardware */
    mem = CUT_THROUGH_ATTRIBUTEm;
    sal_memset(&ct_entry, 0, sizeof(ct_entry));
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                          FL_TSN_SRFLOWS_IDX_TO_HW_ID(1, idx), &ct_entry);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                          TSN_SRFLOWS_IDX_TO_HW_ID(1, idx), &ct_entry);
    }
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    if (soc_mem_field32_get(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf)) {
        config->config.flags |= BCM_TSN_SR_TX_FLOW_CONFIG_DO_NOT_CUT_THROUGH;
    }
    /* MTU SR tx flow warm boot read */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        mem = ING_MTU_CHECK_2m;

        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              FL_TSN_SRFLOWS_IDX_TO_HW_ID(1, idx), &mtu_ent);
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                              TSN_SRFLOWS_IDX_TO_HW_ID(1, idx), &mtu_ent);
        }
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Failed to read mem[%s]"),
                                SOC_MEM_NAME(unit, mem)));
            return rv;
        }
        mtu_idx = soc_mem_field32_get(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf);
        rv = bcmi_gh2_tsn_mtu_profile_encode(unit, mtu_idx,
                                             bcmTsnMtuProfileTypeIngress,
                                             &mtu_id);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to encode mtu profile"
                                  " from idx = %d"), mtu_idx));
            return rv;
        }
        config->config.ingress_mtu_profile = mtu_id;
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* SR flow mgmt - get flow status */
STATIC int
bcmi_gh2_tsn_sr_flows_get_tx_flow_status(
    int unit,
    int idx,
    bcm_tsn_sr_tx_flow_status_t *status)
{
    soc_mem_t mem;
    sr_tx_entry_t entry;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == status ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_tx) {
        return BCM_E_PARAM;
    }

    /* Get status from HW with HW index idx+1 */
    mem = SR_TXm;
    sal_memset(&entry, 0, sizeof(entry));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    status->seq_num = soc_mem_field32_get(unit, mem, &entry, NEXT_SNf);

    return BCM_E_NONE;
}

/* SR flow mgmt - get flow status */
STATIC int
bcmi_gh2_tsn_sr_flows_get_rx_flow_status(
    int unit,
    int idx,
    bcm_tsn_sr_rx_flow_status_t *status)
{
    soc_mem_t mem;
    sr_rx_entry_t entry;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == status ||
        idx < 0 || idx >= SRFLOWS_BKINFO(unit).max_flows_rx) {
        return BCM_E_PARAM;
    }

    /* Get status from HW with HW index idx+1 */
    mem = SR_RXm;
    sal_memset(&entry, 0, sizeof(entry));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx + 1, &entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    status->seqnum_win_in_reset =
        (int)soc_mem_field32_get(unit, mem, &entry, SN_RESET_STATEf);
    status->seqnum_age_countdown =
        (int)soc_mem_field32_get(unit, mem, &entry, SN_AGE_OUT_COUNT_DOWNf);
    status->last_history_win_seqnum =
        soc_mem_field32_get(unit, mem, &entry, LAST_WINDOW_SNf);
    status->last_accepted_seqnum_lan[0] =
        soc_mem_field32_get(unit, mem, &entry, LAST_RECEIVED_SN_LAN_Af);
    status->last_accepted_seqnum_lan[1] =
        soc_mem_field32_get(unit, mem, &entry, LAST_RECEIVED_SN_LAN_Bf);
    status->wrong_lan[0] =
        (int)soc_mem_field32_get(unit, mem, &entry, WRONG_LAN_Af);
    status->wrong_lan[1] =
        (int)soc_mem_field32_get(unit, mem, &entry, WRONG_LAN_Bf);
    status->wrong_lan_age_countdown[0] = (int)soc_mem_field32_get(
            unit, mem, &entry, WRONG_LAN_A_AGE_OUT_COUNT_DOWNf);
    status->wrong_lan_age_countdown[1] = (int)soc_mem_field32_get(
            unit, mem, &entry, WRONG_LAN_B_AGE_OUT_COUNT_DOWNf);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_sr_port_config_get
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
bcmi_gh2_tsn_sr_port_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_sr_port_config_t *config)
{
    bcm_port_t              port;
    sr_port_table_entry_t   sr_port_ent;
    soc_mem_t               mem;
    int                     rv = BCM_E_NONE;
    uint32                  val;

    /* Parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    /* Initialize the data first */
    bcm_tsn_sr_port_config_t_init(config);

    /* Use PORT_TAB as major reference since all others should be the same */
    mem = SR_PORT_TABLEm;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &sr_port_ent);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Check if SR enabled */
    val = soc_mem_field32_get(
            unit, mem, &sr_port_ent, ING_SR_ENABLEf);
    if (val) {
        /* SR port type */
        val = soc_mem_field32_get(unit, mem, &sr_port_ent, ING_SR_TYPEf);
        config->port_type = (bcm_tsn_sr_port_type_t)(val + 1);

        /* Interlink role */
        val = soc_mem_field32_get(unit, mem, &sr_port_ent, ING_SR_PORT_ROLEf);
        config->interlink_role = val ? 0 : 1;

        /* sr mode */
        config->port_mode = (bcm_tsn_sr_port_mode_t)
            soc_mem_field32_get(unit, mem, &sr_port_ent, ING_SR_MODEf);

        /* LAN ID  */
        config->lan_id = (uint8)
            soc_mem_field32_get(unit, mem, &sr_port_ent, ING_SR_LAN_IDf);

        /* NET ID */
        config->net_id = (uint8)
            soc_mem_field32_get(unit, mem, &sr_port_ent, ING_SR_NET_IDf);

        /* mac_da_flows */
        val = soc_mem_field32_get(
                unit, mem, &sr_port_ent, FLOW_ID_SOURCE_PRIORITYf);
        config->mac_da_flows = val ? 1 : 0;
    } else {
        /* SR disabled */
        config->port_type = bcmTsnSrPortTypeNone;
    }

    return BCM_E_NONE;
}

/*
 * SR port stub for config set to support Modified-PRP configuration.
 *    Note: Use port_type=bcmTsnSrPortTypeCount to configure Modified-PRP port
 */
STATIC int
bcmi_gh2_tsn_sr_port_config_apply(
    int unit,
    bcm_port_t port,
    bcm_tsn_sr_port_config_t *ori_config,
    bcm_tsn_sr_port_config_t *new_config)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t fld;
    uint32 val;
    int ptab;
    bcm_trunk_t trunk_id;
    int rv;

    /* Port tables to write */
    const soc_mem_t mem_list[] = {
        SR_PORT_TABLEm,
        SR_LPORT_TABm,
        SR_ING_EGR_PORTm,
        SR_ING_EGR_TRUNKm,
        SR_EGR_PORTm,
        SR_EGR_ING_PORTm
    };

    /* Fields for different port tables */
    const soc_field_t fields_sr_enable[] = {
        ING_SR_ENABLEf,
        ING_SR_ENABLEf,
        EGR_SR_ENABLEf,
        EGR_SR_ENABLEf,
        EGR_SR_ENABLEf,
        ING_SR_ENABLEf
    };
    const soc_field_t fields_sr_type[] = {
        ING_SR_TYPEf,
        ING_SR_TYPEf,
        EGR_SR_TYPEf,
        EGR_SR_TYPEf,
        EGR_SR_TYPEf,
        ING_SR_TYPEf
    };
    const soc_field_t fields_sr_role[] = {
        ING_SR_PORT_ROLEf,
        ING_SR_PORT_ROLEf,
        EGR_SR_PORT_ROLEf,
        EGR_SR_PORT_ROLEf,
        EGR_SR_PORT_ROLEf,
        ING_SR_PORT_ROLEf
    };
    const soc_field_t fields_sr_mode[] = {
        ING_SR_MODEf,
        ING_SR_MODEf,
        EGR_SR_MODEf,
        EGR_SR_MODEf,
        EGR_SR_MODEf,
        ING_SR_MODEf
    };
    const soc_field_t fields_sr_lanid[] = {
        ING_SR_LAN_IDf,
        ING_SR_LAN_IDf,
        EGR_SR_LAN_IDf,
        EGR_SR_LAN_IDf,
        EGR_SR_LAN_IDf,
        ING_SR_LAN_IDf
    };
    const soc_field_t fields_sr_netid[] = {
        ING_SR_NET_IDf,
        ING_SR_NET_IDf,
        EGR_SR_NET_IDf,
        EGR_SR_NET_IDf,
        EGR_SR_NET_IDf,
        ING_SR_NET_IDf
    };
    const soc_field_t fields_sr_flowsrc[] = {
        FLOW_ID_SOURCE_PRIORITYf,
        FLOW_ID_SOURCE_PRIORITYf,
        INVALIDf,
        INVALIDf,
        INVALIDf,
        INVALIDf
    };
    int mem_idx;
    bcm_module_t my_modid = -1;
    int src_trk_idx = 0; /* Source Trunk table index.*/
    uint32 mem_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    assert(NULL != ori_config && NULL != new_config);

    /* Update HW only if it's changed */
    if (!sal_memcmp(ori_config, new_config, sizeof(*ori_config))) {
        return BCM_E_NONE;
    }

    /* Apply to all the port tables */
    for (ptab = 0; ptab < COUNTOF(mem_list); ptab++) {
        mem = mem_list[ptab];

        if (mem == SR_LPORT_TABm) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                      port, &src_trk_idx));

            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                             src_trk_idx, mem_entry));

            mem_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                          mem_entry, LPORT_PROFILE_IDXf);
        } else if (mem == SR_ING_EGR_TRUNKm) {
            /* Check if the port belongs to a trunk */
            rv = bcm_esw_trunk_find(unit, my_modid, port, &trunk_id);
            if (BCM_SUCCESS(rv)) {
                /* Port is member of a valid trunk */
                mem_idx = trunk_id;
            } else {
                /* Port is not member of any valid trunk */
                continue;
            }
        } else {
            mem_idx = port;
        }
        /* Read the original entry for modification */
        sal_memset(entry, 0, sizeof(entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, mem_idx, entry));

        /* Check SR enable */
        if (new_config->port_type == bcmTsnSrPortTypeNone) {
            /*
             * SR disabled: just write SR_ENABLE and all other fields are 0
             */

            /* SR enable = 0 */
            fld = fields_sr_enable[ptab];
            val = 0;
            soc_mem_field32_set(unit, mem, entry, fld, val);
        } else {
            /*
             * SR enabled: write configuration and enable it at last
             */

            /* SR type */
            fld = fields_sr_type[ptab];
            val = new_config->port_type - 1;
            soc_mem_field32_set(unit, mem, entry, fld, val);

            /* SR role */
            fld = fields_sr_role[ptab];
            val = new_config->interlink_role ? 0 : 1;
            soc_mem_field32_set(unit, mem, entry, fld, val);

            /* SR mode */
            fld = fields_sr_mode[ptab];
            val = new_config->port_mode;
            soc_mem_field32_set(unit, mem, entry, fld, val);

            /* SR LAN ID */
            fld = fields_sr_lanid[ptab];
            val = new_config->lan_id;
            soc_mem_field32_set(unit, mem, entry, fld, val);

            /* SR NET ID */
            fld = fields_sr_netid[ptab];
            val = new_config->net_id;
            soc_mem_field32_set(unit, mem, entry, fld, val);

            /* SR flow ID source priority (not for all tables) */
            fld = fields_sr_flowsrc[ptab];
            if (fld != INVALIDf) {
                val = new_config->mac_da_flows ? 1 : 0;
                soc_mem_field32_set(unit, mem, entry, fld, val);
            }

            /* Write SR enable at last */
            fld = fields_sr_enable[ptab];
            val = 1;
            soc_mem_field32_set(unit, mem, entry, fld, val);
        }

        /* Write the new configuration */
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_idx, entry));
    }

    return BCM_E_NONE;
}

/* SR PRP port forwarding: callback for destroying customer header match */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_destroy_switch_match(
    int unit,
    int match_id,
    bcm_switch_match_config_info_t *config_info,
    void *user_data)
{
    bcm_port_t prp_port;

    if (NULL == user_data || NULL == config_info) {
        return BCM_E_INTERNAL; /* should not happen */
    }

    prp_port = *((bcm_port_t *)user_data);
    if (config_info->value32 == PRP_FORWARDING_CH_ENCAP_VALUE(0, prp_port) ||
        config_info->value32 == PRP_FORWARDING_CH_ENCAP_VALUE(1, prp_port)) {
        uint32                      match_id_hw;
        bcm_switch_match_service_t  match_service;

        /* need to clean PRP_PACKETf LPORT_PROFILE_IDXf by ourselves */
        BCM_IF_ERROR_RETURN(
            bcmi_switch_match_hw_id_get(
                unit, match_id, &match_service, &match_id_hw));
        if (bcmSwitchMatchServiceCustomHeader != match_service) {
            BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_modify(
                unit, CUSTOM_HEADER_POLICY_TABLEm, match_id_hw,
                PRP_PACKETf, 0));
        BCM_IF_ERROR_RETURN(
            soc_mem_field32_modify(
                unit, CUSTOM_HEADER_POLICY_TABLEm, match_id_hw,
                LPORT_PROFILE_IDXf, 0));

        /* invoke API to delete switch_match */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_match_config_delete(
                unit, bcmSwitchMatchServiceCustomHeader, match_id));
    }

    return BCM_E_NONE;
}

/* SR PRP port forwarding: callback for destroying customer header encap */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_destroy_switch_encap(
    int unit,
    bcm_if_t encap_id,
    bcm_switch_encap_info_t *encap_info,
    void *user_data)
{
    bcm_port_t prp_port;

    if (NULL == user_data || NULL == encap_info) {
        return BCM_E_INTERNAL; /* should not happen */
    }

    prp_port = *((bcm_port_t *)user_data);
    if (encap_info->value32 == PRP_FORWARDING_CH_ENCAP_VALUE(0, prp_port) ||
        encap_info->value32 == PRP_FORWARDING_CH_ENCAP_VALUE(1, prp_port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_encap_destroy(unit, encap_id));
    }

    return BCM_E_NONE;
}


/* helper function to remove all ifp rules that created on this specific port */
STATIC int
bcmi_gh2_tsn_sr_remove_all_field_entries(
    int unit,
    bcm_field_group_t field_group,
    bcm_port_t target_port)
{
    int               ifp_group_entry_num;
    bcm_field_entry_t *ifp_group_entry_array = NULL;
    int               ifp_group_entry_array_size;
    int               i;

    /* 1st time to get the array size that we need */
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_entry_multi_get(
            unit, PRP_FORWARDING_REDIRECT_GROUP(unit), 0,
            NULL, &ifp_group_entry_num));

    ifp_group_entry_array_size = ifp_group_entry_num;
    ifp_group_entry_array = (bcm_field_entry_t *)
        sal_alloc(sizeof(bcm_field_entry_t) * ifp_group_entry_array_size,
                  "ifp_group_entry_array");
    if (NULL == ifp_group_entry_array) {
        PRP_FORWARDING_ERROR_RETURN(BCM_E_MEMORY);
    }

    /* 2nd time to get all entries to the array that we just allocated */
    PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(
        bcm_esw_field_entry_multi_get(
            unit, PRP_FORWARDING_REDIRECT_GROUP(unit),
            ifp_group_entry_array_size,
            ifp_group_entry_array, &ifp_group_entry_num),
        sal_free(ifp_group_entry_array));
    assert(ifp_group_entry_num == ifp_group_entry_array_size);

    /* remove the entries that qulified on this specific target port */
    for (i = 0; i < ifp_group_entry_array_size; i++) {
        bcm_port_t data = 0, mask = 0;

        PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(
            bcm_esw_field_qualify_InPort_get(
                unit, ifp_group_entry_array[i], &data, &mask),
            sal_free(ifp_group_entry_array));
        if (data == target_port) {
            PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(
                bcm_esw_field_entry_remove(
                    unit, ifp_group_entry_array[i]),
                sal_free(ifp_group_entry_array));
            PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(
                bcm_esw_field_entry_destroy(
                    unit, ifp_group_entry_array[i]),
                sal_free(ifp_group_entry_array));
            break;
        }
    }
    sal_free(ifp_group_entry_array);
    ifp_group_entry_array = NULL;

    return BCM_E_NONE;
}


/* SR PRP port forwarding: cancel prp forwarding at specific port */
STATIC void
bcmi_gh2_tsn_sr_port_prp_forwarding_destroy(
    int unit,
    bcm_port_t  prp_port)
{
    /* destroy IFP rule on this prp port
     *  : we traverse each entry in IFP group
     *    delete it if this entry qualify this specified prp_port
     */
    assert(PRP_FORWARDING_REDIRECT_GROUP_INVALID !=
           PRP_FORWARDING_REDIRECT_GROUP(unit));
    PRP_FORWARDING_DESTROY_ERROR_RETURN(
        bcmi_gh2_tsn_sr_remove_all_field_entries(
            unit, PRP_FORWARDING_REDIRECT_GROUP(unit), prp_port));

    /* cleanup CustomHeader match and policy */
    PRP_FORWARDING_DESTROY_ERROR_RETURN(
        bcm_esw_switch_match_config_traverse(
            unit, bcmSwitchMatchServiceCustomHeader,
            bcmi_gh2_tsn_sr_port_prp_forwarding_destroy_switch_match,
            (void *)&prp_port));

    /* cleanup CustomHeader encap */
    PRP_FORWARDING_DESTROY_ERROR_RETURN(
        bcm_esw_switch_encap_traverse(
            unit,
            bcmi_gh2_tsn_sr_port_prp_forwarding_destroy_switch_encap,
            (void *)&prp_port));

}

/*
 * SR PRP port forwarding: setup prp forwarding at specific port
 *
 * Note
 *  1. Two encacp entries(CH1 and CH2) for this PRP port
 *      - CH1 is for Non-PRP CustomHeader and CH2 is for PRP CustomHeader.
 *      - API returned encap_id for CH1 is N and CH2's encap_id must be N + 1
 *          >> HW behavior assert CH1 and CH2 encacp entries is continued.
 *  2. CustomHeader is expected will be engaged for PRP support only on the
 *      GH2 device declared TSN-SR for PRP is supported.
 *      - Thus two CustomHeader encap table entries directly mapped for
 *         assigned PRP port.
 *        >> CH1 entry : encacp_id = CustomHeader_type || prp_port * 2
 *        >> CH2 entry : encacp_id = CustomHeader_type || prp_port * 2 + 1
 *  3. One IFP rule for PRP-Port encacp on CH1(1st entry) and the HW
 *      automatically select the 2nd entry for CH2.
 *  4. two CustomHeader Match/Policy entries required for PRP_Port (no
 *      assertion on continued entries in HW).
 *      >> SW apply two entries as continued as well.
 */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_create(
    int unit,
    bcm_port_t  prp_port)
{
    bcm_field_entry_t                   fp_entry;
    bcm_if_t                            encap_id_prp, encap_id_non_prp;
    bcm_switch_encap_info_t             encap_info;
    int                                 match_id_prp, match_id_non_prp;
    int                                 i;
    int lport_profile_idx;
    bcm_module_t my_modid = -1;
    int src_trk_idx = 0; /* Source Trunk table index.*/
    uint32 mem_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* setup switch_match on lb_port if 1st time to create prp port */
    if (0 == PRP_FORWARDING_SWITCH_INIT(unit)) {
        bcm_switch_match_control_info_t match_control_info;

        /* enable CustomHeader match/encap on loopbcak port
         *   Note. because tsn module is inited before switch module
         *         so we let switch_match_control setup at 1st prp-port create
         */
        bcm_switch_match_control_info_t_init(&match_control_info);
        match_control_info.port_enable = TRUE;
        match_control_info.mask32 = PRP_FORWARDING_CH_HEADER_MASK;
        PRP_FORWARDING_CREATE_ERROR_RETURN(
            bcm_esw_switch_match_control_set(
                unit, bcmSwitchMatchServiceCustomHeader,
                bcmSwitchMatchControlPortEnable,
                PRP_FORWARDING_REDIRECT_GPORT(unit), &match_control_info));

        PRP_FORWARDING_CREATE_ERROR_RETURN(
            bcm_esw_switch_match_control_set(
                unit, bcmSwitchMatchServiceCustomHeader,
                bcmSwitchMatchControlMatchMask,
                PRP_FORWARDING_REDIRECT_GPORT(unit), &match_control_info));

        PRP_FORWARDING_CREATE_ERROR_RETURN(
            bcm_esw_port_control_set(
                unit, PRP_FORWARDING_REDIRECT_PORT(unit),
                bcmPortControlCustomHeaderEncapEnable, TRUE));

        PRP_FORWARDING_SWITCH_INIT(unit) = 1;
    }

    /* create two CustomHeader encap :
     *   step 1. create two continuous encap id
     *           (with the same encap value for non-prp at first)
     *   step 2. set 2nd encap id as prp value
     */
    bcm_switch_encap_info_t_init(&encap_info);
    encap_info.encap_service = BCM_SWITCH_ENCAP_SERVICE_CUSTOM_HEADER;
    encap_info.value32 = PRP_FORWARDING_CH_ENCAP_VALUE(1, prp_port);
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        _bcm_hr3_switch_encap_create(
            unit, &encap_info, 2, &encap_id_non_prp));

    encap_id_prp = encap_id_non_prp + 1;
    encap_info.value32 = PRP_FORWARDING_CH_ENCAP_VALUE(0, prp_port);
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_switch_encap_set(
            unit, encap_id_prp, &encap_info));

    /* setup two CustomHeader match and policy :
     *  ==> let packet get correct orignal prp port
     */
    for (i = 0; i < 2; i++) {
        int is_prp = i;
        int *match_id_ptr = is_prp ? &match_id_prp : &match_id_non_prp;
        bcm_switch_match_config_info_t match_info;
        bcm_switch_match_service_t     match_service;
        uint32                         match_id_hw;

        bcm_switch_match_config_info_t_init(&match_info);
        match_info.value32 = PRP_FORWARDING_CH_ENCAP_VALUE(is_prp ? 0 : 1,
                                                           prp_port);
        PRP_FORWARDING_CREATE_ERROR_RETURN(
            bcm_esw_switch_match_config_add(
                unit, bcmSwitchMatchServiceCustomHeader,
                &match_info, match_id_ptr));
        PRP_FORWARDING_CREATE_ERROR_RETURN(
            bcmi_switch_match_hw_id_get(
                unit, *match_id_ptr, &match_service, &match_id_hw));
        if (bcmSwitchMatchServiceCustomHeader != match_service) {
            PRP_FORWARDING_CREATE_ERROR_RETURN(BCM_E_INTERNAL);
        }

        PRP_FORWARDING_CREATE_ERROR_RETURN(
            soc_mem_field32_modify(
                unit, CUSTOM_HEADER_POLICY_TABLEm, match_id_hw,
                PRP_PACKETf, is_prp ? 1 : 0));

        BCM_IF_ERROR_RETURN(
            _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                  prp_port, &src_trk_idx));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                         src_trk_idx, mem_entry));

        lport_profile_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                mem_entry, LPORT_PROFILE_IDXf);

        PRP_FORWARDING_CREATE_ERROR_RETURN(
            soc_mem_field32_modify(
                unit, CUSTOM_HEADER_POLICY_TABLEm, match_id_hw,
                LPORT_PROFILE_IDXf, lport_profile_idx));
    }

    /* add IFP rule that apply on the packet from this PRP port
     *   1. change NET_ID source from packet (for validate the packet path ID)
     *   2. encap customer header
     *      (we use id "encap_id_non_prp"
     *       , hw will auto select "encap_id_prp" if the packet is prp-packet)
     *   3. redirect packet to loopback port
     */
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_entry_create(
            unit, PRP_FORWARDING_REDIRECT_GROUP(unit), &fp_entry));
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_qualify_InPort(unit, fp_entry,
                                     prp_port, BCM_FIELD_EXACT_MATCH_MASK));
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_action_add(unit, fp_entry,
                                 bcmFieldActionSRNetIdSource,
                                 BCM_FIELD_SR_NET_ID_SOURCE_PACKET, 0));
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_action_add(unit, fp_entry,
                                 bcmFieldActionSwitchEncap,
                                 encap_id_non_prp, 0));
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_action_add(unit, fp_entry,
                                 bcmFieldActionRedirectPort,
                                 SOC_BASE_MODID(unit),
                                 PRP_FORWARDING_REDIRECT_PORT(unit)));
    PRP_FORWARDING_CREATE_ERROR_RETURN(
        bcm_esw_field_entry_install(unit, fp_entry));

    return BCM_E_NONE;
}

/* SR PRP port forwarding: cleanup all prp forwaring */
STATIC void
bcmi_gh2_tsn_sr_port_prp_forwarding_cleanup(
    int unit,
    int init_error)
{
    bcm_tsn_sr_port_config_t        old_config;
    bcm_tsn_sr_port_config_t        new_config;

    /* skip prp-init if sdk not enable this feature */
    if (0 == PRP_FORWARDING_ENABLE(unit)) {
        return;
    }

    /* set loopback port back to prp disable */
    PRP_FORWARDING_CLEAN_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_get(
            unit, PRP_FORWARDING_REDIRECT_GPORT(unit), &old_config));
    new_config = old_config;
    new_config.port_type = bcmTsnSrPortTypeNone;
    new_config.interlink_role = 1;
    PRP_FORWARDING_CLEAN_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_apply(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            &old_config, &new_config));

    /* destroy IFP group for prp port */
    if (PRP_FORWARDING_REDIRECT_GROUP_INVALID !=
        PRP_FORWARDING_REDIRECT_GROUP(unit)) {

        /* if "init failed in tsn module" or
         *    "only init tsn module" (SOC_F_ALL_MODULES_INITED flag is setup)
         *   ==> need to clean IFP /customer header created by prp-forwarding
         * if during "init all"
         *   (all module would clean resource by themselves,
         *    and SOC_F_ALL_MODULES_INITED flag would not be setup)
         *   ==> need not clean IFP /customer header
         */
        if ((1 == init_error) ||
            (SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED)) {

            /* For each PRP-port been setup before,
             * clean their CH encap and IFP entries
             */
            bcm_port_t                port;
            bcm_tsn_sr_port_config_t  config;

            PBMP_ITER(PBMP_PORT_ALL(unit), port) {
                bcm_gport_t gport;

                PRP_FORWARDING_CLEAN_ERROR_RETURN(
                    bcm_esw_port_gport_get(unit, port, &gport));

                PRP_FORWARDING_CLEAN_ERROR_RETURN(
                    bcmi_gh2_tsn_sr_port_config_get(unit, gport, &config));

                if (1 == PRP_FORWARDING_NEED_PRP_SETTING(config)) {
                    bcmi_gh2_tsn_sr_port_prp_forwarding_destroy(unit, port);
                }
            }

            /* delete ifp rule for loobpack port in the group */
            PRP_FORWARDING_CLEAN_ERROR_RETURN(
                bcmi_gh2_tsn_sr_remove_all_field_entries(
                    unit, PRP_FORWARDING_REDIRECT_GROUP(unit),
                    PRP_FORWARDING_REDIRECT_PORT(unit)));

            /* destroy ifp group */
            PRP_FORWARDING_CLEAN_ERROR_RETURN(
                bcm_esw_field_group_destroy(
                    unit, PRP_FORWARDING_REDIRECT_GROUP(unit)));

        } else {
            /* group_id should has been free by field module, do nothing */
        }
        PRP_FORWARDING_REDIRECT_GROUP(unit) =
            PRP_FORWARDING_REDIRECT_GROUP_INVALID;
    }

    /* destroy mutex */
    if (gh2_tsn_bkinfo[unit]->gh2_prp_forwarding_bk_info.mutex != NULL) {
        sal_mutex_destroy(
            gh2_tsn_bkinfo[unit]->gh2_prp_forwarding_bk_info.mutex);
        gh2_tsn_bkinfo[unit]->gh2_prp_forwarding_bk_info.mutex = NULL;
    }
}

/* SR PRP port forwarding: init prp forwaring feature */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_init(
    int unit)
{
    uint32                          vlan_flags;
    bcm_field_qset_t                qset;
    bcm_field_entry_t               fp_entry;
    bcm_tsn_sr_port_config_t        old_config;
    bcm_tsn_sr_port_config_t        new_config;
    pbmp_t                          pbmp;
    bcm_vlan_data_t                 *vlan_list = NULL;
    int                             vlan_cnt = 0;
    int                             port;

    /*
     *  INIT BOOKKEEPING
     */

    /* skip prp-init if sdk not enable this feature */
    port = soc_property_get(unit, spn_SR_PRP_ENABLE, 0);
    PRP_FORWARDING_ENABLE(unit) = 0;
    if (BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        /* Check if belong to valid ports, except port 0 */
        PRP_FORWARDING_ENABLE(unit) = 1;
    } else {
        /*
         * port = 0 : feature disabled.
         * other invalid port id : error configuration.
         */
        if (port != 0) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit,
                                  "PRP loopback port %d is an "
                                  "invalid port.\n"), port));
            return BCM_E_CONFIG;
        }
    }

    if (0 == PRP_FORWARDING_ENABLE(unit)) {
        return BCM_E_NONE;
    }

    /* config switch_match until 1st time to setup prp-port
     *  (because switch module is inited after tsn module)
     */
    PRP_FORWARDING_SWITCH_INIT(unit) = 0;

    PRP_FORWARDING_REDIRECT_PORT(unit) = port;
    BCM_GPORT_LOCAL_SET(PRP_FORWARDING_REDIRECT_GPORT(unit),
                        PRP_FORWARDING_REDIRECT_PORT(unit));

    PRP_FORWARDING_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_get(
            unit, PRP_FORWARDING_REDIRECT_GPORT(unit), &old_config));

    /* Create mutex */
    gh2_tsn_bkinfo[unit]->gh2_prp_forwarding_bk_info.mutex =
        sal_mutex_create("prp_forwarding_mutex");
    if (gh2_tsn_bkinfo[unit]->gh2_prp_forwarding_bk_info.mutex == NULL) {
        PRP_FORWARDING_ERROR_RETURN(BCM_E_MEMORY);
    }

    /*
     *  SETUP HW-RELATED SETTING
     *  (ex. port table / ifp / customer header / l2 / vlan )
     *    1. skip these if it is in warmboot
     *    2. prp_redirect_group of bookkeeping
     *       would be recovered in reload()
     */
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        return BCM_E_NONE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* setup loopback port as PRP-modified */
    new_config = old_config;
    new_config.port_type = bcmTsnSrPortTypeCount; /* for PRP-modified usage */
    new_config.interlink_role = 0;
    new_config.net_id = 5; /* For PathID to be b'1010 (A) or b'1011 (B) */
    PRP_FORWARDING_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_apply(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            &old_config, &new_config));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_encap_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit), BCM_PORT_ENCAP_IEEE));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_autoneg_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit), 0));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_loopback_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit), BCM_PORT_LOOPBACK_MAC));

    /* create a IFP group on prp-port for redirect to loopback-port
     *  Note. we need to set this group as lowest-priority
     *        (use BCM_FIELD_GROUP_PRIO_ANY,
     *         it would use lowest-priority in default)
     *        Becuase the user maybe want to setup another IFP rule on prp port
     *        (ex. setup action DROP or PKT_NET_ID_SOURCE)
     */
    PRP_FORWARDING_REDIRECT_GROUP(unit) = PRP_FORWARDING_REDIRECT_GROUP_INVALID;
    bcm_field_qset_t_init(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_group_create(
            unit, qset, BCM_FIELD_GROUP_PRIO_ANY,
            &PRP_FORWARDING_REDIRECT_GROUP(unit)));
    assert(PRP_FORWARDING_REDIRECT_GROUP(unit) !=
           PRP_FORWARDING_REDIRECT_GROUP_INVALID);

    /* For this loopback-port
     *   1. disable ingress/egress vlan check
     *   2. disable L2 learning
     *   3. disable L2 moving
     *   4. clear egress mask first
     *   5. remove this port from all VLANs
     *   6. change net_id source to be from ingress port.
     */
    /* .1 */
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_vlan_member_get(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit), &vlan_flags));
    vlan_flags &= ~(BCM_PORT_VLAN_MEMBER_INGRESS);
    vlan_flags &= ~(BCM_PORT_VLAN_MEMBER_EGRESS);
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_vlan_member_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit), vlan_flags));
    /* .2 */
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_control_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            bcmPortControlL2Learn, BCM_PORT_LEARN_FWD));
    /* .3 */
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_control_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            bcmPortControlL2Move, BCM_PORT_LEARN_FWD));
    /* .4 */
    BCM_PBMP_CLEAR(pbmp);
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_port_egress_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            SOC_BASE_MODID(unit), pbmp));
    /* .5 */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, PRP_FORWARDING_REDIRECT_PORT(unit));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_vlan_list_by_pbmp(
            unit, pbmp, &vlan_list, &vlan_cnt));
    if (vlan_cnt != 0) {
        int i;

        if (NULL == vlan_list) {
            PRP_FORWARDING_ERROR_RETURN(BCM_E_INTERNAL);
        }
        for (i = 0; i < vlan_cnt; i++) {
            PRP_FORWARDING_ERROR_RETURN_WITH_CLEAN(
                bcm_esw_vlan_port_remove(
                    unit, vlan_list[i].vlan_tag, pbmp),
                bcm_esw_vlan_list_destroy(unit, vlan_list, vlan_cnt));
        }
    }
    if (NULL != vlan_list) {
        bcm_esw_vlan_list_destroy(unit, vlan_list, vlan_cnt);
        vlan_list = NULL;
    }
    /* .6 */
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_entry_create(
            unit, PRP_FORWARDING_REDIRECT_GROUP(unit), &fp_entry));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_qualify_InPort(
            unit, fp_entry, PRP_FORWARDING_REDIRECT_PORT(unit),
            BCM_FIELD_EXACT_MATCH_MASK));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_action_add(
            unit, fp_entry, bcmFieldActionSRNetIdSource,
             BCM_FIELD_SR_NET_ID_SOURCE_SRC_PORT, 0));
    PRP_FORWARDING_ERROR_RETURN(
        bcm_esw_field_entry_install(unit, fp_entry));

    /* re-setup IFP/CustomerHeader
     * for each PRP-port (according to hw port table)
     */
    {
        bcm_port_t port;
        bcm_tsn_sr_port_config_t  config;

        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            bcm_gport_t gport;

            PRP_FORWARDING_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));

            PRP_FORWARDING_ERROR_RETURN(
                bcmi_gh2_tsn_sr_port_config_get(unit, gport, &config));

            if (1 == PRP_FORWARDING_NEED_PRP_SETTING(config)) {
                PRP_FORWARDING_ERROR_RETURN(
                    bcmi_gh2_tsn_sr_port_prp_forwarding_create(unit, port));
            }
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* sync prp-forwarding info to scache */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_sync(
    int unit,
    soc_scache_handle_t scache_handle)
{
    uint8 *scache_ptr = NULL;
    int rv = BCM_E_NONE;

    if (0 == PRP_FORWARDING_ENABLE(unit)) {
        return BCM_E_NONE;
    }

    rv = _bcm_esw_scache_ptr_get(
             unit, scache_handle, TRUE, sizeof(bcm_field_group_t),
             &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);

    if (BCM_SUCCESS(rv)) {
        if (scache_ptr == NULL) {
            rv = BCM_E_INTERNAL;
        }
    }

    PRP_FORWARDING_BKINFO_LOCK(unit);
    if (BCM_SUCCESS(rv)) {
        if (PRP_FORWARDING_REDIRECT_GROUP_INVALID ==
            PRP_FORWARDING_REDIRECT_GROUP(unit)) {
           rv = BCM_E_INTERNAL;
        }
    }

    if (BCM_SUCCESS(rv)) {
        sal_memcpy(scache_ptr, &(PRP_FORWARDING_REDIRECT_GROUP(unit)),
                   sizeof(bcm_field_group_t));
    }
    PRP_FORWARDING_BKINFO_UNLOCK(unit);

    return rv;
}

/* recover prp-forwarding info from scache */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_reload(
    int unit,
    soc_scache_handle_t scache_handle)
{
    uint8 *scache_ptr = NULL;
    int rv = BCM_E_NONE;

    rv = _bcm_esw_scache_ptr_get(
             unit, scache_handle, FALSE, sizeof(bcm_field_group_t),
             &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Maybe proceed with Level 1 Warm Boot */
        return BCM_E_NONE;
    } else if (BCM_E_NONE == rv) {
        if (NULL == scache_ptr) {
            rv = BCM_E_INTERNAL;
        } else {
            sal_memcpy(&(PRP_FORWARDING_REDIRECT_GROUP(unit)), scache_ptr,
                       sizeof(bcm_field_group_t));
        }
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* add this port into egress mask of loopback port */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_egress_add(
    int unit,
    bcm_port_t port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_egress_get(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            SOC_BASE_MODID(unit), &pbmp));
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_egress_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            SOC_BASE_MODID(unit), pbmp));

    return BCM_E_NONE;
}

/* delete this port from egress mask of loopback port */
STATIC int
bcmi_gh2_tsn_sr_port_prp_forwarding_egress_remove(
    int unit,
    bcm_port_t port)
{
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_egress_get(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            SOC_BASE_MODID(unit), &pbmp));
    BCM_PBMP_PORT_REMOVE(pbmp, port);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_egress_set(
            unit, PRP_FORWARDING_REDIRECT_PORT(unit),
            SOC_BASE_MODID(unit), pbmp));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_sr_port_config_set
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
bcmi_gh2_tsn_sr_port_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_sr_port_config_t *config)
{
    bcm_port_t               port;
    bcm_tsn_sr_port_config_t old_config;
    bcm_pbmp_t               netid_bmp;
    netid_bitmap_entry_t     netid_bmp_entry;
    uint8                    old_netid, new_netid;
    int                      rv;

    /* Parameter check */
    if (config == NULL) {
        return BCM_E_PARAM;
    }

    /* This API implementation doesn't accept bcmTsnSrPortTypeCount */
    if (config->port_type < bcmTsnSrPortTypeNone ||
        config->port_type >= bcmTsnSrPortTypeCount) {
        return BCM_E_PARAM;
    }
    if (config->port_mode < bcmTsnSrPortModeDefault ||
        config->port_mode >= bcmTsnSrPortModeCount) {
        return BCM_E_PARAM;
    }
    if (config->lan_id > 1) {
        return BCM_E_PARAM;
    }
    if (config->net_id > 7) {
        return BCM_E_PARAM;
    }

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    /* get old previous config */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_get(unit, gport, &old_config));

    /* handle prp forwarding */
    if (0 == PRP_FORWARDING_NEED_PRP_SETTING(old_config) &&
        1 == PRP_FORWARDING_NEED_PRP_SETTING(*config)) {
        if (0 == PRP_FORWARDING_ENABLE(unit)) {
            return BCM_E_UNAVAIL;
        }
        PRP_FORWARDING_BKINFO_LOCK(unit);
        rv = bcmi_gh2_tsn_sr_port_prp_forwarding_create(unit, port);
        PRP_FORWARDING_BKINFO_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else if (1 == PRP_FORWARDING_NEED_PRP_SETTING(old_config) &&
               0 == PRP_FORWARDING_NEED_PRP_SETTING(*config)) {
        if (0 == PRP_FORWARDING_ENABLE(unit)) {
            return BCM_E_UNAVAIL;
        }
        PRP_FORWARDING_BKINFO_LOCK(unit);
        bcmi_gh2_tsn_sr_port_prp_forwarding_destroy(unit, port);
        PRP_FORWARDING_BKINFO_UNLOCK(unit);
    }

    /* need handle egress mask on loopback port if PRP enable */
    if (1 == PRP_FORWARDING_ENABLE(unit)) {
        if (0 == PRP_FORWARDING_NEED_EGRESS_MASK_SETTING(old_config) &&
            1 == PRP_FORWARDING_NEED_EGRESS_MASK_SETTING(*config)) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_tsn_sr_port_prp_forwarding_egress_add(unit, port));
        } else if (1 == PRP_FORWARDING_NEED_EGRESS_MASK_SETTING(old_config) &&
                   0 == PRP_FORWARDING_NEED_EGRESS_MASK_SETTING(*config)) {
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_tsn_sr_port_prp_forwarding_egress_remove(unit, port));
        }
    }

    /* Configure NET_ID bitmap for NET_ID filtering on mutlicast packets */
    old_netid = 0;
    new_netid = 0;
    if (old_config.port_type == bcmTsnSrPortTypePrp ||
        old_config.port_mode == bcmTsnSrPortModeInterworkingPrp) {
        old_netid = old_config.net_id;
    }
    if (config->port_type == bcmTsnSrPortTypePrp ||
        config->port_mode == bcmTsnSrPortModeInterworkingPrp) {
        new_netid = config->net_id;
    }
    if (old_netid != new_netid) {
        sal_memset(&netid_bmp_entry, 0, sizeof(netid_bmp_entry));
        if (old_netid) {
            /* Remove this port from old NET_ID bitmap if applicable */
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, NETID_BITMAPm, MEM_BLOCK_ANY,
                             old_netid, &netid_bmp_entry));
            soc_mem_pbmp_field_get(
                unit, NETID_BITMAPm, &netid_bmp_entry, BITMAPf, &netid_bmp);
            BCM_PBMP_PORT_REMOVE(netid_bmp, port);
            soc_mem_pbmp_field_set(
                unit, NETID_BITMAPm, &netid_bmp_entry, BITMAPf, &netid_bmp);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, NETID_BITMAPm, MEM_BLOCK_ALL,
                             old_netid, &netid_bmp_entry));
        }
        if (new_netid) {
            /* Add this port to new NET_ID bitmap if applicable */
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, NETID_BITMAPm, MEM_BLOCK_ANY,
                             new_netid, &netid_bmp_entry));
            soc_mem_pbmp_field_get(
                unit, NETID_BITMAPm, &netid_bmp_entry, BITMAPf, &netid_bmp);
            BCM_PBMP_PORT_ADD(netid_bmp, port);
            soc_mem_pbmp_field_set(
                unit, NETID_BITMAPm, &netid_bmp_entry, BITMAPf, &netid_bmp);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, NETID_BITMAPm, MEM_BLOCK_ALL,
                             new_netid, &netid_bmp_entry));
        }
    }

    /* Call the shared configuration apply to configure */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_apply(unit, port, &old_config, config));
    return BCM_E_NONE;
}
#endif /* BCM_TSN_SR_SUPPORT */

/* TSN flow mgmt - get number of flows */
STATIC int
bcmi_gh2_tsn_flows_get_num_flows(int unit, int *max_flows)
{
    /* parameter check */
    if (!SOC_UNIT_VALID(unit) || NULL == max_flows) {
        return BCM_E_PARAM;
    }

    *max_flows = TSN_FLOWS_MAX_NUM_OF_FLOWS;

    return BCM_E_NONE;
}

/* TSN flow mgmt - convert flow index to HW flow ID */
STATIC int
bcmi_gh2_tsn_flows_get_hw_flow_id(int unit, int fidx, uint32 *hw_id)
{
    /* parameter check */
    if (!SOC_UNIT_VALID(unit) || NULL == hw_id || fidx < 0 ||
        fidx >= TSN_FLOWS_MAX_NUM_OF_FLOWS) {
        return BCM_E_PARAM;
    }

    /* Convert it */
    *hw_id = (uint32)TSN_FLOWS_IDX_TO_HW_ID(fidx);
    return BCM_E_NONE;
}

/* TSN flow mgmt - convert flow index to HW flow ID */
STATIC int
bcmi_gh2_tsn_flows_get_sw_flow_idx(int unit, uint32 hw_id, int *fidx)
{
    int idx;

    /* parameter check */
    if (!SOC_UNIT_VALID(unit) || NULL == fidx) {
        return BCM_E_PARAM;
    }

    /* Convert it */
    idx = TSN_FLOWS_HW_ID_TO_IDX((int)hw_id);

    /* Check if it's valid */
    if (idx < 0 || idx >= TSN_FLOWS_MAX_NUM_OF_FLOWS) {
        return BCM_E_PARAM;
    }

    *fidx = idx;
    return BCM_E_NONE;
}

/* TSN flow mgmt - check flow config */
STATIC int
bcmi_gh2_tsn_flows_check_flow_config(int unit, bcm_tsn_flow_config_t *cfg)
{
    int index;
    bcm_tsn_mtu_profile_type_t type;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (NULL == cfg) {
        return BCM_E_PARAM;
    }

    /* Check MTU profile id */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        /* Using MTU decode function to do profile id validation */
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_mtu_profile_decode(
                              unit, cfg->ingress_mtu_profile, &type, &index));
        if (type == bcmTsnMtuProfileTypeEgress) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/* TSN flow mgmt - write flow config */
STATIC int
bcmi_gh2_tsn_flows_write_flow(
    int unit,
    int idx,
    bcm_tsn_flow_config_t *config)
{
    soc_mem_t mem;
    uint32 v = 0;
    cut_through_attribute_entry_t ct_entry;
    ing_mtu_check_1_entry_t mtu_ent;
    uint32 hw_id;
    int mtu_hw_index;
    bcm_tsn_mtu_profile_type_t type;
    int rv;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (NULL == config || idx < 0 || idx >= TSN_FLOWS_MAX_NUM_OF_FLOWS) {
        return BCM_E_PARAM;
    }

    /*
     * Per flow disable cut-through
     * Write to HW with HW index idx+1 since entry 0 is not visible to SW
     */
    if (soc_feature(unit, soc_feature_tsn_flow_no_asf_select)) {
        mem = CUT_THROUGH_ATTRIBUTEm;
        sal_memset(&ct_entry, 0, sizeof(ct_entry));
        v = (config->flags & BCM_TSN_FLOW_CONFIG_DO_NOT_CUT_THROUGH) ? 1 : 0;
        soc_mem_field32_set(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf, v);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                           TSN_FLOWS_IDX_TO_HW_ID(idx),
                           &ct_entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    } else {
        if (config->flags & BCM_TSN_FLOW_CONFIG_DO_NOT_CUT_THROUGH) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "TSN flow disable cut-through "
                                    "is not configurable\n")));
            return BCM_E_PARAM;
        }
    }
    /* MTU profile id HW writing */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        hw_id = (uint32)TSN_FLOWS_IDX_TO_HW_ID(idx);
        mem = ING_MTU_CHECK_1m;
        MEM_LOCK(unit, mem);
        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to read mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        type = bcmTsnMtuProfileTypeIngress;
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_mtu_profile_decode(
                              unit, config->ingress_mtu_profile,
                              &type, &mtu_hw_index));
        soc_mem_field32_set(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf,
                            mtu_hw_index);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        MEM_UNLOCK(unit, mem);
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* TSN flow mgmt - read flow config (for warm boot) */
STATIC int
bcmi_gh2_tsn_flows_read_flow(
    int unit,
    int idx,
    bcm_tsn_flow_config_t *config)
{
    soc_mem_t mem;
    cut_through_attribute_entry_t ct_entry;
    int rv = BCM_E_NONE;
    ing_mtu_check_1_entry_t mtu_ent;
    uint32 hw_id;
    int mtu_idx, mtu_id;

    /* parameter check */
    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn)) {
        return (BCM_E_UNAVAIL);
    }
    if (NULL == config || idx < 0 || idx >= TSN_FLOWS_MAX_NUM_OF_FLOWS) {
        return BCM_E_PARAM;
    }

    /* This routine is for warm boot only */
    if (!SOC_WARM_BOOT(unit)) {
        return BCM_E_UNAVAIL;
    }

    /* Get config from hardware */
    mem = CUT_THROUGH_ATTRIBUTEm;
    sal_memset(&ct_entry, 0, sizeof(ct_entry));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      TSN_FLOWS_IDX_TO_HW_ID(idx), &ct_entry);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    if (soc_mem_field32_get(unit, mem, &ct_entry, DO_NOT_CUT_THROUGHf)) {
        config->flags |= BCM_TSN_FLOW_CONFIG_DO_NOT_CUT_THROUGH;
    }

    /* MTU TSN flow warm boot read */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        mem = ING_MTU_CHECK_1m;
        hw_id = (uint32)TSN_FLOWS_IDX_TO_HW_ID(idx);
        sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "Failed to read mem[%s]"),
                                SOC_MEM_NAME(unit, mem)));
            return rv;
        }
        mtu_idx = soc_mem_field32_get(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf);
        rv = bcmi_gh2_tsn_mtu_profile_encode(unit, mtu_idx,
                                             bcmTsnMtuProfileTypeIngress,
                                             &mtu_id);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to encode mtu profile"
                                  " from idx = %d"), mtu_idx));
            return rv;
        }
        config->ingress_mtu_profile = mtu_id;
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/* TSN flow mgmt - clear flow config */
STATIC int
bcmi_gh2_tsn_flows_clear_flow_mtu(int unit, int idx)
{
    soc_mem_t mem;
    ing_mtu_check_1_entry_t mtu_ent;
    int rv;
    uint32 hw_id;

    TSN_BKINFO_IS_INIT(unit);
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

    hw_id = (uint32)TSN_FLOWS_IDX_TO_HW_ID(idx);
    mem = ING_MTU_CHECK_1m;
    MEM_LOCK(unit, mem);
    sal_memset(&mtu_ent, 0, sizeof(mtu_ent));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to read mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field32_set(unit, mem, &mtu_ent, MTU_PROFILE_INDEXf, 0);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_id, &mtu_ent);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "Failed to write mem[%s]"),
                              SOC_MEM_NAME(unit, mem)));
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    MEM_UNLOCK(unit, mem);

    return BCM_E_NONE;
}


/* TSN flow mgmt - clear flow config */
STATIC int
bcmi_gh2_tsn_flows_clear_flow(int unit, int idx)
{
    /* parameter check */
    if (!SOC_UNIT_VALID(unit) ||
        idx < 0 || idx >= TSN_FLOWS_MAX_NUM_OF_FLOWS) {
        return BCM_E_PARAM;
    }

    /* Clear MTU profile id HW */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return bcmi_gh2_tsn_flows_clear_flow_mtu(unit, idx);
    }

    return BCM_E_NONE;
}

/*
 * TSN/SR table size is 8 * 16, sw_idx = 0 ~ 7
 * each sw_idx represent a group of 16 new priority entries
 */
#define GH2_PRI_MAP_TSN_SHIFT        (4)
#define GH2_PRI_MAP_TSN_MASK         (0xF)
#define GH2_PRI_MAP_TSN_TAB_SIZE     (8)
#define GH2_PRI_MAP_TSN_ENT_SIZE     (16)
#if defined(BCM_TSN_SR_SUPPORT)
#define GH2_PRI_MAP_SR_SHIFT         (4)
#define GH2_PRI_MAP_SR_MASK          (0xF)
#define GH2_PRI_MAP_SR_TAB_SIZE      (8)
#define GH2_PRI_MAP_SR_ENT_SIZE      (16)
#endif /* BCM_TSN_SR_SUPPORT */

/*
 * Egress STU/MTU table is 128x8, and the 128 is the max port count.
 * However, it could be smaller in fact. The max actual number of ports is
 * more reasonable.
 * refer to GH2 EGR_MTU_CHECK/EGR_STU_CHECK (66 ports)
 */
#define GH2_PRI_MAP_MTU_SHIFT        (3)
#define GH2_PRI_MAP_MTU_MASK         (0x7)
#define GH2_PRI_MAP_MTU_TAB_SIZE     (66)
#define GH2_PRI_MAP_MTU_ENT_SIZE     (8)
#define GH2_PRI_MAP_STU_SHIFT        (3)
#define GH2_PRI_MAP_STU_MASK         (0x7)
#define GH2_PRI_MAP_STU_TAB_SIZE     (66)
#define GH2_PRI_MAP_STU_ENT_SIZE     (8)

/* OFFSET is only 3 bits and INT_PRI is only 4 bits */
#define GH2_PRI_MAP_TSN_MAX_FLOW_VALUE        (8)
#define GH2_PRI_MAP_SR_MAX_FLOW_VALUE         (8)
#define GH2_PRI_MAP_TSN_MAX_NEW_PRI_VALUE     (16)
#define GH2_PRI_MAP_SR_MAX_NEW_PRI_VALUE      (16)
/*
 * The default setting is placed in the 1st profile (sw_idx = 0)
 * of TSN/SR flow initially
 * Note:
 * 2rd to Nth profiles with values default_offset and default_new_pri
 * are recognized as empty setting.
 */
STATIC const uint32 default_offset[GH2_PRI_MAP_TSN_ENT_SIZE] =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
STATIC const uint32 default_new_pri[GH2_PRI_MAP_TSN_ENT_SIZE] =
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_tsn_set
 * Purpose:
 *      Set the Priority Map configuration to hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_set(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = TSN_PRI_OFFSETm;
    tsn_pri_offset_entry_t data;
    uint32 field_data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;
    bcm_tsn_pri_map_entry_t *map_entry;

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    if (!(sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert(sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE);
        return BCM_E_PARAM;
    }

    /* Parameter validation */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Set the flow_offset and new_int_pri to TSN_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_TSN_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_TSN_SHIFT) + GH2_PRI_MAP_TSN_ENT_SIZE;
         hw_idx++) {

        map_entry = &(config->map_entries[entry_idx]);
        is_default_offset =
            (BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET & (map_entry->flags)) ?
            FALSE : TRUE;
        is_default_new_pri =
            (BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI & (map_entry->flags)) ?
            FALSE : TRUE;

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            if (TRUE == is_default_offset) {
                field_data = default_offset[entry_idx];
            }
            else {
                field_data = map_entry->flow_offset;
            }

            /* Check if the value fit the target field range */
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_fit(
                    unit, mem, OFFSETf,
                    field_data));
            soc_TSN_PRI_OFFSETm_field_set(unit, &data, OFFSETf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            if (TRUE == is_default_new_pri) {
                field_data = default_new_pri[entry_idx];
            }
            else {
                field_data = map_entry->new_int_pri;
            }

            /* Check if the value fit the target field range */
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_fit(
                    unit, mem, INT_PRIf,
                    field_data));
            soc_TSN_PRI_OFFSETm_field_set(unit, &data, INT_PRIf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, &data));
        entry_idx++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_tsn_get
 * Purpose:
 *      Get the Priority Map configuration from hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_get(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = TSN_PRI_OFFSETm;
    tsn_pri_offset_entry_t data;
    uint32 field_data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    if (!(sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert(sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE);
        return BCM_E_PARAM;
    }

    /* Parameter validation */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Get the flow_offset and new_int_pri from TSN_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_TSN_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_TSN_SHIFT) + GH2_PRI_MAP_TSN_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            soc_TSN_PRI_OFFSETm_field_get(unit, &data, OFFSETf, &field_data);
            config->map_entries[entry_idx].flow_offset = field_data;

            /* check if the current setting from HW is default setting */
            if (default_offset[entry_idx] != field_data) {
                is_default_offset = FALSE;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            soc_TSN_PRI_OFFSETm_field_get(unit, &data, INT_PRIf, &field_data);
            config->map_entries[entry_idx].new_int_pri = field_data;

            /* check if the current setting from HW is default setting */
            if (default_new_pri[entry_idx] != field_data) {
                is_default_new_pri = FALSE;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }
        entry_idx++;
    }

    /* Set TSN related setting */
    config->map_type = bcmTsnPriMapTypeTsnFlow;
    config->num_map_entries = GH2_PRI_MAP_TSN_ENT_SIZE;

    /* Check if the HW setting is default setting, and set valid flags. */
    for (entry_idx = 0;
         entry_idx < GH2_PRI_MAP_TSN_ENT_SIZE;
         entry_idx++) {
        config->map_entries[entry_idx].flags = 0;

        if (FALSE == is_default_offset) {
            config->map_entries[entry_idx].flags |=
                BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET;
        }
        if (FALSE == is_default_new_pri) {
            config->map_entries[entry_idx].flags |=
                BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_tsn_delete
 * Purpose:
 *      Delete the Priority Map configuration on hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = TSN_PRI_OFFSETm;
    tsn_pri_offset_entry_t data;
    uint32 field_data;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_tsn_delete:"
                            "sw_idx %d\n"),
                            sw_idx));

    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* 1st profile should exist all the time, cannot delete sw_idx 0 */
    if (!((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE));
        return BCM_E_PARAM;
    }

    if (sw_idx == 0) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "cannot delete default profile(sw_idx = 0).\n")));
        return BCM_E_NONE;
    }

    /* Set the flow_offset and new_int_pri to TSN_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_TSN_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_TSN_SHIFT) + GH2_PRI_MAP_TSN_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            field_data = default_offset[entry_idx];
            soc_TSN_PRI_OFFSETm_field_set(unit, &data, OFFSETf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            field_data = default_new_pri[entry_idx];
            soc_TSN_PRI_OFFSETm_field_set(unit, &data, INT_PRIf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, &data));
        entry_idx++;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_tsn_hw_index_get
 * Purpose:
 *      Get HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_hw_index_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_index)
{
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == hw_index ||
        GH2_PRI_MAP_TSN_TAB_SIZE <= sw_idx) {
        return BCM_E_PARAM;
    }
    *hw_index = sw_idx;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_tsn_hw_index_get:"
                            "sw_idx %d, hw_index %d\n"),
                            sw_idx, *hw_index));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_tsn_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_index - (IN) HW memory base index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_sw_idx_get(
    int unit,
    uint32 hw_index,
    uint32 *sw_idx)
{
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == sw_idx ||
        GH2_PRI_MAP_TSN_TAB_SIZE * GH2_PRI_MAP_TSN_ENT_SIZE <= hw_index) {
        return BCM_E_PARAM;
    }
    *sw_idx = hw_index;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_tsn_sw_idx_get:"
                            "hw_index %d, sw_idx %d\n"),
                            hw_index, *sw_idx));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_pri_map_tsn_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit   - (IN) Unit being initialized
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_gh2_tsn_pri_map_tsn_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    uint32 hw_idx;
    uint32 entry_idx;
    uint32 field_data;
    soc_mem_t mem = TSN_PRI_OFFSETm;
    tsn_pri_offset_entry_t data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_tsn_wb_hw_existed:"
                            "sw_idx %d\n"),
                            sw_idx));

    if (!SOC_WARM_BOOT(unit) ||
        !soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }

    /* 1st profile should exist all the time, dont need to do wb */
    if (!((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_TSN_TAB_SIZE));
        return BCM_E_PARAM;
    }

    /* Get the flow_offset and new_int_pri from TSN_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_TSN_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_TSN_SHIFT) + GH2_PRI_MAP_TSN_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            soc_TSN_PRI_OFFSETm_field_get(unit, &data, OFFSETf, &field_data);

            /* Check if the current setting from HW is default setting */
            if (default_offset[entry_idx] != field_data) {
                is_default_offset = FALSE;
                break;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            soc_TSN_PRI_OFFSETm_field_get(unit, &data, INT_PRIf, &field_data);

            /* Check if the current setting from HW is default setting */
            if (default_new_pri[entry_idx] != field_data) {
                is_default_new_pri = FALSE;
                break;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }
        entry_idx++;
    }

    /* Return BCM_E_NOT_FOUND means HW doesn't exist */
    if ((is_default_offset == TRUE) && (is_default_new_pri == TRUE)) {
        return BCM_E_NOT_FOUND;
    }

    /* Return BCM_E_NONE means HW existed */
    return BCM_E_NONE;
#else /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

#if defined(BCM_TSN_SR_SUPPORT)
/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_sr_set
 * Purpose:
 *      Set the Priority Map configuration to hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_set(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = SR_PRI_OFFSETm;
    sr_pri_offset_entry_t data;
    uint32 field_data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;
    bcm_tsn_pri_map_entry_t *map_entry;

    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    if (!(sw_idx < GH2_PRI_MAP_SR_TAB_SIZE)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert(sw_idx < GH2_PRI_MAP_SR_TAB_SIZE);
        return BCM_E_PARAM;
    }

    /* Parameter validation */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Set the flow_offset and new_int_pri to SR_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_SR_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_SR_SHIFT) + GH2_PRI_MAP_SR_ENT_SIZE;
         hw_idx++) {

        map_entry = &(config->map_entries[entry_idx]);
        is_default_offset =
            (BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET & (map_entry->flags)) ?
            FALSE : TRUE;
        is_default_new_pri =
            (BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI & (map_entry->flags)) ?
            FALSE : TRUE;

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            if (TRUE == is_default_offset) {
                field_data = default_offset[entry_idx];
            }
            else {
                field_data = config->map_entries[entry_idx].flow_offset;
            }

            /* Check if the value fit the target field range */
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_fit(
                    unit, mem, OFFSETf,
                    field_data));
            soc_SR_PRI_OFFSETm_field_set(unit, &data, OFFSETf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            if (TRUE == is_default_new_pri) {
                field_data = default_new_pri[entry_idx];
            }
            else {
                field_data = config->map_entries[entry_idx].new_int_pri;
            }
            if (field_data >= GH2_PRI_MAP_SR_MAX_NEW_PRI_VALUE) {
                return BCM_E_PARAM;
            }

            /* Check if the value fit the target field range */
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_fit(
                    unit, mem, INT_PRIf,
                    field_data));
            soc_SR_PRI_OFFSETm_field_set(unit, &data, INT_PRIf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, &data));
        entry_idx++;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_sr_get
 * Purpose:
 *      Get the Priority Map configuration from hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 *      config - (OUT) Priority Map configuration
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_get(
    int unit,
    uint32 sw_idx,
    bcm_tsn_pri_map_config_t *config)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = SR_PRI_OFFSETm;
    sr_pri_offset_entry_t data;
    uint32 field_data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;

    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    if (!(sw_idx < GH2_PRI_MAP_SR_TAB_SIZE)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert(sw_idx < GH2_PRI_MAP_SR_TAB_SIZE);
        return BCM_E_PARAM;
    }

    /* Parameter validation */
    if (NULL == config) {
        return BCM_E_PARAM;
    }

    /* Get the flow_offset and new_int_pri from SR_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_SR_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_SR_SHIFT) + GH2_PRI_MAP_SR_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            soc_SR_PRI_OFFSETm_field_get(unit, &data, OFFSETf, &field_data);
            config->map_entries[entry_idx].flow_offset = field_data;

            /* Check if the current setting from HW is default setting */
            if (default_offset[entry_idx] != field_data) {
                is_default_offset = FALSE;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            soc_SR_PRI_OFFSETm_field_get(unit, &data, INT_PRIf, &field_data);
            config->map_entries[entry_idx].new_int_pri = field_data;

            /* Check if the current setting from HW is default setting */
            if (default_new_pri[entry_idx] != field_data) {
                is_default_new_pri = FALSE;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }
        entry_idx++;
    }

    /* Set SR related setting */
    config->map_type = bcmTsnPriMapTypeSrFlow;
    config->num_map_entries = GH2_PRI_MAP_SR_ENT_SIZE;

    /* Check if the HW setting is default setting, and set valid flags. */
    for (entry_idx = 0;
         entry_idx < GH2_PRI_MAP_SR_ENT_SIZE;
         entry_idx++) {
        config->map_entries[entry_idx].flags = 0;

        if (FALSE == is_default_offset) {
            config->map_entries[entry_idx].flags |=
                BCM_TSN_PRI_MAP_ENTRY_FLOW_OFFSET;
        }
        if (FALSE == is_default_new_pri) {
            config->map_entries[entry_idx].flags |=
                BCM_TSN_PRI_MAP_ENTRY_NEW_INT_PRI;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_sr_delete
 * Purpose:
 *      Delete the Priority Map configuration on hardware
 * Parameters:
 *      unit   - (IN) Unit number
 *      sw_idx - (IN) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_delete(
    int unit,
    uint32 sw_idx)
{
    uint32 hw_idx;
    uint32 entry_idx;
    soc_mem_t mem = SR_PRI_OFFSETm;
    sr_pri_offset_entry_t data;
    uint32 field_data;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_sr_delete:"
                            "sw_idx %d\n"),
                            sw_idx));

    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    /* 1st profile should exist all the time, cannot delete sw_idx 0 */
    if (!((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_SR_TAB_SIZE))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_SR_TAB_SIZE));
        return BCM_E_PARAM;
    }

    if (sw_idx == 0) {
        LOG_WARN(BSL_LS_BCM_TSN,
                 (BSL_META_U(unit,
                             "cannot delete default profile(sw_idx = 0).\n")));
        return BCM_E_NONE;
    }

    /* Set the flow_offset and new_int_pri to SR_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_SR_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_SR_SHIFT) + GH2_PRI_MAP_SR_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            field_data = default_offset[entry_idx];
            soc_SR_PRI_OFFSETm_field_set(unit, &data, OFFSETf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            field_data = default_new_pri[entry_idx];
            soc_SR_PRI_OFFSETm_field_set(unit, &data, INT_PRIf, &field_data);
        }
        else {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, &data));
        entry_idx++;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_sr_hw_index_get
 * Purpose:
 *      Get HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_hw_index_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_index)
{
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == hw_index ||
        GH2_PRI_MAP_SR_TAB_SIZE <= sw_idx) {
        return BCM_E_PARAM;
    }
    *hw_index = sw_idx;
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_sr_hw_index_get:"
                            "sw_idx %d, hw_index %d\n"),
                            sw_idx, *hw_index));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_sr_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_index - (IN) HW memory base index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_sw_idx_get(
    int unit,
    uint32 hw_index,
    uint32 *sw_idx)
{
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == sw_idx ||
        GH2_PRI_MAP_SR_TAB_SIZE <= hw_index) {
        return BCM_E_PARAM;
    }
    *sw_idx = hw_index;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_sr_sw_idx_get:"
                            "hw_index %d, sw_idx %d\n"),
                            hw_index, *sw_idx));
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_pri_map_sr_wb_hw_existed
 * Purpose:
 *   check if the HW existed value or not
 * Parameters:
 *   unit   - (IN) Unit being initialized
 *   sw_idx - (IN) SW memory index
 * Returns:
 *   BCM_E_NOT_FOUND means HW doesn't exist
 *   BCM_E_NONE means HW existed
 */
STATIC int
bcmi_gh2_tsn_pri_map_sr_wb_hw_existed(
    int unit,
    uint32 sw_idx)
{
#if defined(BCM_WARM_BOOT_SUPPORT)
    uint32 hw_idx;
    uint32 entry_idx;
    uint32 field_data;
    soc_mem_t mem = SR_PRI_OFFSETm;
    sr_pri_offset_entry_t data;
    int is_default_offset = TRUE, is_default_new_pri = TRUE;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_sr_wb_hw_existed:"
                            "sw_idx %d\n"),
                            sw_idx));

    if (!SOC_WARM_BOOT(unit) ||
        !soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }

    /* 1st profile should exist all the time, cannot delete sw_idx 0 */
    if (!((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_SR_TAB_SIZE))) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Invalid sw_idx(%d).\n"),
                              sw_idx));
        assert((sw_idx > 0) && (sw_idx < GH2_PRI_MAP_SR_TAB_SIZE));
        return BCM_E_PARAM;
    }

    /* Get the flow_offset and new_int_pri from SR_PRI_OFFSETm */
    entry_idx = 0;
    for (hw_idx = (sw_idx << GH2_PRI_MAP_SR_SHIFT);
         hw_idx < (sw_idx << GH2_PRI_MAP_SR_SHIFT) + GH2_PRI_MAP_SR_ENT_SIZE;
         hw_idx++) {

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, &data));

        if (SOC_MEM_FIELD_VALID(unit, mem, OFFSETf)) {
            soc_SR_PRI_OFFSETm_field_get(unit, &data, OFFSETf, &field_data);

            /* check if the current setting from HW is default setting */
            if (default_offset[entry_idx] != field_data) {
                is_default_offset = FALSE;
                break;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }

        if (SOC_MEM_FIELD_VALID(unit, mem, INT_PRIf)) {
            soc_SR_PRI_OFFSETm_field_get(unit, &data, INT_PRIf, &field_data);

            /* Check if the current setting from HW is default setting */
            if (default_new_pri[entry_idx] != field_data) {
                is_default_new_pri = FALSE;
                break;
            }
        }
        else {
            return BCM_E_INTERNAL;
        }
        entry_idx++;
    }

    /* Return BCM_E_NOT_FOUND means HW doesn't exist */
    if ((is_default_offset == TRUE) && (is_default_new_pri == TRUE)) {
        return BCM_E_NOT_FOUND;
    }

    /* Return BCM_E_NONE means HW existed */
    return BCM_E_NONE;
#else /* BCM_WARM_BOOT_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_WARM_BOOT_SUPPORT */
}

#endif /* BCM_TSN_SR_SUPPORT */


/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_mtu_hw_index_get
 * Purpose:
 *      Get HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_mtu_hw_index_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_index)
{
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == hw_index ||
        GH2_PRI_MAP_MTU_TAB_SIZE <= sw_idx) {
        return BCM_E_PARAM;
    }
    *hw_index = sw_idx * GH2_PRI_MAP_MTU_ENT_SIZE;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_mtu_hw_index_get:"
                            "sw_idx = %d, hw_index = %d\n"),
                            sw_idx, *hw_index));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_mtu_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_index - (IN) HW memory base index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_mtu_sw_idx_get(
    int unit,
    uint32 hw_index,
    uint32 *sw_idx)
{
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == sw_idx ||
        GH2_PRI_MAP_MTU_MASK & hw_index ||
        GH2_PRI_MAP_MTU_TAB_SIZE * GH2_PRI_MAP_MTU_ENT_SIZE <= hw_index) {
        return BCM_E_PARAM;
    }
    *sw_idx = hw_index >> GH2_PRI_MAP_MTU_SHIFT;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_mtu_sw_idx_get:"
                            "hw_index %d, sw_idx %d\n"),
                            hw_index, *sw_idx));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_stu_hw_index_get
 * Purpose:
 *      Get HW memory base index
 * Parameters:
 *      unit     - (IN) Unit number
 *      sw_idx   - (IN) SW memory index
 *      hw_index - (OUT) HW memory base index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_stu_hw_index_get(
    int unit,
    uint32 sw_idx,
    uint32 *hw_index)
{
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == hw_index ||
        GH2_PRI_MAP_STU_TAB_SIZE <= sw_idx) {
        return BCM_E_PARAM;
    }
    *hw_index = sw_idx * GH2_PRI_MAP_STU_ENT_SIZE;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_stu_hw_index_get:"
                            "sw_idx %d, hw_index %d\n"),
                            sw_idx, *hw_index));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_pri_map_stu_sw_idx_get
 * Purpose:
 *      Get SW memory index
 * Parameters:
 *      unit     - (IN) Unit number
 *      hw_index - (IN) HW memory base index
 *      sw_idx   - (OUT) SW memory index
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_pri_map_stu_sw_idx_get(
    int unit,
    uint32 hw_index,
    uint32 *sw_idx)
{
    if (!soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == sw_idx ||
        GH2_PRI_MAP_STU_MASK & hw_index ||
        GH2_PRI_MAP_STU_TAB_SIZE * GH2_PRI_MAP_STU_ENT_SIZE <= hw_index) {
        return BCM_E_PARAM;
    }
    *sw_idx = hw_index >> GH2_PRI_MAP_STU_SHIFT;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_gh2_tsn_pri_map_stu_sw_idx_get:"
                            "hw_index %d, sw_idx %d\n"),
                            hw_index, *sw_idx));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_control_set
 * Description:
 *     Configure device-wide operation modes for TSN.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (IN) argument to be set
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_control_set(int unit, bcm_tsn_control_t type, uint32 arg)
{
#if defined(BCM_TSN_SR_SUPPORT)
    soc_reg_t config_reg = INVALIDr;
    soc_mem_t config_mem = INVALIDm;
    soc_mem_t config_mem_egr = INVALIDm;
    soc_field_t config_field = INVALIDf;
    uint32 fldval = 0;
    uint32 regval = 0;
    int memidx = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
#endif /* BCM_TSN_SR_SUPPORT */

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_control_set: type = %d arg = %d\n"),
                            type, arg));

#if defined(BCM_TSN_SR_SUPPORT)
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
#endif /* BCM_TSN_SR_SUPPORT */

    switch (type) {
#if defined(BCM_TSN_SR_SUPPORT)
        case bcmTsnControlSrAgeTickInterval:
            if (((int)arg < 0) || (arg > 0xffff)) {
                return BCM_E_PARAM;
            }
            config_reg = SR_AGING_TIMERr;
            config_field = AGE_OUT_TIMEf;
            fldval = arg;
            if (EVENTS_BKINFO(unit) != NULL) {
                EVENTS_BKINFO(unit)->ageout_tick_ms = arg;
            }
            break;
        case bcmTsnControlSrAgeOutEnable:
            if (((int)arg < 0) || (arg > 1)) {
                return BCM_E_PARAM;
            }
            config_reg = SR_AGING_TIMERr;
            config_field = AGE_ENAf;
            fldval = arg;
            if (EVENTS_BKINFO(unit) != NULL) {
                EVENTS_BKINFO(unit)->age_enable = arg;
            }
            break;
        case bcmTsnControlSrSeqNumWindowResetMode:
            config_reg = SR_AGING_TIMERr;
            config_field = SN_WINDOW_RESET_TYPEf;
            if (arg == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1) {
                fldval = 0;
            } else if (arg == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW2) {
                fldval = 1;
            } else if (arg == BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW) {
                fldval = 2;
            } else {
                return BCM_E_PARAM;
            }

            if (EVENTS_BKINFO(unit) != NULL) {
                EVENTS_BKINFO(unit)->window_reset_mode = arg;
            }
            break;
        case bcmTsnControlSrLooseOutOfOrder:
            if (((int)arg < 0) || (arg > 1)) {
                return BCM_E_PARAM;
            }
            config_reg = SR_OUT_OF_ORDER_CONTROLr;
            config_field = OUT_OF_ORDER_CONTROLf;
            fldval = arg;
            break;
        case bcmTsnControlSrHsrEthertype:
            if (arg > 0xffff) {
                return BCM_E_PARAM;
            }
            config_mem = SR_ETHERTYPESm;
            config_mem_egr = EGR_SR_ETHERTYPESm;
            config_field = HSR_ETHERTYPEf;
            memidx = 0;
            fldval = arg;
            break;
        case bcmTsnControlSrPrpEthertype:
            if (arg > 0xffff) {
                return BCM_E_PARAM;
            }
            config_mem = SR_ETHERTYPESm;
            config_mem_egr = EGR_SR_ETHERTYPESm;
            config_field = PRP_SUFFIXf;
            memidx = 0;
            fldval = arg;
            break;
        case bcmTsnControlSrDot1cbEthertype:
            if (arg > 0xffff) {
                return BCM_E_PARAM;
            }
            config_mem = SR_ETHERTYPESm;
            config_mem_egr = EGR_SR_ETHERTYPESm;
            config_field = DOT1CB_ETHERTYPEf;
            memidx = 0;
            fldval = arg;
            break;
#endif /* BCM_TSN_SR_SUPPORT */
        default:
            return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (config_reg != INVALIDr) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, config_reg, REG_PORT_ANY, 0, &regval));
        soc_reg_field_set(unit, config_reg, &regval,
                          config_field, fldval);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, config_reg, REG_PORT_ANY, 0, regval));
    }

    if (config_mem != INVALIDm) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, config_mem, MEM_BLOCK_ANY, memidx, &entry));
        soc_mem_field32_set(unit, config_mem, &entry, config_field, fldval);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    }

    if (config_mem_egr != INVALIDm) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, config_mem_egr, MEM_BLOCK_ANY,
                          memidx, &entry));
        soc_mem_field32_set(unit, config_mem_egr, &entry, config_field, fldval);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, config_mem_egr, MEM_BLOCK_ALL,
                           memidx, &entry));
    }
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_control_get
 * Description:
 *     Get the configure of device-wide TSN operation modes.
 * Parameters:
 *     unit - (IN) Unit number.
 *     type - (IN) Control type of bcm_tsn_control_t.
 *     arg  - (OUT) argument got
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_control_get(int unit, bcm_tsn_control_t type, uint32 *arg)
{
#if defined(BCM_TSN_SR_SUPPORT)
    uint32 fldval = 0;
    uint32 regval = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
#endif /* BCM_TSN_SR_SUPPORT */

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_control_get: type = %d"),
                            type));

#if defined(BCM_TSN_SR_SUPPORT)
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return (BCM_E_UNAVAIL);
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Parameter validation */
    if (NULL == arg) {
        return BCM_E_PARAM;
    }

    switch (type) {
#if defined(BCM_TSN_SR_SUPPORT)
        case bcmTsnControlSrAgeTickInterval:
            BCM_IF_ERROR_RETURN(READ_SR_AGING_TIMERr(unit, &regval));
            fldval = soc_reg_field_get(unit, SR_AGING_TIMERr,
                                       regval, AGE_OUT_TIMEf);
            break;
        case bcmTsnControlSrAgeOutEnable:
            BCM_IF_ERROR_RETURN(READ_SR_AGING_TIMERr(unit, &regval));
            fldval = soc_reg_field_get(unit, SR_AGING_TIMERr,
                                       regval, AGE_ENAf);
            break;
        case bcmTsnControlSrSeqNumWindowResetMode:
            BCM_IF_ERROR_RETURN(READ_SR_AGING_TIMERr(unit, &regval));
            fldval = soc_reg_field_get(unit, SR_AGING_TIMERr,
                                       regval, SN_WINDOW_RESET_TYPEf);
            if (fldval == 0) {
                fldval = BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW1;
            } else if (fldval == 1) {
                fldval = BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_HW2;
            } else if (fldval == 2) {
                fldval = BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW;
            } else {
                return BCM_E_FAIL;
            }
            break;
        case bcmTsnControlSrLooseOutOfOrder:
            BCM_IF_ERROR_RETURN(READ_SR_OUT_OF_ORDER_CONTROLr(unit, &regval));
            fldval = soc_reg_field_get(unit, SR_OUT_OF_ORDER_CONTROLr,
                                       regval, OUT_OF_ORDER_CONTROLf);
            break;
        case bcmTsnControlSrHsrEthertype:
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY, 0, &entry));
            soc_mem_field_get(unit, SR_ETHERTYPESm,
                              entry, HSR_ETHERTYPEf, &fldval);
            break;
        case bcmTsnControlSrPrpEthertype:
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY, 0, &entry));
            soc_mem_field_get(unit, SR_ETHERTYPESm,
                              entry, PRP_SUFFIXf, &fldval);
            break;
        case bcmTsnControlSrDot1cbEthertype:
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, SR_ETHERTYPESm, MEM_BLOCK_ANY, 0, &entry));
            soc_mem_field_get(unit, SR_ETHERTYPESm,
                              entry, DOT1CB_ETHERTYPEf, &fldval);
            break;
#endif /* BCM_TSN_SR_SUPPORT */
        default:
            return BCM_E_PARAM;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    *arg = fldval;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "arg = %d\n"),
                            *arg));
#endif /* BCM_TSN_SR_SUPPORT */
    return BCM_E_NONE;
}

/* Helper function to get the reference count of the profile memory entry */
STATIC int
bcmi_gh2_tsn_mtu_profmem_refcnt_get(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    int index,
    int *ref_count)
{
    soc_profile_mem_t *pmem;

    if (ref_count == NULL) {
        return BCM_E_PARAM;
    }
    TSN_MTU_BKINFO_IS_INIT(unit, type);
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    MTU_BKINFO_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit, pmem,
                                                      index, ref_count));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_decode
 * Purpose:
 *      Decode the profile id into
 *      HW memory index and mtu profile type
 * Parameters:
 *      unit - (IN) Unit number
 *      mtu_profile_id - (IN) profile id
 *      type - (OUT) Indicates ingress or egress mtu profile
 *      index -(OUT) Indicates HW index of the mem
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_decode(
    int unit,
    int mtu_profile_id,
    bcm_tsn_mtu_profile_type_t *type,
    int *index)
{
    soc_profile_mem_table_t *table;

    if (type == NULL || index == NULL) {
        return BCM_E_PARAM;
    }
    /* If it is an ingress mtu profile id */
    if ((mtu_profile_id & EGR_MTU_STU_PROFILE_ID_ENCODE) == 0) {
        *type = bcmTsnMtuProfileTypeIngress;
        TSN_MTU_BKINFO_IS_INIT(unit, *type);
    } else {
        /* If it is an egress mtu profile id */
        *type = bcmTsnMtuProfileTypeEgress;
        TSN_MTU_BKINFO_IS_INIT(unit, *type);
        mtu_profile_id &= ~EGR_MTU_STU_PROFILE_ID_ENCODE;
    }

    /* Check whether the memory index is valid */
    MTU_BKINFO_LOCK(unit);
    table = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[*type]->tables;
    if (mtu_profile_id > table->index_max ||
        mtu_profile_id < table->index_min) {
        MTU_BKINFO_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    MTU_BKINFO_UNLOCK(unit);
    *index = mtu_profile_id;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_esw_tsn_mtu_profile_encode
 * Purpose:
 *      Encode the HW memory index and type
 *      into profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW memory index
 *      type - (IN) Indicates ingress or egress mtu profile
 *      mtu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_encode(
    int unit,
    int index,
    bcm_tsn_mtu_profile_type_t type,
    int *mtu_profile_id)
{
    soc_profile_mem_table_t *table;

    /* The validity of the type is checked in the previous layer */
    if (mtu_profile_id == NULL) {
        return BCM_E_PARAM;
    }
    if (TSN_BKINFO(unit) != NULL) {
        TSN_MTU_BKINFO_IS_INIT(unit, type);
        /* Check whether the memory index is valid */
        MTU_BKINFO_LOCK(unit);
        table = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type]->tables;
        if (index > table->index_max || index < table->index_min) {
            MTU_BKINFO_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        MTU_BKINFO_UNLOCK(unit);
    }
    if (type == bcmTsnMtuProfileTypeIngress) {
        *mtu_profile_id = index;
    } else {
        *mtu_profile_id = index | EGR_MTU_STU_PROFILE_ID_ENCODE;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_gh2_tsn_mtu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_traverse(
    int unit,
    bcm_tsn_mtu_profile_traverse_cb cb,
    void *user_data)
{
    int i, profile_id, rv;
    soc_profile_mem_table_t *table;
    bcm_tsn_mtu_profile_type_t type;

    if (user_data == NULL || cb == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse on Profile Mem */
    MTU_BKINFO_LOCK(unit);
    for (type = bcmTsnMtuProfileTypeIngress; type < bcmTsnMtuProfileTypeCount;
         type++) {
         TSN_MTU_BKINFO_IS_INIT(unit, type);
         table = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type]->tables;

         for (i = table->index_min; i <= table->index_max; i++) {
            bcmi_gh2_tsn_mtu_profile_encode(unit, i, type, &profile_id);
            /* profile id 0 is the default value, won't traverse it */
            if (table->entries[i].ref_count > 0  && i != 0) {
                rv = cb(unit, profile_id, user_data);
                if (BCM_FAILURE(rv)) {
                    MTU_BKINFO_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }
    MTU_BKINFO_UNLOCK(unit);

    return BCM_E_NONE;
}


/* helper function to get the profile memory index */
STATIC int
bcmi_gh2_tsn_mtu_profile_mem_idx_get(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    int *min,
    int *max)
{
    if (min == NULL || max == NULL) {
        return BCM_E_PARAM;
    }
    if (type != bcmTsnMtuProfileTypeIngress &&
        type != bcmTsnMtuProfileTypeEgress) {
        return BCM_E_PARAM;
    }
    *min = soc_mem_index_min(unit, gh2_mtu_profile_mem[type]);
    *max = soc_mem_index_max(unit, gh2_mtu_profile_mem[type]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_mtu_profile_mem_init
 * Purpose:
 *      Initiate mtu profile mem management structure
 *      and set MTU_PROFILEm entries to zero.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress mtu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_mem_init(
    int unit,
    bcm_tsn_mtu_profile_type_t type)
{
    soc_mem_t mem;
    soc_mem_t mems[1];
    int mem_words[1];
    soc_profile_mem_t *pmem;
    int rv;

    TSN_MTU_BKINFO_IS_INIT(unit, type);

    /* Set ingress MTU profile memory HW table entries to zero */
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit))
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, gh2_mtu_profile_mem[type], MEM_BLOCK_ALL, 0));
    }
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    soc_profile_mem_t_init(pmem);
    mem = gh2_mtu_profile_mem[type];
    if (SOC_MEM_IS_VALID(unit, mem)) {
        mems[0] = mem;
        if (type == bcmTsnMtuProfileTypeIngress) {
            mem_words[0] = BYTES2WORDS(sizeof(mtu_profile_entry_t));
        } else {
            mem_words[0] = BYTES2WORDS(sizeof(egr_mtu_profile_entry_t));
        }
    } else {
        MTU_BKINFO_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }
    rv = soc_profile_mem_create(unit, mems, mem_words, 1, pmem);

    MTU_BKINFO_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_mtu_profile_create
 * Purpose:
 *      Create mtu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress mtu profile
 *      config - (IN) configuration of the mtu profile
 *      index - (OUT) HW index of ingress mtu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_create(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    bcm_tsn_mtu_config_t *config,
    uint32 *index)
{
    void *entries[1];
    soc_profile_mem_t *pmem;
    void *entry;
    mtu_profile_entry_t entry_i;
    egr_mtu_profile_entry_t entry_e;
    int rv;

    if (index == NULL || config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_MTU_BKINFO_IS_INIT(unit, type);

    if (type == bcmTsnMtuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_mtu_profile_mem[type],
                             MTU_SIZEf, config->size));
    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_mtu_profile_mem[type],
                             MTU_ERROR_ACTIONf, config->drop));
    soc_mem_field32_set(unit, gh2_mtu_profile_mem[type], (uint32 *)entry,
                        MTU_SIZEf, config->size);
    soc_mem_field32_set(unit, gh2_mtu_profile_mem[type], (uint32 *)entry,
                        MTU_ERROR_ACTIONf, config->drop);
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    entries[0] = entry;
    rv = soc_profile_mem_add(unit, pmem, entries, 1, index);
    MTU_BKINFO_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_mtu_profile_get
 * Purpose:
 *      Get mtu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW index of ingress mtu profile
 *      type - (IN) Indicates ingress or egress mtu profile
 *      config - (OUT) configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_get(
    int unit,
    int index,
    bcm_tsn_mtu_profile_type_t type,
    bcm_tsn_mtu_config_t *config)
{
    void *entries[1];
    soc_profile_mem_t *pmem;
    void *entry;
    mtu_profile_entry_t entry_i;
    egr_mtu_profile_entry_t entry_e;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_MTU_BKINFO_IS_INIT(unit, type);

    if (type == bcmTsnMtuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    entries[0] = entry;
    BCM_IF_ERROR_RETURN(soc_profile_mem_get(unit, pmem, index, 1, entries));
    if (SOC_MEM_FIELD_VALID(unit, gh2_mtu_profile_mem[type], MTU_SIZEf) &&
        SOC_MEM_FIELD_VALID(unit, gh2_mtu_profile_mem[type],
                            MTU_ERROR_ACTIONf)) {
        config->size =
            soc_mem_field32_get(unit, gh2_mtu_profile_mem[type],
                                (uint32 *)entry, MTU_SIZEf);
        config->drop =
            soc_mem_field32_get(unit, gh2_mtu_profile_mem[type],
                                (uint32 *)entry, MTU_ERROR_ACTIONf);
    } else {
        MTU_BKINFO_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }
    MTU_BKINFO_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_mtu_profile_set
 * Purpose:
 *      Set mtu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW index of ingress mtu profile
 *      type - (IN) Indicates ingress or egress mtu profile
 *      config - (IN) configuration of the mtu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_set(
    int unit,
    int index,
    bcm_tsn_mtu_profile_type_t type,
    bcm_tsn_mtu_config_t *config)
{
    int idx, profile_idx, rv;
    void *entries[1];
    bcm_tsn_mtu_config_t config_repl;
    soc_profile_mem_table_t *table;
    soc_profile_mem_t *pmem;
    void *entry;
    mtu_profile_entry_t entry_i;
    egr_mtu_profile_entry_t entry_e;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_MTU_BKINFO_IS_INIT(unit, type);

    /*
    * Search for whether replicated profile mem config exist,
    * if existed return BCM_E_UNAVAIL
    */
    bcm_tsn_mtu_config_t_init(&config_repl);
    MTU_BKINFO_LOCK(unit);
    table = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type]->tables;
    for (idx = table->index_min; idx <= table->index_max; idx++) {
        bcmi_gh2_tsn_mtu_profile_encode(unit, idx, type, &profile_idx);
        bcmi_gh2_tsn_mtu_profile_get(unit, idx, type, &config_repl);
        if ((config_repl.size == config->size) &&
            (config_repl.drop == config->drop)) {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                    (BSL_META_U(unit,
                                "config already existed, found"
                                " mtu profile id(%d)\n"), profile_idx));
            MTU_BKINFO_UNLOCK(unit);
            return BCM_E_EXISTS;
        }
    }
    MTU_BKINFO_UNLOCK(unit);

    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_mtu_profile_mem[type],
                             MTU_SIZEf, config->size));
    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_mtu_profile_mem[type], MTU_ERROR_ACTIONf,
                             config->drop));

    if (type == bcmTsnMtuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }

    if (SOC_MEM_FIELD_VALID(unit, gh2_mtu_profile_mem[type], MTU_SIZEf) &&
        SOC_MEM_FIELD_VALID(unit, gh2_mtu_profile_mem[type],
                            MTU_ERROR_ACTIONf)) {
        soc_mem_field32_set(unit, gh2_mtu_profile_mem[type],
                            entry, MTU_SIZEf, config->size);
        soc_mem_field32_set(unit, gh2_mtu_profile_mem[type], entry,
                            MTU_ERROR_ACTIONf, config->drop);
    } else {
        return BCM_E_INTERNAL;
    }
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    entries[0] = entry;
    rv = soc_profile_mem_set(unit, pmem, entries, index);
    MTU_BKINFO_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_mtu_profile_destroy
 * Purpose:
 *      Clear the mtu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress mtu profile
 *      index - (IN) Indicated the ingress mtu profile entry
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_mtu_profile_destroy(
    int unit,
    bcm_tsn_mtu_profile_type_t type,
    int index)
{
    uint32 buf;
    soc_profile_mem_t *pmem;
    void *entry;
    mtu_profile_entry_t entry_i;
    egr_mtu_profile_entry_t entry_e;
    int rv;

    TSN_MTU_BKINFO_IS_INIT(unit, type);
    if (type == bcmTsnMtuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }
    MTU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
    rv = soc_profile_mem_delete(unit, pmem, index);
    MTU_BKINFO_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* clear the HW table to zero */
    buf = 0x00000000;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, gh2_mtu_profile_mem[type],
                                     MEM_BLOCK_ANY, index, entry));
    soc_mem_field_set(unit, gh2_mtu_profile_mem[type],
                      (uint32 *)entry, MTU_SIZEf, &buf);
    soc_mem_field_set(unit, gh2_mtu_profile_mem[type], (uint32 *)entry,
                      MTU_ERROR_ACTIONf, &buf);
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, gh2_mtu_profile_mem[type], MEM_BLOCK_ALL,
                      index, entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_mtu_config_set
 * Purpose:
 *      Set the priority of source ingress
 *      mtu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the mtu profile priority
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_ingress_mtu_config_set(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    uint32 i, reg_val, val;
    int user_valid[bcmTsnMtuSourceCount];

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    /* source_order_count is mandatory to set as bcmTsnMtuSourceCount */
    if (config->source_order_count != bcmTsnMtuSourceCount) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < config->source_order_count; i++) {
        user_valid[i] = 0;
    }
    /* Check the user config is correct */
    for (i = 0; i < config->source_order_count; i++) {
        switch (config->source_order[i]) {
            case bcmTsnMtuSourceTsnFlow:
                user_valid[bcmTsnMtuSourceTsnFlow]++;
                break;
            case bcmTsnMtuSourceSrFlow:
                user_valid[bcmTsnMtuSourceSrFlow]++;
                break;
            case bcmTsnMtuSourcePort:
                user_valid[bcmTsnMtuSourcePort]++;
                break;
            case bcmTsnMtuSourceFieldLookup:
                user_valid[bcmTsnMtuSourceFieldLookup]++;
                break;
            default:
                return BCM_E_PARAM;
        }
    }
    for (i = 0; i < config->source_order_count; i++) {
        if (user_valid[i] != 1) {
            return BCM_E_PARAM;
        }
    }
    /*
     * higher value in the register field means higher priority, and less
     * array index in the source_order means higher priority
     */
    BCM_IF_ERROR_RETURN(READ_MTU_PRIORITYr(unit, &reg_val));
    for (i = 0; i < config->source_order_count; i++) {
        val = bcmTsnMtuSourceCount - 1 - i;
        switch (config->source_order[i]) {
            case bcmTsnMtuSourceTsnFlow:
                soc_reg_field_set(unit, MTU_PRIORITYr, &reg_val,
                                  L2_TSN_PRIORITYf, val);
                break;
            case bcmTsnMtuSourceSrFlow:
                soc_reg_field_set(unit, MTU_PRIORITYr, &reg_val,
                                  L2_SR_PRIORITYf, val);
                break;
            case bcmTsnMtuSourcePort:
                soc_reg_field_set(unit, MTU_PRIORITYr, &reg_val,
                                  PORT_PRIORITYf, val);
                break;
            case bcmTsnMtuSourceFieldLookup:
                soc_reg_field_set(unit, MTU_PRIORITYr, &reg_val,
                                  VFP_PRIORITYf, val);
                break;
            default:
                return BCM_E_PARAM;
        }
    }
    BCM_IF_ERROR_RETURN(WRITE_MTU_PRIORITYr(unit, reg_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_mtu_config_get
 * Purpose:
 *      Get the priority of source ingress
 *      mtu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the mtu profile priority
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_ingress_mtu_config_get(
    int unit,
    bcm_tsn_ingress_mtu_config_t *config)
{
    int i;
    uint32 reg_val, val;
    static const soc_field_t mtu_pri_field[] = {
        L2_TSN_PRIORITYf, L2_SR_PRIORITYf,
        PORT_PRIORITYf, VFP_PRIORITYf
    };
    static const bcm_tsn_mtu_source_t mtu_src[] = {
        bcmTsnMtuSourceTsnFlow, bcmTsnMtuSourceSrFlow,
        bcmTsnMtuSourcePort, bcmTsnMtuSourceFieldLookup
    };

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    /*
     * higher value in the register field means higher priority, and less
     * array index in the source_order means higher priority
     */

    config->source_order_count = bcmTsnMtuSourceCount;
    BCM_IF_ERROR_RETURN(READ_MTU_PRIORITYr(unit, &reg_val));
    for (i = 0; i < config->source_order_count; i++) {
        if (SOC_REG_FIELD_VALID(unit, MTU_PRIORITYr, mtu_pri_field[i])) {
            val = soc_reg_field_get(unit, MTU_PRIORITYr,
                                    reg_val, mtu_pri_field[i]);
        } else {
            return BCM_E_INTERNAL;
        }
        if (val < bcmTsnMtuSourceCount) {
            config->source_order[(bcmTsnMtuSourceCount - 1) - val] =
                                                                mtu_src[i];
        } else {
            return BCM_E_INTERNAL;
        }
    }
    return BCM_E_NONE;
}

/* Helper function to setup EGR_MTU_CHECKm */
STATIC int
bcmi_gh2_tsn_port_control_egress_mtu_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_config_t *config)
{
    bcm_port_t port;
    int pri, index, index_base, egr_prof_index;
    bcm_tsn_mtu_profile_type_t type;
    egr_mtu_check_entry_t egr_mtu;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (config->map_type != bcmTsnPriMapTypeEgressMtuProfile) {
        return BCM_E_PARAM;
    }
    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }

    index_base = port * EGRESS_MTU_STU_INT_PRI_NUM;
    sal_memset(&egr_mtu, 0, sizeof(egr_mtu));
    for (pri = 0; pri < config->num_map_entries; pri++) {
        index = index_base + pri;
        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_MTU_CHECKm, MEM_BLOCK_ANY, index,
                         &egr_mtu));
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_mtu_profile_decode(
                                unit, config->map_entries[pri].profile_id,
                                &type, &egr_prof_index));
        soc_mem_field32_set(unit, EGR_MTU_CHECKm, &egr_mtu,
                            MTU_PROFILE_INDEXf, egr_prof_index);
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, EGR_MTU_CHECKm, MEM_BLOCK_ALL, index,
                          &egr_mtu));
    }

    return BCM_E_NONE;
}

/* Helper function to set PORT_TABm MTU_PROFILE_INDEXf */
STATIC int
bcmi_gh2_tsn_port_control_ingress_mtu_set(
    int unit,
    bcm_gport_t gport,
    int index)
{
    bcm_port_t port;
    sr_port_table_entry_t mtu_profile;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }
    /* check whether index is in the valid range */
    SOC_IF_ERROR_RETURN(
        soc_mem_field32_fit(unit, SR_PORT_TABLEm, MTU_PROFILE_INDEXf, index));
    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY, port,
                     &mtu_profile));
    soc_mem_field32_set(unit, SR_PORT_TABLEm, &mtu_profile,
                        MTU_PROFILE_INDEXf, index);
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, SR_PORT_TABLEm, MEM_BLOCK_ALL, port,
                      &mtu_profile));

    return BCM_E_NONE;
}

/* Helper function to get PORT_TABm MTU_PROFILE_INDEXf */
STATIC int
bcmi_gh2_tsn_port_control_ingress_mtu_get(
    int unit,
    bcm_gport_t gport,
    int *index)
{
    bcm_port_t port;
    sr_port_table_entry_t mtu_profile;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM || index == NULL) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY, port,
                     &mtu_profile));
    *index = soc_mem_field32_get(unit, SR_PORT_TABLEm, &mtu_profile,
                                 MTU_PROFILE_INDEXf);

    return BCM_E_NONE;
}

/* Helper function to get the reference count of the profile memory entry */
STATIC int
bcmi_gh2_tsn_stu_profmem_refcnt_get(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    int index,
    int *ref_count)
{
    soc_profile_mem_t *pmem;

    if (ref_count == NULL) {
        return BCM_E_PARAM;
    }
    TSN_STU_BKINFO_IS_INIT(unit, type);
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    STU_BKINFO_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit, pmem,
                                                      index, ref_count));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_decode
 * Purpose:
 *      Decode the profile id into
 *      HW memory index and stu profile type
 * Parameters:
 *      unit - (IN) Unit number
 *      stu_profile_id - (IN) profile id
 *      type - (OUT) Indicates ingress or egress stu profile
 *      index -(OUT) Indicates HW index of the mem
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_decode(
    int unit,
    int stu_profile_id,
    bcm_tsn_stu_profile_type_t *type,
    int *index)
{
    soc_profile_mem_table_t *table;

    if (type == NULL || index == NULL) {
        return BCM_E_PARAM;
    }
    /* If it is an ingress stu profile id */
    if ((stu_profile_id & EGR_MTU_STU_PROFILE_ID_ENCODE) == 0) {
        *type = bcmTsnStuProfileTypeIngress;
        TSN_STU_BKINFO_IS_INIT(unit, *type);
    } else {
        /* If it is an egress stu profile id */
        *type = bcmTsnStuProfileTypeEgress;
        TSN_STU_BKINFO_IS_INIT(unit, *type);
        stu_profile_id &= ~EGR_MTU_STU_PROFILE_ID_ENCODE;
    }

    /* Check whether the memory index is valid */
    STU_BKINFO_LOCK(unit);
    table = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[*type]->tables;
    if (stu_profile_id > table->index_max ||
        stu_profile_id < table->index_min) {
        STU_BKINFO_UNLOCK(unit);
        return BCM_E_PARAM;
    }
    STU_BKINFO_UNLOCK(unit);
    *index = stu_profile_id;

    return BCM_E_NONE;
}

#if defined(BCM_TSN_SR_SUPPORT)

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_table_size_get
 * Purpose:
 *   Get the table size for auto learn
 * Parameters:
 *   unit - (IN) Unit number
 *   table_size - (OUT) table size
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_table_size_get(
    int unit,
    uint32 *table_size)
{
    /* Parameter NULL error handling */
    if (NULL == table_size) {
        return BCM_E_PARAM;
    }

    /*
      * SR Auto Learn table
      * Since each SR group need 3 ports at least,
      * divide with 3 is a reasonable total flow size
      */
    *table_size = NUM_PORT(unit) / 3;
    return BCM_E_NONE;
}

/* Per port check if SR is enabled and port role is correct matched. */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_group_port_config_check(
    int unit,
    bcmi_tsn_sr_auto_learn_group_port_type_t group_port_type,
    bcm_gport_t gport)
{
    bcm_tsn_sr_port_config_t org_config;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "%2d "), gport));

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_port_config_get(unit, gport, &org_config));

    /* Port pre-configuration check */
    if (bcmTsnSrPortTypeNone == org_config.port_type) {
        return BCM_E_CONFIG;
    } else if ((bcmi_sr_auto_learn_group_l2mc == group_port_type) &&
               (org_config.interlink_role != 0)) {
        return BCM_E_CONFIG;
    }

    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_group_port_set
 * Purpose:
 *   Set value to the ports of given bitmap
 * Parameters:
 *   unit            - (IN) Unit number
 *   group_port_type - (IN) SR Auto Learn Group port type
 *   port_pbmp       - (IN) port bitmap
 *   val             - (IN) value to write
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_group_port_set(
    int unit,
    bcmi_tsn_sr_auto_learn_group_port_type_t group_port_type,
    bcm_pbmp_t port_pbmp,
    int val)
{
    sr_port_table_entry_t data;
    soc_mem_t mem = INVALIDm;
    soc_field_t port_field = INVALIDf;
    bcm_port_t port;
    bcm_gport_t gport;
    int rv;
    int ptab;
    /* Port tables to write */
    const soc_mem_t mem_list[] = {
        SR_PORT_TABLEm,
        SR_LPORT_TABm
    };
    int mem_idx;
    bcm_module_t my_modid = -1;
    int src_trk_idx = 0; /* Source Trunk table index.*/
    uint32 mem_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &my_modid));

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "group_port_type %d , val %d\n"),
                            group_port_type, val));

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    if (group_port_type == bcmi_sr_auto_learn_group_l2mc) {
        port_field = L2MCf;
    } else if (group_port_type == bcmi_sr_auto_learn_group_macp) {
        port_field = SR_MAC_PROXY_PROFILE_PTRf;
    } else {
        return BCM_E_PARAM;
    }

    for (ptab = 0; ptab < COUNTOF(mem_list); ptab++) {
        mem = mem_list[ptab];
        if (!SOC_MEM_IS_VALID(unit, mem) ||
            !SOC_MEM_FIELD_VALID(unit, mem, port_field)) {
            return BCM_E_INTERNAL;
        }

        /* Check if the value fit the target field range */
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_fit(
                unit, mem, port_field, (uint32)val));
    }

    for (ptab = 0; ptab < COUNTOF(mem_list); ptab++) {
        mem = mem_list[ptab];
        PBMP_ITER(port_pbmp, port) {
            LOG_VERBOSE(BSL_LS_BCM_TSN,
                        (BSL_META_U(unit,
                                    "port %2d: "), port));
            /* convert port to gport */
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));

            /* Port SR related config check */
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_tsn_sr_auto_learn_group_port_config_check(
                    unit, group_port_type, gport));

            if (mem == SR_LPORT_TABm) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                          port, &src_trk_idx));
                BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                                 src_trk_idx, mem_entry));

                mem_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                              mem_entry, LPORT_PROFILE_IDXf);
            } else {
                mem_idx = port;
            }
            MEM_LOCK(unit, mem);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, mem_idx, &data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to read mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            soc_mem_field32_set(unit, mem, &data, port_field, (uint32)val);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_idx, &data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to write mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            MEM_UNLOCK(unit, mem);
        }
    }
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_group_port_delete
 * Purpose:
 *   Reset the value of the ports of given bitmap
 * Parameters:
 *   unit            - (IN) Unit number
 *   group_port_type - (IN) SR Auto Learn Group port type
 *   port_pbmp       - (IN) port bitmap
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_group_port_delete(
    int unit,
    bcmi_tsn_sr_auto_learn_group_port_type_t group_port_type,
    bcm_pbmp_t port_pbmp)
{
    bcm_port_t port;
    int val;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit, "group_port_type %d "), group_port_type));

    PBMP_ITER(port_pbmp, port) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit, "%2d "), port));
    }

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    val = ((group_port_type == bcmi_sr_auto_learn_group_l2mc) ?
        GH2_SR_AUTO_LEARN_GROUP_PORTS_L2MC_DEFAULT_VALUE :
        GH2_SR_AUTO_LEARN_GROUP_PORTS_MACP_DEFAULT_VALUE);

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_auto_learn_group_port_set(
            unit, group_port_type, port_pbmp,
            val));
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_group_port_reset_all
 * Purpose:
 *   Reset the port value for all ports
 * Parameters:
 *   unit            - (IN) Unit number
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_group_port_reset_all(
    int unit)
{
    sr_port_table_entry_t data;
    soc_mem_t mem = INVALIDm;
    bcm_port_t port;
    bcm_pbmp_t ports_pbmp;
    int rv;
    int ptab;
    /* Port tables to write */
    const soc_mem_t mem_list[] = {
        SR_PORT_TABLEm,
        SR_LPORT_TABm
    };
    int mem_idx;
    bcm_module_t my_modid = -1;
    int src_trk_idx = 0; /* Source Trunk table index.*/
    uint32 mem_entry[SOC_MAX_MEM_WORDS];

    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &my_modid));

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    for (ptab = 0; ptab < COUNTOF(mem_list); ptab++) {
        mem = mem_list[ptab];
        if (!SOC_MEM_IS_VALID(unit, mem) ||
            !SOC_MEM_FIELD_VALID(unit, mem, L2MCf) ||
            !SOC_MEM_FIELD_VALID(unit, mem, SR_MAC_PROXY_PROFILE_PTRf)) {
            return BCM_E_INTERNAL;
        }
    }

    BCM_PBMP_ASSIGN(ports_pbmp, PBMP_PORT_ALL(unit));

    for (ptab = 0; ptab < COUNTOF(mem_list); ptab++) {
        mem = mem_list[ptab];
        PBMP_ITER(ports_pbmp, port) {
            if (mem == SR_LPORT_TABm) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_src_mod_port_table_index_get(unit, my_modid,
                                                          port, &src_trk_idx));
                BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                                 src_trk_idx, mem_entry));

                mem_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                              mem_entry, LPORT_PROFILE_IDXf);
            } else {
                mem_idx = port;
            }
            MEM_LOCK(unit, mem);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, mem_idx, &data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to read mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            soc_mem_field32_set(unit, mem, &data, L2MCf,
                GH2_SR_AUTO_LEARN_GROUP_PORTS_L2MC_DEFAULT_VALUE);
            soc_mem_field32_set(unit, mem, &data, SR_MAC_PROXY_PROFILE_PTRf,
                GH2_SR_AUTO_LEARN_GROUP_PORTS_MACP_DEFAULT_VALUE);

            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_idx, &data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "Failed to write mem[%s]"),
                                      SOC_MEM_NAME(unit, mem)));
                MEM_UNLOCK(unit, mem);
                return rv;
            }
            MEM_UNLOCK(unit, mem);
        }
    }

    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_set
 * Purpose:
 *   Set flow id control for auto learn subsystem
 * Parameters:
 *   unit        - (IN) Unit number
 *   flow_type   - (IN) SR Auto learn TX/RX flow type
 *   current_fid - (IN) current flow element
 *   last_fid    - (IN) last flow element
 *   id_size     - (IN) total flow elements for auto learn
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_set(
    int unit,
    bcmi_tsn_sr_auto_learn_flow_t flow_type,
    uint32 current_fid,
    uint32 last_fid,
    uint32 id_size)
{
    uint64 rval64 = COMPILER_64_INIT(0, 0);
    soc_reg_t sr_flow_id_control_reg;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "current_fid %d, last_fid %d, id_size %d\n"),
                            current_fid, last_fid, id_size));

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    if (bcmi_sr_auto_learn_flow_tx == flow_type) {
        sr_flow_id_control_reg = SR_TX_FLOW_ID_CONTROLr;
    } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
        sr_flow_id_control_reg = SR_RX_FLOW_ID_CONTROLr;
    } else {
        return BCM_E_PARAM;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(
            soc_reg_get(unit, sr_flow_id_control_reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(
            unit, sr_flow_id_control_reg, &rval64,
            CURRENT_PTRf, current_fid);
        soc_reg64_field32_set(
            unit, sr_flow_id_control_reg, &rval64,
            LAST_PTRf, last_fid);
        soc_reg64_field32_set(unit, sr_flow_id_control_reg, &rval64,
            ID_SIZEf, id_size);
        BCM_IF_ERROR_RETURN(
            soc_reg_set(unit, sr_flow_id_control_reg, REG_PORT_ANY, 0, rval64));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(
            soc_reg64_get(unit, sr_flow_id_control_reg, REG_PORT_ANY, 0, &rval64));
        soc_reg64_field32_set(
            unit, sr_flow_id_control_reg, &rval64,
            CURRENT_PTRf, current_fid);
        soc_reg64_field32_set(
            unit, sr_flow_id_control_reg, &rval64,
            LAST_PTRf, last_fid);
        soc_reg64_field32_set(unit, sr_flow_id_control_reg, &rval64,
            ID_SIZEf, id_size);
        BCM_IF_ERROR_RETURN(
            soc_reg64_set(unit, sr_flow_id_control_reg, REG_PORT_ANY, 0, rval64));
    }
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_get
 * Purpose:
 *   Get flow id pool for auto learn subsystem
 * Parameters:
 *   unit      - (IN) Unit number
 *   flow_type - (IN) SR Auto learn TX/RX flow type
 *   prev_fid  - (IN) previous HW flow ID
 *   fid       - (OUT) HW flow ID
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_get(
    int unit,
    bcmi_tsn_sr_auto_learn_flow_t flow_type,
    uint32 prev_fid,
    uint32 *fid)
{
    soc_mem_t mem = INVALIDm;
    soc_field_t field = NEXT_FLOW_IDf;
    sr_tx_flow_id_pool_entry_t tx_data;
    sr_rx_flow_id_pool_entry_t rx_data;
    void *data;
    uint32 hw_idx;
    int rx_flows, tx_flows;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");
    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == fid) {
        return BCM_E_PARAM;
    }

    /* Get fid from POOL.NEXT_FLOW_ID[prev_fid] */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        hw_idx = (uint32)FL_AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(prev_fid);
    } else
#endif /* end of BCM_FIRELIGHT_SUPPORT */
    {
        hw_idx = (uint32)AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(prev_fid);
    }
    if (hw_idx >= soc_mem_index_count(unit, SR_SN_HISTORY_0m)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_flows_get_num_flows(unit, &rx_flows, &tx_flows));

    if (bcmi_sr_auto_learn_flow_tx == flow_type) {
        mem = SR_TX_FLOW_ID_POOLm;
        sal_memset(&tx_data, 0, sizeof(sr_tx_flow_id_pool_entry_t));
        data = (sr_tx_flow_id_pool_entry_t *)&tx_data;
        if (hw_idx >= tx_flows) {
            return BCM_E_PARAM;
        }
    } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
        mem = SR_RX_FLOW_ID_POOLm;
        sal_memset(&rx_data, 0, sizeof(sr_tx_flow_id_pool_entry_t));
        data = (sr_rx_flow_id_pool_entry_t *)&rx_data;
        if (hw_idx >= rx_flows) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }

    if (SOC_MEM_IS_VALID(unit, mem) || SOC_MEM_FIELD_VALID(unit, mem, field)) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, data));
        soc_mem_field_get(unit, mem,
                          data, field, fid);
    } else {
        return BCM_E_INTERNAL;
    }
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        *fid = (uint32)FL_AUTO_LEARN_HW_FLOW_ID_TO_FLOW_ID(flow_type, *fid);
    } else
#endif /* end of BCM_FIRELIGHT_SUPPORT */
    {
        *fid = (uint32)AUTO_LEARN_HW_FLOW_ID_TO_FLOW_ID(flow_type, *fid);
    }

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "flow_type %d, prev_fid %d, *fid %d\n"),
                            flow_type, prev_fid, *fid));
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_get
 * Purpose:
 *   get flow id control for auto learn subsystem
 * Parameters:
 *   unit      - (IN) Unit number
 *   flow_type - (IN) SR Auto learn TX/RX flow type
 *   current   - (OUT) current flow element
 *   last_fid  - (OUT) last flow element
 *   id_size   - (OUT) total flow elements for auto learn
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_get(
    int unit,
    bcmi_tsn_sr_auto_learn_flow_t flow_type,
    uint32 *current_fid,
    uint32 *last_fid,
    uint32 *id_size)
{
    uint64 rval64 = COMPILER_64_INIT(0, 0);
    soc_reg_t sr_flow_id_control_reg;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == current_fid ||
        NULL == last_fid ||
        NULL == id_size) {
        return BCM_E_PARAM;
    }

    if (bcmi_sr_auto_learn_flow_tx == flow_type) {
        sr_flow_id_control_reg = SR_TX_FLOW_ID_CONTROLr;
    } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
        sr_flow_id_control_reg = SR_RX_FLOW_ID_CONTROLr;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, sr_flow_id_control_reg, REG_PORT_ANY, 0, &rval64));
    *current_fid =
        soc_reg64_field32_get(unit, sr_flow_id_control_reg, rval64,
            CURRENT_PTRf);
    *last_fid =
        soc_reg64_field32_get(unit, sr_flow_id_control_reg, rval64,
            LAST_PTRf);
    *id_size =
        soc_reg64_field32_get(unit, sr_flow_id_control_reg, rval64,
            ID_SIZEf);

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "*current_fid %d, *last_fid %d, *id_size %d\n"),
                            *current_fid, *last_fid, *id_size));
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_set
 * Purpose:
 *   Set flow id pool for auto learn subsystem
 * Parameters:
 *   unit      - (IN) Unit number
 *   flow_type - (IN) SR Auto learn TX/RX flow type
 *   prev_fid  - (IN) previous HW flow ID
 *   fid       - (IN) HW flow ID
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_set(
    int unit,
    bcmi_tsn_sr_auto_learn_flow_t flow_type,
    uint32 prev_fid,
    uint32 fid)
{
    soc_mem_t mem = INVALIDm;
    soc_field_t field = NEXT_FLOW_IDf;
    sr_tx_flow_id_pool_entry_t tx_data;
    sr_rx_flow_id_pool_entry_t rx_data;
    void *data;
    uint32 hw_idx;
    uint32 next_fid;
    int rx_flows, tx_flows;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "flow_type %d, prev_fid %d, fid %d\n"),
                            flow_type, prev_fid, fid));

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        /* Covert it to next_fflow ID */
        next_fid = (uint32)FL_AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(fid);

        /* POOL.NEXT_FLOW_ID[prev_fid] = fid; */
        hw_idx = (uint32)FL_AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(prev_fid);
    } else
#endif /* end of BCM_FIRELIGHT_SUPPORT */
    {
        /* Covert it to next_fflow ID */
        next_fid = (uint32)AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(fid);

        /* POOL.NEXT_FLOW_ID[prev_fid] = fid; */
        hw_idx = (uint32)AUTO_LEARN_FLOW_ID_TO_HW_FLOW_ID(prev_fid);
    }

    BCM_IF_ERROR_RETURN(
        bcmi_gh2_tsn_sr_flows_get_num_flows(unit, &rx_flows, &tx_flows));

    if (bcmi_sr_auto_learn_flow_tx == flow_type) {
        mem = SR_TX_FLOW_ID_POOLm;
        sal_memset(&tx_data, 0, sizeof(sr_tx_flow_id_pool_entry_t));
        data = (sr_tx_flow_id_pool_entry_t *)&tx_data;
        if (hw_idx >= tx_flows) {
            return BCM_E_PARAM;
        }
    } else if (bcmi_sr_auto_learn_flow_rx == flow_type) {
        mem = SR_RX_FLOW_ID_POOLm;
        sal_memset(&rx_data, 0, sizeof(sr_tx_flow_id_pool_entry_t));
        data = (sr_rx_flow_id_pool_entry_t *)&rx_data;
        if (hw_idx >= rx_flows) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }

    if (SOC_MEM_IS_VALID(unit, mem) && SOC_MEM_FIELD_VALID(unit, mem, field)) {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, hw_idx, data));
        soc_mem_field32_set(unit, mem, data, field, next_fid);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_idx, data));
    } else {
        return BCM_E_INTERNAL;
    }
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_enable
 * Purpose:
 *   Enable/disable SR flow auto learn subsystem
 * Parameters:
 *   unit   - (IN) Unit number
 *   enable - (IN) Enable/disable auto learn
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_enable(
    int unit,
    int enable)
{
    uint32 rval = 0, val = 0;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "enable %d\n"), enable));

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(READ_HW_MAC_LEARNINGr(unit, &rval));
    if (enable) {
        val = 1;
    } else {
        val = 0;
    }
    soc_reg_field_set(unit,
                      HW_MAC_LEARNINGr,
                      &rval,
                      HW_MAC_LEARNING_ENABLEf,
                      val);
    BCM_IF_ERROR_RETURN(WRITE_HW_MAC_LEARNINGr(unit, rval));
    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_gh2_tsn_sr_auto_learn_enable_get
 * Purpose:
 *   Get enable/disable status of SR flow auto learn subsystem
 * Parameters:
 *   unit    - (IN) Unit number
 *   enabled - (OUT) enable/disable of SR flow auto learn
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_gh2_tsn_sr_auto_learn_enable_get(
    int unit,
    int *enabled)
{
    uint32 rval = 0;

    GH2_TSN_FUNCTION_TRACE(unit, "IN");

    if (!soc_feature(unit, soc_feature_tsn_sr) ||
        !soc_feature(unit, soc_feature_tsn_sr_auto_learn)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation */
    if (NULL == enabled) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_HW_MAC_LEARNINGr(unit, &rval));
    *enabled = soc_reg_field_get(unit,
                                 HW_MAC_LEARNINGr,
                                 rval,
                                 HW_MAC_LEARNING_ENABLEf);
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "enabled %d\n"), *enabled));

    GH2_TSN_FUNCTION_TRACE(unit, "OUT");
    return BCM_E_NONE;
}
#endif /* BCM_TSN_SR_SUPPORT */

/*
 * Function:
 *      bcmi_esw_tsn_stu_profile_encode
 * Purpose:
 *      Encode the HW memory index and type
 *      into profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW memory index
 *      type - (IN) Indicates ingress or egress stu profile
 *      stu_profile_id - (OUT) profile id
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_encode(
    int unit,
    int index,
    bcm_tsn_stu_profile_type_t type,
    int *stu_profile_id)
{
    soc_profile_mem_table_t *table;

    /* The validity of the type is checked in the previous layer */
    if (stu_profile_id == NULL) {
        return BCM_E_PARAM;
    }
    if (TSN_BKINFO(unit) != NULL) {
        TSN_STU_BKINFO_IS_INIT(unit, type);
        /* Check whether the memory index is valid */
        STU_BKINFO_LOCK(unit);
        table = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type]->tables;
        if (index > table->index_max || index < table->index_min) {
            STU_BKINFO_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        STU_BKINFO_UNLOCK(unit);
    }
    if (type == bcmTsnStuProfileTypeIngress) {
        *stu_profile_id = index;
    } else {
        *stu_profile_id = index | EGR_MTU_STU_PROFILE_ID_ENCODE;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_gh2_tsn_stu_profile_traverse
 * Purpose:
 *      Traverse the created match config.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_traverse(
    int unit,
    bcm_tsn_stu_profile_traverse_cb cb,
    void *user_data)
{
    int i, profile_id, rv;
    soc_profile_mem_table_t *table;
    bcm_tsn_stu_profile_type_t type;

    if (user_data == NULL || cb == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse on Profile Mem */
    STU_BKINFO_LOCK(unit);
    for (type = bcmTsnStuProfileTypeIngress; type < bcmTsnStuProfileTypeCount;
         type++) {
         TSN_STU_BKINFO_IS_INIT(unit, type);
         table = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type]->tables;

         for (i = table->index_min; i <= table->index_max; i++) {
            bcmi_gh2_tsn_stu_profile_encode(unit, i, type, &profile_id);
            /* profile id 0 is the default value, won't traverse it */
            if (table->entries[i].ref_count > 0 && i != 0) {
                rv = cb(unit, profile_id, user_data);
                if (BCM_FAILURE(rv)) {
                    STU_BKINFO_UNLOCK(unit);
                    return rv;
                }
            }
        }
    }
    STU_BKINFO_UNLOCK(unit);

    return BCM_E_NONE;
}

/* helper function to get the profile memory index */
STATIC int
bcmi_gh2_tsn_stu_profile_mem_idx_get(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    int *min,
    int *max)
{
    if (min == NULL || max == NULL) {
        return BCM_E_PARAM;
    }
    if (type != bcmTsnStuProfileTypeIngress &&
        type != bcmTsnStuProfileTypeEgress) {
        return BCM_E_PARAM;
    }
    *min = soc_mem_index_min(unit, gh2_stu_profile_mem[type]);
    *max = soc_mem_index_max(unit, gh2_stu_profile_mem[type]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_stu_profile_mem_init
 * Purpose:
 *      Initiate stu profile mem management structure
 *      and set STU_PROFILEm entries to zero.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress stu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_mem_init(
    int unit,
    bcm_tsn_stu_profile_type_t type)
{
    soc_mem_t mem;
    soc_mem_t mems[1];
    int mem_words[1];
    soc_profile_mem_t *pmem;
    int rv;

    TSN_STU_BKINFO_IS_INIT(unit, type);

    /* Set ingress STU profile memory HW table entries to zero */
#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit))
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, gh2_stu_profile_mem[type], MEM_BLOCK_ALL, 0));
    }
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    soc_profile_mem_t_init(pmem);
    mem = gh2_stu_profile_mem[type];
    if (SOC_MEM_IS_VALID(unit, mem)) {
        mems[0] = mem;
        if (type == bcmTsnStuProfileTypeIngress) {
            mem_words[0] = BYTES2WORDS(sizeof(stu_profile_entry_t));
        } else {
            mem_words[0] = BYTES2WORDS(sizeof(egr_stu_profile_entry_t));
        }
    } else {
        STU_BKINFO_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }
    rv = soc_profile_mem_create(unit, mems, mem_words, 1, pmem);

    STU_BKINFO_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_stu_profile_create
 * Purpose:
 *      Create stu profile with the config pointer
 *      and return the assigned profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress stu profile
 *      config - (IN) configuration of the stu profile
 *      index - (OUT) HW index of ingress stu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_create(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    bcm_tsn_stu_config_t *config,
    uint32 *index)
{
    void *entries[1];
    soc_profile_mem_t *pmem;
    void *entry;
    stu_profile_entry_t entry_i;
    egr_stu_profile_entry_t entry_e;
    int rv;

    if (index == NULL || config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_STU_BKINFO_IS_INIT(unit, type);

    if (type == bcmTsnStuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_stu_profile_mem[type],
                             STU_SIZEf, config->size));
    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_stu_profile_mem[type],
                             STU_ERROR_ACTIONf, config->drop));
    soc_mem_field32_set(unit, gh2_stu_profile_mem[type], (uint32 *)entry,
                        STU_SIZEf, config->size);
    soc_mem_field32_set(unit, gh2_stu_profile_mem[type], (uint32 *)entry,
                        STU_ERROR_ACTIONf, config->drop);
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    entries[0] = entry;
    rv = soc_profile_mem_add(unit, pmem, entries, 1, index);
    STU_BKINFO_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_stu_profile_get
 * Purpose:
 *      Get stu profile configuration
 *      with the profile id.
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW index of ingress stu profile
 *      type - (IN) Indicates ingress or egress stu profile
 *      config - (OUT) configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_get(
    int unit,
    int index,
    bcm_tsn_stu_profile_type_t type,
    bcm_tsn_stu_config_t *config)
{
    void *entries[1];
    soc_profile_mem_t *pmem;
    void *entry;
    stu_profile_entry_t entry_i;
    egr_stu_profile_entry_t entry_e;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_STU_BKINFO_IS_INIT(unit, type);

    if (type == bcmTsnStuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    entries[0] = entry;
    BCM_IF_ERROR_RETURN(soc_profile_mem_get(unit, pmem, index, 1, entries));
    if (SOC_MEM_FIELD_VALID(unit, gh2_stu_profile_mem[type], STU_SIZEf) &&
        SOC_MEM_FIELD_VALID(unit, gh2_stu_profile_mem[type],
                            STU_ERROR_ACTIONf)) {
        config->size =
            soc_mem_field32_get(unit, gh2_stu_profile_mem[type],
                                (uint32 *)entry, STU_SIZEf);
        config->drop =
            soc_mem_field32_get(unit, gh2_stu_profile_mem[type],
                                (uint32 *)entry, STU_ERROR_ACTIONf);
    } else {
        STU_BKINFO_UNLOCK(unit);
        return BCM_E_INTERNAL;
    }
    STU_BKINFO_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_stu_profile_set
 * Purpose:
 *      Set stu profile with the profile id
 *      and config parameter.
 * Parameters:
 *      unit - (IN) Unit number
 *      index - (IN) HW index of ingress stu profile
 *      type - (IN) Indicates ingress or egress stu profile
 *      config - (IN) configuration of the stu profile
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_set(
    int unit,
    int index,
    bcm_tsn_stu_profile_type_t type,
    bcm_tsn_stu_config_t *config)
{
    int idx, profile_idx, rv;
    void *entries[1];
    bcm_tsn_stu_config_t config_repl;
    soc_profile_mem_table_t *table;
    soc_profile_mem_t *pmem;
    void *entry;
    stu_profile_entry_t entry_i;
    egr_stu_profile_entry_t entry_e;

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    TSN_STU_BKINFO_IS_INIT(unit, type);

    /*
    * Search for whether replicated profile mem config exist,
    * if existed return BCM_E_UNAVAIL
    */
    bcm_tsn_stu_config_t_init(&config_repl);
    STU_BKINFO_LOCK(unit);
    table = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type]->tables;
    for (idx = table->index_min; idx <= table->index_max; idx++) {
        bcmi_gh2_tsn_stu_profile_encode(unit, idx, type, &profile_idx);
        bcmi_gh2_tsn_stu_profile_get(unit, idx, type, &config_repl);
        if ((config_repl.size == config->size) &&
            (config_repl.drop == config->drop)) {
            LOG_WARN(BSL_LS_BCMAPI_TSN,
                    (BSL_META_U(unit,
                                "config already existed, found"
                                " stu profile id(%d)\n"), profile_idx));
            STU_BKINFO_UNLOCK(unit);
            return BCM_E_EXISTS;
        }
    }
    STU_BKINFO_UNLOCK(unit);

    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_stu_profile_mem[type],
                             STU_SIZEf, config->size));
    SOC_IF_ERROR_RETURN
        (soc_mem_field32_fit(unit, gh2_stu_profile_mem[type], STU_ERROR_ACTIONf,
                             config->drop));

    if (type == bcmTsnStuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }

    if (SOC_MEM_FIELD_VALID(unit, gh2_stu_profile_mem[type], STU_SIZEf) &&
        SOC_MEM_FIELD_VALID(unit, gh2_stu_profile_mem[type],
                            STU_ERROR_ACTIONf)) {
        soc_mem_field32_set(unit, gh2_stu_profile_mem[type],
                            entry, STU_SIZEf, config->size);
        soc_mem_field32_set(unit, gh2_stu_profile_mem[type], entry,
                            STU_ERROR_ACTIONf, config->drop);
    } else {
        return BCM_E_INTERNAL;
    }
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    entries[0] = entry;
    rv = soc_profile_mem_set(unit, pmem, entries, index);
    STU_BKINFO_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_gh2_tsn_stu_profile_destroy
 * Purpose:
 *      Clear the stu profile with the profile id
 * Parameters:
 *      unit - (IN) Unit number
 *      type - (IN) Indicates ingress or egress stu profile
 *      index - (IN) Indicated the ingress stu profile entry
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_stu_profile_destroy(
    int unit,
    bcm_tsn_stu_profile_type_t type,
    int index)
{
    uint32 buf;
    soc_profile_mem_t *pmem;
    void *entry;
    stu_profile_entry_t entry_i;
    egr_stu_profile_entry_t entry_e;
    int rv;

    TSN_STU_BKINFO_IS_INIT(unit, type);
    if (type == bcmTsnStuProfileTypeIngress) {
        entry = &entry_i;
    } else {
        entry = &entry_e;
    }
    STU_BKINFO_LOCK(unit);
    pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
    rv = soc_profile_mem_delete(unit, pmem, index);
    STU_BKINFO_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* clear the HW table to zero */
    buf = 0x00000000;
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, gh2_stu_profile_mem[type],
                                     MEM_BLOCK_ANY, index, entry));
    soc_mem_field_set(unit, gh2_stu_profile_mem[type],
                      (uint32 *)entry, STU_SIZEf, &buf);
    soc_mem_field_set(unit, gh2_stu_profile_mem[type], (uint32 *)entry,
                      STU_ERROR_ACTIONf, &buf);
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, gh2_stu_profile_mem[type], MEM_BLOCK_ALL,
                      index, entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_stu_config_set
 * Purpose:
 *      Set the priority of source ingress
 *      stu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (IN) Configuration of the stu profile priority
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_ingress_stu_config_set(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    uint32 i, reg_val, val;
    int user_valid[bcmTsnStuSourceCount];

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    /* source_order_count is mandatory to set as bcmTsnStuSourceCount */
    if (config->source_order_count != bcmTsnStuSourceCount) {
        return BCM_E_PARAM;
    }
    for (i = 0; i < config->source_order_count; i++) {
        user_valid[i] = 0;
    }
    /* Check the user config is correct */
    for (i = 0; i < config->source_order_count; i++) {
        switch (config->source_order[i]) {
            case bcmTsnStuSourceFieldLookup:
                user_valid[bcmTsnStuSourceFieldLookup]++;
                break;
            case bcmTsnStuSourcePort:
                user_valid[bcmTsnStuSourcePort]++;
                break;
            default:
                return BCM_E_PARAM;
        }
    }
    for (i = 0; i < config->source_order_count; i++) {
        if (user_valid[i] != 1) {
            return BCM_E_PARAM;
        }
    }
    /*
     * higher value in the register field means higher priority, and less
     * array index in the source_order means higher priority
     */
    BCM_IF_ERROR_RETURN(READ_STU_PRIORITYr(unit, &reg_val));
    for (i = 0; i < config->source_order_count; i++) {
        val = bcmTsnStuSourceCount - 1 - i;
        switch (config->source_order[i]) {
            case bcmTsnStuSourceFieldLookup:
                soc_reg_field_set(unit, STU_PRIORITYr, &reg_val,
                                  VFP_PRIORITYf, val);
                break;
            case bcmTsnStuSourcePort:
                soc_reg_field_set(unit, STU_PRIORITYr, &reg_val,
                                  PORT_PRIORITYf, val);
                break;
            default:
                return BCM_E_PARAM;
        }
    }
    BCM_IF_ERROR_RETURN(WRITE_STU_PRIORITYr(unit, reg_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_ingress_stu_config_get
 * Purpose:
 *      Get the priority of source ingress
 *      stu profile index.
 * Parameters:
 *      unit - (IN) Unit number
 *      config - (OUT) Configuration of the stu profile priority
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_ingress_stu_config_get(
    int unit,
    bcm_tsn_ingress_stu_config_t *config)
{
    int i;
    uint32 reg_val, val;
    static const soc_field_t stu_pri_field[] = {
        VFP_PRIORITYf, PORT_PRIORITYf
    };
    static const bcm_tsn_stu_source_t stu_src[] = {
        bcmTsnStuSourceFieldLookup, bcmTsnStuSourcePort
    };

    if (config == NULL) {
        return BCM_E_PARAM;
    }
    /*
     * higher value in the register field means higher priority, and less
     * array index in the source_order means higher priority
     */
    config->source_order_count = bcmTsnStuSourceCount;
    BCM_IF_ERROR_RETURN(READ_STU_PRIORITYr(unit, &reg_val));
    for (i = 0; i < config->source_order_count; i++) {
        if (SOC_REG_FIELD_VALID(unit, STU_PRIORITYr, stu_pri_field[i])) {
            val = soc_reg_field_get(unit, STU_PRIORITYr,
                                    reg_val, stu_pri_field[i]);
        } else {
            return BCM_E_INTERNAL;
        }
        if (val < bcmTsnStuSourceCount) {
            config->source_order[(bcmTsnStuSourceCount - 1) - val] =
                                                                stu_src[i];
        } else {
            return BCM_E_INTERNAL;
        }
    }
    return BCM_E_NONE;
}

/* Helper function to setup EGR_STU_CHECKm */
STATIC int
bcmi_gh2_tsn_port_control_egress_stu_set(
    int unit,
    bcm_gport_t gport,
    bcm_tsn_pri_map_config_t *config)
{
    bcm_port_t port;
    int pri, index, index_base, egr_prof_index;
    bcm_tsn_stu_profile_type_t type;
    egr_stu_check_entry_t egr_stu;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (config->map_type != bcmTsnPriMapTypeEgressStuProfile) {
        return BCM_E_PARAM;
    }

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }

    index_base = port * EGRESS_MTU_STU_INT_PRI_NUM;
    sal_memset(&egr_stu, 0, sizeof(egr_stu));
    for (pri = 0; pri < config->num_map_entries; pri++) {
        index = index_base + pri;
        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_STU_CHECKm, MEM_BLOCK_ANY, index,
                         &egr_stu));
        BCM_IF_ERROR_RETURN(bcmi_gh2_tsn_stu_profile_decode(
                                unit, config->map_entries[pri].profile_id,
                                &type, &egr_prof_index));
        soc_mem_field32_set(unit, EGR_STU_CHECKm, &egr_stu,
                            STU_PROFILE_INDEXf, egr_prof_index);
        SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, EGR_STU_CHECKm, MEM_BLOCK_ALL, index,
                          &egr_stu));
    }

    return BCM_E_NONE;
}

/* Helper function to set PORT_TABm STU_PROFILE_INDEXf */
STATIC int
bcmi_gh2_tsn_port_control_ingress_stu_set(
    int unit,
    bcm_gport_t gport,
    int index)
{
    bcm_port_t port;
    sr_port_table_entry_t stu_profile;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }
    /* check whether index is in the valid range */
    SOC_IF_ERROR_RETURN(
        soc_mem_field32_fit(unit, SR_PORT_TABLEm, STU_PROFILE_INDEXf, index));
    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY, port,
                     &stu_profile));
    soc_mem_field32_set(unit, SR_PORT_TABLEm, &stu_profile,
                        STU_PROFILE_INDEXf, index);
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, SR_PORT_TABLEm, MEM_BLOCK_ALL, port,
                      &stu_profile));

    return BCM_E_NONE;
}

/* Helper function to get PORT_TABm STU_PROFILE_INDEXf */
STATIC int
bcmi_gh2_tsn_port_control_ingress_stu_get(
    int unit,
    bcm_gport_t gport,
    int *index)
{
    bcm_port_t port;
    sr_port_table_entry_t stu_profile;

    /* Validate and convert gport to local port */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, gport, &port));

    if (port < 0 || port >= GH2_LOGIC_PORT_MAX_NUM || index == NULL) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SR_PORT_TABLEm, MEM_BLOCK_ANY, port,
                     &stu_profile));
    *index = soc_mem_field32_get(unit, SR_PORT_TABLEm, &stu_profile,
                                 STU_PROFILE_INDEXf);

    return BCM_E_NONE;
}

/* Free and clear the  MTU bookkeeping info structure */
STATIC int
bcmi_gh2_tsn_mtu_cleanup(int unit)
{
    bcm_tsn_mtu_profile_type_t type;

    TSN_BKINFO_IS_INIT(unit);
    for (type = bcmTsnMtuProfileTypeIngress; type < bcmTsnMtuProfileTypeCount;
         type++) {
        if (gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mtu_profile[type] != NULL) {
            soc_profile_mem_destroy(
                unit, gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mtu_profile[type]);
            TSN_FREE(unit,
                    gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mtu_profile[type],
                    0);
            gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mtu_profile[type] = NULL;
        }
    }
    if (gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mutex != NULL) {
        sal_mutex_destroy(gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mutex);
        gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mutex = NULL;
    }

    return BCM_E_NONE;
}

/* Allocate MTU bookkeeping info resource */
STATIC int
bcmi_gh2_tsn_mtu_init(int unit)
{
    bcm_tsn_mtu_profile_type_t type;
    bcm_tsn_mtu_config_t config;
    uint32 idx;
    int rv;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int min, max, i;
    void *entry;
    mtu_profile_entry_t entry_i;
    egr_mtu_profile_entry_t entry_e;
    uint32 buff;
    soc_mem_t mem;
    soc_profile_mem_t *pmem;
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * The mtu config is not used, but to occupy profile memory id 0 as invalid
     */
    config.size = 0;
    config.drop = 0;

    /* Create MTU mutex */
    gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mutex =
        sal_mutex_create("mtu_mutex");
    if (gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mutex == NULL) {
        bcmi_gh2_tsn_mtu_cleanup(unit);
        return BCM_E_MEMORY;
    }

    for (type = bcmTsnMtuProfileTypeIngress; type < bcmTsnMtuProfileTypeCount;
         type++) {
        TSN_ALLOC(
            unit, gh2_tsn_bkinfo[unit]->gh2_mtu_bk_info.mtu_profile[type],
            soc_profile_mem_t, sizeof(soc_profile_mem_t),
            "GH2 TSN MTU Ingress bookkeeping info", 0, rv);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tsn_mtu_cleanup(unit);
            return rv;
        }

        /* Initialize and Create ingress mtu profile memory */
        rv = bcmi_gh2_tsn_mtu_profile_mem_init(unit, type);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tsn_mtu_cleanup(unit);
            return rv;
        }
        /* Profile memory id 0 as default/invalid */
        rv = bcmi_gh2_tsn_mtu_profile_create(unit, type, &config, &idx);
        if (BCM_FAILURE(rv) || idx != 0) {
            bcmi_gh2_tsn_mtu_cleanup(unit);
            return rv;
        }
#if defined(BCM_WARM_BOOT_SUPPORT)
        if (SOC_WARM_BOOT(unit)) {
            pmem = TSN_BKINFO(unit)->gh2_mtu_bk_info.mtu_profile[type];
            mem = gh2_mtu_profile_mem[type];

            if (type == bcmTsnMtuProfileTypeIngress) {
                entry = &entry_i;
            } else {
                entry = &entry_e;
            }
            MTU_BKINFO_LOCK(unit);
            rv = bcmi_gh2_tsn_mtu_profile_mem_idx_get(unit, type, &min, &max);
            if (BCM_FAILURE(rv)) {
                MTU_BKINFO_UNLOCK(unit);
                return rv;
            }
            /* Recovering SW soc_profile_mem management */
            for (i = min; i <= max; i++) {
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry);
                if (SOC_FAILURE(rv)) {
                    MTU_BKINFO_UNLOCK(unit);
                    return rv;
                }
                soc_mem_field_get(unit, mem, (uint32 *)entry, MTU_SIZEf, &buff);
                if (buff > 0) {
                    rv = soc_profile_mem_reference(unit, pmem, i, 1);
                    if (SOC_FAILURE(rv)) {
                        MTU_BKINFO_UNLOCK(unit);
                        return rv;
                    }
                }
            }
            MTU_BKINFO_UNLOCK(unit);
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
     return BCM_E_NONE;
}

/* Free and clear the  STU bookkeeping info structure */
STATIC int
bcmi_gh2_tsn_stu_cleanup(int unit)
{
    bcm_tsn_stu_profile_type_t type;

    TSN_BKINFO_IS_INIT(unit);
    for (type = bcmTsnStuProfileTypeIngress; type < bcmTsnStuProfileTypeCount;
         type++) {
        if (gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.stu_profile[type] != NULL) {
            soc_profile_mem_destroy(
                unit, gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.stu_profile[type]);
            TSN_FREE(unit,
                    gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.stu_profile[type],
                    0);
            gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.stu_profile[type] = NULL;
        }
    }
    if (gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.mutex != NULL) {
        sal_mutex_destroy(gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.mutex);
        gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.mutex = NULL;
    }

    return BCM_E_NONE;
}

/* Allocate STU bookkeeping info resource */
STATIC int
bcmi_gh2_tsn_stu_init(int unit)
{
    bcm_tsn_stu_profile_type_t type;
    bcm_tsn_stu_config_t config;
    uint32 idx;
    int rv;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int min, max, i;
    void *entry;
    stu_profile_entry_t entry_i;
    egr_stu_profile_entry_t entry_e;
    uint32 buff;
    soc_mem_t mem;
    soc_profile_mem_t *pmem;
#endif /*BCM_WARM_BOOT_SUPPORT*/


    /*
     * The stu config is not used, but to occupy profile memory id 0 as invalid
     */
    config.size = 0;
    config.drop = 0;

    /* Create STU mutex */
    gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.mutex =
        sal_mutex_create("stu_mutex");
    if (gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.mutex == NULL) {
        bcmi_gh2_tsn_stu_cleanup(unit);
        return BCM_E_MEMORY;
    }

    for (type = bcmTsnStuProfileTypeIngress; type < bcmTsnStuProfileTypeCount;
         type++) {
        TSN_ALLOC(
            unit, gh2_tsn_bkinfo[unit]->gh2_stu_bk_info.stu_profile[type],
            soc_profile_mem_t, sizeof(soc_profile_mem_t),
            "GH2 TSN STU Ingress bookkeeping info", 0, rv);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tsn_stu_cleanup(unit);
            return rv;
        }

        /* Initialize and Create ingress stu profile memory */
        rv = bcmi_gh2_tsn_stu_profile_mem_init(unit, type);
        if (BCM_FAILURE(rv)) {
            bcmi_gh2_tsn_stu_cleanup(unit);
            return rv;
        }

        /* Profile memory id 0 as default/invalid */
        rv = bcmi_gh2_tsn_stu_profile_create(unit, type, &config, &idx);
        if (BCM_FAILURE(rv) || idx != 0) {
            bcmi_gh2_tsn_stu_cleanup(unit);
            return rv;
        }

#if defined(BCM_WARM_BOOT_SUPPORT)
        if (SOC_WARM_BOOT(unit)) {
            pmem = TSN_BKINFO(unit)->gh2_stu_bk_info.stu_profile[type];
            mem = gh2_stu_profile_mem[type];

            if (type == bcmTsnStuProfileTypeIngress) {
                entry = &entry_i;
            } else {
                entry = &entry_e;
            }

            STU_BKINFO_LOCK(unit);
            rv = bcmi_gh2_tsn_stu_profile_mem_idx_get(unit, type, &min, &max);
            if (BCM_FAILURE(rv)) {
                STU_BKINFO_UNLOCK(unit);
                return rv;
            }
            /* Recovering SW soc_profile_mem management */
            for (i = min; i <= max; i++) {
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, entry);
                if (SOC_FAILURE(rv)) {
                    STU_BKINFO_UNLOCK(unit);
                    return rv;
                }
                soc_mem_field_get(unit, mem, (uint32 *)entry, STU_SIZEf, &buff);
                if (buff > 0) {
                    rv = soc_profile_mem_reference(unit, pmem, i, 1);
                    if (SOC_FAILURE(rv)) {
                        STU_BKINFO_UNLOCK(unit);
                        return rv;
                    }
                }
            }
            STU_BKINFO_UNLOCK(unit);
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }
     return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      bcmi_gh2_tsn_egress_mtu_wb_reload
 * Purpose:
 *      Reload the scache data into egr mtu sw data structure
 * Parameters:
 *      unit - (IN) Unit number
 *      idx - (IN) HW index
 *      config - (OUT) pri_map_cofig
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_egress_mtu_wb_reload(
    int unit,
    int idx,
    bcm_tsn_pri_map_config_t *config)
{
    int i, mem_idx, profile_id, mtu_idx;
    egr_mtu_check_entry_t egr_entry;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (idx < 0 || idx >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }

    /* Recovered the SW priority map data structure */
    config->map_type = bcmTsnPriMapTypeEgressMtuProfile;
    config->num_map_entries = EGRESS_MTU_STU_INT_PRI_NUM;
    for (i = 0; i < config->num_map_entries; i++) {
        bcm_tsn_pri_map_entry_t_init(&config->map_entries[i]);
        config->map_entries[i].flags = BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID;
        mem_idx = idx * EGRESS_MTU_STU_INT_PRI_NUM + i;
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EGR_MTU_CHECKm, MEM_BLOCK_ANY, mem_idx,
                          &egr_entry));
        if (SOC_MEM_FIELD_VALID(unit, EGR_MTU_CHECKm,
                                MTU_PROFILE_INDEXf)) {
            mtu_idx = soc_mem_field32_get(unit, EGR_MTU_CHECKm, &egr_entry,
                                     MTU_PROFILE_INDEXf);
        } else {
            return BCM_E_INTERNAL;
        }
        bcmi_gh2_tsn_mtu_profile_encode(
            unit, mtu_idx, bcmTsnMtuProfileTypeEgress, &profile_id);
        config->map_entries[i].profile_id = profile_id;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_tsn_egress_stu_wb_reload
 * Purpose:
 *      Reload the scache data into egr stu sw data structure
 * Parameters:
 *      unit - (IN) Unit number
 *      idx - (IN) HW index
 *      config - (OUT) pri_map_cofig
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_gh2_tsn_egress_stu_wb_reload(
    int unit,
    int idx,
    bcm_tsn_pri_map_config_t *config)
{
    int i, mem_idx, profile_id, stu_idx;
    egr_stu_check_entry_t egr_entry;

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    if (idx < 0 || idx >= GH2_LOGIC_PORT_MAX_NUM) {
        return BCM_E_PARAM;
    }

    /* Recovered the SW priority map data structure */
    config->map_type = bcmTsnPriMapTypeEgressStuProfile;
    config->num_map_entries = EGRESS_MTU_STU_INT_PRI_NUM;
    for (i = 0; i < config->num_map_entries; i++) {
        bcm_tsn_pri_map_entry_t_init(&config->map_entries[i]);
        config->map_entries[i].flags = BCM_TSN_PRI_MAP_ENTRY_PROFILE_ID;
        mem_idx = idx * EGRESS_MTU_STU_INT_PRI_NUM + i;
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EGR_STU_CHECKm, MEM_BLOCK_ANY, mem_idx,
                          &egr_entry));
        if (SOC_MEM_FIELD_VALID(unit, EGR_STU_CHECKm,
                                STU_PROFILE_INDEXf)) {
            stu_idx = soc_mem_field32_get(unit, EGR_STU_CHECKm, &egr_entry,
                                     STU_PROFILE_INDEXf);
        } else {
            return BCM_E_INTERNAL;
        }
        bcmi_gh2_tsn_stu_profile_encode(
            unit, stu_idx, bcmTsnStuProfileTypeEgress, &profile_id);
        config->map_entries[i].profile_id = profile_id;
    }
    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TSN_SR_SUPPORT)
/* Per-chip SR MAC Proxy info */
STATIC const tsn_sr_mac_proxy_info_t gh2_sr_mac_proxy_info = {
    SR_MAC_PROXY_PROFILEm, /* soc_mem_t mem */
    SR_PROXY_BITMAPf       /* soc_field_t field */
};

/* SR flow management platform specific data */
STATIC const bcmi_tsn_sr_flows_dev_info_t gh2_tsn_sr_flows_dev_info = {
    bcmi_gh2_tsn_sr_flows_get_num_flows,        /* get_num_flows */
    bcmi_gh2_tsn_sr_flows_get_hw_flow_id,       /* get_hw_flow_id */
    bcmi_gh2_tsn_sr_flows_get_sw_flow_idx,      /* get_sw_flow_idx */
    bcmi_gh2_tsn_sr_flows_check_rx_flow_config, /* check_rx_flow_config */
    bcmi_gh2_tsn_sr_flows_check_tx_flow_config, /* check_tx_flow_config */
    bcmi_gh2_tsn_sr_flows_clear_rx_flow,        /* clear_rx_flow */
    bcmi_gh2_tsn_sr_flows_clear_tx_flow,        /* clear_tx_flow */
    bcmi_gh2_tsn_sr_flows_write_rx_flow,        /* write_rx_flow */
    bcmi_gh2_tsn_sr_flows_write_tx_flow,        /* write_tx_flow */
#ifdef BCM_WARM_BOOT_SUPPORT
    bcmi_gh2_tsn_sr_flows_read_rx_flow,         /* read_rx_flow */
    bcmi_gh2_tsn_sr_flows_read_tx_flow,         /* read_tx_flow */
#endif /* BCM_WARM_BOOT_SUPPORT */
    bcmi_gh2_tsn_sr_flows_get_rx_flow_status,   /* get_rx_flow_status */
    bcmi_gh2_tsn_sr_flows_get_tx_flow_status,   /* get_tx_flow_status */
    bcmi_gh2_tsn_sr_flows_reset_rx_flow,        /* reset_rx_flow */
    bcmi_gh2_tsn_sr_flows_seqnum_slice_alloc,   /* seqnum_slice_alloc */
    bcmi_gh2_tsn_sr_flows_seqnum_slice_free,    /* seqnum_slice_free */
    bcmi_gh2_tsn_sr_flows_seqnum_hist_read,     /* seqnum_hist_read */
    bcmi_gh2_tsn_sr_flows_seqnum_hist_write,    /* seqnum_hist_write */
    bcmi_gh2_tsn_sr_flows_seqnum_hist_reset,    /* seqnum_hist_reset */
    bcmi_gh2_tsn_sr_flows_clear_rx_flow_mtu,    /* clear_rx_flow_mtu */
    bcmi_gh2_tsn_sr_flows_clear_tx_flow_mtu     /* clear_tx_flow_mtu */
};

/* TSN-SR Port config set/get */
STATIC const tsn_sr_port_dev_info_t gh2_sr_port_dev_info = {
#ifdef BCM_WARM_BOOT_SUPPORT
    bcmi_gh2_tsn_sr_port_prp_forwarding_sync,   /* prp_forwarding_sync */
    bcmi_gh2_tsn_sr_port_prp_forwarding_reload, /* prp_forwarding_reload */
#endif /* BCM_WARM_BOOT_SUPPORT */
    bcmi_gh2_tsn_sr_port_config_set,            /* sr_port_config_set */
    bcmi_gh2_tsn_sr_port_config_get             /* sr_port_config_get */
};

#endif /* BCM_TSN_SR_SUPPORT */

/* TSN event management structures */
/* device wrong lan interrupt register/field information */
typedef struct gh2_event_wrong_lan_info_s {
    bcm_tsn_event_type_t event_type;
    soc_field_t enable_status_field;
    soc_reg_t   sts_reg;
    soc_field_t idx_field;
    soc_field_t mul_err_field;
    soc_field_t err_field;
} gh2_event_wrong_lan_info_t;

STATIC const gh2_event_wrong_lan_info_t gh2_event_lan_err[] = {
    {bcmTsnEventTypeSrWrongLanA,
     IFP_WRONG_LAN_A_INTRf,
     IFP_WRONG_LAN_A_STATUS_INTRr, ENTRY_IDXf, MULTIPLE_WRONG_LAN_A_ERRf,
     WRONG_LAN_A_ERRf},
    {bcmTsnEventTypeSrWrongLanB,
     IFP_WRONG_LAN_B_INTRf,
     IFP_WRONG_LAN_B_STATUS_INTRr, ENTRY_IDXf, MULTIPLE_WRONG_LAN_B_ERRf,
     WRONG_LAN_B_ERRf}
};

/* device counter threshold register/field information */
typedef struct gh2_event_cnt_threshold_s {
    soc_field_t sts_field;
    bcm_tsn_stat_t  stat_type;
    bcm_tsn_stat_threshold_source_t th_src;
} gh2_event_cnt_threshold_t;

/* Information from IP2_SR_TH_INTR_STATUSr */
STATIC const gh2_event_cnt_threshold_t gh2_event_cnt_thr_ip[] = {
    {SR_FLOW_INTR_RX_TAGGEDf,
     bcmTsnStatIngressSrTaggedPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RXf,
     bcmTsnStatIngressNonLinkLocalPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_WRONG_LANf,
     bcmTsnStatIngressSrWrongLanPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_ERRORf,
     bcmTsnStatIngressSrRxErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_TAG_ERRORf,
     bcmTsnStatIngressSrTagErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_DUPLICATESf,
     bcmTsnStatIngressSrDuplicatePackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_OUTOFORDERf,
     bcmTsnStatIngressSrOutOfOrderSrPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_OWN_RXf,
     bcmTsnStatIngressSrOwnRxPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_UNEXPECTED_FRAMEf,
     bcmTsnStatIngressSrUnexpectedPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_MTU_ERRORf,
     bcmTsnStatIngressMtuErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_STU_ERRORf,
     bcmTsnStatIngressStuErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_RX_PASSEDf,
     bcmTsnStatIngressSrAcceptedSrPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_INTR_PROXY_MAC_ERRORf,
     bcmTsnStatIngressSrSaSrcFilteredPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_PORT_INTR_RX_TAGGEDf,
     bcmTsnStatIngressSrTaggedPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RXf,
     bcmTsnStatIngressNonLinkLocalPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_WRONG_LANf,
     bcmTsnStatIngressSrWrongLanPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_ERRORf,
     bcmTsnStatIngressSrRxErrorPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_TAG_ERRORf,
     bcmTsnStatIngressSrTagErrorPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_DUPLICATESf,
     bcmTsnStatIngressSrDuplicatePackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_OUTOFORDERf,
     bcmTsnStatIngressSrOutOfOrderSrPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_OWN_RXf,
     bcmTsnStatIngressSrOwnRxPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_UNEXPECTED_FRAMEf,
     bcmTsnStatIngressSrUnexpectedPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_MTU_ERRORf,
     bcmTsnStatIngressMtuErrorPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_STU_ERRORf,
     bcmTsnStatIngressStuErrorPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_RX_PASSEDf,
     bcmTsnStatIngressSrAcceptedSrPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_INTR_PROXY_MAC_ERRORf,
     bcmTsnStatIngressSrSaSrcFilteredPackets, bcmTsnStatThresholdSourcePort}
};

/* Information from EGR_MISC_INTR_STATUSr */
STATIC const gh2_event_cnt_threshold_t gh2_event_cnt_thr_ep[] = {
    {SR_FLOW_COUNT_THRESHOLD_TX_TAGGEDf,
     bcmTsnStatEgressSrTaggedPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_COUNT_THRESHOLD_TXf,
     bcmTsnStatEgressNonLinkLocalPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_THRESHOLD_TX_MTU_ERRORf,
     bcmTsnStatEgressMtuErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_FLOW_THRESHOLD_TX_STU_ERRORf,
     bcmTsnStatEgressStuErrorPackets, bcmTsnStatThresholdSourceSRFlow},
    {SR_PORT_COUNT_THRESHOLD_TX_TAGGEDf,
     bcmTsnStatEgressSrTaggedPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_COUNT_THRESHOLD_TXf,
     bcmTsnStatEgressNonLinkLocalPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_THRESHOLD_TX_MTU_ERRORf,
     bcmTsnStatEgressMtuErrorPackets, bcmTsnStatThresholdSourcePort},
    {SR_PORT_THRESHOLD_TX_STU_ERRORf,
     bcmTsnStatEgressStuErrorPackets, bcmTsnStatThresholdSourcePort}
};

STATIC int
bcmi_gh2_tsn_event_mask(int unit, bcm_tsn_event_type_t event, uint32 enable)
{
    uint64 val;

    if (event == bcmTsnEventTypeSrWrongLanA) {
        BCM_IF_ERROR_RETURN(READ_IP2_INTR_ENABLEr(unit, &val));
        soc_reg64_field32_set(unit, IP2_INTR_ENABLEr, &val,
                              IFP_WRONG_LAN_A_INTRf, enable);
        BCM_IF_ERROR_RETURN(WRITE_IP2_INTR_ENABLEr(unit, val));
    }
    if (event == bcmTsnEventTypeSrWrongLanB) {
        BCM_IF_ERROR_RETURN(READ_IP2_INTR_ENABLEr(unit, &val));
        soc_reg64_field32_set(unit, IP2_INTR_ENABLEr, &val,
                              IFP_WRONG_LAN_B_INTRf, enable);
        BCM_IF_ERROR_RETURN(WRITE_IP2_INTR_ENABLEr(unit, val));
    }
    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META("TSN EVT: event(%d) mask enable = %d\n"),
              event, enable));
    return BCM_E_NONE;
}

/*
 * Update the flow bitmap by specifying the event and hw flow id
 * when event == -1, update the active rx flow bitmap
 */
STATIC int
bcmi_gh2_tsn_event_rx_flow_update(
    int unit,
    int event,
    int idx,
    int val)
{
    int                     rv = BCM_E_NONE, fid, max;
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        return BCM_E_UNAVAIL;
    }
    EVENT_MGMT_LOCK(unit);
    if ((bcm_tsn_event_type_t)event == bcmTsnEventTypeSrWrongLanA) {
        if (val) {
            SHR_BITSET(bkinfo->wrong_lana_fbmp, idx);
        } else {
            SHR_BITCLR(bkinfo->wrong_lana_fbmp, idx);
        }
    } else if ((bcm_tsn_event_type_t)event == bcmTsnEventTypeSrWrongLanB) {
        if (val) {
            SHR_BITSET(bkinfo->wrong_lanb_fbmp, idx);
        } else {
            SHR_BITCLR(bkinfo->wrong_lanb_fbmp, idx);
        }
    } else if ((bcm_tsn_event_type_t)event ==
               bcmTsnEventTypeSrSeqNumWindowAgeOut) {
        if (val) {
            SHR_BITSET(bkinfo->sn_reset_fbmp, idx);
        } else {
            SHR_BITCLR(bkinfo->sn_reset_fbmp, idx);
        }
    } else if (event == -1) {
        /* update the valid rx flow bmp */
        if (val) {
            SHR_BITSET(bkinfo->active_rx_fbmp, idx);
            if (bkinfo->active_rx_fbmp_min == -1) {
                bkinfo->active_rx_fbmp_min = idx;
                bkinfo->active_rx_fbmp_max = idx;
            }
            if (idx < bkinfo->active_rx_fbmp_min) {
                bkinfo->active_rx_fbmp_min = idx;
            }
            if (idx > bkinfo->active_rx_fbmp_max) {
                bkinfo->active_rx_fbmp_max = idx;
            }
        } else {
            SHR_BITCLR(bkinfo->active_rx_fbmp, idx);
            if (SHR_BITNULL_RANGE(bkinfo->active_rx_fbmp, 0,
                                  bkinfo->max_rx_flows)) {
                /* active_rx_fbmp is empty */
                bkinfo->active_rx_fbmp_min = bkinfo->active_rx_fbmp_max = -1;
            } else {
                if (idx == bkinfo->active_rx_fbmp_min) {
                    /* need to get new min */
                    SHR_BIT_ITER(bkinfo->active_rx_fbmp,
                                 bkinfo->active_rx_fbmp_max, fid) {
                        bkinfo->active_rx_fbmp_min = fid;
                        break;
                    }
                }
                if (idx == bkinfo->active_rx_fbmp_max) {
                    max = bkinfo->active_rx_fbmp_max;
                    /* need to get new max */
                    SHR_BIT_ITER(bkinfo->active_rx_fbmp,
                                 bkinfo->active_rx_fbmp_max, fid) {
                        max = fid;
                    }
                    bkinfo->active_rx_fbmp_max = max;
                }
            }
        }
    } else {
        /* don't care the other types for GH2 */
        rv = BCM_E_NONE;
    }
    EVENT_MGMT_UNLOCK(unit);
    return rv;
}

STATIC int
bcmi_gh2_tsn_event_flow_stat_get(
    int unit,
    bcm_tsn_event_type_t event,
    SHR_BITDCL *flowstate,
    SHR_BITDCL *filterflowstate,
    uint32 flow_number)
{
    int             min, max, rv, fidx, min_cd_val, cd_val;
    sr_rx_entry_t   *rx_entries, *sr_rx;
    soc_mem_t       mem;
    soc_field_t     field, cd_field;
    int             rx_flows, min_age_val, age_val;
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    if ((bkinfo->active_rx_fbmp_min == -1) ||
        (bkinfo->active_rx_fbmp_max == -1)) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "No active rx flow.\n")));
        return BCM_E_INTERNAL;
    }
    if (flowstate != NULL && filterflowstate != NULL &&
        (flow_number < bkinfo->max_rx_flows)) {
        return BCM_E_PARAM;
    }
    if ((filterflowstate == NULL) ^ (flowstate == NULL)) {
        return BCM_E_PARAM;
    }
    rx_flows = bkinfo->max_rx_flows;
    /* flowstate could be null when try to update the min count down value */
    EVENT_MGMT_LOCK(unit);
    if (event == bcmTsnEventTypeSrWrongLanA) {
        field = WRONG_LAN_Af;
        cd_field = WRONG_LAN_A_AGE_OUT_COUNT_DOWNf;
    } else if (event == bcmTsnEventTypeSrWrongLanB) {
        field = WRONG_LAN_Bf;
        cd_field = WRONG_LAN_B_AGE_OUT_COUNT_DOWNf;
    } else if (event == bcmTsnEventTypeSrSeqNumWindowAgeOut) {
        field = AGE_OUT_INDICATORf;
        cd_field = SN_AGE_OUT_COUNT_DOWNf;
    } else {
        EVENT_MGMT_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    min = bkinfo->active_rx_fbmp_min;
    max = bkinfo->active_rx_fbmp_max;

    rx_entries = soc_cm_salloc(unit, (max - min + 1) * sizeof(*rx_entries),
                               "rx flow dma");
    if (rx_entries == NULL) {
        EVENT_MGMT_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    mem = SR_RXm;
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, min, max, rx_entries);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, rx_entries);
        EVENT_MGMT_UNLOCK(unit);
        return rv;
    }
    min_cd_val = 0;
    min_age_val = 0;
    for (fidx = min; fidx <= max; fidx++) {
        sr_rx = soc_mem_table_idx_to_pointer(unit, mem, sr_rx_entry_t *,
                                             rx_entries, (fidx - min));
        if (soc_mem_field32_get(unit, mem, sr_rx, field)) {
            if (flowstate != NULL) {
                SHR_BITSET(flowstate, fidx);
            }
        }
        age_val = soc_mem_field32_get(unit, mem, sr_rx, AGE_OUT_MULTIPLIERf);
        if (age_val) {
            if ((min_age_val == 0) || (age_val < min_age_val)) {
                min_age_val = age_val;
            }
        }
        cd_val = soc_mem_field32_get(unit, mem, sr_rx, cd_field);
        /* when count down value > age tick, assign count down value = 0 */
        if (cd_val >= (1 << age_val)) {
            cd_val = 0;
        }
        if (cd_val) {
            if ((min_cd_val == 0) || (cd_val < min_cd_val)) {
                min_cd_val = cd_val;
            }
        }
    }
    soc_cm_sfree(unit, rx_entries);

    if (min_cd_val > (1 << min_age_val)) {
        min_cd_val = (1 << min_age_val) - 1;
    }
    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit, "TSN EVT (%d): Min. count downtick %d\n"),
              event, min_cd_val));

    if (event == bcmTsnEventTypeSrWrongLanA) {
        bkinfo->wrong_lana_min_cd = min_cd_val;
        if (flowstate != NULL) {
            SHR_BITAND_RANGE(flowstate, bkinfo->wrong_lana_fbmp, 0, rx_flows,
                             filterflowstate);
        }
    } else if (event == bcmTsnEventTypeSrWrongLanB) {
        bkinfo->wrong_lanb_min_cd = min_cd_val;
        if (flowstate != NULL) {
            SHR_BITAND_RANGE(flowstate, bkinfo->wrong_lanb_fbmp, 0, rx_flows,
                             filterflowstate);
        }
    } else if (event == bcmTsnEventTypeSrSeqNumWindowAgeOut) {
        bkinfo->sn_reset_min_cd = min_cd_val;
        if (flowstate != NULL) {
            SHR_BITAND_RANGE(flowstate, bkinfo->sn_reset_fbmp, 0, rx_flows,
                             filterflowstate);
        }
    }

    EVENT_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_event_flow_stat_clear(
    int unit,
    bcm_tsn_event_type_t event,
    SHR_BITDCL *flowstate,
    uint32 flow_number)
{
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);
    sr_rx_entry_t   entry;
    soc_mem_t       mem;
    soc_field_t     field;
    int             rx_flows, fidx, rv;

    /* Parameter check */
    if (flowstate == NULL) {
        return BCM_E_PARAM;
    }
    if (flow_number < bkinfo->max_rx_flows) {
        return BCM_E_PARAM;
    }

    rx_flows = bkinfo->max_rx_flows;
    EVENT_MGMT_LOCK(unit);
    if (event == bcmTsnEventTypeSrWrongLanA) {
        field = WRONG_LAN_Af;
    } else if (event == bcmTsnEventTypeSrWrongLanB) {
        field = WRONG_LAN_Bf;
    } else if (event == bcmTsnEventTypeSrSeqNumWindowAgeOut) {
        field = AGE_OUT_INDICATORf;
    } else {
        EVENT_MGMT_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    mem = SR_RXm;
    MEM_LOCK(unit, mem);
    SHR_BIT_ITER(flowstate, rx_flows, fidx) {
        sal_memset(&entry, 0, sizeof(entry));
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, fidx, &entry);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to read mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
        soc_mem_field32_set(unit, mem, &entry, field, 0);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, fidx, &entry);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META_U(unit, "Failed to write mem[%s]"),
                                  SOC_MEM_NAME(unit, mem)));
            MEM_UNLOCK(unit, mem);
            return rv;
        }
    }
    MEM_UNLOCK(unit, mem);
    EVENT_MGMT_UNLOCK(unit);
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_event_poll(int unit)
{
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);
    sal_usecs_t             ctime; /* current time */
    int                     diff, dtick = 0;

    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        /* Do nothing */
        return BCM_E_NONE;
    }

    if ((bkinfo->ageout_tick_ms == 0) || (bkinfo->age_enable == 0)) {
        return BCM_E_NONE;
    }

    /* Check any active rx flow */
    if ((bkinfo->active_rx_fbmp_min == -1) ||
        (bkinfo->active_rx_fbmp_max == -1)) {
        /* Do nothing */
        return BCM_E_NONE;
    }

    EVENT_MGMT_LOCK(unit);

    ctime = sal_time_usecs();
    if (bkinfo->ptime != 0) {
        diff = SAL_USECS_SUB(ctime, bkinfo->ptime);
        bkinfo->ptime = ctime;
        dtick = diff / (bkinfo->ageout_tick_ms * MILLISECOND_USEC);
    } else {
        bkinfo->ptime = ctime;
    }

    if ((bkinfo->wla_mask == 0) && (bkinfo->wla_tick == 0)) {
        /*
         * Get the updated min count down value
         * when interrupt mask is disabled and tick count down to 0.
         */
        (void)bcmi_gh2_tsn_event_flow_stat_get(
                unit, bcmTsnEventTypeSrWrongLanA, NULL, NULL, 0);
        bkinfo->wla_tick = bkinfo->wrong_lana_min_cd;
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "TSN EVT: WrongLanA tick %d\n"),
                  bkinfo->wla_tick));
    }
    bkinfo->wla_tick -= dtick;

    if ((bkinfo->wlb_mask == 0) && (bkinfo->wlb_tick == 0)) {
        /*
         * Get the updated min count down value
         * when interrupt mask is disabled and tick count down to 0.
         */
        (void)bcmi_gh2_tsn_event_flow_stat_get(
                unit, bcmTsnEventTypeSrWrongLanB, NULL, NULL, 0);
        bkinfo->wlb_tick = bkinfo->wrong_lanb_min_cd;
        LOG_DEBUG(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit, "TSN EVT: WrongLanB tick %d\n"),
                  bkinfo->wlb_tick));
    }
    bkinfo->wlb_tick -= dtick;

    if (bkinfo->window_reset_mode ==
        BCM_TSN_CONTROL_SR_SEQNUM_WINDOW_RESET_MODE_SW) {
        if (bkinfo->sn_reset_min_cd == 0) {
            /* Update sn_reset_min_cd */
            (void)bcmi_gh2_tsn_event_flow_stat_get(
                unit, bcmTsnEventTypeSrSeqNumWindowAgeOut, NULL, NULL, 0);
        }
        if (bkinfo->sn_reset_min_cd > 0) {
            if (bkinfo->reset_tick == 0) {
                bkinfo->reset_tick = bkinfo->sn_reset_min_cd;
                LOG_DEBUG(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit, "TSN EVT: sn reset tick %d\n"),
                          bkinfo->sn_reset_min_cd));
            }
            bkinfo->reset_tick -= dtick;

            /* notify callback to check the age-out indicator */
            if (bkinfo->reset_tick <= 0) {
                if (bkinfo->gh2_flow_event_notify) {
                    bkinfo->gh2_flow_event_notify(
                        unit, bcmTsnEventTypeSrSeqNumWindowAgeOut, 1, 0);
                }
                bkinfo->reset_tick = 0;
            }
        }
    }
    if (!SHR_BITNULL_RANGE(bkinfo->wrong_lana_fbmp, 0, bkinfo->max_rx_flows)) {
        /* check the wrong LanA ageout ticks to enable the interrupt mask */
        if (bkinfo->wla_tick <= 0) {
            if (bkinfo->wla_mask == 0) {
                (void)bcmi_gh2_tsn_event_mask(unit,
                                              bcmTsnEventTypeSrWrongLanA, 1);
                bkinfo->wla_mask = 1;
            }
            bkinfo->wla_tick = 0;
        }
    }
    if (!SHR_BITNULL_RANGE(bkinfo->wrong_lanb_fbmp, 0, bkinfo->max_rx_flows)) {
        /* check the wrong LanB ageout ticks to enable the interrupt mask */
        if (bkinfo->wlb_tick <= 0) {
            if (bkinfo->wlb_mask == 0) {
                (void)bcmi_gh2_tsn_event_mask(unit,
                                              bcmTsnEventTypeSrWrongLanB, 1);
                bkinfo->wlb_mask = 1;
            }
            bkinfo->wlb_tick = 0;
        }
    }

    EVENT_MGMT_UNLOCK(unit);

    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_flow_event_notify_register(
    int unit,
    bcmi_esw_tsn_flow_event_notify_cb cb)
{
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    bkinfo->gh2_flow_event_notify = cb;
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_cnt_excceed_notify_register(
    int unit,
    bcmi_esw_tsn_cnt_exceed_notify_cb cb)
{
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    bkinfo->gh2_cnt_exceed_notify = cb;
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_event_interrupt_handler(
    int unit,
    uint8 lan_err,
    uint8 ip_cnt_err,
    uint8 ep_cnt_err)
{
    const gh2_event_wrong_lan_info_t    *lan_err_info;
    const gh2_event_cnt_threshold_t     *cnt_th_info;
    uint64                              rval64, clear_field_64;
    uint32                              rval32, multiple, clear_field;
    soc_reg_t                           sts_reg;
    bcmi_esw_tsn_flow_event_notify_cb   fe_cb = NULL;
    bcmi_esw_tsn_cnt_exceed_notify_cb   ce_cb = NULL;
    int                                 idx, i;
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    if (bkinfo->gh2_flow_event_notify) {
        fe_cb = bkinfo->gh2_flow_event_notify;
    }
    if (bkinfo->gh2_cnt_exceed_notify) {
        ce_cb = bkinfo->gh2_cnt_exceed_notify;
    }

    if (ip_cnt_err) {
        multiple = 1;
        idx = -1;
        BCM_IF_ERROR_RETURN(
            soc_reg_get(unit, IP2_SR_TH_INTR_STATUSr, REG_PORT_ANY, 0,
                        &rval64));
        COMPILER_64_COPY(clear_field_64, rval64);

        for (i = 0; i < COUNTOF(gh2_event_cnt_thr_ip); i++) {
            cnt_th_info = &gh2_event_cnt_thr_ip[i];
            if (soc_reg64_field32_get(unit, IP2_SR_TH_INTR_STATUSr, rval64,
                                      cnt_th_info->sts_field)) {
                if (ce_cb) {
                    (void)ce_cb(unit, cnt_th_info->stat_type,
                                cnt_th_info->th_src, multiple, idx);
                }
                soc_reg64_field32_set(unit, IP2_SR_TH_INTR_STATUSr,
                                      &clear_field_64,
                                      cnt_th_info->sts_field, 0);
            }
        }
        /* Clear status */
        BCM_IF_ERROR_RETURN(
            soc_reg_set(unit, IP2_SR_TH_INTR_STATUSr, REG_PORT_ANY, 0,
                        clear_field_64));
    }
    if (ep_cnt_err) {
        multiple = 1;
        idx = -1;
        BCM_IF_ERROR_RETURN(READ_EGR_MISC_INTR_STATUSr(unit, &rval32));
        clear_field = rval32;
        for (i = 0; i < COUNTOF(gh2_event_cnt_thr_ep); i++) {
            cnt_th_info = &gh2_event_cnt_thr_ep[i];
            if (soc_reg_field_get(unit, EGR_MISC_INTR_STATUSr, rval32,
                                  cnt_th_info->sts_field)) {
                if (ce_cb) {
                    (void)ce_cb(unit, cnt_th_info->stat_type,
                                cnt_th_info->th_src, multiple, idx);
                }
                soc_reg_field_set(unit, EGR_MISC_INTR_STATUSr, &clear_field,
                                  cnt_th_info->sts_field, 0);
            }
        }
        /* Clear status */
        BCM_IF_ERROR_RETURN(WRITE_EGR_MISC_INTR_STATUSr(unit, clear_field));
    }
    if (lan_err) {
        /* which lan error */
        idx = -1;
        BCM_IF_ERROR_RETURN(READ_IP2_INTR_STATUSr(unit, &rval64));
        for (i = 0; i < COUNTOF(gh2_event_lan_err); i++) {
            lan_err_info = &gh2_event_lan_err[i];
            if (soc_reg64_field32_get(unit, IP2_INTR_STATUSr, rval64,
                                    lan_err_info->enable_status_field)) {
                sts_reg = lan_err_info->sts_reg;
                BCM_IF_ERROR_RETURN(
                    soc_reg32_get(unit, sts_reg,
                                  REG_PORT_ANY, 0, &rval32));
                if (soc_reg_field_get(unit, sts_reg, rval32,
                                      lan_err_info->err_field)) {
                    multiple = soc_reg_field_get(unit, sts_reg, rval32,
                                                 lan_err_info->mul_err_field);
                    idx = soc_reg_field_get(unit, sts_reg, rval32,
                                            lan_err_info->idx_field);

                    if (fe_cb) {
                        (void)fe_cb(unit, lan_err_info->event_type,
                                    multiple, idx);
                    }
                    /*
                     * Disable the event mask to avoid
                     * continuous wrong lan interrupt
                     */
                    if (bkinfo->age_enable) {
                        (void)bcmi_gh2_tsn_event_mask(unit,
                                                      lan_err_info->event_type,
                                                      0);
                        if (lan_err_info->event_type ==
                            bcmTsnEventTypeSrWrongLanA) {
                            bkinfo->wla_mask = 0;
                        }
                        if (lan_err_info->event_type ==
                            bcmTsnEventTypeSrWrongLanB) {
                            bkinfo->wlb_mask = 0;
                        }

                    }

                    /* Clear status */
                    BCM_IF_ERROR_RETURN(
                        soc_reg32_set(unit, sts_reg, REG_PORT_ANY, 0, 0));
                }
            }
        }
    }
    return BCM_E_NONE;
}

/* Check if the device supports tsn event or not */
STATIC int
bcmi_gh2_tsn_event_validate(int unit, bcmi_tsn_evt_inst_type_t inst_type)
{
    switch (inst_type) {
        case bcmiTsnEvtCESys:
        case bcmiTsnEvtCEPort:
            return BCM_E_NONE;
        case bcmiTsnEvtCEFlow:
        case bcmiTsnEvtWLASys:
        case bcmiTsnEvtWLAFlow:
        case bcmiTsnEvtWLBSys:
        case bcmiTsnEvtWLBFlow:
        case bcmiTsnEvtSNSys:
        case bcmiTsnEvtSNFlow:
            if (!soc_feature(unit, soc_feature_tsn_sr)) {
                return BCM_E_UNAVAIL;
            }
            return BCM_E_NONE;
        default :
            return BCM_E_UNAVAIL;
    }
}

STATIC int
bcmi_gh2_tsn_event_mgmt_cleanup(int unit)
{
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    /* Free resource */
    if (bkinfo->active_rx_fbmp != NULL) {
        sal_free(bkinfo->active_rx_fbmp);
        bkinfo->active_rx_fbmp = NULL;
    }
    if (bkinfo->wrong_lana_fbmp != NULL) {
        sal_free(bkinfo->wrong_lana_fbmp);
        bkinfo->wrong_lana_fbmp = NULL;
    }
    if (bkinfo->wrong_lanb_fbmp != NULL) {
        sal_free(bkinfo->wrong_lanb_fbmp);
        bkinfo->wrong_lanb_fbmp = NULL;
    }
    if (bkinfo->sn_reset_fbmp != NULL) {
        sal_free(bkinfo->sn_reset_fbmp);
        bkinfo->sn_reset_fbmp = NULL;
    }

    if (bkinfo->event_mutex != NULL) {
        sal_mutex_destroy(bkinfo->event_mutex);
        bkinfo->event_mutex = NULL;
    }

    /* Unregister event handler */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        soc_fl_tsn_event_handler_register(unit, NULL);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        soc_gh2_tsn_event_handler_register(unit, NULL);
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_tsn_event_mgmt_init(int unit)
{
#if defined(BCM_TSN_SR_SUPPORT)
    int rx_flows, tx_flows;
#endif /* BCM_TSN_SR_SUPPORT */
    gh2_tsn_events_bkinfo_t *bkinfo = EVENTS_BKINFO(unit);

    sal_memset(bkinfo, 0, sizeof(gh2_tsn_events_bkinfo_t));
#if defined(BCM_TSN_SR_SUPPORT)
    /* Allocate resource */
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* Get # of flows and create the flow bmp */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_tsn_sr_flows_get_num_flows(unit, &rx_flows, &tx_flows));
        bkinfo->max_rx_flows = rx_flows;
        /* Allocate flows bitmap */
        bkinfo->active_rx_fbmp = (SHR_BITDCL *)
            sal_alloc(SHR_BITALLOCSIZE(rx_flows), "gh2 event rx flow bmp");
        if (bkinfo->active_rx_fbmp == NULL) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("EVENT: failed to allocate flow bmp\n")));
            (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->active_rx_fbmp, 0, SHR_BITALLOCSIZE(rx_flows));
        bkinfo->active_rx_fbmp_min = -1;
        bkinfo->active_rx_fbmp_max = -1;

        bkinfo->wrong_lana_fbmp = (SHR_BITDCL *)
            sal_alloc(SHR_BITALLOCSIZE(rx_flows),
                      "gh2 event wrong lan flow bmp");
        if (bkinfo->wrong_lana_fbmp == NULL) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("EVENT: failed to allocate wrong lan fbmp\n")));
            (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->wrong_lana_fbmp, 0, SHR_BITALLOCSIZE(rx_flows));
        bkinfo->wrong_lanb_fbmp = (SHR_BITDCL *)
            sal_alloc(SHR_BITALLOCSIZE(rx_flows),
                      "gh2 event wrong lan flow bmp");
        if (bkinfo->wrong_lanb_fbmp == NULL) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("EVENT: failed to allocate wrong lan fbmp\n")));
            (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->wrong_lanb_fbmp, 0, SHR_BITALLOCSIZE(rx_flows));
        bkinfo->sn_reset_fbmp = (SHR_BITDCL *)
            sal_alloc(SHR_BITALLOCSIZE(rx_flows),
                      "gh2 event sn reset flow bmp");
        if (bkinfo->sn_reset_fbmp == NULL) {
            LOG_ERROR(BSL_LS_BCM_TSN,
                      (BSL_META("EVENT: failed to allocate sn reset fbmp\n")));
            (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(bkinfo->sn_reset_fbmp, 0, SHR_BITALLOCSIZE(rx_flows));
        /* Get the inital value of the window reset mode */
        (void)bcmi_gh2_tsn_control_get(unit,
                                       bcmTsnControlSrSeqNumWindowResetMode,
                                       &bkinfo->window_reset_mode);
        (void)bcmi_gh2_tsn_control_get(unit, bcmTsnControlSrAgeOutEnable,
                                       &bkinfo->age_enable);
        (void)bcmi_gh2_tsn_control_get(unit, bcmTsnControlSrAgeTickInterval,
                                       &bkinfo->ageout_tick_ms);
    }
#endif

    /* Create mutex for protecting event management */
    bkinfo->event_mutex = sal_mutex_create("gh2 event mgmt");
    if (NULL == bkinfo->event_mutex) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META("EVENT: failed to create gh2 mgmt mutex\n")));
        (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
        return BCM_E_MEMORY;
    }

    /* Register the event interrupt handler */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        soc_fl_tsn_event_handler_register(unit,
                                       bcmi_gh2_tsn_event_interrupt_handler);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        soc_gh2_tsn_event_handler_register(unit,
                                       bcmi_gh2_tsn_event_interrupt_handler);
    }

    return BCM_E_NONE;
}

/* TSN event management platform specific data */
STATIC const bcmi_tsn_events_dev_info_t gh2_tsn_events_dev_info = {
    bcmi_gh2_tsn_flow_event_notify_register,    /* register_flow_event_cb */
    bcmi_gh2_tsn_cnt_excceed_notify_register,   /* register_cnt_exceed_cb */
    bcmi_gh2_tsn_event_validate,                /* event validate */
    bcmi_gh2_tsn_event_mask,                    /* Enable/disable event mask */
    bcmi_gh2_tsn_event_rx_flow_update,          /* update_rx_flow */
    bcmi_gh2_tsn_event_flow_stat_get,           /* Get the flow event state */
    bcmi_gh2_tsn_event_flow_stat_clear,         /* Clear the flow event state */
    bcmi_gh2_tsn_event_poll                     /* poll function */
};

/* MTU platform specific data */
STATIC const tsn_mtu_info_t gh2_mtu_ctrl_info = {
    /* per-chip control drivers */
#ifdef BCM_WARM_BOOT_SUPPORT
    bcmi_gh2_tsn_egress_mtu_wb_reload,
#endif /* BCM_WARM_BOOT_SUPPORT */
    GH2_LOGIC_PORT_MAX_NUM,
    bcmi_gh2_tsn_mtu_profile_mem_idx_get,
    bcmi_gh2_tsn_mtu_profile_create,
    bcmi_gh2_tsn_mtu_profile_set,
    bcmi_gh2_tsn_mtu_profile_get,
    bcmi_gh2_tsn_mtu_profile_destroy,
    bcmi_gh2_tsn_ingress_mtu_config_set,
    bcmi_gh2_tsn_ingress_mtu_config_get,
    bcmi_gh2_tsn_port_control_egress_mtu_set,
    bcmi_gh2_tsn_mtu_profile_decode,
    bcmi_gh2_tsn_mtu_profile_encode,
    bcmi_gh2_tsn_mtu_profile_traverse,
    bcmi_gh2_tsn_mtu_profmem_refcnt_get,
    bcmi_gh2_tsn_port_control_ingress_mtu_set,
    bcmi_gh2_tsn_port_control_ingress_mtu_get
};

/* STU platform specific data */
STATIC const tsn_stu_info_t gh2_stu_ctrl_info = {
    /* per-chip control drivers */
#ifdef BCM_WARM_BOOT_SUPPORT
    bcmi_gh2_tsn_egress_stu_wb_reload,
#endif /* BCM_WARM_BOOT_SUPPORT */
    GH2_LOGIC_PORT_MAX_NUM,
    bcmi_gh2_tsn_stu_profile_mem_idx_get,
    bcmi_gh2_tsn_stu_profile_create,
    bcmi_gh2_tsn_stu_profile_set,
    bcmi_gh2_tsn_stu_profile_get,
    bcmi_gh2_tsn_stu_profile_destroy,
    bcmi_gh2_tsn_ingress_stu_config_set,
    bcmi_gh2_tsn_ingress_stu_config_get,
    bcmi_gh2_tsn_port_control_egress_stu_set,
    bcmi_gh2_tsn_stu_profile_decode,
    bcmi_gh2_tsn_stu_profile_encode,
    bcmi_gh2_tsn_stu_profile_traverse,
    bcmi_gh2_tsn_stu_profmem_refcnt_get,
    bcmi_gh2_tsn_port_control_ingress_stu_set,
    bcmi_gh2_tsn_port_control_ingress_stu_get
};

/* TSN flow management platform specific data */
STATIC const bcmi_tsn_flows_dev_info_t gh2_tsn_flows_dev_info = {
    bcmi_gh2_tsn_flows_get_num_flows,       /* get_num_flows */
    bcmi_gh2_tsn_flows_get_hw_flow_id,      /* get_hw_flow_id */
    bcmi_gh2_tsn_flows_get_sw_flow_idx,     /* get_sw_flow_idx */
    bcmi_gh2_tsn_flows_check_flow_config,   /* check_flow_config */
    bcmi_gh2_tsn_flows_write_flow,          /* write_flow */
#ifdef BCM_WARM_BOOT_SUPPORT
    bcmi_gh2_tsn_flows_read_flow,           /* read_flow */
#endif /* BCM_WARM_BOOT_SUPPORT */
    bcmi_gh2_tsn_flows_clear_flow,          /* clear_flow */
    bcmi_gh2_tsn_flows_clear_flow_mtu       /* clear_flow_mtu */
};

/* per-chip TSN Priority Map driver set */
STATIC const tsn_pri_map_info_t bcmi_gh2_tsn_pri_map_info = {
    "gh2_tsn_priority_map",                /* table_name */
    GH2_PRI_MAP_TSN_TAB_SIZE,              /* table_size */
    GH2_PRI_MAP_TSN_ENT_SIZE,              /* entry_size */
    bcmi_gh2_tsn_pri_map_tsn_set,          /* tsn_pri_map_set */
    bcmi_gh2_tsn_pri_map_tsn_get,          /* tsn_pri_map_get */
    bcmi_gh2_tsn_pri_map_tsn_delete,       /* tsn_pri_map_delete */
    bcmi_gh2_tsn_pri_map_tsn_hw_index_get, /* tsn_pri_map_hw_index_get */
    bcmi_gh2_tsn_pri_map_tsn_sw_idx_get,   /* tsn_pri_map_sw_idx_get */
    bcmi_gh2_tsn_pri_map_tsn_wb_hw_existed /* tsn_pri_map_wb_hw_existed */
};

#if defined(BCM_TSN_SR_SUPPORT)
/* per-chip SR Priority Map driver set */
STATIC const tsn_pri_map_info_t bcmi_gh2_sr_pri_map_info = {
    "gh2_tsn_priority_map",                /* table_name */
    GH2_PRI_MAP_SR_TAB_SIZE,               /* table_size */
    GH2_PRI_MAP_SR_ENT_SIZE,               /* entry_size */
    bcmi_gh2_tsn_pri_map_sr_set,           /* tsn_pri_map_set */
    bcmi_gh2_tsn_pri_map_sr_get,           /* tsn_pri_map_get */
    bcmi_gh2_tsn_pri_map_sr_delete,        /* tsn_pri_map_delete */
    bcmi_gh2_tsn_pri_map_sr_hw_index_get,  /* tsn_pri_map_hw_index_get */
    bcmi_gh2_tsn_pri_map_sr_sw_idx_get,    /* tsn_pri_map_sw_idx_get */
    bcmi_gh2_tsn_pri_map_sr_wb_hw_existed  /* tsn_pri_map_wb_hw_existed */
};

/* Per-chip SR Auto Learn driver set */
STATIC const tsn_sr_auto_learn_info_t gh2_sr_auto_learn_info = {
    "gh2_tsn_sr_auto_learn_platform_info",        /* table_name */
    bcmi_gh2_tsn_sr_auto_learn_table_size_get,    /* table_size */
    {   /* max_flow_num[bcmi_sr_auto_learn_flow_count] */
        GH2_SR_AUTO_LEARN_TX_FLOW_SIZE,
        GH2_SR_AUTO_LEARN_RX_FLOW_SIZE
    },
    bcmi_gh2_tsn_sr_auto_learn_group_port_set,    /* group_port_set */
    bcmi_gh2_tsn_sr_auto_learn_group_port_delete, /* group_port_delete */
    bcmi_gh2_tsn_sr_auto_learn_group_port_reset_all, /* group_port_reset_all */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_set,  /* flow_id_ctrl_set */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_get,  /* flow_id_ctrl_get */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_set,  /* flow_id_pool_set */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_get,  /* flow_id_pool_get */
    bcmi_gh2_tsn_sr_auto_learn_enable,            /* enable */
    bcmi_gh2_tsn_sr_auto_learn_enable_get         /* enable_get */
};

#ifdef BCM_FIRELIGHT_SUPPORT
STATIC const tsn_sr_auto_learn_info_t fl_sr_auto_learn_info = {
    "gh2_tsn_sr_auto_learn_platform_info",        /* table_name */
    bcmi_gh2_tsn_sr_auto_learn_table_size_get,    /* table_size */
    {   /* max_flow_num[bcmi_sr_auto_learn_flow_count] */
        FL_SR_AUTO_LEARN_TX_FLOW_SIZE,
        FL_SR_AUTO_LEARN_RX_FLOW_SIZE
    },
    bcmi_gh2_tsn_sr_auto_learn_group_port_set,    /* group_port_set */
    bcmi_gh2_tsn_sr_auto_learn_group_port_delete, /* group_port_delete */
    bcmi_gh2_tsn_sr_auto_learn_group_port_reset_all, /* group_port_reset_all */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_set,  /* flow_id_ctrl_set */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_ctrl_get,  /* flow_id_ctrl_get */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_set,  /* flow_id_pool_set */
    bcmi_gh2_tsn_sr_auto_learn_flow_id_pool_get,  /* flow_id_pool_get */
    bcmi_gh2_tsn_sr_auto_learn_enable,            /* enable */
    bcmi_gh2_tsn_sr_auto_learn_enable_get         /* enable_get */
};
#endif /* BCM_FIRELIGHT_SUPPORT */

#endif /* BCM_TSN_SR_SUPPORT */

/* per-chip MTU Priority Map driver set */
STATIC const tsn_pri_map_info_t bcmi_gh2_mtu_pri_map_info = {
    "gh2_mtu_priority_map",                /* table_name */
    GH2_PRI_MAP_MTU_TAB_SIZE,              /* table_size */
    GH2_PRI_MAP_MTU_ENT_SIZE,              /* entry_size */
    NULL,                                  /* tsn_pri_map_set */
    NULL,                                  /* tsn_pri_map_get */
    NULL,                                  /* tsn_pri_map_delete */
    bcmi_gh2_tsn_pri_map_mtu_hw_index_get, /* tsn_pri_map_hw_index_get */
    bcmi_gh2_tsn_pri_map_mtu_sw_idx_get,   /* tsn_pri_map_sw_idx_get */
    NULL                                   /* tsn_pri_map_wb_hw_existed */
};

/* per-chip STU Priority Map driver set */
STATIC const tsn_pri_map_info_t bcmi_gh2_stu_pri_map_info = {
    "gh2_stu_priority_map",                /* table_name */
    GH2_PRI_MAP_STU_TAB_SIZE,              /* table_size */
    GH2_PRI_MAP_STU_ENT_SIZE,              /* entry_size */
    NULL,                                  /* tsn_pri_map_set */
    NULL,                                  /* tsn_pri_map_get */
    NULL,                                  /* tsn_pri_map_delete */
    bcmi_gh2_tsn_pri_map_stu_hw_index_get, /* tsn_pri_map_hw_index_get */
    bcmi_gh2_tsn_pri_map_stu_sw_idx_get,   /* tsn_pri_map_sw_idx_get */
    NULL                                   /* tsn_pri_map_wb_hw_existed */
};

/* Per-chip control driver set */
STATIC const tsn_chip_ctrl_info_t gh2_chip_ctrl_info = {
    /* per-chip control drivers */
    bcmi_gh2_tsn_control_set,
    bcmi_gh2_tsn_control_get
};

/* GH2 TSN device info */
STATIC const bcmi_esw_tsn_dev_info_t gh2_tsn_devinfo = {
#if defined(BCM_TSN_SR_SUPPORT)
    &gh2_sr_mac_proxy_info,
    &gh2_tsn_sr_flows_dev_info,
    &gh2_sr_port_dev_info,
    &gh2_sr_auto_learn_info,
#endif /* BCM_TSN_SR_SUPPORT */
    &gh2_tsn_flows_dev_info,

    {   /* tsn_pri_map_info[bcmTsnPriMapTypeCount] */
        &bcmi_gh2_tsn_pri_map_info,
#if defined(BCM_TSN_SR_SUPPORT)
        &bcmi_gh2_sr_pri_map_info,
#else
        NULL,
#endif
        &bcmi_gh2_mtu_pri_map_info,
        &bcmi_gh2_stu_pri_map_info,
        &bcmi_taf_gate_pri_map_info
    },
    &gh2_mtu_ctrl_info,
    &gh2_stu_ctrl_info,
    &gh2_tsn_events_dev_info,
    &gh2_chip_ctrl_info
};

#ifdef BCM_FIRELIGHT_SUPPORT
STATIC const bcmi_esw_tsn_dev_info_t fl_tsn_devinfo = {
#if defined(BCM_TSN_SR_SUPPORT)
    &gh2_sr_mac_proxy_info,
    &gh2_tsn_sr_flows_dev_info,
    &gh2_sr_port_dev_info,
    &fl_sr_auto_learn_info,
#endif /* BCM_TSN_SR_SUPPORT */
    &gh2_tsn_flows_dev_info,

    {   /* tsn_pri_map_info[bcmTsnPriMapTypeCount] */
        &bcmi_gh2_tsn_pri_map_info,
#if defined(BCM_TSN_SR_SUPPORT)
        &bcmi_gh2_sr_pri_map_info,
#else
        NULL,
#endif
        &bcmi_gh2_mtu_pri_map_info,
        &bcmi_gh2_stu_pri_map_info,
        &bcmi_taf_gate_pri_map_info
    },
    &gh2_mtu_ctrl_info,
    &gh2_stu_ctrl_info,
    &gh2_tsn_events_dev_info,
    &gh2_chip_ctrl_info
};
#endif /* end BCM_FIRELIGHT_SUPPORT */
/*
 * Function:
 *     bcmi_gh2_bkinfo_cleanup
 * Purpose:
 *     Clear bookkeeping.
 * Parameters:
 *     unit             - (IN) unit number
 *     init_error       - (IN) 1 : init gh2_bkinfo failed so need cleanup
 *                             0 : others
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_bkinfo_cleanup(int unit, int init_error)
{
    (void)bcmi_gh2_tsn_event_mgmt_cleanup(unit);
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* SR flow management clean up */
        SRFLOWS_BKINFO(unit).max_flows_rx = 0;
        SRFLOWS_BKINFO(unit).max_flows_tx = 0;
        (void)bcmi_gh2_tsn_sr_flows_seqnum_slices_cleanup(unit);
        (void)bcmi_gh2_tsn_sr_port_prp_forwarding_cleanup(unit, init_error);
    }
#endif /* BCM_TSN_SR_SUPPORT */

    /* Free and clear the  MTU & STU bookkeeping info structure */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        (void)bcmi_gh2_tsn_mtu_cleanup(unit);
        (void)bcmi_gh2_tsn_stu_cleanup(unit);
    }

    /* Free and clear the bookkeeping info structure */
    if (gh2_tsn_bkinfo[unit] != NULL) {
        TSN_FREE(unit, gh2_tsn_bkinfo[unit], 0);
        gh2_tsn_bkinfo[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_info_init
 * Description:
 *     Setup the chip specific info.
 * Parameters:
 *     unit    - (IN) Unit number.
 *     devinfo - (OUT) device info structure.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_tsn_info_init(
    int unit,
    const bcmi_esw_tsn_dev_info_t **devinfo)
{
    gh2_tsn_bkinfo_t *bkinfo = NULL;
    int rv = BCM_E_NONE;
    int i;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (devinfo == NULL) {
        return BCM_E_PARAM;
    }

    /* GH2 TSN bookkeeping info initialization */
    if (!gh2_tsn_bkinfo_initialized) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            gh2_tsn_bkinfo[i] = NULL;
        }
        gh2_tsn_bkinfo_initialized = 1;
    }

    /* Clean up the resource */
    if (gh2_tsn_bkinfo[unit] != NULL) {
        rv = bcmi_gh2_bkinfo_cleanup(unit, 0);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Allocate bookkeeping info resource */
    /* coverity[bad_memset] */
    TSN_ALLOC(unit, bkinfo, gh2_tsn_bkinfo_t,
              sizeof(gh2_tsn_bkinfo_t),
              "GH2 TSN bookkeeping info", 0, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    gh2_tsn_bkinfo[unit] = bkinfo;

    /* Allocate MTU & STU bookkeeping info resource */
    if (soc_feature(unit, soc_feature_tsn_mtu_stu)) {
        rv = bcmi_gh2_tsn_mtu_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_gh2_bkinfo_cleanup(unit, 1);
            return rv;
        }

        rv = bcmi_gh2_tsn_stu_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_gh2_bkinfo_cleanup(unit, 1);
            return rv;
        }
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        /* SR flow management - RX seqnum slice management initialization */
        rv = bcmi_gh2_tsn_sr_flows_seqnum_slices_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_gh2_bkinfo_cleanup(unit, 1);
            return rv;
        }

        /* SR PRP port forwarding initialization */
        rv = bcmi_gh2_tsn_sr_port_prp_forwarding_init(unit);
        if (BCM_FAILURE(rv)) {
            (void)bcmi_gh2_bkinfo_cleanup(unit, 1);
            return rv;
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    rv = bcmi_gh2_tsn_event_mgmt_init(unit);
    if (BCM_FAILURE(rv)) {
        (void)bcmi_gh2_bkinfo_cleanup(unit, 1);
        return rv;
    }
    /* Return the chip info structure */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        *devinfo = &fl_tsn_devinfo;
    } else
#endif /* end BCM_FIRELIGHT_SUPPORT */
    {
        *devinfo = &gh2_tsn_devinfo;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_noninit_dev_info_get
 * Description:
 *     Get the chip specific info without TSN module initialized.
 * Parameters:
 *     unit    - (IN) Unit number.
 *     devinfo - (OUT) device info structure.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_tsn_noninit_dev_info_get(
    int unit,
    const bcmi_esw_tsn_dev_info_t **devinfo)
{
    if (devinfo == NULL) {
        return (BCM_E_PARAM);
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Return the chip info structure */
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        *devinfo = &fl_tsn_devinfo;
    } else
#endif /* end BCM_FIRELIGHT_SUPPORT */
    {
        *devinfo = &gh2_tsn_devinfo;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_tsn_info_detach
 * Description:
 *     Detach the chip specific info.
 * Parameters:
 *     unit    - (IN) Unit number.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_tsn_info_detach(int unit)
{
    /* Detach bookkeeping info */
    BCM_IF_ERROR_RETURN(bcmi_gh2_bkinfo_cleanup(unit, 0));

    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND2_SUPPORT && BCM_TSN_SUPPORT */
