/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *  oam.c
 *
 * Purpose:
 *  ETH-OAM implementation for Monterey device.
 */

#include <shared/bsl.h>
#include <shared/idxres_fl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>
#include <soc/uc.h>

#include <bcm/oam.h>
#include <bcm/field.h>

#include <bcm_int/esw/fp_oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw_dispatch.h>

#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>
#include <soc/shared/shr_pkt.h>

#include <soc/shared/eth_oam.h>
#include <soc/shared/eth_oam_msg.h>
#include <soc/shared/eth_oam_pack.h>

#if defined(INCLUDE_CCM_SLM)

/*
 * Macro:
 *     _BCM_ETH_OAM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_ETH_OAM_ALLOC(_ptr_,_ptype_,_size_,_descr_)                \
    do {                                                                \
        if (NULL == (_ptr_)) {                                          \
            (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_));       \
        }                                                               \
        if((_ptr_) != NULL) {                                           \
            sal_memset((_ptr_), 0, (_size_));                           \
        }  else {                                                       \
            LOG_ERROR(BSL_LS_BCM_OAM,                                   \
                      (BSL_META("OAM Error: Allocation failure %s\n"),  \
                       (_descr_)));                                     \
        }                                                               \
    } while (0)

/* ETH-OAM app SDK messaging version. */
#define ETH_OAM_SDK_MSG_VERSION (0)

/* Minimum Eapp messaging version expected. */
#define ETH_OAM_UC_MSG_MIN_VERSION (0)

/* Invalid uC number */
#define _BCM_MO_ETH_OAM_INVALID_UC (-1)

/* Event thread priority. */
#define _BCM_MO_ETH_OAM_EVENT_THREAD_PRIO (200)

/* Valid group flags mask */
#define _BCM_MO_ETH_OAM_GROUP_VALID_FLAGS_MASK               \
    (~(BCM_OAM_GROUP_REPLACE                               | \
       BCM_OAM_GROUP_WITH_ID                               | \
       BCM_OAM_GROUP_REMOTE_DEFECT_TX))

/* Filter out transient flags from group flags. */
#define _BCM_MO_ETH_OAM_GROUP_FILTER_FLAGS(_flags_)     \
    ((_flags_) & (BCM_OAM_GROUP_REMOTE_DEFECT_TX))

/* Valid endpoint flags */
#define _BCM_MO_ETH_OAM_ENDPOINT_LOCAL_VALID_FLAGS_MASK      \
    (~(BCM_OAM_ENDPOINT_REPLACE                            | \
       BCM_OAM_ENDPOINT_WITH_ID                            | \
       BCM_OAM_ENDPOINT_PORT_STATE_TX                      | \
       BCM_OAM_ENDPOINT_INTERFACE_STATE_TX                 | \
       BCM_OAM_ENDPOINT_MATCH_INNER_VLAN                   | \
       BCM_OAM_ENDPOINT_PORT_STATE_UPDATE                  | \
       BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE             | \
       BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN))

#define _BCM_MO_ETH_OAM_ENDPOINT_REMOTE_VALID_FLAGS_MASK     \
    (~(BCM_OAM_ENDPOINT_REPLACE                            | \
       BCM_OAM_ENDPOINT_WITH_ID                            | \
       BCM_OAM_ENDPOINT_REMOTE))

/* Filter out transient flags from endpoint flags. */
#define _BCM_MO_ETH_OAM_ENDPOINT_FILTER_FLAGS(_flags_)                  \
    ((_flags_) & (BCM_OAM_ENDPOINT_REMOTE                             | \
                  BCM_OAM_ENDPOINT_PORT_STATE_TX                      | \
                  BCM_OAM_ENDPOINT_INTERFACE_STATE_TX                 | \
                  BCM_OAM_ENDPOINT_MATCH_INNER_VLAN                   | \
                  BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN))

/* Valid loss flags mask */
#define _BCM_MO_ETH_OAM_LOSS_VALID_FLAGS_MASK                  \
    (~(BCM_OAM_LOSS_TX_ENABLE                                | \
       BCM_OAM_LOSS_SINGLE_ENDED                             | \
       BCM_OAM_LOSS_WITH_ID                                  | \
       BCM_OAM_LOSS_UPDATE                                   | \
       BCM_OAM_LOSS_SLM))

/* Filter out transient flags from loss flags. */
#define _BCM_MO_ETH_OAM_LOSS_FILTER_FLAGS(_flags_)            \
    ((_flags_) & (BCM_OAM_LOSS_TX_ENABLE                    | \
                  BCM_OAM_LOSS_SINGLE_ENDED                 | \
                  BCM_OAM_LOSS_SLM))

/* Loss flags that cannot change on an update. */
#define _BCM_MO_ETH_OAM_LOSS_NON_UPDATE_FLAGS(_flags_)        \
    ((_flags_) & (BCM_OAM_LOSS_SINGLE_ENDED                 | \
                  BCM_OAM_LOSS_SLM))

/* Minimum possible MEP-ID value. */
#define _BCM_MO_ETH_OAM_MEP_ID_MIN (1)

/* Maximum possible MEP-ID value. */
#define _BCM_MO_ETH_OAM_MEP_ID_MAX (8191)

/* Invalid raw data read and wite index. */
#define _BCM_MO_ETH_OAM_PM_RAW_DATA_INVALID_IDX (-1)

/* Length of the endpoint lookup key. */
#define _BCM_MO_ETH_OAM_KEY_LENGTH (6)

/* Set a bit in a bitmap */
#define _BIT_SET(_bmp_, _bit_)                   \
    (_bmp_) = (_bmp_) | (1 << (_bit_))

/* Get a bit in a bitmap */
#define _BIT_GET(_bmp_, _bit_)                  \
    (((_bmp_) >> (_bit_)) & 0x1)

/* Log error and return. */
#define _LOG_ERROR(_unit_, _str_)                                    \
    LOG_ERROR(BSL_LS_BCM_OAM,                                        \
              (BSL_META_U((_unit_),                                  \
                          "OAM Error: %s %s():%d \n"),               \
               (_str_), __FUNCTION__, __LINE__));                    \

typedef enum _bcm_mo_eth_oam_endpoint_types_e {
    _BCM_MO_ETH_OAM_ENDPOINT_TYPE_NONE   = 0,
    _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL  = 1,
    _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE = 2,
    _BCM_MO_ETH_OAM_ENDPOINT_TYPE_COUNT
} _bcm_mo_eth_oam_endpoint_types_t;

/* Endpoint ID to internal ID map information. Indexed by the endpoint ID */
typedef struct _bcm_mo_eth_oam_endpoint_map_s {
    /* Indicates the type endpoint. */
    int type;

    /*
     * The internal id to which this endpoint maps to. For local endpoint, this
     * is the index of _bcm_mo_eth_oam_lmep_t and for remote endpoint this is
     * the index of _bcm_mo_eth_oam_endpoint_remote_t.
     */
    int int_id;
} _bcm_mo_eth_oam_endpoint_map_t;

/* Group information. */
typedef struct _bcm_mo_eth_oam_group_s {
    int in_use;
    uint32 flags;
    uint8 name[BCM_OAM_GROUP_NAME_LENGTH];
    bcm_oam_group_fault_alarm_defect_priority_t lowest_alarm_priority;
    uint32 clear_persistent_faults;
} _bcm_mo_eth_oam_group_t;

/* Local endpoint information. */
typedef struct _bcm_mo_eth_oam_endpoint_local_s {
    int in_use;
    bcm_oam_endpoint_t id;
    uint32 flags;
    uint16 name;
    uint8 level;
    int ccm_period;
    bcm_vlan_t vlan;
    uint16 outer_tpid;
    uint8 pkt_pri;
    bcm_vlan_t inner_vlan;
    uint16 inner_tpid;
    uint8 inner_pkt_pri;
    bcm_gport_t gport;
    int trunk_index;
    bcm_cos_t int_pri;
    bcm_mac_t src_mac;
    bcm_mac_t dst_mac;
    uint8 port_state;
    uint8 interface_state;
} _bcm_mo_eth_oam_endpoint_local_t;

/* RMEP information. */
typedef struct _bcm_mo_eth_oam_rmep_s {
    int in_use;
    bcm_oam_endpoint_t id;
    bcm_oam_group_t group_id;
    uint32 flags;
    uint16 name;
    int ccm_period;
    uint32 clear_persistent_faults;
    int num_slms;
} _bcm_mo_eth_oam_rmep_t;

/* LMEP information. */
typedef struct _bcm_mo_eth_oam_lmep_s {
    /* Group information. */
    _bcm_mo_eth_oam_group_t group;

    /* Local endpoint information. */
    _bcm_mo_eth_oam_endpoint_local_t endpoint;

    /* Number of RMEPs attached to the LMEP. */
    int num_rmeps;
} _bcm_mo_eth_oam_lmep_t;

/* SLM information. */
typedef struct _bcm_mo_eth_oam_slm_s {
    /* Indicates SLM is in use. */
    int in_use;

    /* Loss flags. */
    uint32 flags;

    /* LMEP ID. */
    uint16 lmep_id;

    /* RMEP ID. */
    uint16 rmep_id;

    /* Test ID. */
    uint32 test_id;

    /* Measurement period. */
    uint32 measurement_period;

    /* Tx period. */
    int period;

    /* Packet priority */
    uint8 pkt_pri;

    /* Inner packet priority */
    uint8 inner_pkt_pri;

    /* int pri. */
    uint8 int_pri;

    /* MAC address of the peer device. */
    bcm_mac_t peer_mac;
} _bcm_mo_eth_oam_slm_t;

/* Event handler information. */
typedef struct _bcm_mo_eth_oam_event_handler_s {
    /* Mask of LMEP events. */
    uint32 lmep_mask;

    /* Mask of RMEP events. */
    uint32 rmep_mask;

    /* Registered callback routine. */
    bcm_oam_event_cb cb;

    /* Application supplied data. */
    void *user_data;

    /* Pointer to next event. */
    struct _bcm_mo_eth_oam_event_handler_s *next;
} _bcm_mo_eth_oam_event_handler_t;

/* PM event handler information. */
typedef struct _bcm_mo_eth_oam_pm_event_handler_s {
    /* Registered callback routine. */
    bcm_oam_pm_event_cb cb;

    /* Application supplied data. */
    void *user_data;

    /* Pointer to next event. */
    struct _bcm_mo_eth_oam_pm_event_handler_s *next;
} _bcm_mo_eth_oam_pm_event_handler_t;

/* Global eth oam control. */
typedef struct _bcm_mo_eth_oam_ctrl_s {
    /* Core on which the app is running. */
    int uC;

    /* Max groups. */
    int max_groups;

    /* Max RMEPs. */
    int max_rmeps;

    /* Max CCM sessions. */
    int max_ccm_sessions;

    /* Max LM sessions. */
    int max_slm_sessions;

    /* DMA Buffer used for messaging between SDK and UKERNEL. */
    uint8 *dma_buffer;

    /* Length of the DMA buffer */
    int dma_buffer_len;

    /* Number of groups created. */
    int num_groups;

    /* Number of RMEPs created. */
    int num_rmeps;

    /* Number of SLM sessions created. */
    int num_slm_sessions;

    /* Number of Tx CCM sessions created. */
    int num_tx_ccm_sessions;

    /* Number of Rx CCM sessions created. */
    int num_rx_ccm_sessions;

    /* Group pool. */
    shr_idxres_list_handle_t group_pool;

    /* Endpoint pool. */
    shr_idxres_list_handle_t endpoint_pool;

    /* RMEP pool. */
    shr_idxres_list_handle_t rmep_pool;

    /* SLM pool. */
    shr_idxres_list_handle_t slm_pool;

    /* LMEP table. */
    _bcm_mo_eth_oam_lmep_t *lmeps;

    /* RMEP table. */
    _bcm_mo_eth_oam_rmep_t *rmeps;

    /* SLM sessions table. */
    _bcm_mo_eth_oam_slm_t *slm_sessions;

    /* Endpoint ID mapping table. */
    _bcm_mo_eth_oam_endpoint_map_t *endpoint_map;

    /* LMEP event mask. */
    uint32 lmep_event_mask;

    /* RMEP event mask. */
    uint32 rmep_event_mask;

    /* Event thread Prio */
    sal_thread_t event_thread_id;

    /* Event handler information. */
    _bcm_mo_eth_oam_event_handler_t *event_handler_list;

    /* PM mode. */
    int pm_mode;

    /* DMA Buffer used for messaging PM raw data between SDK and UKERNEL. */
    uint8 *pm_dma_buffer;

    /* Length of the PM DMA buffer */
    int pm_dma_buffer_len;

    /* Temperory buffer to hold PM data. */
    uint8 *pm_rstats_buffer;

    /* PM raw data. */
    bcm_oam_pm_raw_data_t *pm_raw_data;

    /* PM raw data buffer read index. */
    int pm_read_idx;

    /* PM raw data buffer write index. */
    int pm_write_idx;

    /* PM event handler information. */
    _bcm_mo_eth_oam_pm_event_handler_t *pm_event_handler_list;
} _bcm_mo_eth_oam_ctrl_t;

static _bcm_mo_eth_oam_ctrl_t *g_eth_oam_ctrl[BCM_MAX_NUM_UNITS] = {0};

/* Mapping table to convert LMEP events to the SDK enum. */
static bcm_oam_event_type_t g_lmep_event_map[] = {
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_RDI      */ bcmOAMEventGroupRemote,
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_RDI_CLR  */ bcmOAMEventGroupRemoteClear,
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_MAC      */ bcmOAMEventGroupMACStatus,
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_MAC_CLR  */ bcmOAMEventGroupMACStatusClear,
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_CCM      */ bcmOAMEventGroupCCMTimeout,
    /* SHR_ETH_OAM_LMEP_EVENT_SOME_CCM_CLR  */ bcmOAMEventGroupCCMTimeoutClear,
    /* SHR_ETH_OAM_LMEP_EVENT_CCM_ERR       */ bcmOAMEventGroupCCMError,
    /* SHR_ETH_OAM_LMEP_EVENT_CCM_ERR_CLR   */ bcmOAMEventGroupCCMErrorClear,
    /* SHR_ETH_OAM_LMEP_EVENT_XCON          */ bcmOAMEventGroupCCMxcon,
    /* SHR_ETH_OAM_LMEP_EVENT_XCON_CLR      */ bcmOAMEventGroupCCMxconClear
};

/* Mapping table to convert RMEP events to the SDK enum. */
static bcm_oam_event_type_t g_rmep_event_map[] = {
    /* SHR_ETH_OAM_RMEP_EVENT_CCM_TIMEOUT                   */ bcmOAMEventEndpointCCMTimeout,
    /* SHR_ETH_OAM_RMEP_EVENT_CCM_TIMEOUT_CLR               */ bcmOAMEventEndpointCCMTimein,
    /* SHR_ETH_OAM_RMEP_EVENT_RDI                           */ bcmOAMEventEndpointRemote,
    /* SHR_ETH_OAM_RMEP_EVENT_RDI_CLR                       */ bcmOAMEventEndpointRemoteUp,
    /* SHR_ETH_OAM_RMEP_EVENT_PORT_STATUS_DEFECT            */ bcmOAMEventEndpointPortDown,
    /* SHR_ETH_OAM_RMEP_EVENT_PORT_STATUS_DEFECT_CLR        */ bcmOAMEventEndpointPortUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_DOWN        */ bcmOAMEventEndpointInterfaceDown,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_TESTING     */ bcmOAMEventEndpointInterfaceTesting,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_UNKNOWN     */ bcmOAMEventEndpointInterfaceUnkonwn,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_DORMANT     */ bcmOAMEventEndpointInterfaceDormant,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_NOT_PRESENT */ bcmOAMEventEndpointInterfaceNotPresent,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UP_TO_LLDOWN      */ bcmOAMEventEndpointInterfaceLLDown,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_DOWN_TO_UP        */ bcmOAMEventEndpointInterfaceUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_TESTING_TO_UP     */ bcmOAMEventEndpointInterfaceTestingToUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_UNKNOWN_TO_UP     */ bcmOAMEventEndpointInterfaceUnknownToUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_DORMANT_TO_UP     */ bcmOAMEventEndpointInterfaceDormantToUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_NOT_PRESENT_TO_UP */ bcmOAMEventEndpointInterfaceNotPresentToUp,
    /* SHR_ETH_OAM_RMEP_EVENT_INTF_STATUS_LLDOWN_TO_UP      */ bcmOAMEventEndpointInterfaceLLDownToUp,
};
extern int
_bcm_olp_l2_hdr_get(int unit, int glp, uint8 nbo, soc_olp_l2_hdr_t *l2hdr);

#ifdef BCM_WARM_BOOT_SUPPORT
int _bcm_mo_eth_oam_init_status_scache_alloc(int unit, int create);
int _bcm_mo_eth_oam_wb_init_status_recover(int unit, int *init);
int _bcm_mo_eth_oam_cfg_scache_alloc(int unit, int create);
int _bcm_mo_eth_oam_wb_cfg_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_mo_eth_oam_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
        case SHR_ETH_OAM_UC_E_NONE:
            rv = BCM_E_NONE;
            break;
        case SHR_ETH_OAM_UC_E_INTERNAL:
            rv = BCM_E_INTERNAL;
            break;
        case SHR_ETH_OAM_UC_E_MEMORY:
            rv = BCM_E_MEMORY;
            break;
        case SHR_ETH_OAM_UC_E_UNIT:
            rv = BCM_E_UNIT;
            break;
        case SHR_ETH_OAM_UC_E_PARAM:
            rv = BCM_E_PARAM;
            break;
        case SHR_ETH_OAM_UC_E_EMPTY:
            rv = BCM_E_EMPTY;
            break;
        case SHR_ETH_OAM_UC_E_FULL:
            rv = BCM_E_FULL;
            break;
        case SHR_ETH_OAM_UC_E_NOT_FOUND:
            rv = BCM_E_NOT_FOUND;
            break;
        case SHR_ETH_OAM_UC_E_EXISTS:
            rv = BCM_E_EXISTS;
            break;
        case SHR_ETH_OAM_UC_E_TIMEOUT:
            rv = BCM_E_TIMEOUT;
            break;
        case SHR_ETH_OAM_UC_E_BUSY:
            rv = BCM_E_BUSY;
            break;
        case SHR_ETH_OAM_UC_E_FAIL:
            rv = BCM_E_FAIL;
            break;
        case SHR_ETH_OAM_UC_E_DISABLED:
            rv = BCM_E_DISABLED;
            break;
        case SHR_ETH_OAM_UC_E_BADID:
            rv = BCM_E_BADID;
            break;
        case SHR_ETH_OAM_UC_E_RESOURCE:
            rv = BCM_E_RESOURCE;
            break;
        case SHR_ETH_OAM_UC_E_CONFIG:
            rv = BCM_E_CONFIG;
            break;
        case SHR_ETH_OAM_UC_E_UNAVAIL:
            rv = BCM_E_UNAVAIL;
            break;
        case SHR_ETH_OAM_UC_E_INIT:
            rv = BCM_E_INIT;
            break;
        case SHR_ETH_OAM_UC_E_PORT:
            rv = BCM_E_PORT;
            break;
        default:
            rv = BCM_E_INTERNAL;
            break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_msg_send_receive
 * Purpose:
 *      Sends given ETH-OAM control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_subclass  - (IN) BFD message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_mo_eth_oam_msg_send_receive(int unit, uint8 s_subclass,
                                 uint16 s_len, uint32 s_data,
                                 uint8 r_subclass, uint16 *r_len)
{
    int rv;
    mos_msg_data_t send, reply;
    uint32 uc_rv;
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *dma_buffer;
    int dma_buffer_len;

    dma_buffer = eoc->dma_buffer;
    dma_buffer_len = eoc->dma_buffer_len;

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_ETH_OAM;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, eoc->uC,
                                      &send, &reply,
                                      SHR_ETH_OAM_MAX_UC_MSG_TIMEOUT);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert ETH_OAM uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_mo_eth_oam_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /* Check reply class and subclass */
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_ETH_OAM) ||
                              (reply.s.subclass != r_subclass))) {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_appl_callback
 * Purpose:
 *      Update ETH-OAM FW appl state
 * Parameters:
 *      unit      - (IN) Unit number.
 *      uC        - (IN) core number.
 *      stage     - (IN) core reset stage.
 *      user_data - (IN) data pointer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_bcm_mo_eth_oam_appl_callback(int unit,
                              int uC,
                              soc_cmic_uc_shutdown_stage_t stage,
                              void *user_data) {
   _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    eoc->uC = _BCM_MO_ETH_OAM_INVALID_UC;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_appl_init
 * Purpose:
 *      Send the application init message to the ETH-OAM app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      uC         - (OUT) uC number
 * Returns:
 *      None
 */
STATIC void
_bcm_mo_eth_oam_uc_appl_init(int unit, int *uC)
{
    int rv;
    int status;

    for (*uC = 0; *uC < SOC_INFO(unit).num_ucs; (*uC)++) {
        rv = soc_uc_status(unit, *uC, &status);
        if ((rv == SOC_E_NONE) && (status != 0)) {
            rv = soc_cmic_uc_appl_init(unit, *uC, MOS_MSG_CLASS_ETH_OAM,
                                       SHR_ETH_OAM_MAX_UC_MSG_TIMEOUT,
                                       ETH_OAM_SDK_MSG_VERSION,
                                       ETH_OAM_UC_MSG_MIN_VERSION,
                                       _bcm_mo_eth_oam_appl_callback, NULL);
            if (SOC_E_NONE == rv) {
                break;
            }
        }
    }

    if (*uC >= SOC_INFO(unit).num_ucs) {
        *uC = _BCM_MO_ETH_OAM_INVALID_UC;
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_init
 * Purpose:
 *      Send the init message to the OAM app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_init(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_init_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));

    msg.max_lmeps         = eoc->max_groups;
    msg.max_rmeps         = eoc->max_rmeps;
    msg.max_ccm_sessions  = eoc->max_ccm_sessions;
    msg.max_slm_sessions  = eoc->max_slm_sessions;
    msg.pm_mode           = eoc->pm_mode;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_init_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_INIT,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_INIT_REPLY,
                                          &reply_len));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_shutdown
 * Purpose:
 *      Send the shutdown message to the OAM app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_shutdown(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int status = 0;
    uint16 reply_len = 0;

    if (eoc->uC != _BCM_MO_ETH_OAM_INVALID_UC) {
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, eoc->uC, &status));
        if (status) {
            BCM_IF_ERROR_RETURN
                (_bcm_mo_eth_oam_msg_send_receive(unit,
                                                  MOS_MSG_SUBCLASS_ETH_OAM_SHUTDOWN,
                                                  0, 0,
                                                  MOS_MSG_SUBCLASS_ETH_OAM_SHUTDOWN_REPLY,
                                                  &reply_len));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_warmboot
 * Purpose:
 *      Send the warmboot message to the OAM app.
 * Parameters:
 *      unit       - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_warmboot(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int status = 0;
    uint16 reply_len = 0;

    SOC_IF_ERROR_RETURN(soc_uc_status(unit, eoc->uC, &status));
    if (status) {
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_msg_send_receive(unit,
                                              MOS_MSG_SUBCLASS_ETH_OAM_WARMBOOT,
                                              0, 0,
                                              MOS_MSG_SUBCLASS_ETH_OAM_WARMBOOT_REPLY,
                                              &reply_len));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_lmep_set
 * Purpose:
 *      Send the LMEP set message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      msg        - (IN)  Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_lmep_set(int unit,
                                shr_eth_oam_msg_ctrl_lmep_set_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_lmep_set_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_lmep_get
 * Purpose:
 *      Send the LMEP get message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  LMEP ID
 *      msg        - (OUT) Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_lmep_get(int unit, uint16 id,
                                shr_eth_oam_msg_ctrl_lmep_get_t *msg)
{

    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(msg, 0, sizeof(*msg));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_GET,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_GET_REPLY,
                                          &reply_len));


    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_lmep_get_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_lmep_del
 * Purpose:
 *      Send the LMEP delete message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      msg        - (IN)  Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_lmep_del(int unit, shr_eth_oam_msg_ctrl_lmep_del_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_lmep_del_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_DEL,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_LMEP_DEL_REPLY,
                                          &reply_len));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_rmep_set
 * Purpose:
 *      Send the RMEP set message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      msg        - (IN)  Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_rmep_set(int unit,
                                shr_eth_oam_msg_ctrl_rmep_set_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_rmep_set_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_rmep_get
 * Purpose:
 *      Send the RMEP get message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  RMEP ID
 *      msg        - (OUT) Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_rmep_get(int unit, uint16 id,
                                shr_eth_oam_msg_ctrl_rmep_get_t *msg)
{

    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(msg, 0, sizeof(*msg));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_GET,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_GET_REPLY,
                                          &reply_len));


    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_rmep_get_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_rmep_del
 * Purpose:
 *      Send the RMEP delete message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  RMEP ID
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_rmep_del(int unit, uint16 id)
{
    uint16 reply_len = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_DEL,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_RMEP_DEL_REPLY,
                                          &reply_len));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_event_masks_set
 * Purpose:
 *      Send the event masks set message.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      lmep_mask  - (IN)  LMEP mask
 *      rmep_mask  - (IN)  RMEP mask
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_event_masks_set(int unit, uint32 lmep_mask, uint32 rmep_mask)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_event_masks_set_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));
    msg.lmep_mask = lmep_mask;
    msg.rmep_mask = rmep_mask;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_event_masks_set_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_EVENT_MASKS_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_EVENT_MASKS_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_trunk_set
 * Purpose:
 *      Send the trunk set message.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      trunk_gport  - (IN)  Trunk gport
 *      num_ports    - (IN)  Number of ports to add/remove from trunk.
 *      ports        - (IN)  List of ports to be added/removed
 *      add          - (IN)  1 - Add, 0 - Remove
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_event_trunk_set(int unit, bcm_gport_t trunk_gport,
                                       int num_ports, bcm_gport_t *ports,
                                       int add)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_trunk_set_t msg;
    bcm_module_t my_modid;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int i;


    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    sal_memset(&msg, 0, sizeof(msg));
    msg.num_ports = num_ports;
    if (add) {
        msg.tid = BCM_GPORT_TRUNK_GET(trunk_gport);
    } else {
        msg.tid = SHR_ETH_OAM_INVALID_TID;
    }

    for (i = 0; i < num_ports; i++) {
        if (BCM_GPORT_IS_MODPORT(ports[i])) {
            if (my_modid != BCM_GPORT_MODPORT_MODID_GET(ports[i])) {
                /* Non-local port. */
                _LOG_ERROR(unit, "Non local gport");
                return BCM_E_PARAM;
            }
            msg.ports[i] = BCM_GPORT_MODPORT_PORT_GET(ports[i]);
        } else if (BCM_GPORT_IS_LOCAL(ports[i])) {
            msg.ports[i] = BCM_GPORT_LOCAL_GET(ports[i]);
        } else {
            _LOG_ERROR(unit, "Invalid gport type");
            return BCM_E_PARAM;
        }
    }

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_trunk_set_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_TRUNK_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_TRUNK_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_trunk_get
 * Purpose:
 *      Send the trunk get message.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      trunk_gport  - (IN)  Trunk gport
 *      msg          - (OUT) Trunk get message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_trunk_get(int unit, bcm_gport_t trunk_gport,
                                 shr_eth_oam_msg_ctrl_trunk_get_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(msg, 0, sizeof(*msg));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_TRUNK_GET,
                                          BCM_GPORT_TRUNK_GET(trunk_gport), 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_TRUNK_GET_REPLY,
                                          &reply_len));

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_trunk_get_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_slm_set
 * Purpose:
 *      Send the SLM set message.
 * Parameters:
 *      unit         - (IN)  BCM device number
 *      msg          - (OUT) SLM set message.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_slm_set(int unit, shr_eth_oam_msg_ctrl_slm_set_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_slm_set_pack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_slm_get
 * Purpose:
 *      Send the SLM get message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  SLM ID
 *      msg        - (OUT) Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_slm_get(int unit, uint16 id,
                               shr_eth_oam_msg_ctrl_slm_get_t *msg)
{

    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(msg, 0, sizeof(*msg));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_GET,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_GET_REPLY,
                                          &reply_len));


    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_slm_get_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_slm_del
 * Purpose:
 *      Send the SLM delete message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  SLM ID
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_slm_del(int unit, uint16 id)
{
    uint16 reply_len = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_DEL,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_SLM_DEL_REPLY,
                                          &reply_len));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_pm_pstats_slm_get
 * Purpose:
 *      Send the PM-SLM stats get message.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      id         - (IN)  SLM ID
 *      msg        - (OUT) Message
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_pm_pstats_slm_get(int unit, uint16 id,
                                        shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(msg, 0, sizeof(*msg));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_PM_PSTATS_SLM_GET,
                                          id, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_PM_PSTATS_SLM_GET_REPLY,
                                          &reply_len));


    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_pm_pstats_slm_get_unpack(buffer_req, msg);
    buffer_len = buffer_ptr - buffer_req;

    if (buffer_len != reply_len) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_uc_msg_pm_rstats_addr_set
 * Purpose:
 *      Send the PM raw stats address set message
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      enable     - (IN)  1 - Enable Raw stats DMA
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_uc_msg_pm_rstats_addr_set(int unit, int enable)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_pm_rstats_addr_set_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&msg, 0, sizeof(msg));
    msg.enable = enable ? 1 : 0;
    msg.addr   = soc_cm_l2p(unit, eoc->pm_dma_buffer);

    buffer_req = eoc->dma_buffer;
    buffer_ptr = shr_eth_oam_msg_ctrl_pm_rstats_addr_set_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_msg_send_receive(unit,
                                          MOS_MSG_SUBCLASS_ETH_OAM_PM_RSTATS_ADDR_SET,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_ETH_OAM_PM_RSTATS_ADDR_SET_REPLY,
                                          &reply_len));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_gport_resolve
 * Purpose:
 *      Resolve the gport and get the Tx and Rx port.
 * Parameters:
 *      unit         - (IN)  Unit number.
 *      gport        - (IN)  gport
 *      trunk_index  - (IN)  Trunk index passed by user.
 *      is_trunk     - (OUT) 1 - Gport is a trunk
 *      mod_id       - (OUT) Module ID of the Tx port.
 *      tx_port      - (OUT) Port on which to send packets
 *      rx_port      - (OUT) Port on which packets are received
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_gport_resolve(int unit,
                                       bcm_gport_t gport,
                                       int trunk_index,
                                       int *is_trunk,
                                       bcm_module_t *mod_id, bcm_port_t *tx_port,
                                       bcm_port_t *rx_port)
{
    bcm_module_t        module_id;
    bcm_port_t          port_id;
    int                 local_id;
    bcm_trunk_t         trunk_id, trunk_id_tmp;
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_member_t *member_array = NULL;
    int                 member_count = 0;
    int                 is_local;
    int                 rv;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module_id, &port_id,
                                &trunk_id, &local_id));

    if (BCM_GPORT_IS_TRUNK(gport)) {
        if (BCM_TRUNK_INVALID == trunk_id)  {
            return (BCM_E_PARAM);
        }
        /* Get count of ports in this trunk. */
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 0, NULL, &member_count));
        if (0 == member_count) {
            /* No members have been added to the trunk group yet */
            _LOG_ERROR(unit, "Empty trunk");
            return BCM_E_PARAM;
        }

        _BCM_ETH_OAM_ALLOC(member_array, bcm_trunk_member_t,
                           sizeof(bcm_trunk_member_t) * member_count,
                           "Trunk info");
        if (NULL == member_array) {
            return (BCM_E_MEMORY);
        }

        /* Get Trunk Info for the Trunk ID. */
        rv = bcm_esw_trunk_get(unit, trunk_id, &trunk_info,
                               member_count, member_array, &member_count);
        if (BCM_FAILURE(rv)) {
            sal_free(member_array);
            return rv;
        }

        /* Check if the input trunk_index is valid. */
        if (trunk_index >= member_count) {
            sal_free(member_array);
            _LOG_ERROR(unit, "Invalid trunk index");
            return BCM_E_PARAM;
        }

        /* Get the Modid and Port value using Trunk Index value. */
        rv = _bcm_esw_gport_resolve(unit,
                                    member_array[trunk_index].gport,
                                    &module_id, &port_id, &trunk_id_tmp,
                                    &local_id);
        sal_free(member_array);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module_id, &is_local));
        if (!is_local) {
            _LOG_ERROR(unit, "Non local gport");
            return BCM_E_PARAM;
        }

        *is_trunk = 1;
        *mod_id   = module_id;
        *rx_port  = trunk_id;
        *tx_port  = port_id;
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module_id, &is_local));
        if (!is_local) {
            _LOG_ERROR(unit, "Non local gport");
            return BCM_E_PARAM;
        }

        *is_trunk = 0;
        *mod_id   = module_id;
        *rx_port  = port_id;
        *tx_port  = port_id;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        *is_trunk = 0;
        *mod_id   = module_id;
        *rx_port  = port_id;
        *tx_port  = port_id;
    } else {
        _LOG_ERROR(unit, "Invalid gport type");
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_lmep_faults_convert
 * Purpose:
 *      Convert between LMEP faults from SDK <--> uC
 * Parameters:
 *      faults     - (IN) faults
 *      sdk_to_uc  - (IN) 1 - SDK --> uC, 0 - uC --> SDK
 * Returns
 *      Converted faults
 * Notes:
 */
STATIC uint32
_bcm_mo_eth_oam_lmep_faults_convert(uint32 faults, int sdk_to_uc)
{
    uint32 map[] = {
        /* SHR_ETH_OAM_LMEP_FAULT_SOME_RDI */ BCM_OAM_GROUP_FAULT_REMOTE,
        /* SHR_ETH_OAM_LMEP_FAULT_SOME_MAC */ BCM_OAM_GROUP_FAULT_CCM_MAC,
        /* SHR_ETH_OAM_LMEP_FAULT_SOME_CCM */ BCM_OAM_GROUP_FAULT_CCM_TIMEOUT,
        /* SHR_ETH_OAM_LMEP_FAULT_CCM_ERR */  BCM_OAM_GROUP_FAULT_CCM_ERROR,
        /* SHR_ETH_OAM_LMEP_FAULT_XCON */     BCM_OAM_GROUP_FAULT_CCM_XCON
    };
    uint32 converted_flags = 0;
    int i;

    if (sdk_to_uc) {
        for (i = 0; i < COUNTOF(map); i++) {
            if (faults & map[i]) {
                _BIT_SET(converted_flags, i);
            }
        }
    } else {
        for (i = 0; i < COUNTOF(map); i++) {
            if (_BIT_GET(faults, i)) {
                converted_flags |= map[i];
            }
        }
    }

    return converted_flags;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_rmep_faults_convert
 * Purpose:
 *      Convert between RMEP faults from SDK <--> uC
 * Parameters:
 *      faults    - (IN) faults
 *      sdk_to_uc - (IN) 1 - SDK --> uC, 0 - uC --> SDK
 * Returns:
 *      Converted faults
 * Notes:
 */
STATIC uint32
_bcm_mo_eth_oam_rmep_faults_convert(uint32 faults, int sdk_to_uc)
{
    uint32 map[] = {
        /* SHR_ETH_OAM_RMEP_FAULT_CCM_TIMEOUT */ BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT,
        /* SHR_ETH_OAM_RMEP_FAULT_RDI */         BCM_OAM_ENDPOINT_FAULT_REMOTE,
        /* SHR_ETH_OAM_RMEP_FAULT_PORT_DOWN */   BCM_OAM_ENDPOINT_FAULT_PORT_DOWN,
        /* SHR_ETH_OAM_RMEP_FAULT_INTF_DOWN */   BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN
    };
    uint32 converted_flags = 0;
    int i;

    if (sdk_to_uc) {
        for (i = 0; i < COUNTOF(map); i++) {
            if (faults & map[i]) {
                _BIT_SET(converted_flags, i);
            }
        }
    } else {
        for (i = 0; i < COUNTOF(map); i++) {
            if (_BIT_GET(faults, i)) {
                converted_flags |= map[i];
            }
        }
    }

    return converted_flags;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_period_convert
 * Purpose:
 *      Convert period to uC format
 * Parameters:
 *      period  - (IN) period
 * Returns:
 *      Converted period in uC format
 * Notes:
 */
STATIC uint8
_bcm_mo_eth_oam_period_convert(int period)
{
    switch(period) {
        case BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED:
            return SHR_ETH_OAM_PERIOD_INVALID;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_3MS:
            return SHR_ETH_OAM_PERIOD_3_3_MSEC;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_10MS:
            return SHR_ETH_OAM_PERIOD_10_MSEC;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_100MS:
            return SHR_ETH_OAM_PERIOD_100_MSEC;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_1S:
            return SHR_ETH_OAM_PERIOD_1_SEC;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_10S:
            return SHR_ETH_OAM_PERIOD_10_SEC;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_1M:
            return SHR_ETH_OAM_PERIOD_1_MIN;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_10M:
            return SHR_ETH_OAM_PERIOD_10_MIN;

        default:
            return SHR_ETH_OAM_PERIOD_INVALID;
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_alarm_mask_convert
 * Purpose:
 *      Convert the lowets alarm priority to RDI mask
 * Parameters:
 *      lowest_alarm_priority - (IN) Lowest alarm priority
 * Returns:
 *      Converted to the fault in uC format
 * Notes:
 */
STATIC uint8
_bcm_mo_eth_oam_alarm_mask_convert(bcm_oam_group_fault_alarm_defect_priority_t lowest_alarm_priority)
{
    uint8 mask = 0;

    switch (lowest_alarm_priority) {
        /*
         * Alarms in increasing order of priority. Convert alarm to bitmap
         * with <= priority alarms set in it.
         */
        case bcmOAMGroupFaultAlarmPriorityDefectsAll:
        case bcmOAMGroupFaultAlarmPriorityDefectRDICCM:
            _BIT_SET(mask, SHR_ETH_OAM_LMEP_FAULT_SOME_RDI);
            /* passthru */

        case bcmOAMGroupFaultAlarmPriorityDefectMACStatus:
            _BIT_SET(mask, SHR_ETH_OAM_LMEP_FAULT_SOME_MAC);
            /* passthru */

        case bcmOAMGroupFaultAlarmPriorityDefectRemoteCCM:
            _BIT_SET(mask, SHR_ETH_OAM_LMEP_FAULT_SOME_CCM);
            /* passthru */

        case bcmOAMGroupFaultAlarmPriorityDefectErrorCCM:
            _BIT_SET(mask, SHR_ETH_OAM_LMEP_FAULT_CCM_ERR);
            /* passthru */

        case bcmOAMGroupFaultAlarmPriorityDefectXconCCM:
            _BIT_SET(mask, SHR_ETH_OAM_LMEP_FAULT_XCON);
            break;

        case bcmOAMGroupFaultAlarmPriorityDefectsNone:
            mask = 0;
            break;

        default:
            break;
    }

    return mask;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_l2_hdr_get
 * Purpose:
 *      Get the L2 header from endpoint information
 * Parameters:
 *      ep       - (IN)  Endpoint
 *      l2       - (OUT) L2 header
 * Returns:
 *      None
 * Notes:
 */
STATIC void
_bcm_mo_eth_oam_l2_hdr_get(_bcm_mo_eth_oam_endpoint_local_t *ep,
                           shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    sal_memcpy(l2->dst_mac, ep->dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(l2->src_mac, ep->src_mac, sizeof(bcm_mac_t));

    l2->outer_vlan_tag.tpid     = ep->outer_tpid;
    l2->outer_vlan_tag.tci.prio = ep->pkt_pri;
    l2->outer_vlan_tag.tci.vid  = ep->vlan;

    l2->inner_vlan_tag.tpid     = ep->inner_tpid;
    l2->inner_vlan_tag.tci.prio = ep->inner_pkt_pri;
    l2->inner_vlan_tag.tci.vid  = ep->inner_vlan;

    l2->etype = 0x8902;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_olp_l2_hdr_pack
 * Purpose:
 *      Pack the OLP L2 header
 * Parameters:
 *      unit   - (IN) BCM device number
 *      buf    - (IN) Buffer to pack the header
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_olp_l2_hdr_pack(int unit, uint8 *buf)
{
    bcm_gport_t cpu_dglp;
    bcm_module_t my_modid = 0;
    soc_olp_l2_hdr_t olp_l2_hdr;
    int i;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID,
                                               my_modid, CMIC_PORT(unit),
                                               &cpu_dglp));
    BCM_IF_ERROR_RETURN(_bcm_olp_l2_hdr_get(unit, cpu_dglp, 0, &olp_l2_hdr));

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buf, olp_l2_hdr.dst_mac[i]);
    }
    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buf, olp_l2_hdr.src_mac[i]);
    }
    _SHR_PACK_U16(buf, olp_l2_hdr.tpid);
    _SHR_PACK_U16(buf, olp_l2_hdr.vlan);
    _SHR_PACK_U16(buf, olp_l2_hdr.etherType);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_ccm_pkt_get
 * Purpose:
 *      Construct CCM packet including OLP and set in message.
 * Parameters:
 *      unit      - (IN)  BCM device number
 *      ep        - (IN)  Endpoint information
 *      msg       - (OUT) LMEP set message
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_ccm_pkt_get(int unit, _bcm_mo_eth_oam_lmep_t *lmep,
                            shr_eth_oam_msg_ctrl_lmep_set_t *msg)
{
    uint8 *cur_ptr;
    soc_olp_tx_hdr_t olp_tx_hdr;
    shr_l2_header_t l2;
    bcm_port_t tx_port, rx_port;
    bcm_module_t mod_id;
    uint8 flags;
    int is_trunk;
    int i;

    sal_memset(msg->ccm_pkt, 0, SHR_ETH_OAM_CCM_TX_PKT_MAX_LENGTH);

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_gport_resolve(unit, lmep->endpoint.gport,
                                                lmep->endpoint.trunk_index,
                                                &is_trunk, &mod_id,
                                                &tx_port, &rx_port));

    /* Pack the OLP L2 header. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_olp_l2_hdr_pack(unit, msg->ccm_pkt));
    cur_ptr = msg->ccm_pkt + sizeof(soc_olp_l2_hdr_t);

    /* Pack OLP Tx header. */
    sal_memset(&olp_tx_hdr, 0, sizeof(olp_tx_hdr));
    SOC_OLP_TX_PORT(&olp_tx_hdr)  = tx_port;
    SOC_OLP_TX_MODID(&olp_tx_hdr) = mod_id;
    SOC_OLP_TX_PRI(&olp_tx_hdr)   = lmep->endpoint.int_pri;

    cur_ptr = shr_olp_tx_header_pack(cur_ptr, &olp_tx_hdr);

    /* Pack L2 header. */
    _bcm_mo_eth_oam_l2_hdr_get(&(lmep->endpoint), &l2);
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    msg->ccm_offset = cur_ptr - msg->ccm_pkt;

    /* Pack the CCM header. */
    flags = _bcm_mo_eth_oam_period_convert(lmep->endpoint.ccm_period);
    _SHR_PACK_U8(cur_ptr, lmep->endpoint.level << 5);
    _SHR_PACK_U8(cur_ptr, SHR_ETH_OAM_OPCODE_CCM);
    _SHR_PACK_U8(cur_ptr, flags);
    _SHR_PACK_U8(cur_ptr, 70); /* TLV offset */

    _SHR_PACK_U32(cur_ptr, 0); /* Sequence number */

    _SHR_PACK_U16(cur_ptr, lmep->endpoint.name);

    for (i = 0; i < SHR_ETH_OAM_MEG_ID_LENGTH; i++) {
        _SHR_PACK_U8(cur_ptr, lmep->group.name[i]);
    }

    _SHR_PACK_U32(cur_ptr, 0); /* TxFcf */
    _SHR_PACK_U32(cur_ptr, 0); /* RxFcb */
    _SHR_PACK_U32(cur_ptr, 0); /* TxFcb */
    _SHR_PACK_U32(cur_ptr, 0); /* Reserved */

    if (lmep->endpoint.flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
        _SHR_PACK_U8(cur_ptr, 2); /* Type */
        _SHR_PACK_U16(cur_ptr, 4); /* Length */
        _SHR_PACK_U8(cur_ptr, lmep->endpoint.port_state); /* Value */
    }

    if (lmep->endpoint.flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {
        _SHR_PACK_U8(cur_ptr, 4); /* Type */
        _SHR_PACK_U16(cur_ptr, 4); /* Length */
        _SHR_PACK_U8(cur_ptr, lmep->endpoint.interface_state); /* Value */
    }

    _SHR_PACK_U8(cur_ptr, 0); /* End TLV */

    msg->ccm_pkt_length = (cur_ptr - msg->ccm_pkt) + SHR_ETH_OAM_CRC_LENGTH;
    if (msg->ccm_pkt_length < SHR_ETH_OAM_TX_PKT_MIN_LENGTH) {
        msg->ccm_pkt_length = SHR_ETH_OAM_TX_PKT_MIN_LENGTH;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_slm_pkt_get
 * Purpose:
 *      Construct SLM packet including OLP and set in message.
 * Parameters:
 *      unit      - (IN)  BCM device number
 *      slm       - (IN)  SLM information
 *      msg       - (OUT) SLM set message
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_slm_pkt_get(int unit, _bcm_mo_eth_oam_slm_t *slm,
                            shr_eth_oam_msg_ctrl_slm_set_t *msg)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    uint8 *cur_ptr;
    soc_olp_tx_hdr_t olp_tx_hdr;
    shr_l2_header_t l2;
    bcm_port_t tx_port, rx_port;
    bcm_module_t mod_id;
    int is_trunk;

    sal_memset(msg->pkt, 0, SHR_ETH_OAM_SLM_TX_PKT_MAX_LENGTH);

    lmep = &(eoc->lmeps[slm->lmep_id]);


    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_gport_resolve(unit, lmep->endpoint.gport,
                                                lmep->endpoint.trunk_index,
                                                &is_trunk, &mod_id,
                                                &tx_port, &rx_port));

    /* Pack the OLP L2 header. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_olp_l2_hdr_pack(unit, msg->pkt));
    cur_ptr = msg->pkt + sizeof(soc_olp_l2_hdr_t);

    /* Pack OLP Tx header. */
    sal_memset(&olp_tx_hdr, 0, sizeof(olp_tx_hdr));
    SOC_OLP_TX_PORT(&olp_tx_hdr)  = tx_port;
    SOC_OLP_TX_MODID(&olp_tx_hdr) = mod_id;
    SOC_OLP_TX_PRI(&olp_tx_hdr)   = slm->int_pri;

    cur_ptr = shr_olp_tx_header_pack(cur_ptr, &olp_tx_hdr);

    /* Pack L2 header. */
    _bcm_mo_eth_oam_l2_hdr_get(&(lmep->endpoint), &l2);
    memcpy(l2.dst_mac, slm->peer_mac, sizeof(bcm_mac_t));
    l2.outer_vlan_tag.tci.prio = slm->pkt_pri;
    l2.inner_vlan_tag.tci.prio = slm->inner_pkt_pri;
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    msg->slm_offset = cur_ptr - msg->pkt;

    /* Pack the SLM header. */
    _SHR_PACK_U8(cur_ptr, lmep->endpoint.level << 5);
    if (slm->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        _SHR_PACK_U8(cur_ptr, SHR_ETH_OAM_OPCODE_SLM);
    } else {
        _SHR_PACK_U8(cur_ptr, SHR_ETH_OAM_OPCODE_1SL);
    }
    _SHR_PACK_U8(cur_ptr, 0);  /* Flags */
    _SHR_PACK_U8(cur_ptr, 16); /* TLV offset */

    _SHR_PACK_U16(cur_ptr, lmep->endpoint.name); /* Src MEP ID. */
    _SHR_PACK_U16(cur_ptr, 0); /* Responder MEP ID. */
    _SHR_PACK_U32(cur_ptr, slm->test_id);

    _SHR_PACK_U32(cur_ptr, 0); /* TcFcf */
    _SHR_PACK_U32(cur_ptr, 0); /* TcFcb */

    _SHR_PACK_U8(cur_ptr, 0); /* End TLV */

    msg->pkt_length = (cur_ptr - msg->pkt) + SHR_ETH_OAM_CRC_LENGTH;
    if (msg->pkt_length < SHR_ETH_OAM_TX_PKT_MIN_LENGTH) {
        msg->pkt_length = SHR_ETH_OAM_TX_PKT_MIN_LENGTH;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_event_masks_get
 * Purpose:
 *      Get the LMEP/RMEP event mask with the event.
 * Parameters:
 *      event         - (IN)  Event type.
 *      lmep_mask     - (OUT) LMEP event mask.
 *      rmep_mask     - (OUT) RMEP event mask.
 * Returns:
 *      None
 * Notes:
 */
STATIC void
_bcm_mo_eth_oam_event_masks_get(bcm_oam_event_type_t event,
                                uint32 *lmep_mask, uint32 *rmep_mask)
{
    int i;

    /* Check LMEP events */
    for (i = 0; i < SHR_ETH_OAM_LMEP_EVENTS_COUNT; i++) {
        if (event == g_lmep_event_map[i]) {
            _BIT_SET(*lmep_mask, i);
            return;
        }
    }

    /* Check RMEP events */
    for (i = 0; i < SHR_ETH_OAM_RMEP_EVENTS_COUNT; i++) {
        if (event == g_rmep_event_map[i]) {
            _BIT_SET(*rmep_mask, i);
            return;
        }
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_event_masks_update
 * Purpose:
 *      Get the global event masks and send it to uC.
 * Parameters:
 *      unit          - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_event_masks_update(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_event_handler_t *hdlr;
    uint32 lmep_mask = 0, rmep_mask = 0;

    for (hdlr = eoc->event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        lmep_mask |= hdlr->lmep_mask;
        rmep_mask |= hdlr->rmep_mask;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_event_masks_set(unit, lmep_mask, rmep_mask));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_lmep_event_process
 * Purpose:
 *      Process LMEP events from the uC
 * Parameters:
 *      unit   - (IN) Unit number.
 *      id     - (IN) LMEP/Group ID.
 *      events - (IN) event bitmap.
 * Returns:
 *      None
 */
STATIC void
_bcm_mo_eth_oam_lmep_event_process(int unit, uint16 id, uint32 events)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_event_handler_t *hdlr;
    uint32 masked_events;
    int event;

    lmep = &(eoc->lmeps[id]);

    for (hdlr = eoc->event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        /* Find if there is any event in the bitmap registered on this callback. */
        masked_events = hdlr->lmep_mask & events;
        if (masked_events != 0) {
            /*
             * Some event in the bitmap is registered using this handler,
             * iterate and find out.
             */
            for (event = 0; event <  SHR_ETH_OAM_LMEP_EVENTS_COUNT; event++) {
                if (_BIT_GET(hdlr->lmep_mask & events, event)) {
                    /* Invoke the callback. */
                    hdlr->cb(unit, 0, g_lmep_event_map[event],
                             id, lmep->endpoint.id, hdlr->user_data);
                }
            }
        }
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_rmep_event_process
 * Purpose:
 *      Process RMEP events from the uC
 * Parameters:
 *      unit   - (IN) Unit number.
 *      id     - (IN) RMEP ID.
 *      events - (IN) event bitmap.
 * Returns:
 *      None
 */
STATIC void
_bcm_mo_eth_oam_rmep_event_process(int unit, uint16 id, uint32 events)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_event_handler_t *hdlr;
    uint32 masked_events;
    int event;

    rmep = &(eoc->rmeps[id]);

    for (hdlr = eoc->event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        /* Find if there is any event in the bitmap registered on this callback. */
        masked_events = hdlr->rmep_mask & events;
        if (masked_events != 0) {
            /*
             * Some event in the bitmap is registered using this handler,
             * iterate and find out.
             */
            for (event = 0; event <  SHR_ETH_OAM_RMEP_EVENTS_COUNT; event++) {
                if (_BIT_GET(hdlr->rmep_mask & events, event)) {
                    /* Invoke the callback. */
                    hdlr->cb(unit, 0, g_rmep_event_map[event],
                             rmep->group_id, rmep->id, hdlr->user_data);
                }
            }
        }
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_pm_rstats_slm_unpack
 * Purpose:
 *      Unpack a SLM sample.
 * Parameters:
 *      buf     - (IN)  Pointer to the buffer to unpack
 *      sample  - (OUT) Unpacked sample
 * Returns:
 *      Pointer after unpacking SLM sample
 */
STATIC uint8 *
_bcm_mo_eth_oam_pm_rstats_slm_unpack(uint8 *buf, bcm_oam_pm_raw_samples_t *sample)
{
    uint8 *cur_ptr = buf;
    uint32 tx_fcf, tx_fcb;

    _SHR_UNPACK_U32(cur_ptr, tx_fcf);
    _SHR_UNPACK_U32(cur_ptr, tx_fcb);

    if (sample->lm_count >= BCM_OAM_PM_RAW_DATA_MAX_SAMPLES) {
        /* This should never happen. */
        return cur_ptr;
    }

    sample->raw_counter[sample->lm_count].tx_fcf = tx_fcf;
    sample->raw_counter[sample->lm_count].tx_fcb = tx_fcb;
    sample->lm_count++;

    return cur_ptr;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_pm_rstats_process
 * Purpose:
 *      Process PM raw stats event.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      length  - (IN) Length of the raw data.
 * Returns:
 *      None
 */
STATIC void
_bcm_mo_eth_oam_pm_rstats_process(int unit, uint16 length)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_pm_event_handler_t *hdlr;
    bcm_oam_pm_raw_data_t *raw_data = eoc->pm_raw_data;
    bcm_oam_pm_raw_buffer_t *raw_buffer;
    uint8 *cur_ptr, *tlv_ptr;
    uint32 version;
    uint8 type;
    uint16 tlv_length;
    uint16 id;

    /* Copy to local buffer from DMA buffer. */
    sal_memcpy(eoc->pm_rstats_buffer, eoc->pm_dma_buffer, length);

    if (raw_data == NULL) {
        return;
    }

    if (((eoc->pm_write_idx + 1) % BCM_OAM_MAX_PM_BUFFERS) == eoc->pm_read_idx) {
        /* No free buffers. */
        return;
    }
    raw_data->write_index = (eoc->pm_write_idx + 1) % BCM_OAM_MAX_PM_BUFFERS;
    raw_data->read_index  = eoc->pm_read_idx;

    raw_buffer = &(raw_data->raw_buffer[raw_data->write_index]);
    sal_memset(raw_buffer, 0, sizeof(*raw_buffer));

    cur_ptr = eoc->pm_rstats_buffer;

    _SHR_UNPACK_U32(cur_ptr, version);
    if (version != SHR_ETH_OAM_PM_RSTATS_VERSION) {
        /* Incorrect version number. */
        _LOG_ERROR(unit, "Incorrect version number");
        return;
    }
    _SHR_UNPACK_U32(cur_ptr, raw_buffer->seq_num);

    tlv_ptr = cur_ptr;
    _SHR_UNPACK_U8(cur_ptr, type);
    while (type != SHR_ETH_OAM_PM_RSTATS_TYPE_END) {
        switch (type) {
            case SHR_ETH_OAM_PM_RSTATS_TYPE_SLM:
                _SHR_UNPACK_U16(cur_ptr, tlv_length);
                _SHR_UNPACK_U16(cur_ptr, id);

                raw_buffer->raw_sample[id].pm_id = id;
                (void) _bcm_mo_eth_oam_pm_rstats_slm_unpack(cur_ptr,
                                                            &(raw_buffer->raw_sample[id]));
                break;

            default:
                _SHR_UNPACK_U16(cur_ptr, tlv_length);
                break;
        }

        /* Advance the pointers. */
        tlv_ptr += tlv_length;
        cur_ptr = tlv_ptr;
        _SHR_UNPACK_U8(cur_ptr, type);
    }

    eoc->pm_write_idx = raw_data->write_index;

    for (hdlr = eoc->pm_event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        /* Invoke the callback. */
        hdlr->cb(unit, bcmOAMEventEthLmDmRawData, raw_data, hdlr->user_data);
    }

}

/*
 * Function:
 *      _bcm_mo_eth_oam_event_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uC.
 * Parameters:
 *      param - (IN) Unit number.
 * Returns:
 *      None
 */
STATIC void
_bcm_mo_eth_oam_event_callback_thread(void *param)
{
    int unit = PTR_TO_INT(param);
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    mos_msg_data_t msg;
    uint16 id;
    uint32 events;
    int rv;

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(unit, eoc->uC,
                                     MOS_MSG_CLASS_ETH_OAM_EVENT, &msg,
                                     sal_sem_FOREVER);
        if (BCM_FAILURE(rv)) {
            break;
        }

        switch (msg.s.subclass) {
            case MOS_MSG_SUBCLASS_ETH_OAM_EVENT_LMEP:
                id     = bcm_ntohs(msg.s.len);
                events = bcm_ntohl(msg.s.data);
                if (id > eoc->max_groups) {
                    /* Invalid LMEP ID. */
                    continue;
                }
                _bcm_mo_eth_oam_lmep_event_process(unit, id, events);
                break;

            case MOS_MSG_SUBCLASS_ETH_OAM_EVENT_RMEP:
                id     = bcm_ntohs(msg.s.len);
                events = bcm_ntohl(msg.s.data);
                if (id > eoc->max_rmeps) {
                    /* Invalid RMEP ID. */
                    continue;
                }
                _bcm_mo_eth_oam_rmep_event_process(unit, id, events);
                break;

            case MOS_MSG_SUBCLASS_ETH_OAM_PM_RSTATS:
                _bcm_mo_eth_oam_pm_rstats_process(unit, bcm_ntohs(msg.s.len));
                break;

            default:
                /* Unknown event. */
                continue;
        }

    }

    eoc->event_thread_id = NULL;
    sal_thread_exit(0);
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_period_validate
 * Purpose:
 *      Validate if the period is one of the supported periods.
 * Parameters:
 *      period     - (IN) Period to be validated.
 *      min_period - (IN) Minimum possible period
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_period_validate(int period, int min_period)
{
    switch(period) {
        case BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED:
            break;

        case BCM_OAM_ENDPOINT_CCM_PERIOD_3MS:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10MS:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_100MS:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1S:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10S:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1M:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10M:
            if (period < min_period) {
                return BCM_E_PARAM;
            }
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_mep_id_validate
 * Purpose:
 *      Get the number of MEPs with a particular MEP ID in the group.
 * Parameters:
 *      group_id  - (IN) Group in which to search.
 *      mep_id    - (IN) MEP ID to search
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_mep_id_validate(int unit, bcm_oam_group_t group_id, uint16 mep_id)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep = &(eoc->lmeps[group_id]);
    int count = 0;
    int i;

    if ((mep_id < _BCM_MO_ETH_OAM_MEP_ID_MIN) ||
        (mep_id > _BCM_MO_ETH_OAM_MEP_ID_MAX)) {
        _LOG_ERROR(unit, "Invalid MEP-ID");
        return BCM_E_PARAM;
    }

    if (lmep->endpoint.in_use) {
        if (lmep->endpoint.name == mep_id) {
            count++;
        }
    }

     for (i = 0; i < eoc->num_rmeps; i++) {
         if (eoc->rmeps[i].in_use) {
             if (eoc->rmeps[i].group_id == group_id) {
                 if (eoc->rmeps[i].name == mep_id) {
                     count++;
                 }
             }
         }
     }

     if (count > 0) {
        _LOG_ERROR(unit, "MEP-ID not unique");
         return BCM_E_PARAM;
     }

     return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_type_get
 * Purpose:
 *      Get the type of an endpoint.
 * Parameters:
 *      flags     - (IN) Endpoint flags
 *      vlan      - (IN) Outer vlan
 * Returns:
 *      Endpoint type
 * Notes:
 */
STATIC uint8
_bcm_mo_eth_oam_endpoint_type_get(uint32 flags, bcm_vlan_t vlan)
{
    if (flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN){
        return SHR_ETH_OAM_MEP_TYPE_C_S_VLAN;
    } else if (flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
        return SHR_ETH_OAM_MEP_TYPE_CVLAN;
    } else if (vlan > 0) {
        return SHR_ETH_OAM_MEP_TYPE_SVLAN;
    } else {
        return SHR_ETH_OAM_MEP_TYPE_PORT;
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_port_get
 * Purpose:
 *      Get the port or TID from gport
 * Parameters:
 *      gport       - (IN)  Gport
 *      port        - (OUT) Port/Trunk ID
 *      is_trunk    - (IN)  1 - Port is trunk
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_mo_eth_oam_port_get(bcm_gport_t gport, uint16 *port, int *is_trunk)
{
    *is_trunk = 0;

    if (BCM_GPORT_IS_TRUNK(gport)) {
        *port = BCM_GPORT_TRUNK_GET(gport);
        *is_trunk = 1;
    } else if (BCM_GPORT_MODPORT_PORT_GET(gport)) {
        *port = BCM_GPORT_MODPORT_PORT_GET(gport);
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        *port = BCM_GPORT_LOCAL_GET(gport);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_key_construct
 * Purpose:
 *      Construct the key for an endpoint to check if it is a duplicate
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      type        - (IN)  Endpoint type
 *      gport       - (IN)  Gport
 *      outer_vlan  - (IN)  Outer Vlan
 *      inner_vlan  - (IN)  Inner Vlan
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_key_construct(int unit, uint8 type, bcm_gport_t gport,
                                       bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                       uint8 *key)
{
    uint8 *cur_ptr = key;
    uint16 port = 0;
    int is_trunk;

    sal_memset(key, 0, _BCM_MO_ETH_OAM_KEY_LENGTH);

    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_port_get(gport, &port, &is_trunk));
    if (is_trunk) {
        port |= (1 << 15);
    }

    switch (type) {
        case SHR_ETH_OAM_MEP_TYPE_PORT:
            _SHR_PACK_U16(cur_ptr, port);
            break;

        case SHR_ETH_OAM_MEP_TYPE_SVLAN:
            _SHR_PACK_U16(cur_ptr, port);
            _SHR_PACK_U16(cur_ptr, outer_vlan);
            break;

        case SHR_ETH_OAM_MEP_TYPE_CVLAN:
            _SHR_PACK_U16(cur_ptr, port);
            _SHR_PACK_U16(cur_ptr, inner_vlan);
            break;

        case SHR_ETH_OAM_MEP_TYPE_C_S_VLAN:
            _SHR_PACK_U16(cur_ptr, port);
            _SHR_PACK_U16(cur_ptr, outer_vlan);
            _SHR_PACK_U16(cur_ptr, inner_vlan);
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_duplicate_check
 * Purpose:
 *      Check if the endpoint is a duplicate of an already created endpoint.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      endpoint_info - (IN)  Endpoint Info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_duplicate_check(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    uint8 type;
    uint8 key1[_BCM_MO_ETH_OAM_KEY_LENGTH];
    uint8 key2[_BCM_MO_ETH_OAM_KEY_LENGTH];
    int i;

    type = _bcm_mo_eth_oam_endpoint_type_get(endpoint_info->flags,
                                             endpoint_info->vlan);

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_key_construct(unit, type, endpoint_info->gport,
                                                endpoint_info->vlan,
                                                endpoint_info->inner_vlan, key1));

    for (i = 0; i < eoc->max_groups; i++) {
        lmep = &(eoc->lmeps[i]);
        if (lmep->endpoint.in_use == 0) {
            continue;
        }

        type = _bcm_mo_eth_oam_endpoint_type_get(lmep->endpoint.flags,
                                                 lmep->endpoint.vlan);

        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_endpoint_key_construct(unit, type, lmep->endpoint.gport,
                                                    lmep->endpoint.vlan,
                                                    lmep->endpoint.inner_vlan, key2));

        if (sal_memcmp(key1, key2, _BCM_MO_ETH_OAM_KEY_LENGTH) == 0) {
            _LOG_ERROR(unit, "Duplicate endpoint exists");
            return BCM_E_EXISTS;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function: bcm_mo_eth_oam_pm_init
 *
 * Purpose:
 *     Initialize the PM sub- module.
 * Parameters:
 *     unit    - (IN)  BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_pm_init(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    if (eoc->pm_mode != SHR_ETH_OAM_PM_MODE_RAW) {
        return BCM_E_NONE;
    }

    if (eoc->max_slm_sessions > BCM_OAM_MAX_PM_ENDPOINTS) {
        return BCM_E_RESOURCE;
    }
    eoc->pm_dma_buffer_len = SHR_ETH_OAM_PM_RSTATS_BUF_SIZE(eoc->max_slm_sessions);
    eoc->pm_dma_buffer = soc_cm_salloc(unit, eoc->pm_dma_buffer_len,
                                       "ETH-OAM PM DMA buffer");
    if (eoc->dma_buffer == NULL) {
        return BCM_E_MEMORY;
    }

    _BCM_ETH_OAM_ALLOC(eoc->pm_raw_data, bcm_oam_pm_raw_data_t,
                       sizeof(bcm_oam_pm_raw_data_t),
                       "ETH-OAM PM raw data");
    if (eoc->pm_raw_data == NULL) {
        return BCM_E_MEMORY;
    }

    _BCM_ETH_OAM_ALLOC(eoc->pm_rstats_buffer, uint8,
                       eoc->pm_dma_buffer_len,
                       "ETH-OAM PM temp buffer");
    if (eoc->pm_raw_data == NULL) {
        return BCM_E_MEMORY;
    }

    eoc->pm_read_idx  = _BCM_MO_ETH_OAM_PM_RAW_DATA_INVALID_IDX;
    eoc->pm_write_idx = _BCM_MO_ETH_OAM_PM_RAW_DATA_INVALID_IDX;

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_mo_eth_oam_group_update
 * Purpose:
 *      Update an ETH-OAM group object
 * Parameters:
 *      unit       - (IN)     Unit number.
 *      group_info - (IN/OUT) Pointer to an OAM group structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_group_update(int unit, bcm_oam_group_info_t *group_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_lmep_set_t msg;
    _bcm_mo_eth_oam_lmep_t *lmep;
    int cur_rdi, new_rdi;

    lmep = &(eoc->lmeps[group_info->id]);

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_F_UPDATE;

    if (sal_memcmp(lmep->group.name, group_info->name, BCM_OAM_GROUP_NAME_LENGTH) != 0) {
        /* MEG-ID cannot change. */
        _LOG_ERROR(unit, "Cannot modify MEG-ID");
        return BCM_E_PARAM;
    }


    /* Check what flags have changed. */
    cur_rdi = lmep->group.flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX ? 1 : 0;
    new_rdi = group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX ? 1 : 0;

    if (cur_rdi != new_rdi) {
        msg.sw_rdi = new_rdi;
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_SW_RDI;
    }
    lmep->group.flags = _BCM_MO_ETH_OAM_GROUP_FILTER_FLAGS(group_info->flags);


    if (lmep->group.lowest_alarm_priority != group_info->lowest_alarm_priority) {
        msg.alarm_mask = _bcm_mo_eth_oam_alarm_mask_convert(group_info->lowest_alarm_priority);
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_ALARM_MASK;
        lmep->group.lowest_alarm_priority = group_info->lowest_alarm_priority;
    }

    if (lmep->group.clear_persistent_faults != group_info->clear_persistent_faults) {
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_STICKY_FAULTS_CLR;
        msg.sticky_faults_clear =
            _bcm_mo_eth_oam_lmep_faults_convert(group_info->clear_persistent_faults, 1);
    }

    /* Only send message if the LMEP in the uC is configured. */
    if (lmep->endpoint.in_use) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_lmep_set(unit, &msg));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_local_validate
 * Purpose:
 *      Validate a local ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_local_validate(int unit,
                                        bcm_oam_endpoint_info_t *endpoint_info)
{
    if (endpoint_info->flags & _BCM_MO_ETH_OAM_ENDPOINT_LOCAL_VALID_FLAGS_MASK) {
        _LOG_ERROR(unit, "Invalid flags set");
        return BCM_E_PARAM;
    }

    if ((endpoint_info->level < 0) || (endpoint_info->level > 7)) {
        _LOG_ERROR(unit, "Invalid MDL");
        return BCM_E_PARAM;
    }

    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_mep_id_validate(unit, endpoint_info->group,
                                             endpoint_info->name));
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_period_validate(endpoint_info->ccm_period,
                                                  BCM_OAM_ENDPOINT_CCM_PERIOD_3MS));

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) &&
        (endpoint_info->inner_vlan == 0)) {
        _LOG_ERROR(unit, "Invalid Vlan configuration");
        return BCM_E_PARAM;
    }

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) &&
        ((endpoint_info->inner_vlan == 0) || (endpoint_info->vlan == 0))) {
        _LOG_ERROR(unit, "Invalid Vlan configuration");
        return BCM_E_PARAM;
    }

    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_endpoint_duplicate_check(unit, endpoint_info));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_local_copy
 * Purpose:
 *      Copy the local endpoint details from API to local structure
 * Parameters:
 *      src   - (IN)  API
 *      dst   - (OUT) Local structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC void
_bcm_mo_eth_oam_endpoint_local_copy(bcm_oam_endpoint_info_t *src,
                                    _bcm_mo_eth_oam_endpoint_local_t *dst)
{
    sal_memset(dst, 0, sizeof(*dst));
    dst->in_use = 1;

    dst->id              = src->id;
    dst->flags           = _BCM_MO_ETH_OAM_ENDPOINT_FILTER_FLAGS(src->flags);
    dst->name            = src->name;
    dst->level           = src->level;
    dst->ccm_period      = src->ccm_period;
    dst->gport           = src->gport;
    dst->trunk_index     = src->trunk_index;
    dst->int_pri         = src->int_pri;

    if (src->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
        dst->port_state = src->port_state;
    }
    if (src->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {
        dst->interface_state = src->interface_state;
    }

    sal_memcpy(dst->dst_mac,
               src->dst_mac_address,
               sizeof(bcm_mac_t));

    sal_memcpy(dst->src_mac,
               src->src_mac_address,
               sizeof(bcm_mac_t));

    if (src->vlan > 0) {
        dst->vlan       = src->vlan;
        dst->outer_tpid = src->outer_tpid;
        dst->pkt_pri    = src->pkt_pri;
    }
    if (src->inner_vlan > 0) {
        dst->inner_vlan    = src->inner_vlan;
        dst->inner_tpid    = src->inner_tpid;
        dst->inner_pkt_pri = src->inner_pkt_pri;
    }
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_local_update
 * Purpose:
 *      Update a local ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_local_update(int unit,
                                      bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_endpoint_local_t ep_tmp1, ep_tmp2;
    shr_eth_oam_msg_ctrl_lmep_set_t msg;
    int ccm_session_change = 0;
    int rv;

    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint_info->id);
    if (rv != BCM_E_EXISTS) {
        _LOG_ERROR(unit, "Endpoint not found");
        return rv;
    }

    if ((eoc->endpoint_map[endpoint_info->id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL) ||
        (eoc->endpoint_map[endpoint_info->id].int_id != endpoint_info->group)) {
        _LOG_ERROR(unit, "Not a local endpoint");
        return BCM_E_PARAM;
    }


    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_local_validate(unit, endpoint_info));

    lmep = &(eoc->lmeps[endpoint_info->group]);
    sal_memcpy(&ep_tmp1, &(lmep->endpoint), sizeof(_bcm_mo_eth_oam_endpoint_local_t));

    sal_memset(&msg, 0, sizeof(msg));
    msg.id = endpoint_info->group;
    msg.flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_F_UPDATE;

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) !=
        (ep_tmp1.flags & BCM_OAM_ENDPOINT_PORT_STATE_TX)) {
        ep_tmp1.flags &= ~BCM_OAM_ENDPOINT_PORT_STATE_TX;
        ep_tmp1.flags |= (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX);
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
        if (ep_tmp1.port_state != endpoint_info->port_state) {
            ep_tmp1.port_state = endpoint_info->port_state;
            msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
        }
    }

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) !=
        (ep_tmp1.flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX)) {
        ep_tmp1.flags &= ~BCM_OAM_ENDPOINT_INTERFACE_STATE_TX;
        ep_tmp1.flags |= (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX);
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
    }

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {
        if (ep_tmp1.interface_state != endpoint_info->interface_state) {
            ep_tmp1.interface_state = endpoint_info->interface_state;
            msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
        }
    }

    if (ep_tmp1.pkt_pri != endpoint_info->pkt_pri) {
        if (ep_tmp1.vlan > 0) {
            ep_tmp1.pkt_pri = endpoint_info->pkt_pri;
            msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
        }
    }

    if (ep_tmp1.inner_pkt_pri != endpoint_info->inner_pkt_pri) {
        if (ep_tmp1.inner_vlan > 0) {
            ep_tmp1.inner_pkt_pri = endpoint_info->inner_pkt_pri;
            msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
        }
    }

    if (ep_tmp1.int_pri != endpoint_info->int_pri) {
        ep_tmp1.int_pri = endpoint_info->int_pri;
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
    }

    if (ep_tmp1.ccm_period != endpoint_info->ccm_period) {
        if (ep_tmp1.ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            if (eoc->num_tx_ccm_sessions >= eoc->max_ccm_sessions) {
                return BCM_E_RESOURCE;
            }
            ccm_session_change = 1;
        } else if (endpoint_info->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            ccm_session_change = -1;
        }

        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_TX;
        ep_tmp1.ccm_period = endpoint_info->ccm_period;

        msg.ccm_period = _bcm_mo_eth_oam_period_convert(ep_tmp1.ccm_period);
        msg.alarm_mask = _bcm_mo_eth_oam_alarm_mask_convert(lmep->group.lowest_alarm_priority);
        msg.sw_rdi = lmep->group.flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX ? 1 : 0;
    }

    if (ep_tmp1.trunk_index != endpoint_info->trunk_index) {
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT;
        ep_tmp1.trunk_index = endpoint_info->trunk_index;
    }

    /* No other field is modifiable. */
    _bcm_mo_eth_oam_endpoint_local_copy(endpoint_info, &ep_tmp2);
    if (sal_memcmp(&ep_tmp1, &ep_tmp2, sizeof(_bcm_mo_eth_oam_endpoint_local_t)) != 0) {
        /* Something that shouldn't change has changed. */
        _LOG_ERROR(unit, "Invalid parameter modification");
        return BCM_E_PARAM;
    }

    /* All validations passed, update the DB. */
    sal_memcpy(&(lmep->endpoint), &ep_tmp1, sizeof(_bcm_mo_eth_oam_endpoint_local_t));

    eoc->num_tx_ccm_sessions += ccm_session_change;

    if ((msg.update_flags & SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_TX) ||
        (msg.update_flags & SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_CCM_PKT)) {
        /* Reconstruct the packet. */
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_ccm_pkt_get(unit, lmep, &msg));
    }

    if (msg.update_flags != 0) {
        /* Send the message. */
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_lmep_set(unit, &msg));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_local_create
 * Purpose:
 *      Create a local ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN)     Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_local_create(int unit,
                                      bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    shr_eth_oam_msg_ctrl_lmep_set_t msg;
    bcm_port_t tx_port, rx_port;
    bcm_module_t mod_id;
    int is_trunk;
    int rv;

    lmep = &(eoc->lmeps[endpoint_info->group]);

    if (lmep->endpoint.in_use) {
        /* A local endpoint is already created in the group. */
        return BCM_E_EXISTS;
    }

    if (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        if (eoc->num_tx_ccm_sessions >= eoc->max_ccm_sessions) {
            _LOG_ERROR(unit, "No free CCM Tx resource");
            return BCM_E_RESOURCE;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_local_validate(unit, endpoint_info));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_gport_resolve(unit, endpoint_info->gport,
                                                endpoint_info->trunk_index,
                                                &is_trunk, &mod_id,
                                                &tx_port, &rx_port));

    /* All validations passed, populate the DB. */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint_info->id);
        if (rv != BCM_E_NOT_FOUND) {
            /* Another endpoint is already using the index. */
            _LOG_ERROR(unit, "ID in use");
            return rv;
        }
        rv = shr_idxres_list_reserve(eoc->endpoint_pool,
                                     endpoint_info->id, endpoint_info->id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = shr_idxres_list_alloc(eoc->endpoint_pool,
                                   (shr_idxres_element_t *)&(endpoint_info->id));
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Update the mapping table. */
    eoc->endpoint_map[endpoint_info->id].type   = _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL;
    eoc->endpoint_map[endpoint_info->id].int_id = endpoint_info->group;

    _bcm_mo_eth_oam_endpoint_local_copy(endpoint_info, &(lmep->endpoint));

    if (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        eoc->num_tx_ccm_sessions++;
    }

    /* Construct the LMEP set message. */
    sal_memset(&msg, 0, sizeof(msg));
    if (is_trunk) {
        msg.flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_F_TRUNK;
    }
    msg.id     = endpoint_info->group;
    msg.port   = rx_port;
    msg.svlan  = lmep->endpoint.vlan;
    msg.cvlan  = lmep->endpoint.inner_vlan;
    msg.mep_id = lmep->endpoint.name;
    msg.mdl    = lmep->endpoint.level;
    msg.type   = _bcm_mo_eth_oam_endpoint_type_get(lmep->endpoint.flags,
                                                   lmep->endpoint.vlan);

    msg.sticky_faults_clear =
        _bcm_mo_eth_oam_lmep_faults_convert(lmep->group.clear_persistent_faults, 1);
    memcpy(msg.meg_id, lmep->group.name, SHR_ETH_OAM_MEG_ID_LENGTH);

    msg.ccm_period = _bcm_mo_eth_oam_period_convert(lmep->endpoint.ccm_period);
    msg.alarm_mask = _bcm_mo_eth_oam_alarm_mask_convert(lmep->group.lowest_alarm_priority);
    msg.sw_rdi = lmep->group.flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX ? 1 : 0;


    if (endpoint_info->ccm_period > BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        /* Construct the CCM packet. */
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_ccm_pkt_get(unit, lmep, &msg));
    }

    /* Send the message. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_lmep_set(unit, &msg));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_local_get
 * Purpose:
 *      Get an ETH-OAM local endpoint object
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      endpoint      - (IN)  The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_local_get(int unit,
                                   bcm_oam_endpoint_t endpoint,
                                   bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;

    lmep = &(eoc->lmeps[eoc->endpoint_map[endpoint].int_id]);

    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info->id              = endpoint;
    endpoint_info->group           = eoc->endpoint_map[endpoint].int_id;
    endpoint_info->flags           = lmep->endpoint.flags;
    endpoint_info->name            = lmep->endpoint.name;
    endpoint_info->level           = lmep->endpoint.level;
    endpoint_info->ccm_period      = lmep->endpoint.ccm_period;
    endpoint_info->gport           = lmep->endpoint.gport;
    endpoint_info->trunk_index     = lmep->endpoint.trunk_index;
    endpoint_info->int_pri         = lmep->endpoint.int_pri;
    endpoint_info->port_state      = lmep->endpoint.port_state;
    endpoint_info->interface_state = lmep->endpoint.interface_state;

    sal_memcpy(endpoint_info->dst_mac_address,
               lmep->endpoint.dst_mac,
               sizeof(bcm_mac_t));

    sal_memcpy(endpoint_info->src_mac_address,
               lmep->endpoint.src_mac,
               sizeof(bcm_mac_t));

    endpoint_info->vlan          = lmep->endpoint.vlan;
    endpoint_info->outer_tpid    = lmep->endpoint.outer_tpid;
    endpoint_info->pkt_pri       = lmep->endpoint.pkt_pri;
    endpoint_info->inner_vlan    = lmep->endpoint.inner_vlan;
    endpoint_info->inner_tpid    = lmep->endpoint.inner_tpid;
    endpoint_info->inner_pkt_pri = lmep->endpoint.inner_pkt_pri;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_local_destroy
 * Purpose:
 *      Destroy an ETH-OAM local endpoint object
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the OAM endpoint object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_local_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    shr_eth_oam_msg_ctrl_lmep_del_t msg;
    int is_trunk;

    lmep = &(eoc->lmeps[eoc->endpoint_map[endpoint].int_id]);

    if (lmep->num_rmeps > 0) {
        /* RMEPs need to be destroyed first. */
        _LOG_ERROR(unit, "Remote endpoints need to be destroyed prior to local");
        return BCM_E_CONFIG;
    }

    if (lmep->endpoint.ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        eoc->num_tx_ccm_sessions--;
    }

    /* Send the LMEP delete message. */
    sal_memset(&msg, 0, sizeof(msg));
    msg.id = eoc->endpoint_map[endpoint].int_id;
    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_port_get(lmep->endpoint.gport, &msg.port, &is_trunk));
    if (is_trunk) {
        msg.flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_DEL_F_TRUNK;
    }
    msg.svlan = lmep->endpoint.vlan;
    msg.cvlan = lmep->endpoint.inner_vlan;
    msg.mdl   = lmep->endpoint.level;
    msg.type  = _bcm_mo_eth_oam_endpoint_type_get(lmep->endpoint.flags,
                                                  lmep->endpoint.vlan);


    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_lmep_del(unit, &msg));
    sal_memset(&(lmep->endpoint), 0, sizeof(lmep->endpoint));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_remote_validate
 * Purpose:
 *      Validate a remote ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_remote_validate(int unit,
                                         bcm_oam_endpoint_info_t *endpoint_info)
{
    if (endpoint_info->flags & _BCM_MO_ETH_OAM_ENDPOINT_REMOTE_VALID_FLAGS_MASK) {
        _LOG_ERROR(unit, "Invalid flag configuration");
        return BCM_E_PARAM;
    }

    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_mep_id_validate(unit, endpoint_info->group,
                                             endpoint_info->name));
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_period_validate(endpoint_info->ccm_period,
                                                  BCM_OAM_ENDPOINT_CCM_PERIOD_3MS));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_remote_update
 * Purpose:
 *      Update a remote ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN)     Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_remote_update(int unit,
                                       bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    shr_eth_oam_msg_ctrl_rmep_set_t msg;
    int ccm_session_change = 0;
    int rv;

    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint_info->id);
    if (rv != BCM_E_EXISTS) {
        _LOG_ERROR(unit, "Invalid endpoint ID");
        return rv;
    }

    if (eoc->endpoint_map[endpoint_info->id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE) {
        _LOG_ERROR(unit, "Endpoint not remote");
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_remote_validate(unit, endpoint_info));

    rmep = &(eoc->rmeps[eoc->endpoint_map[endpoint_info->id].int_id]);

    /* Check if something that cannot be updated has changed. */
    if (rmep->flags != _BCM_MO_ETH_OAM_ENDPOINT_FILTER_FLAGS(endpoint_info->flags)) {
        _LOG_ERROR(unit, "Invalid flag update");
        return BCM_E_PARAM;
    }

    if (rmep->group_id != endpoint_info->group) {
        _LOG_ERROR(unit, "Group cannot be updated");
        return BCM_E_PARAM;
    }
    if (rmep->name != endpoint_info->name) {
        _LOG_ERROR(unit, "MEP ID cannot be updated");
        return BCM_E_PARAM;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.id = eoc->endpoint_map[endpoint_info->id].int_id;
    msg.flags |= SHR_ETH_OAM_MSG_CTRL_RMEP_SET_F_UPDATE;

    if (rmep->ccm_period != endpoint_info->ccm_period) {
        if (rmep->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            if (eoc->num_rx_ccm_sessions >= eoc->max_ccm_sessions) {
                _LOG_ERROR(unit, "No free CCM Rx resource");
                return BCM_E_RESOURCE;
            }
            ccm_session_change = 1;
        } else if (endpoint_info->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            ccm_session_change = -1;
        }

        rmep->ccm_period = endpoint_info->ccm_period;

        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_RMEP_SET_UPDATE_F_CCM_PERIOD;
        msg.ccm_period = _bcm_mo_eth_oam_period_convert(endpoint_info->ccm_period);
    }

    if (rmep->clear_persistent_faults != endpoint_info->clear_persistent_faults) {
        rmep->clear_persistent_faults = endpoint_info->clear_persistent_faults;

        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_LMEP_SET_UPDATE_F_STICKY_FAULTS_CLR;
        msg.sticky_faults_clear =
            _bcm_mo_eth_oam_rmep_faults_convert(endpoint_info->clear_persistent_faults, 1);
    }

    eoc->num_rx_ccm_sessions += ccm_session_change;

    /* Send the message. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_rmep_set(unit, &msg));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_remote_create
 * Purpose:
 *      Create a remote ETH-OAM endpoint.
 * Parameters:
 *      unit          - (IN)     Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_remote_create(int unit,
                                       bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_rmep_t *rmep;
    shr_eth_oam_msg_ctrl_rmep_set_t msg;
    uint16 rmep_id;
    int rv;

    lmep = &(eoc->lmeps[endpoint_info->group]);

    if (eoc->num_rmeps >= eoc->max_rmeps) {
        _LOG_ERROR(unit, "No free RMEPs");
        return BCM_E_RESOURCE;
    }

    if (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        if (eoc->num_rx_ccm_sessions >= eoc->max_ccm_sessions) {
            _LOG_ERROR(unit, "No free CCM Rx resource");
            return BCM_E_RESOURCE;
        }
    }

    if (lmep->endpoint.in_use == 0) {
        /* Local endpoint needs to be created first before remote can be created. */
        _LOG_ERROR(unit, "Local endpoints need to be created prior to remote");
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_remote_validate(unit, endpoint_info));


    /* All validations passed, populate the DB. */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint_info->id);
        if (rv != BCM_E_NOT_FOUND) {
            /* Another endpoint is already using the index. */
            _LOG_ERROR(unit, "Endpoint ID is in use");
            return rv;
        }
        rv = shr_idxres_list_reserve(eoc->endpoint_pool,
                                     endpoint_info->id, endpoint_info->id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = shr_idxres_list_alloc(eoc->endpoint_pool,
                                   (shr_idxres_element_t *)&(endpoint_info->id));
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get an ID from the RMEP pool for internal storage and app messaging. */
    rv = shr_idxres_list_alloc(eoc->rmep_pool,
                               (shr_idxres_element_t *)&(rmep_id));
    BCM_IF_ERROR_RETURN(rv);

    /* Update the mapping table. */
    eoc->endpoint_map[endpoint_info->id].type   = _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE;
    eoc->endpoint_map[endpoint_info->id].int_id = rmep_id;

    /* Increment the counts. */
    eoc->num_rmeps++;
    lmep->num_rmeps++;
    if (endpoint_info->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        eoc->num_rx_ccm_sessions++;
    }

    /* Populate local DB. */
    rmep = &(eoc->rmeps[rmep_id]);
    sal_memset(rmep, 0, sizeof(*rmep));

    rmep->in_use                  = 1;
    rmep->id                      = endpoint_info->id;
    rmep->group_id                = endpoint_info->group;
    rmep->flags                   = _BCM_MO_ETH_OAM_ENDPOINT_FILTER_FLAGS(endpoint_info->flags);
    rmep->name                    = endpoint_info->name;
    rmep->ccm_period              = endpoint_info->ccm_period;
    rmep->clear_persistent_faults = endpoint_info->clear_persistent_faults;

    /* Construct the RMEP set message. */
    msg.id = rmep_id;
    msg.lmep_id = endpoint_info->group;
    msg.mep_id = endpoint_info->name;
    msg.ccm_period = _bcm_mo_eth_oam_period_convert(endpoint_info->ccm_period);
    msg.sticky_faults_clear =
        _bcm_mo_eth_oam_rmep_faults_convert(endpoint_info->clear_persistent_faults, 1);


    /* Send the message. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_rmep_set(unit, &msg));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_remote_get
 * Purpose:
 *      Get an ETH-OAM remote endpoint object
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      endpoint      - (IN)  The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_remote_get(int unit,
                                   bcm_oam_endpoint_t endpoint,
                                   bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    shr_eth_oam_msg_ctrl_rmep_get_t msg;
    uint16 rmep_id;

    rmep_id = eoc->endpoint_map[endpoint].int_id;
    rmep = &(eoc->rmeps[rmep_id]);

    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info->id                      = endpoint;
    endpoint_info->group                   = rmep->group_id;
    endpoint_info->flags                   = rmep->flags;
    endpoint_info->name                    = rmep->name;
    endpoint_info->ccm_period              = rmep->ccm_period;
    endpoint_info->clear_persistent_faults = rmep->clear_persistent_faults;

    if (rmep->ccm_period > BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        /* Get the faults. */
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_uc_msg_rmep_get(unit, rmep_id, &msg));

        endpoint_info->faults =
            _bcm_mo_eth_oam_rmep_faults_convert(msg.faults, 0);

        endpoint_info->persistent_faults =
            _bcm_mo_eth_oam_rmep_faults_convert(msg.sticky_faults, 0);

        endpoint_info->port_state      = msg.port_status;
        endpoint_info->interface_state = msg.intf_status;

        sal_memcpy(endpoint_info->src_mac_address, msg.src_mac, sizeof(bcm_mac_t));
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_oam_endpoint_remote_destroy
 * Purpose:
 *      Destroy an ETH-OAM remote endpoint object
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the OAM endpoint object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_endpoint_remote_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_rmep_t *rmep;

    rmep = &(eoc->rmeps[eoc->endpoint_map[endpoint].int_id]);
    lmep = &(eoc->lmeps[rmep->group_id]);

    if (rmep->num_slms > 0) {
        /* SLMs need to be destroyed first. */
        _LOG_ERROR(unit, "SLM need to destroyed prior to destroying Remote endpoint");
        return BCM_E_CONFIG;
    }

    if (rmep->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
        eoc->num_rx_ccm_sessions--;
    }


    /* Send the RMEP delete message. */
    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_rmep_del(unit,
                                         eoc->endpoint_map[endpoint].int_id));
    sal_memset(rmep, 0, sizeof(*rmep));

    lmep->num_rmeps--;
    eoc->num_rmeps--;
    BCM_IF_ERROR_RETURN
        (shr_idxres_list_free(eoc->rmep_pool,
                              eoc->endpoint_map[endpoint].int_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_loss_validate
 * Purpose:
 *        Validate loss measurement
 * Parameters:
 *     unit   - (IN) Device unit number
 *     loss_p - (IN) Loss information
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_mo_eth_oam_loss_validate(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int rv;

    if (loss_p->flags & _BCM_MO_ETH_OAM_LOSS_VALID_FLAGS_MASK) {
        _LOG_ERROR(unit, "Invalid flags");
        return BCM_E_PARAM;
    }

    if (!(loss_p->flags & BCM_OAM_LOSS_SLM)) {
        /* Only SLM is supported. */
        _LOG_ERROR(unit, "Only SLM is supported");
        return BCM_E_PARAM;
    }

    /* Validate period */
    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_period_validate(loss_p->period,
                                                  BCM_OAM_ENDPOINT_CCM_PERIOD_10MS));

    /* Check if the local and remote endpoints are created. */
    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, loss_p->id);
    if (rv != BCM_E_EXISTS) {
        _LOG_ERROR(unit, "Local endpoint not found");
        return rv;
    }
    if (eoc->endpoint_map[loss_p->id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL) {
        _LOG_ERROR(unit, "Local endpoint not found");
        return BCM_E_NOT_FOUND;
    }

    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, loss_p->remote_id);
    if (rv != BCM_E_EXISTS) {
        _LOG_ERROR(unit, "Remote endpoint not found");
        return rv;
    }
    if (eoc->endpoint_map[loss_p->remote_id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE) {
        _LOG_ERROR(unit, "Remote endpoint not found");
        return BCM_E_NOT_FOUND;
    }

    if (eoc->pm_mode == SHR_ETH_OAM_PM_MODE_PROCESSED) {
        if (loss_p->measurement_period > 0) {
            if (loss_p->measurement_period < 1000) {
                _LOG_ERROR(unit, "Invalid measurement period");
                return BCM_E_PARAM;
            }
            if (loss_p->period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
                if (loss_p->measurement_period < (10 * loss_p->period)) {
                    /* Minimum of 10 samples in an interval. */
                    _LOG_ERROR(unit, "Invalid measurement period");
                    return BCM_E_PARAM;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_loss_update
 * Purpose:
 *        Loss Measurement /update
 * Parameters:
 *     unit   - (IN) Device unit number
 *     loss_p - (IN) Loss information
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_mo_eth_oam_loss_update(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_slm_t *slm;
    shr_eth_oam_msg_ctrl_slm_set_t msg;
    int cur_period, new_period;
    int mac_chng;


    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_loss_validate(unit, loss_p));

    slm = &(eoc->slm_sessions[loss_p->loss_id]);

    /* Check if something that cannot be updated has changed. */
    if (_BCM_MO_ETH_OAM_LOSS_NON_UPDATE_FLAGS(slm->flags) !=
        _BCM_MO_ETH_OAM_LOSS_NON_UPDATE_FLAGS(loss_p->flags)) {
        _LOG_ERROR(unit, "Invalid flags modification");
        return BCM_E_PARAM;
    }

    if (slm->lmep_id != eoc->endpoint_map[loss_p->id].int_id) {
        _LOG_ERROR(unit, "Local endpoint ID cannot be modified");
        return BCM_E_PARAM;
    }

    if (slm->rmep_id != eoc->endpoint_map[loss_p->remote_id].int_id) {
        _LOG_ERROR(unit, "Remote endpoint ID cannot be modified");
        return BCM_E_PARAM;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_F_UPDATE;

    /* Find out what has changed. */
    if (slm->flags & BCM_OAM_LOSS_TX_ENABLE) {
        cur_period = slm->period;
    } else {
        cur_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
    }
    if (loss_p->flags & BCM_OAM_LOSS_TX_ENABLE) {
        new_period = loss_p->period;
    } else {
        new_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
    }

    if (cur_period != new_period) {
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_PERIOD;
        msg.period = _bcm_mo_eth_oam_period_convert(new_period);
    }

    if ((loss_p->flags & BCM_OAM_LOSS_TX_ENABLE) &&
        (new_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)) {
        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_ONE_SHOT;
        msg.one_shot = 1;
    }

    mac_chng = sal_memcmp(slm->peer_mac, loss_p->peer_da_mac_address, sizeof(bcm_mac_t));
    if ((slm->pkt_pri       != loss_p->pkt_pri)       ||
        (slm->inner_pkt_pri != loss_p->inner_pkt_pri) ||
        (slm->int_pri       != loss_p->int_pri)       ||
        (mac_chng           != 0)) {

        msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_PKT;
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_slm_pkt_get(unit, slm, &msg));
    }

    if (slm->measurement_period != loss_p->measurement_period) {
        if (eoc->pm_mode == SHR_ETH_OAM_PM_MODE_PROCESSED) {
            msg.update_flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_UPDATE_F_MEASUREMENT_INTERVAL;
            msg.measurement_interval_msecs = loss_p->measurement_period;
        }
    }

    /* Update the local DB. */
    slm->flags              = _BCM_MO_ETH_OAM_LOSS_FILTER_FLAGS(loss_p->flags);
    slm->period             = loss_p->period;
    slm->measurement_period = loss_p->measurement_period;
    slm->pkt_pri            = loss_p->pkt_pri;
    slm->inner_pkt_pri      = loss_p->inner_pkt_pri;
    slm->int_pri            = loss_p->int_pri;

    /* Send the message. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_slm_set(unit, &msg));

    return BCM_E_NONE;
}

/*
 * Function: bcm_mo_eth_oam_detach
 *
 * Purpose:
 *     Detach the ETH-OAM module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_mo_eth_oam_detach(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_event_handler_t *hdlr, *next_hdlr;
    _bcm_mo_eth_oam_pm_event_handler_t *pm_hdlr, *next_pm_hdlr;

    if (eoc == NULL) {
        return BCM_E_NONE;
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_warmboot(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_shutdown(unit));
    }


    /* Destory the ID pools */
    if (eoc->group_pool != NULL) {
       shr_idxres_list_destroy(eoc->group_pool);
       eoc->group_pool = NULL;
    }

    if (eoc->endpoint_pool != NULL) {
       shr_idxres_list_destroy(eoc->endpoint_pool);
       eoc->endpoint_pool = NULL;
    }

    if (eoc->lmeps != NULL) {
        sal_free(eoc->lmeps);
        eoc->lmeps = NULL;
    }

    if (eoc->rmeps != NULL) {
        sal_free(eoc->rmeps);
        eoc->rmeps = NULL;
    }

    if (eoc->endpoint_map != NULL) {
        sal_free(eoc->endpoint_map);
        eoc->endpoint_map = NULL;
    }

    /* Free the event handlers. */
    hdlr = eoc->event_handler_list;
    while (hdlr != NULL) {
        next_hdlr = hdlr->next;
        sal_free(hdlr);
        hdlr = next_hdlr;
    }

    if (eoc->dma_buffer != NULL) {
        soc_cm_sfree(unit, eoc->dma_buffer);
        eoc->dma_buffer = NULL;
    }

    /* Free PM data. */
    if (eoc->pm_dma_buffer != NULL) {
        soc_cm_sfree(unit, eoc->pm_dma_buffer);
        eoc->pm_dma_buffer = NULL;
    }

    if (eoc->pm_raw_data != NULL) {
        sal_free(eoc->pm_raw_data);
        eoc->pm_raw_data = NULL;
    }

    pm_hdlr = eoc->pm_event_handler_list;
    while (pm_hdlr != NULL) {
        next_pm_hdlr = pm_hdlr->next;
        sal_free(pm_hdlr);
        pm_hdlr = next_pm_hdlr;
    }

    sal_free(eoc);
    g_eth_oam_ctrl[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function: bcm_mo_eth_oam_init
 *
 * Purpose:
 *     Initialize the ETH-OAM module.
 * Parameters:
 *     unit    - (IN)  BCM device number
 *     success - (OUT) Indicates if INIT was successfull.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
_bcm_mo_eth_oam_init(int unit, int *success)
{
    _bcm_mo_eth_oam_ctrl_t *eoc;
    int rv = BCM_E_NONE;
    int uC;
    int num_ep;
#ifdef BCM_WARM_BOOT_SUPPORT
    int init = 0;
#endif /* BCM_WARM_BOOT_SUPPORT */

    *success = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_wb_init_status_recover(unit, &init));

        if (init == 0) {
            /* Module was not initialized prior to WB, skip initializing it now */
            return BCM_E_NONE;
        }
    } else {
        rv = _bcm_mo_eth_oam_init_status_scache_alloc(unit, 1);
        if (BCM_FAILURE(rv)) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Send the application init message and find the core where the app
     * is loaded.
     */
    _bcm_mo_eth_oam_uc_appl_init(unit, &uC);

    if (uC == _BCM_MO_ETH_OAM_INVALID_UC) {
        /* App is not loaded, silently return. */
        return BCM_E_NONE;
    }

    if (g_eth_oam_ctrl[unit] != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        g_eth_oam_ctrl[unit] = NULL;
    }

    _BCM_ETH_OAM_ALLOC(g_eth_oam_ctrl[unit], _bcm_mo_eth_oam_ctrl_t,
                       sizeof(_bcm_mo_eth_oam_ctrl_t), "ETH-OAM_INFO");
    if (g_eth_oam_ctrl[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    eoc = g_eth_oam_ctrl[unit];

    eoc->uC = uC;

    /* Read soc properties. */
    eoc->max_groups = soc_property_get(unit,
                                       spn_ETH_OAM_MAX_GROUPS,
                                       128);
    if (eoc->max_groups == 0) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_CONFIG;
    }

    eoc->max_rmeps = soc_property_get(unit,
                                      spn_ETH_OAM_MAX_RMEPS, 128);
    if (eoc->max_rmeps == 0) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_CONFIG;
    }

    eoc->max_ccm_sessions = soc_property_get(unit,
                                             spn_ETH_OAM_MAX_CCM_SESSIONS,
                                             128);

    eoc->max_slm_sessions = soc_property_get(unit,
                                             spn_ETH_OAM_MAX_LM_SESSIONS,
                                             128);

    switch (soc_property_get(unit, spn_ETH_LMDM_DATA_COLLECTION_MODE, 0)) {
        case 0:
            eoc->pm_mode = SHR_ETH_OAM_PM_MODE_NONE;
            break;

        case 1:
            eoc->pm_mode = SHR_ETH_OAM_PM_MODE_PROCESSED;
            break;

        case 2:
            eoc->pm_mode = SHR_ETH_OAM_PM_MODE_RAW;
            break;

        default:
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
            return BCM_E_PARAM;
    }

    eoc->dma_buffer_len = sizeof(shr_eth_oam_msg_ctrl_t);
    eoc->dma_buffer = soc_cm_salloc(unit, eoc->dma_buffer_len,
                                    "ETH-OAM DMA buffer");
    if (eoc->dma_buffer == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(eoc->dma_buffer, 0, eoc->dma_buffer_len);


    rv = _bcm_mo_eth_oam_pm_init(unit);
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return rv;
    }

    /* Create group pool */
    rv = shr_idxres_list_create(&(eoc->group_pool),
                                0, eoc->max_groups - 1,
                                0, eoc->max_groups - 1,
                                "ETH-OAM group pool");
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return rv;
    }

    /* Create endpoint pool. Endpoints can be local or remote.  */
    num_ep = eoc->max_groups + eoc->max_rmeps;
    rv = shr_idxres_list_create(&(eoc->endpoint_pool),
                                0, num_ep,
                                0, num_ep,
                                "ETH-OAM endpoint pool");
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return rv;
    }

    /* Create RMEP pool */
    rv = shr_idxres_list_create(&(eoc->rmep_pool),
                                0, eoc->max_rmeps - 1,
                                0, eoc->max_rmeps - 1,
                                "ETH-OAM RMEP pool");
    if (rv != BCM_E_NONE) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return rv;
    }

    /* Create SLM pool */
    if (eoc->max_slm_sessions > 0) {
        rv = shr_idxres_list_create(&(eoc->slm_pool),
                                    0, eoc->max_slm_sessions - 1,
                                    0, eoc->max_slm_sessions - 1,
                                    "ETH-OAM SLM pool");
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
            return rv;
        }
    }

    /* Alloocate memory for various tables. */
    _BCM_ETH_OAM_ALLOC(eoc->lmeps, _bcm_mo_eth_oam_lmep_t,
                       eoc->max_groups * sizeof(_bcm_mo_eth_oam_lmep_t),
                       "ETH-OAM LMEP table");
    if (eoc->lmeps == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_MEMORY;
    }

    _BCM_ETH_OAM_ALLOC(eoc->rmeps, _bcm_mo_eth_oam_rmep_t,
                       eoc->max_rmeps * sizeof(_bcm_mo_eth_oam_rmep_t),
                       "ETH-OAM RMEP table");
    if (eoc->rmeps == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_MEMORY;
    }

    _BCM_ETH_OAM_ALLOC(eoc->endpoint_map, _bcm_mo_eth_oam_endpoint_map_t,
                       num_ep * sizeof(_bcm_mo_eth_oam_endpoint_map_t),
                       "ETH-OAM EP map table");
    if (eoc->endpoint_map == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
        return BCM_E_MEMORY;
    }

    if (eoc->max_slm_sessions > 0) {
        _BCM_ETH_OAM_ALLOC(eoc->slm_sessions, _bcm_mo_eth_oam_slm_t,
                           eoc->max_slm_sessions * sizeof(_bcm_mo_eth_oam_slm_t),
                           "ETH-OAM SLM table");
        if (eoc->slm_sessions == NULL) {
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
            return BCM_E_MEMORY;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_mo_eth_oam_wb_cfg_recover(unit);
    } else {
        rv = _bcm_mo_eth_oam_cfg_scache_alloc(unit, 1);
    }
    if (BCM_FAILURE(rv)) {
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Send the init message to the app. */
    if (!SOC_WARM_BOOT(unit)) {
        rv = _bcm_mo_eth_oam_uc_msg_init(unit);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
            return rv;
        }
    }

    /* Start event message callback thread */
    if (eoc->event_thread_id == NULL) {
        eoc->event_thread_id =  sal_thread_create("bcmCcmSLm",
                                                  SAL_THREAD_STKSZ,
                                                  _BCM_MO_ETH_OAM_EVENT_THREAD_PRIO,
                                                 _bcm_mo_eth_oam_event_callback_thread,
                                                  INT_TO_PTR(unit));
        if (eoc->event_thread_id == SAL_THREAD_ERROR) {
            eoc->event_thread_id = NULL;
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_detach(unit));
            return (BCM_E_MEMORY);
        }
    }


    *success = 1;
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_create
 * Purpose:
 *      Create or replace an ETH-OAM endpoint object
 * Parameters:
 *      unit          - (IN)     Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an OAM endpoint structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_endpoint_create(int unit,
                                bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int with_id, replace, remote;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (endpoint_info == NULL) {
        return BCM_E_PARAM;
    }

    if (endpoint_info->type != bcmOAMEndpointTypeEthernet) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(eoc->group_pool, endpoint_info->group);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    replace = (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) ? 1 : 0;
    with_id = (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) ? 1 : 0;
    remote  = (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)  ? 1 : 0;

    if (replace) {
        if (!with_id) {
            return BCM_E_PARAM;
        }

        if (remote) {
            return _bcm_mo_eth_oam_endpoint_remote_update(unit, endpoint_info);
        } else {
            return _bcm_mo_eth_oam_endpoint_local_update(unit, endpoint_info);
        }
    } else {
        if (remote) {
            return _bcm_mo_eth_oam_endpoint_remote_create(unit, endpoint_info);
        } else {
            return _bcm_mo_eth_oam_endpoint_local_create(unit, endpoint_info);
        }
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_get
 * Purpose:
 *      Get an ETH-OAM endpoint object
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      endpoint      - (IN)  The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_endpoint_get(int unit,
                             bcm_oam_endpoint_t endpoint,
                             bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (endpoint_info == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    switch (eoc->endpoint_map[endpoint].type) {
        case _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL:
            _bcm_mo_eth_oam_endpoint_local_get(unit, endpoint, endpoint_info);
            break;

        case _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE:
            _bcm_mo_eth_oam_endpoint_remote_get(unit, endpoint, endpoint_info);
            break;

        default:
            return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_destroy
 * Purpose:
 *      Destroy an ETH-OAM endpoint object
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the OAM endpoint object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(eoc->endpoint_pool, endpoint);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    switch (eoc->endpoint_map[endpoint].type) {
        case _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL:
            BCM_IF_ERROR_RETURN
                (_bcm_mo_eth_oam_endpoint_local_destroy(unit, endpoint));
            break;

        case _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE:
            BCM_IF_ERROR_RETURN
                (_bcm_mo_eth_oam_endpoint_remote_destroy(unit, endpoint));
            break;

        default:
            return BCM_E_NOT_FOUND;
    }

    eoc->endpoint_map[endpoint].type = _BCM_MO_ETH_OAM_ENDPOINT_TYPE_NONE;

    BCM_IF_ERROR_RETURN(shr_idxres_list_free(eoc->endpoint_pool, endpoint));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_destroy_all
 * Purpose:
 *      Destroy all ETH-OAM endpoint objects associated with a given OAM
 *      group
 * Parameters:
 *      unit  - (IN) Unit number.
 *      group - (IN) The OAM group whose endpoints should be destroyed
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    int i;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(eoc->group_pool, group);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    /* Destroy RMEPs. */
    for (i = 0; i < eoc->num_rmeps; i++) {
        if ((eoc->rmeps[i].in_use) && (eoc->rmeps[i].group_id == group)) {
            BCM_IF_ERROR_RETURN
                (_bcm_mo_eth_oam_endpoint_destroy(unit, eoc->rmeps[i].id));
        }
    }

    /* Destroy LMEP. */
    lmep = &(eoc->lmeps[group]);
    if (lmep->endpoint.in_use) {
        BCM_IF_ERROR_RETURN
            (_bcm_mo_eth_oam_endpoint_destroy(unit, lmep->endpoint.id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_endpoint_traverse
 * Purpose:
 *      Traverse the set of ETH-OAM endpoints associated with the
 *      specified group, calling a specified callback for each one
 * Parameters:
 *      unit      - (IN) Unit number.
 *      group     - (IN) The OAM group whose endpoints should be traversed
 *      cb        - (IN) A pointer to the callback function to call for each
 *                       OAM endpoint in the specified group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_endpoint_traverse(int unit,
                                  bcm_oam_group_t group,
                                  bcm_oam_endpoint_traverse_cb cb,
                                  void *user_data)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    bcm_oam_endpoint_info_t endpoint_info;
    int rv;
    int i;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(eoc->group_pool, group);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    lmep = &(eoc->lmeps[group]);
    if (lmep->endpoint.in_use == 0) {
        /* No local endpoint, implying no remote endpoints either. */
        return BCM_E_NONE;
    }

    /* Get local endpoint. */
    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_endpoint_get(unit, lmep->endpoint.id, &endpoint_info));
    BCM_IF_ERROR_RETURN(cb(unit, &endpoint_info, user_data));

    /* Get remote endpoints. */
    for (i = 0; i < eoc->num_rmeps; i++) {
        if ((eoc->rmeps[i].in_use) && (eoc->rmeps[i].group_id == group)) {
            BCM_IF_ERROR_RETURN
                (_bcm_mo_eth_oam_endpoint_get(unit, eoc->rmeps[i].id, &endpoint_info));
            BCM_IF_ERROR_RETURN(cb(unit, &endpoint_info, user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_group_create
 * Purpose:
 *      Create or replace an ETH-OAM group object
 * Parameters:
 *      unit - (IN) Unit number.
 *      group_info - (IN/OUT) Pointer to an OAM group structure
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    int with_id, replace;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (group_info == NULL) {
        return BCM_E_PARAM;
    }

    if (group_info->flags & _BCM_MO_ETH_OAM_GROUP_VALID_FLAGS_MASK) {
        return BCM_E_PARAM;
    }

    replace = (group_info->flags & BCM_OAM_GROUP_REPLACE) ? 1 : 0;
    with_id = (group_info->flags & BCM_OAM_GROUP_WITH_ID) ? 1 : 0;

    if (replace) {
        if (!with_id) {
            return BCM_E_PARAM;
        }

        rv = shr_idxres_list_elem_state(eoc->group_pool, group_info->id);
        if (rv != BCM_E_EXISTS) {
            return rv;
        }
        return _bcm_mo_eth_oam_group_update(unit, group_info);
    }

    if (eoc->num_groups >= eoc->max_groups) {
        /* No further groups can be created. */
        return BCM_E_RESOURCE;
    }

    if (with_id) {
        rv = shr_idxres_list_elem_state(eoc->group_pool, group_info->id);
        if (rv != BCM_E_NOT_FOUND) {
            /* Another group is already using the index. */
            return rv;
        }
        rv = shr_idxres_list_reserve(eoc->group_pool,
                                     group_info->id, group_info->id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = shr_idxres_list_alloc(eoc->group_pool,
                                   (shr_idxres_element_t *)&(group_info->id));
        BCM_IF_ERROR_RETURN(rv);
    }
    lmep = &(eoc->lmeps[group_info->id]);

    /*
     * Copy to local DB, the information will be sent to app only when local
     * endpoint is created.
     */
    sal_memset(lmep, 0, sizeof(*lmep));
    lmep->group.in_use = 1;

    sal_memcpy(lmep->group.name, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);
    lmep->group.lowest_alarm_priority = group_info->lowest_alarm_priority;
    lmep->group.clear_persistent_faults = group_info->clear_persistent_faults;
    lmep->group.flags = _BCM_MO_ETH_OAM_GROUP_FILTER_FLAGS(group_info->flags);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_group_get
 * Purpose:
 *      Get an ETH-OAM group object
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      group      - (IN)  The ID of the group object to get
 *      group_info - (OUT) Pointer to an OAM group structure to receive the data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_group_get(int unit,
                          bcm_oam_group_t group,
                          bcm_oam_group_info_t *group_info)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    shr_eth_oam_msg_ctrl_lmep_get_t msg;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (group_info == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(eoc->group_pool, group);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    lmep = &(eoc->lmeps[group]);
    bcm_oam_group_info_t_init(group_info);

    /* Read from local DB. */
    group_info->id = group;
    group_info->flags = lmep->group.flags;
    group_info->clear_persistent_faults = lmep->group.clear_persistent_faults;
    group_info->lowest_alarm_priority = lmep->group.lowest_alarm_priority;
    sal_memcpy(group_info->name, lmep->group.name, BCM_OAM_GROUP_NAME_LENGTH);

    if (lmep->endpoint.in_use) {
        /* Get the faults from uC. */
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_lmep_get(unit, group, &msg));

        group_info->faults = _bcm_mo_eth_oam_lmep_faults_convert(msg.faults, 0);
        group_info->persistent_faults =
            _bcm_mo_eth_oam_lmep_faults_convert(msg.sticky_faults, 0);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_group_destroy
 * Purpose:
 *      Destroy an ETH-OAM group object.  All endpoints associated
 *      with the group will also be destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the OAM group object to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_group_destroy(int unit, bcm_oam_group_t group)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    rv = shr_idxres_list_elem_state(eoc->group_pool, group);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }

    /* Destroy endpoints in the group. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_endpoint_destroy_all(unit, group));

    BCM_IF_ERROR_RETURN(shr_idxres_list_free(eoc->group_pool, group));

    lmep = &(eoc->lmeps[group]);
    sal_memset(lmep, 0, sizeof(*lmep));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_group_destroy_all
 * Purpose:
 *      Destroy all ETH-OAM group objects.  All OAM endpoints will also be
 *      destroyed.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_group_destroy_all(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    int i;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    for (i = 0; i < eoc->max_groups; i++) {
        lmep = &(eoc->lmeps[i]);

        if (lmep->group.in_use) {
            BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_group_destroy(unit, i));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_group_traverse
 * Purpose:
 *      Traverse the entire set of OAM groups, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for each OAM group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_group_traverse(int unit,
                               bcm_oam_group_traverse_cb cb,
                               void *user_data)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    bcm_oam_group_info_t group_info;
    int i;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    for (i = 0; i < eoc->max_groups; i++) {
        lmep = &(eoc->lmeps[i]);

        if (lmep->group.in_use) {
            _bcm_mo_eth_oam_group_get(unit, i, &group_info);

            BCM_IF_ERROR_RETURN(cb(unit, &group_info, user_data));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_event_register
 * Purpose:
 *      Register a callback for handling ETh-OAM events
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified
 *                         callback should be called.
 *      cb          - (IN) A pointer to the callback function to call for the
 *                         specified OAM events.
 *      user_data   - (IN) Pointer to user data to supply in the callback.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_event_register(int unit,
                               bcm_oam_event_types_t event_types,
                               bcm_oam_event_cb cb,
                               void *user_data)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    bcm_oam_event_type_t event;
    _bcm_mo_eth_oam_event_handler_t *hdlr, *prev_hdlr;
    uint32 lmep_mask = 0, rmep_mask = 0;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    SHR_BIT_ITER(event_types.w, bcmOAMEventCount, event) {
        _bcm_mo_eth_oam_event_masks_get(event, &lmep_mask, &rmep_mask);
    }

    if ((lmep_mask == 0) && (rmep_mask == 0)) {
        /* No events that we are interested in. */
        return BCM_E_NONE;
    }

    /* Check if it is an already registered callback. */
    prev_hdlr = NULL;
    for (hdlr = eoc->event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        if (hdlr->cb == cb) {
            break;
        }
        prev_hdlr = hdlr;
    }

    if (hdlr == NULL) {
        /* Allocate a new handler. */
        _BCM_ETH_OAM_ALLOC(hdlr, _bcm_mo_eth_oam_event_handler_t,
                           sizeof(_bcm_mo_eth_oam_event_handler_t),
                           "OAM event handler");
        if (NULL == hdlr) {
            return (BCM_E_MEMORY);
        }
        if (prev_hdlr == NULL) {
            eoc->event_handler_list = hdlr;
        } else {
            prev_hdlr->next = hdlr;
        }
        hdlr->cb = cb;
    }

    hdlr->lmep_mask |= lmep_mask;
    hdlr->rmep_mask |= rmep_mask;
    hdlr->user_data  = user_data;

    /* Update the global mask and send it to uC. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_event_masks_update(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_event_unregister
 * Purpose:
 *      Unregister a callback handling ETH-OAM events
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of OAM events for which the specified
 *                         callback should not be called
 *      cb          - (IN) A pointer to the callback function to unregister
 *                         from the specified OAM events
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_event_unregister(int unit,
                                 bcm_oam_event_types_t event_types,
                                 bcm_oam_event_cb cb)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    bcm_oam_event_type_t event;
    _bcm_mo_eth_oam_event_handler_t *hdlr, *prev_hdlr;
    uint32 lmep_mask = 0, rmep_mask = 0;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    SHR_BIT_ITER(event_types.w, bcmOAMEventCount, event) {
        _bcm_mo_eth_oam_event_masks_get(event, &lmep_mask, &rmep_mask);
    }

    if ((lmep_mask == 0) && (rmep_mask == 0)) {
        /* No events that we are interested in. */
        return BCM_E_NONE;
    }

    /* Find the registered callback. */
    prev_hdlr = NULL;
    for (hdlr = eoc->event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        if (hdlr->cb == cb) {
            break;
        }
        prev_hdlr = hdlr;
    }

    if (hdlr == NULL) {
        return BCM_E_NONE;
    }

    hdlr->lmep_mask &= ~lmep_mask;
    hdlr->rmep_mask &= ~rmep_mask;

    if ((hdlr->lmep_mask == 0) && (hdlr->rmep_mask == 0)) {
        if (prev_hdlr == NULL) {
            eoc->event_handler_list = NULL;
        } else {
            prev_hdlr->next = hdlr->next;
        }
        sal_free(hdlr);
    }

    /* Update the global mask and send it to uC. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_event_masks_update(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_trunk_ports_add
 * Purpose:
 *      Add ports to trunk
 * Parameters:
 *      unit        - (IN) Unit number.
 *      trunk_gport - (IN) Trunk gport
 *      max_ports   - (IN) Number of ports in the port_arr[]
 *      port_arr    - (IN) List of ports.
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_trunk_ports_add(int unit,
                                bcm_gport_t trunk_gport,
                                int max_ports,
                                bcm_gport_t *port_arr)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!BCM_GPORT_IS_TRUNK(trunk_gport)) {
        return BCM_E_PARAM;
    }

    if (max_ports >= SHR_ETH_OAM_MAX_PORTS) {
        return BCM_E_PARAM;
    }

    if (port_arr == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_event_trunk_set(unit, trunk_gport,
                                                max_ports, port_arr, 1));


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_trunk_ports_delete
 * Purpose:
 *      Delete ports from trunk
 * Parameters:
 *      unit        - (IN) Unit number.
 *      trunk_gport - (IN) Trunk gport
 *      max_ports   - (IN) Number of ports in the port_arr[]
 *      port_arr    - (IN) List of ports.
 * results:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mo_eth_oam_trunk_ports_delete(int unit,
                                   bcm_gport_t trunk_gport,
                                   int max_ports,
                                   bcm_gport_t *port_arr)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!BCM_GPORT_IS_TRUNK(trunk_gport)) {
        return BCM_E_PARAM;
    }

    if (max_ports >= SHR_ETH_OAM_MAX_PORTS) {
        return BCM_E_PARAM;
    }

    if (port_arr == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_event_trunk_set(unit, trunk_gport,
                                                max_ports, port_arr, 0));

    return BCM_E_NONE;
}

int
_bcm_mo_eth_oam_trunk_ports_get(int unit,
                                bcm_gport_t trunk_gport,
                                int max_ports,
                                bcm_gport_t *port_arr,
                                int *port_count)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_trunk_get_t msg;
    bcm_module_t my_modid;
    int i;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!BCM_GPORT_IS_TRUNK(trunk_gport)) {
        return BCM_E_PARAM;
    }

    if (max_ports >= SHR_ETH_OAM_MAX_PORTS) {
        return BCM_E_PARAM;
    }

    if ((max_ports > 0) && (port_arr == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_trunk_get(unit, trunk_gport, &msg));

    if (max_ports == 0) {
        *port_count = msg.num_ports;
        return BCM_E_NONE;
    }

    for (i = 0; (i < msg.num_ports) && (i < max_ports); i++) {
        BCM_GPORT_MODPORT_SET(port_arr[i], my_modid, msg.ports[i]);
    }
    *port_count = i;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_loss_add
 * Purpose:
 *        Loss Measurement add/update
 * Parameters:
 *     unit   - (IN)    Device unit number
 *     loss_p - (INOUT) Loss information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_mo_eth_oam_loss_add(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_slm_t *slm;
    shr_eth_oam_msg_ctrl_slm_set_t msg;
    uint16 lmep_id, rmep_id;
    int with_id, update;
    int i;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (loss_p == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_loss_validate(unit, loss_p));

    update  = (loss_p->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;
    with_id = (loss_p->flags & BCM_OAM_LOSS_WITH_ID) ? 1 : 0;

    if (update) {
        if (!with_id) {
            return BCM_E_PARAM;
        }

        rv = shr_idxres_list_elem_state(eoc->slm_pool, loss_p->loss_id);
        if (rv != BCM_E_EXISTS) {
            return rv;
        }
        return _bcm_mo_eth_oam_loss_update(unit, loss_p);
    }

    if (eoc->num_slm_sessions >= eoc->max_slm_sessions) {
        return BCM_E_RESOURCE;
    }
    if (with_id) {
        rv = shr_idxres_list_elem_state(eoc->slm_pool, loss_p->loss_id);
        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    lmep_id = eoc->endpoint_map[loss_p->id].int_id;
    rmep_id = eoc->endpoint_map[loss_p->remote_id].int_id;

    rmep = &(eoc->rmeps[rmep_id]);
    rmep->num_slms++;

    /* Check if the Test ID is unique to the LMEP. */
    for (i = 0; i < eoc->max_slm_sessions; i++) {
        slm = &(eoc->slm_sessions[i]);
        if (slm->in_use == 0) {
            continue;
        }

        if ((slm->lmep_id == lmep_id) && (slm->test_id == loss_p->test_id)) {
            return BCM_E_EXISTS;
        }
    }

    /* All validations done, copy to local DB. */
    if (with_id) {
        rv = shr_idxres_list_reserve(eoc->slm_pool,
                                     loss_p->loss_id, loss_p->loss_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = shr_idxres_list_alloc(eoc->slm_pool,
                                   (shr_idxres_element_t *)&(loss_p->loss_id));
        BCM_IF_ERROR_RETURN(rv);
    }
    if (eoc->pm_mode == SHR_ETH_OAM_PM_MODE_RAW) {
        loss_p->pm_id = loss_p->loss_id;
    }

    slm = &(eoc->slm_sessions[loss_p->loss_id]);

    slm->flags              = _BCM_MO_ETH_OAM_LOSS_FILTER_FLAGS(loss_p->flags);
    slm->lmep_id            = lmep_id;
    slm->rmep_id            = rmep_id;
    slm->test_id            = loss_p->test_id;
    slm->period             = loss_p->period;
    slm->measurement_period = loss_p->measurement_period;
    slm->pkt_pri            = loss_p->pkt_pri;
    slm->inner_pkt_pri      = loss_p->inner_pkt_pri;
    slm->int_pri            = loss_p->int_pri;

    sal_memcpy(slm->peer_mac, loss_p->peer_da_mac_address, sizeof(bcm_mac_t));
    slm->in_use = 1;


    /* Populate the message. */
    sal_memset(&msg, 0, sizeof(msg));

    if (loss_p->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        msg.flags |= SHR_ETH_OAM_MSG_CTRL_SLM_SET_F_SINGLE_ENDED;
    }
    if (loss_p->flags & BCM_OAM_LOSS_TX_ENABLE) {
        msg.period   = _bcm_mo_eth_oam_period_convert(loss_p->period);
        msg.one_shot = loss_p->period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED ? 1 : 0;
    }

    msg.id                         = loss_p->loss_id;
    msg.rmep_id                    = slm->rmep_id;
    msg.test_id                    = slm->test_id;
    msg.measurement_interval_msecs = slm->measurement_period;

    /* Construct the SLM packet. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_slm_pkt_get(unit, slm, &msg));

    /* Send the message. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_slm_set(unit, &msg));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_loss_get
 * Purpose:
 *        Loss Measurement get
 * Parameters:
 *     unit   - (IN)    Device unit number
 *     loss_p - (INOUT) Loss information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_mo_eth_oam_loss_get(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_slm_t *slm;
    shr_eth_oam_msg_ctrl_slm_get_t msg;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    if (loss_p == NULL) {
        return BCM_E_PARAM;
    }

    rv = shr_idxres_list_elem_state(eoc->slm_pool, loss_p->loss_id);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }
    slm = &(eoc->slm_sessions[loss_p->loss_id]);

    loss_p->flags              = _BCM_MO_ETH_OAM_LOSS_FILTER_FLAGS(slm->flags);
    loss_p->id                 = eoc->lmeps[slm->lmep_id].endpoint.id;
    loss_p->remote_id          = eoc->rmeps[slm->rmep_id].id;
    loss_p->test_id            = slm->test_id;
    loss_p->period             = slm->period;
    loss_p->pkt_pri            = slm->pkt_pri;
    loss_p->inner_pkt_pri      = slm->inner_pkt_pri;
    loss_p->int_pri            = slm->int_pri;
    loss_p->measurement_period = slm->measurement_period;
    sal_memcpy(loss_p->peer_da_mac_address, slm->peer_mac, sizeof(bcm_mac_t));
    if (eoc->pm_mode == SHR_ETH_OAM_PM_MODE_RAW) {
        loss_p->pm_id = loss_p->loss_id;
    }

    /* Get the loss counters from uC. */
    BCM_IF_ERROR_RETURN
        (_bcm_mo_eth_oam_uc_msg_slm_get(unit, loss_p->loss_id, &msg));

    if (slm->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        loss_p->tx_nearend = msg.tx_near_end;
        loss_p->rx_farend = msg.tx_far_end;

        loss_p->tx_farend = msg.tx_far_end;
        loss_p->rx_nearend = msg.rx_near_end;

        loss_p->tx_oam_packets = loss_p->tx_nearend;
        loss_p->rx_oam_packets = loss_p->rx_nearend;

        loss_p->loss_nearend  = loss_p->tx_farend - loss_p->rx_nearend;
        loss_p->loss_farend   = loss_p->tx_nearend - loss_p->rx_farend;
    } else {
        loss_p->tx_farend = msg.tx_far_end;
        loss_p->rx_nearend = msg.rx_near_end;

        loss_p->tx_oam_packets = msg.tx_near_end;

        loss_p->loss_nearend  = loss_p->tx_farend - loss_p->rx_nearend;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_loss_delete
 * Purpose:
 *        Loss Measurement delete
 * Parameters:
 *     unit   - (IN)    Device unit number
 *     loss_p - (INOUT) Loss information
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_mo_eth_oam_loss_delete(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_slm_t *slm;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    rv = shr_idxres_list_elem_state(eoc->slm_pool, loss_p->loss_id);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }
    slm  = &(eoc->slm_sessions[loss_p->loss_id]);
    rmep = &(eoc->rmeps[slm->rmep_id]);

    /* Send the SLM delete message to uC. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_slm_del(unit, loss_p->loss_id));

    sal_memset(slm, 0, sizeof(*slm));
    rmep->num_slms--;

    BCM_IF_ERROR_RETURN(shr_idxres_list_free(eoc->slm_pool, loss_p->loss_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_pm_loss_stats_get
 * Purpose:
 *        Get the loss PM statistics
 * Parameters:
 *     unit      - (IN)  Device unit number
 *     loss_id   - (IN)  Loss ID.
 *     stats     - (OUT) Stats.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_mo_eth_oam_pm_loss_stats_get(int unit, int loss_id, bcm_oam_pm_stats_t *stats)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    shr_eth_oam_msg_ctrl_pm_pstats_slm_get_t msg;
    int rv;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }
    rv = shr_idxres_list_elem_state(eoc->slm_pool, loss_id);
    if (rv != BCM_E_EXISTS) {
        return rv;
    }
    if (stats == NULL) {
        return BCM_E_PARAM;
    }

    /* Send the stats get message to uC. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_pm_pstats_slm_get(unit, loss_id, &msg));

    stats->far_loss_min                 = msg.min_flr_loss;
    stats->far_tx_min                   = msg.min_flr_tx;
    stats->far_loss_max                 = msg.max_flr_loss;
    stats->far_tx_max                   = msg.max_flr_tx;
    stats->far_loss                     = msg.total_far_loss;
    stats->near_loss_min                = msg.min_nlr_loss;
    stats->near_tx_min                  = msg.min_nlr_tx;
    stats->near_loss_max                = msg.max_nlr_loss;
    stats->near_tx_max                  = msg.max_nlr_tx;
    stats->near_loss                    = msg.total_near_loss;
    stats->lm_tx_count                  = msg.total_tx;
    stats->lm_rx_count                  = msg.total_rx;
    stats->num_loss_measurement_periods = msg.num_intervals;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_pm_event_register
 * Purpose:
 *     Register a callback for handling OAM PM events
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM PM events for which the specified
 *                        callback should be called.
 *     cb          - (IN) A pointer to the callback function to call for
 *                        the specified OAM PM events
 *     user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_mo_eth_oam_pm_event_register(int unit,
                                  bcm_oam_event_types_t event_types,
                                  bcm_oam_pm_event_cb cb,
                                  void *user_data)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_pm_event_handler_t *hdlr, *prev_hdlr;
    int first = 0;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!SHR_BITGET(event_types.w, bcmOAMEventEthLmDmRawData)) {
        return BCM_E_NONE;
    }

    if (eoc->pm_event_handler_list == NULL) {
        first = 1;
    }

    /* Check if it is an already registered callback. */
    prev_hdlr = NULL;
    for (hdlr = eoc->pm_event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        if (hdlr->cb == cb) {
            break;
        }
        prev_hdlr = hdlr;
    }

    if (hdlr == NULL) {
        /* Allocate a new handler. */
        _BCM_ETH_OAM_ALLOC(hdlr, _bcm_mo_eth_oam_pm_event_handler_t,
                           sizeof(_bcm_mo_eth_oam_pm_event_handler_t),
                           "OAM PM event handler");
        if (NULL == hdlr) {
            return (BCM_E_MEMORY);
        }
        if (prev_hdlr == NULL) {
            eoc->pm_event_handler_list = hdlr;
        } else {
            prev_hdlr->next = hdlr;
        }
        hdlr->cb = cb;
    }
    hdlr->user_data  = user_data;

    if (first) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_pm_rstats_addr_set(unit, 1));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fp_oam_pm_event_unregister
 * Purpose:
 *     Remove a registered event from the event handler list.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM PM events for which the specified
 *                        callback should not be called
 *     cb          - (IN) A pointer to the callback function to unregister
 *                        from the specified OAM PM events
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_mo_eth_oam_pm_event_unregister(int unit,
                                    bcm_oam_event_types_t event_types,
                                    bcm_oam_pm_event_cb cb)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_pm_event_handler_t *hdlr, *prev_hdlr;

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!SHR_BITGET(event_types.w, bcmOAMEventEthLmDmRawData)) {
        return BCM_E_NONE;
    }

    prev_hdlr = NULL;
    for (hdlr = eoc->pm_event_handler_list; hdlr != NULL; hdlr = hdlr->next) {
        if (hdlr->cb == cb) {
            break;
        }
        prev_hdlr = hdlr;
    }

    if (hdlr == NULL) {
        return BCM_E_NONE;
    }

    if (prev_hdlr == NULL) {
        eoc->pm_event_handler_list = NULL;
    } else {
        prev_hdlr->next = hdlr->next;
    }
    sal_free(hdlr);

    if (eoc->pm_event_handler_list == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_uc_msg_pm_rstats_addr_set(unit, 0));
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_pm_raw_data_read_done
 * Purpose:
 *     Indicate that the application has read buffer upto the read_index.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM PM events for which the specified
 *                        callback should not be called
 *     read_index  - (IN) Read index to which the application has read.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_mo_eth_oam_pm_raw_data_read_done(int unit,
                                      bcm_oam_event_types_t event_types,
                                      uint32 read_index)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    if (eoc == NULL) {
        return BCM_E_INIT;
    }

    if (!SHR_BITGET(event_types.w, bcmOAMEventEthLmDmRawData)) {
        return BCM_E_NONE;
    }

    if (read_index >= BCM_OAM_MAX_PM_BUFFERS) {
        return BCM_E_PARAM;
    }

    if (eoc->pm_write_idx == _BCM_MO_ETH_OAM_PM_RAW_DATA_INVALID_IDX) {
        /* No writes have happened yet. */
        return BCM_E_PARAM;
    }

    if (eoc->pm_read_idx <= eoc->pm_write_idx) {
        if (read_index > eoc->pm_write_idx) {
            /* Read index crossing write index. */
            return BCM_E_PARAM;
        }
    } else {
        if ((read_index > eoc->pm_write_idx) && (read_index < eoc->pm_read_idx)) {
            /* Read index crossing write index. */
            return BCM_E_PARAM;
        }
    }

    eoc->pm_read_idx = read_index;


    return BCM_E_NONE;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
#define BCM_WB_ETH_OAM_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_ETH_OAM_DEFAULT_VERSION  BCM_WB_ETH_OAM_VERSION_1_0

#define BCM_WB_ETH_OAM_INIT_STATUS_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_ETH_OAM_INIT_STATUS_DEFAULT_VERSION  BCM_WB_ETH_OAM_INIT_STATUS_VERSION_1_0

#define ETH_OAM_SCACHE_WRITE(_scache, _val, _type)                \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while (0)

#define ETH_OAM_SCACHE_READ(_scache, _var, _type)                 \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while (0)

/*
 * Function:
 *     _bcm_mo_eth_oam_init_status_sync
 * Purpose:
 *     Syncs ETH-OAM init status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_mo_eth_oam_init_status_sync(int unit)
{

    int stable_size;
    soc_scache_handle_t scache_handle;
    uint8 *scache = NULL;
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    /* Get Telemetry module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_OAM, bcmFpOamWbSequenceEthOamInitStatus);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    if (eoc == NULL) {
        /* Module not running */
        ETH_OAM_SCACHE_WRITE(scache, 0, uint8);
    } else {
        /* Module in running state */
        ETH_OAM_SCACHE_WRITE(scache, 1, uint8);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_init_status_scache_size_get
 * Purpose:
 *     Get the size required to sync ETH-OAM init status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_mo_eth_oam_init_status_scache_size_get(int unit)
{
    return (sizeof(uint8));
}

/*
 * Function:
 *      _bcm_mo_eth_oam_init_status_scache_alloc
 * Purpose:
 *      Telemetry WB scache alloc for storing module_status
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_init_status_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_mo_eth_oam_init_status_scache_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_OAM,
                          bcmFpOamWbSequenceEthOamInitStatus);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                                     &scache,
                                     BCM_WB_ETH_OAM_INIT_STATUS_DEFAULT_VERSION,
                                     NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_wb_init_status_recover
 * Purpose:
 *      Find out if the ETH-OAM module was running prior to warmboot
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      init        - (OUT) 1 - Module initialized,
 *                          0 - Module detached
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_mo_eth_oam_wb_init_status_recover(int unit, int *init)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                              BCM_MODULE_OAM,
                              bcmFpOamWbSequenceEthOamInitStatus);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache,
                                     BCM_WB_ETH_OAM_INIT_STATUS_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have
             * warmboot state for module status
             */
            rv = _bcm_mo_eth_oam_init_status_scache_alloc(unit, 1);

            /* Set init = 1 to avoid breaking upgrade from older release */
            *init = 1;
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    ETH_OAM_SCACHE_READ(scache, *init, uint8);

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_init_status_scache_alloc(unit, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_global_size_get
 * Purpose:
 *     Get ETH-OAM config global scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_mo_eth_oam_scache_v0_global_size_get(int unit)
{
    int size = 0;

    /* Reserve 16b for any TLV at the start later. */
    size += sizeof(uint16);

    return size;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_global_sync
 * Purpose:
 *     Sync ETH-OAM global data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC void
_bcm_mo_eth_oam_scache_v0_global_sync(int unit, uint8 **scache)
{
    /* Write 0 to indicate no TLV. */
    ETH_OAM_SCACHE_WRITE(*scache, 0, uint16);
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_global_recover
 * Purpose:
 *     Recover ETH-OAM global data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC int
_bcm_mo_eth_oam_scache_v0_global_recover(int unit, uint8 **scache)
{
    uint16 type, length;

    ETH_OAM_SCACHE_READ(*scache, type, uint16);

    if (type != 0) {
        /* Unknown TLV, skip over. */
        ETH_OAM_SCACHE_READ(*scache, length, uint16);
        scache += (length - sizeof(uint16));
    }

    return BCM_E_NONE;
}

/* LMEP scache v0 data. Do not modify */
typedef struct _bcm_mo_eth_oam_scache_v0_lmep_s {
    struct {
        uint32 flags;
        uint32 clear_persistent_faults;
        bcm_oam_group_fault_alarm_defect_priority_t lowest_alarm_priority;
        uint8 in_use;
        uint8 name[BCM_OAM_GROUP_NAME_LENGTH];
    } group;

    struct {
        uint32 gport;
        uint32 flags;
        uint32 ccm_period;
        uint16 id;
        uint16 name;
        uint16 vlan;
        uint16 outer_tpid;
        uint16 inner_vlan;
        uint16 inner_tpid;
        uint8 in_use;
        uint8 level;
        uint8 pkt_pri;
        uint8 inner_pkt_pri;
        uint8 trunk_index;
        uint8 int_pri;
        uint8 src_mac[6];
        uint8 dst_mac[6];
        uint8 port_state;
        uint8 interface_state;
    } endpoint;
} _bcm_mo_eth_oam_scache_v0_lmep_t;

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_lmep_size_get
 * Purpose:
 *     Get ETH-OAM config LMEP scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_mo_eth_oam_scache_v0_lmep_size_get(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    return (sizeof(_bcm_mo_eth_oam_scache_v0_lmep_t) * eoc->max_groups);
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_lmep_sync
 * Purpose:
 *     Sync ETH-OAM LMEP data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC void
_bcm_mo_eth_oam_scache_v0_lmep_sync(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_scache_v0_lmep_t sync;
    int i;

    for (i = 0; i < eoc->max_groups; i++) {
        lmep = &(eoc->lmeps[i]);

        sal_memset(&sync, 0, sizeof(sync));

        /* Group data. */
        sync.group.in_use = lmep->group.in_use;
        sync.group.flags = lmep->group.flags;
        sal_memcpy(sync.group.name, lmep->group.name, BCM_OAM_GROUP_NAME_LENGTH * sizeof(uint8));
        sync.group.lowest_alarm_priority = lmep->group.lowest_alarm_priority;
        sync.group.clear_persistent_faults = lmep->group.clear_persistent_faults;

        /* Endpoint data. */
        sync.endpoint.in_use = lmep->endpoint.in_use;
        sync.endpoint.id = lmep->endpoint.id;
        sync.endpoint.flags = lmep->endpoint.flags;
        sync.endpoint.name = lmep->endpoint.name;
        sync.endpoint.level = lmep->endpoint.level;
        sync.endpoint.ccm_period = lmep->endpoint.ccm_period;
        sync.endpoint.vlan = lmep->endpoint.vlan;
        sync.endpoint.outer_tpid = lmep->endpoint.outer_tpid;
        sync.endpoint.pkt_pri = lmep->endpoint.pkt_pri;
        sync.endpoint.inner_vlan = lmep->endpoint.inner_vlan;
        sync.endpoint.inner_tpid = lmep->endpoint.inner_tpid;
        sync.endpoint.inner_pkt_pri = lmep->endpoint.inner_pkt_pri;
        sync.endpoint.gport = lmep->endpoint.gport;
        sync.endpoint.trunk_index = lmep->endpoint.trunk_index;
        sync.endpoint.int_pri = lmep->endpoint.int_pri;
        sync.endpoint.port_state = lmep->endpoint.port_state;
        sync.endpoint.interface_state = lmep->endpoint.interface_state;
        sal_memcpy(sync.endpoint.src_mac, lmep->endpoint.src_mac, sizeof(uint8) * 6);
        sal_memcpy(sync.endpoint.dst_mac, lmep->endpoint.dst_mac, sizeof(uint8) * 6);

        ETH_OAM_SCACHE_WRITE(*scache, sync, _bcm_mo_eth_oam_scache_v0_lmep_t);
    }
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_lmep_recover
 * Purpose:
 *     Recover ETH-OAM lmep data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC int
_bcm_mo_eth_oam_scache_v0_lmep_recover(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_lmep_t *lmep;
    _bcm_mo_eth_oam_scache_v0_lmep_t sync;
    int i;

    for (i = 0; i < eoc->max_groups; i++) {
        ETH_OAM_SCACHE_READ(*scache, sync, _bcm_mo_eth_oam_scache_v0_lmep_t);

        if (sync.group.in_use == 0) {
            continue;
        }

        lmep = &(eoc->lmeps[i]);

        /* Group data. */
        lmep->group.in_use = sync.group.in_use;
        lmep->group.flags = sync.group.flags;
        sal_memcpy(lmep->group.name, sync.group.name, BCM_OAM_GROUP_NAME_LENGTH * sizeof(uint8));
        lmep->group.lowest_alarm_priority = sync.group.lowest_alarm_priority;
        lmep->group.clear_persistent_faults = sync.group.clear_persistent_faults;

        /* Local endpoint data. */
        if (sync.endpoint.in_use) {
            lmep->endpoint.in_use = sync.endpoint.in_use;
            lmep->endpoint.id = sync.endpoint.id;
            lmep->endpoint.flags = sync.endpoint.flags;
            lmep->endpoint.name = sync.endpoint.name;
            lmep->endpoint.level = sync.endpoint.level;
            lmep->endpoint.ccm_period = sync.endpoint.ccm_period;
            lmep->endpoint.vlan = sync.endpoint.vlan;
            lmep->endpoint.outer_tpid = sync.endpoint.outer_tpid;
            lmep->endpoint.pkt_pri = sync.endpoint.pkt_pri;
            lmep->endpoint.inner_vlan = sync.endpoint.inner_vlan;
            lmep->endpoint.inner_tpid = sync.endpoint.inner_tpid;
            lmep->endpoint.inner_pkt_pri = sync.endpoint.inner_pkt_pri;
            lmep->endpoint.gport = sync.endpoint.gport;
            lmep->endpoint.trunk_index = sync.endpoint.trunk_index;
            lmep->endpoint.int_pri = sync.endpoint.int_pri;
            lmep->endpoint.port_state = sync.endpoint.port_state;
            lmep->endpoint.interface_state = sync.endpoint.interface_state;

            sal_memcpy(lmep->endpoint.src_mac, sync.endpoint.src_mac, sizeof(uint8) * 6);
            sal_memcpy(lmep->endpoint.dst_mac, sync.endpoint.dst_mac, sizeof(uint8) * 6);

            /* Update the endpoint map. */
            if (eoc->endpoint_map[lmep->endpoint.id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_NONE) {
                return BCM_E_EXISTS;
            }
            eoc->endpoint_map[lmep->endpoint.id].type   = _BCM_MO_ETH_OAM_ENDPOINT_TYPE_LOCAL;
            eoc->endpoint_map[lmep->endpoint.id].int_id = i;

            if (lmep->endpoint.ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
                eoc->num_tx_ccm_sessions++;
            }
            BCM_IF_ERROR_RETURN
                (shr_idxres_list_reserve(eoc->endpoint_pool,
                                         lmep->endpoint.id, lmep->endpoint.id));
        }
        eoc->num_groups++;
        BCM_IF_ERROR_RETURN(shr_idxres_list_reserve(eoc->group_pool, i, i));
    }

    return BCM_E_NONE;
}

/* RMEP scache v0 data. Do not modify */
typedef struct _bcm_mo_eth_oam_scache_v0_rmep_s {
    uint32 flags;
    uint32 ccm_period;
    uint32 clear_persistent_faults;
    uint16 id;
    uint16 group_id;
    uint16 name;
    uint8  in_use;
} _bcm_mo_eth_oam_scache_v0_rmep_t;

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_rmep_size_get
 * Purpose:
 *     Get ETH-OAM config RMEP scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_mo_eth_oam_scache_v0_rmep_size_get(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    return (sizeof(_bcm_mo_eth_oam_scache_v0_rmep_t) * eoc->max_rmeps);
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_rmep_sync
 * Purpose:
 *     Sync ETH-OAM RMEP data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC void
_bcm_mo_eth_oam_scache_v0_rmep_sync(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_scache_v0_rmep_t sync;
    int i;

    for (i = 0; i < eoc->max_rmeps; i++) {
        rmep = &(eoc->rmeps[i]);

        sal_memset(&sync, 0, sizeof(sync));

        sync.in_use = rmep->in_use;
        sync.id = rmep->id;
        sync.group_id = rmep->group_id;
        sync.flags = rmep->flags;
        sync.name = rmep->name;
        sync.ccm_period = rmep->ccm_period;
        sync.clear_persistent_faults = rmep->clear_persistent_faults;

        ETH_OAM_SCACHE_WRITE(*scache, sync, _bcm_mo_eth_oam_scache_v0_rmep_t);
    }
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_rmep_recover
 * Purpose:
 *     Recover ETH-OAM rmep data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC int
_bcm_mo_eth_oam_scache_v0_rmep_recover(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_scache_v0_rmep_t sync;
    int i;

    for (i = 0; i < eoc->max_rmeps; i++) {
        ETH_OAM_SCACHE_READ(*scache, sync, _bcm_mo_eth_oam_scache_v0_rmep_t);

        rmep = &(eoc->rmeps[i]);
        if (sync.in_use == 0) {
            continue;
        }

        rmep->in_use = sync.in_use;
        rmep->id = sync.id;
        rmep->group_id = sync.group_id;
        rmep->flags = sync.flags;
        rmep->name = sync.name;
        rmep->ccm_period = sync.ccm_period;
        rmep->clear_persistent_faults = sync.clear_persistent_faults;

        eoc->num_rmeps++;
        eoc->lmeps[rmep->group_id].num_rmeps++;

        /* Update the endpoint map. */
        if (eoc->endpoint_map[rmep->id].type != _BCM_MO_ETH_OAM_ENDPOINT_TYPE_NONE) {
            return BCM_E_EXISTS;
        }
        eoc->endpoint_map[rmep->id].type   = _BCM_MO_ETH_OAM_ENDPOINT_TYPE_REMOTE;
        eoc->endpoint_map[rmep->id].int_id = i;

        if (rmep->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) {
            eoc->num_rx_ccm_sessions++;
        }

        BCM_IF_ERROR_RETURN
            (shr_idxres_list_reserve(eoc->rmep_pool, i, i));
        BCM_IF_ERROR_RETURN
            (shr_idxres_list_reserve(eoc->endpoint_pool, rmep->id, rmep->id));
    }

    return BCM_E_NONE;
}

/* SLM scache v0 data. Do not modify */
typedef struct _bcm_mo_eth_oam_scache_v0_slm_s {
    uint32 flags;
    uint32 test_id;
    uint32 period;
    uint32 measurement_period;
    uint16 lmep_id;
    uint16 rmep_id;
    uint8 peer_mac[6];
    uint8 pkt_pri;
    uint8 inner_pkt_pri;
    uint8 int_pri;
    uint8 in_use;
} _bcm_mo_eth_oam_scache_v0_slm_t;

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_slm_size_get
 * Purpose:
 *     Get ETH-OAM config SLM scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_mo_eth_oam_scache_v0_slm_size_get(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];

    return (sizeof(_bcm_mo_eth_oam_scache_v0_slm_t) * eoc->max_slm_sessions);
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_slm_sync
 * Purpose:
 *     Sync ETH-OAM SLM data to scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC void
_bcm_mo_eth_oam_scache_v0_slm_sync(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_slm_t *slm;
    _bcm_mo_eth_oam_scache_v0_slm_t sync;
    int i;

    for (i = 0; i < eoc->max_slm_sessions; i++) {
        slm = &(eoc->slm_sessions[i]);

        sal_memset(&sync, 0, sizeof(sync));
        sync.in_use = slm->in_use;
        sync.flags = slm->flags;
        sync.lmep_id = slm->lmep_id;
        sync.rmep_id = slm->rmep_id;
        sync.test_id = slm->test_id;
        sync.measurement_period = slm->measurement_period;
        sync.period = slm->period;
        sync.pkt_pri = slm->pkt_pri;
        sync.inner_pkt_pri = slm->inner_pkt_pri;
        sync.int_pri = slm->int_pri;
        sal_memcpy(sync.peer_mac, slm->peer_mac, sizeof(uint8) * 6);

        ETH_OAM_SCACHE_WRITE(*scache, sync, _bcm_mo_eth_oam_scache_v0_slm_t);
    }
}

/*
 * Function:
 *     _bcm_mo_eth_oam_scache_v0_slm_recover
 * Purpose:
 *     Recover ETH-OAM slm data from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     None
 */
STATIC int
_bcm_mo_eth_oam_scache_v0_slm_recover(int unit, uint8 **scache)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    _bcm_mo_eth_oam_slm_t *slm;
    _bcm_mo_eth_oam_rmep_t *rmep;
    _bcm_mo_eth_oam_scache_v0_slm_t sync;
    int i;

    for (i = 0; i < eoc->max_slm_sessions; i++) {
        ETH_OAM_SCACHE_READ(*scache, sync, _bcm_mo_eth_oam_scache_v0_slm_t);

        if (sync.in_use == 0) {
            continue;
        }

        slm = &(eoc->slm_sessions[i]);

        slm->in_use = sync.in_use;
        slm->flags = sync.flags;
        slm->lmep_id = sync.lmep_id;
        slm->rmep_id = sync.rmep_id;
        slm->test_id = sync.test_id;
        slm->measurement_period = sync.measurement_period;
        slm->period = sync.period;
        slm->pkt_pri = sync.pkt_pri;
        slm->inner_pkt_pri = sync.inner_pkt_pri;
        slm->int_pri = sync.int_pri;
        sal_memcpy(slm->peer_mac, sync.peer_mac, sizeof(uint8) * 6);

        eoc->num_slm_sessions++;

        rmep = &(eoc->rmeps[slm->rmep_id]);
        if (rmep->in_use == 0) {
            return BCM_E_NOT_FOUND;
        }
        rmep->num_slms++;

        BCM_IF_ERROR_RETURN(shr_idxres_list_reserve(eoc->slm_pool, i, i));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_cfg_scache_v0_size_get
 * Purpose:
 *     Get ETH-OAM global config scache v0 size
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Size required
 */
STATIC uint32
_bcm_mo_eth_oam_cfg_scache_v0_size_get(int unit)
{
    uint32 size = 0;

    /* Get global data size. */
    size += _bcm_mo_eth_oam_scache_v0_global_size_get(unit);

    /* Get LMEP data size. */
    size += _bcm_mo_eth_oam_scache_v0_lmep_size_get(unit);

    /* Get RMEP data size. */
    size += _bcm_mo_eth_oam_scache_v0_rmep_size_get(unit);

    /* Get SLM data size. */
    size += _bcm_mo_eth_oam_scache_v0_slm_size_get(unit);

    return size;
}

/*
 * Function:
 *     _bcm_mo_eth_oam_cfg_scache_v0_sync
 * Purpose:
 *     Sync ETH-OAM config scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     None
 */
STATIC void
_bcm_mo_eth_oam_cfg_scache_v0_sync(int unit, uint8 **scache)
{
    /* Sync global data */
    _bcm_mo_eth_oam_scache_v0_global_sync(unit, scache);

    /* Sync LMEP data. */
    _bcm_mo_eth_oam_scache_v0_lmep_sync(unit, scache);

    /* Sync RMEP data. */
    _bcm_mo_eth_oam_scache_v0_rmep_sync(unit, scache);

    /* Sync SLM data. */
    _bcm_mo_eth_oam_scache_v0_slm_sync(unit, scache);
}

/*
 * Function:
 *     _bcm_mo_eth_oam_cfg_scache_v0_recover
 * Purpose:
 *     Recover ETH-OAM config scache v0 data
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_mo_eth_oam_cfg_scache_v0_recover(int unit, uint8 **scache)
{
    /* Recover global data */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_scache_v0_global_recover(unit, scache));

    /* Recover LMEP data. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_scache_v0_lmep_recover(unit, scache));

    /* Recover RMEP data. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_scache_v0_rmep_recover(unit, scache));

    /* Recover SLM data. */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_scache_v0_slm_recover(unit, scache));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_cfg_scache_alloc
 * Purpose:
 *      WB scache alloc for ETH-OAM configuration.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_cfg_scache_alloc(int unit, int create)
{
    soc_scache_handle_t scache_handle;
    uint32 cur_size = 0;
    uint32 rqd_size = 0;
    int rv = BCM_E_NONE;
    uint8 *scache = NULL;

    rqd_size += _bcm_mo_eth_oam_cfg_scache_v0_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                          bcmFpOamWbSequenceEthOamCfg);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                                     &scache, BCM_WB_ETH_OAM_DEFAULT_VERSION,
                                     NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_cfg_sync
 * Purpose:
 *      Warmboot scache sync of ETH-OAM configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_cfg_sync(int unit)
{
    _bcm_mo_eth_oam_ctrl_t *eoc = g_eth_oam_ctrl[unit];
    soc_scache_handle_t scache_handle;
    int stable_size;
    uint8 *scache = NULL;

    if (eoc == NULL) {
        return BCM_E_NONE;
    }

    /* Get ETH-OAM module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                          bcmFpOamWbSequenceEthOamCfg);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                 &scache, BCM_WB_ETH_OAM_DEFAULT_VERSION,
                                 NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    /* Sync ETH-OAM scache */
    _bcm_mo_eth_oam_cfg_scache_v0_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mo_eth_oam_wb_cfg_recover
 * Purpose:
 *      ETH-OAM WB recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_mo_eth_oam_wb_cfg_recover(int unit)
{
    soc_scache_handle_t scache_handle;
    int stable_size;
    uint8 *scache = NULL;
    int rv = BCM_E_NONE;
    uint16 recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                              bcmFpOamWbSequenceEthOamCfg);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_ETH_OAM_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_mo_eth_oam_cfg_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_cfg_scache_v0_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_cfg_scache_alloc(unit, 0));

    return rv;
}

/*
 * Function:
 *     _bcm_mo_oam_eth_oam_sync
 * Purpose:
 *     Store OAM configuration to level two storage cache memory.
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_mo_oam_eth_oam_sync(int unit)
{
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_init_status_sync(unit));
    BCM_IF_ERROR_RETURN(_bcm_mo_eth_oam_cfg_sync(unit));

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* INCLUDE_CCM_SLM */
