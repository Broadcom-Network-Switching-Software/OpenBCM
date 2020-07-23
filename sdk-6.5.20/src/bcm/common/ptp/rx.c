/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    rx.c
 *
 * Purpose: 
 *
 * Functions:
 *      _bcm_ptp_rx_init
 *      _bcm_ptp_rx_stack_create
 *      _bcm_ptp_rx_clock_create
 *      _bcm_ptp_external_rx_response_free
 *      _bcm_ptp_internal_rx_response_free
 *      _bcm_ptp_rx_response_flush
 *      _bcm_ptp_rx_response_get
 *      _bcm_ptp_rx_thread
 *      _bcm_ptp_rx_callback
 *      _bcm_ptp_rx_message_destination_port_get
 *      _bcm_ptp_rx_message_source_clock_identity_get
 *      _bcm_ptp_rx_message_length_get
 *      _bcm_ptp_register_management_callback
 *      _bcm_ptp_register_event_callback
 *      _bcm_ptp_register_signal_callback
 *      _bcm_ptp_register_peers_callback
 *      _bcm_ptp_register_fault_callback
 *      _bcm_ptp_unregister_management_callback
 *      _bcm_ptp_unregister_event_callback
 *      _bcm_ptp_unregister_signal_callback
 *      _bcm_ptp_unregister_peers_callback
 *      _bcm_ptp_event_message_monitor
 *      _bcm_ptp_signal_handler_default
 */

#if defined(INCLUDE_PTP)

#ifdef BCM_HIDE_DISPATCHABLE
#undef BCM_HIDE_DISPATCHABLE
#endif

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
#include <bcm/error.h>
#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
#include <soc/uc_msg.h>
#endif

#include <soc/uc_dbg.h>

#ifdef BCM_ESMC_EXTDPLL_SUPPORT
#include <bcm_int/common/esmc.h>
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

#if !defined(BCM_PTP_RX_POLL_WAIT_USEC)
#define BCM_PTP_RX_POLL_WAIT_USEC (1000)
#endif

void (*_bcm_ptp_arp_callback)(int unit, bcm_ptp_stack_id_t ptp_id,
                              int protocol, int src_addr_offset,
                              int payload_offset, int msg_len, uint8 *msg);

#define PTP_SDK_VERSION         0x01000000
#define PTP_UC_MIN_VERSION      0x01000000

#define PTP_RX_PACKET_MIN_SIZE_OCTETS     (14)
#define PTP_RX_TUNNEL_MSG_MIN_SIZE_OCTETS (11)
#define PTP_RX_EVENT_MSG_MIN_SIZE_OCTETS  (2)
#define PTP_RX_MGMT_MIN_SIZE              (0x64)
#define PTP_RX_UDP_PAYLOAD_OFFSET         (46)

#if defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL)
extern int _bcm_uc_2a_handle_ts1_offset_change_event(int unit, int64 new_ts1_offset);
extern int _bcm_common_tdpll_dpll_handle_1pps_event(int, int, int);
#endif /* defined(BCM_MONTEREY_SUPPORT) & defined(INCLUDE_GDPLL) */

/* PTP clock Rx data. */
typedef struct _bcm_ptp_clock_rx_data_s {
    _bcm_ptp_sem_t response_ready;
    uint8 * volatile response_data;
    volatile int response_len;
} _bcm_ptp_clock_rx_data_t;

/* Stack PTP Rx data arrays. */
typedef struct _bcm_ptp_stack_rx_array_s {
    _bcm_ptp_memstate_t memstate;

#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
    bcm_mac_t host_mac;
    bcm_mac_t top_mac;
    int tpid;
    int vlan;
#endif
#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    sal_thread_t rx_thread;
    volatile int rx_thread_exit;
#endif

    _bcm_ptp_clock_rx_data_t *clock_data;
} _bcm_ptp_stack_rx_array_t;

/* Unit PTP Rx data arrays. */
typedef struct _bcm_ptp_unit_rx_array_s {
    _bcm_ptp_memstate_t memstate;

    bcm_ptp_cb management_cb;
    bcm_ptp_cb event_cb;
    bcm_ptp_cb signal_cb;
    bcm_ptp_cb fault_cb;
    bcm_ptp_cb peers_cb;
    uint8      *management_user_data;
    uint8      *event_user_data;
    uint8      *signal_user_data;
    uint8      *fault_user_data;
    uint8      *peers_user_data;

    _bcm_ptp_stack_rx_array_t *stack_array;
} _bcm_ptp_unit_rx_array_t;

static _bcm_ptp_unit_rx_array_t unit_rx_array[BCM_MAX_NUM_UNITS];



int _bcm_ptp_most_recent_clock_num;
int _bcm_ptp_most_recent_port;
bcm_ptp_protocol_t _bcm_ptp_most_recent_protocol;
int _bcm_ptp_most_recent_src_addr_offset;
int _bcm_ptp_most_recent_msg_offset;

#ifdef BCM_PTP_EXT_SERVO_SUPPORT
extern bcm_ptp_mbox_tsevent_stats_t mbox_tsevent_stats;
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
static void _bcm_ptp_rx_thread(void *arg);
#endif

#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
bcm_rx_t _bcm_ptp_rx_callback(
    int unit,
    bcm_pkt_t *pkt,
    void *cookie);

static int _bcm_ptp_rx_message_destination_port_get(
    uint8 *message,
    uint16 *dest_port);

static int _bcm_ptp_rx_message_source_clock_identity_get(
    uint8 *message,
    bcm_ptp_clock_identity_t *clock_identity);

static int _bcm_ptp_rx_message_length_get(
    uint8 *message,
    uint16 *message_len);

#endif /* defined(BCM_PTP_EXTERNAL_STACK_SUPPORT) */

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT) || defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
STATIC int _bcm_ptp_event_message_monitor(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int ev_data_len,
    uint8 *ev_data,
    int *ev_internal);
#endif

STATIC int _bcm_ptp_signal_handler_default(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data);

/*
 * Function:
 *      _bcm_ptp_rx_init
 * Purpose:
 *      Initialize the PTP Rx framework and data of a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_init(
    int unit)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_ptp_stack_rx_array_t *stack_p;
    int i;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    
    if (NULL == unit_rx_array[unit].stack_array) {
        stack_p = sal_alloc(PTP_MAX_STACKS_PER_UNIT*
                            sizeof(_bcm_ptp_stack_rx_array_t),"Unit Rx arrays");
    } else {
        stack_p = unit_rx_array[unit].stack_array;
    }
    if (!stack_p) {
        unit_rx_array[unit].memstate = PTP_MEMSTATE_FAILURE;
        return BCM_E_MEMORY;
    }
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx, "%s: stack_p: %p\n", 
                          __FUNCTION__, stack_p));

    for (i = 0; i < PTP_MAX_STACKS_PER_UNIT; ++i) {
        stack_p[i].memstate = PTP_MEMSTATE_STARTUP;
    }

    unit_rx_array[unit].stack_array = stack_p;
    unit_rx_array[unit].memstate = PTP_MEMSTATE_INITIALIZED;

    unit_rx_array[unit].management_cb = NULL;
    unit_rx_array[unit].event_cb = NULL;
    unit_rx_array[unit].signal_cb = NULL;
    unit_rx_array[unit].fault_cb = NULL;

    unit_rx_array[unit].management_user_data = NULL;
    unit_rx_array[unit].event_user_data = NULL;
    unit_rx_array[unit].signal_user_data = NULL;
    unit_rx_array[unit].fault_user_data = NULL;

    /*
     * Add default callback function for tunneled PTP signaling messages
     */
    if (BCM_FAILURE(rv = _bcm_ptp_register_signal_callback(unit,
            _bcm_ptp_signal_handler_default, NULL))) {
        PTP_ERROR_FUNC("_bcm_ptp_register_signal_callback()");
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_rx_init_detach
 * Purpose:
 *      Shut down the PTP Rx framework and data of a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_detach(
    int unit)
{
    if(unit_rx_array[unit].stack_array) {
        unit_rx_array[unit].memstate = PTP_MEMSTATE_STARTUP;
        sal_free(unit_rx_array[unit].stack_array);
        unit_rx_array[unit].stack_array = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_rx_stack_create
 * Purpose:
 *      Create the PTP Rx data of a PTP stack.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      ptp_id   - (IN) PTP stack ID.
 *      host_mac - (IN) Host MAC address.
 *      top_mac  - (IN) ToP MAC address.
 *      tpid     - (IN) TPID for Host <-> ToP Communication
 *      vlan     - (IN) VLAN for Host <-> ToP Communication
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_stack_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_mac_t *host_mac,
    bcm_mac_t *top_mac,
    int tpid,
    int vlan)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_ptp_clock_rx_data_t *data_p;
    int i;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "PTP memory state not initialized\n")));
        return BCM_E_UNAVAIL;
    }

    data_p = sal_alloc(PTP_MAX_CLOCK_INSTANCES*
                       sizeof(_bcm_ptp_clock_rx_data_t),
                       "PTP stack Rx array");

    if (!data_p) {
        unit_rx_array[unit].stack_array[ptp_id].memstate =
            PTP_MEMSTATE_FAILURE;
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "PTP memory state error\n")));
        return BCM_E_MEMORY;
    }

    unit_rx_array[unit].stack_array[ptp_id].clock_data = data_p;
    unit_rx_array[unit].stack_array[ptp_id].memstate =
        PTP_MEMSTATE_INITIALIZED;

#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
    if (SOC_HAS_PTP_EXTERNAL_STACK_SUPPORT(unit)) {
        sal_memcpy(unit_rx_array[unit].stack_array[ptp_id].host_mac, host_mac,
                   sizeof(bcm_mac_t));

        sal_memcpy(unit_rx_array[unit].stack_array[ptp_id].top_mac, top_mac,
                   sizeof(bcm_mac_t));

        unit_rx_array[unit].stack_array[ptp_id].tpid = tpid;
        unit_rx_array[unit].stack_array[ptp_id].vlan = vlan;

#ifndef CUSTOMER_CALLBACK
        if (!bcm_rx_active(unit)) {
            if (BCM_FAILURE(rv = bcm_rx_cfg_init(unit))) {
                PTP_ERROR_FUNC("bcm_rx_cfg_init()");
                return rv;
            }

            if (BCM_FAILURE(rv = bcm_rx_start(unit, NULL))) {
                PTP_ERROR_FUNC("bcm_rx_start()");
                return rv;
            }
        }

        if (BCM_FAILURE(rv = bcm_rx_register(unit, "BCM_PTP_Rx", _bcm_ptp_rx_callback,
                                             BCM_RX_PRIO_MAX, (void*)ptp_id, BCM_RCO_F_ALL_COS))) {
            PTP_ERROR_FUNC("bcm_rx_register()");
            return rv;
        }
#endif /* CUSTOMER_CALLBACK */
    }
#endif /* defined(BCM_PTP_EXTERNAL_STACK_SUPPORT) */

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if (SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        _bcm_ptp_stack_info_t *stack_p;
        _bcm_ptp_info_t *ptp_info_p;

        SET_PTP_INFO;
        stack_p = &ptp_info_p->stack_info[ptp_id];

        unit_rx_array[unit].stack_array[ptp_id].rx_thread_exit = FALSE;

        unit_rx_array[unit].stack_array[ptp_id].rx_thread = sal_thread_create("PTP Rx", SAL_THREAD_STKSZ,
                                soc_property_get(unit, spn_UC_MSG_THREAD_PRI, 50) + 1,
                                _bcm_ptp_rx_thread, stack_p);
        if (unit_rx_array[unit].stack_array[ptp_id].rx_thread == SAL_THREAD_ERROR) {
            rv = BCM_E_INTERNAL;
            PTP_ERROR_FUNC("sal_thread_create()");
            return rv;
        }
    }
#endif

    for (i = 0; i < PTP_MAX_CLOCKS_PER_STACK; ++i) {
        if (BCM_FAILURE(rv = _bcm_ptp_rx_clock_create(unit, ptp_id, i))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_clock_create()");
            return rv;
        }
    }

    return rv;
}

int
_bcm_ptp_rx_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_NONE;
    int i;

    if (unit_rx_array[unit].stack_array[ptp_id].memstate == PTP_MEMSTATE_FAILURE) {
        /* Initialization / allocation did not succeed before, so just reset the state */
        unit_rx_array[unit].stack_array[ptp_id].memstate = PTP_MEMSTATE_STARTUP;
        return BCM_E_NONE;
    }

    if (unit_rx_array[unit].stack_array[ptp_id].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_INIT;
    }

    for (i = 0; i < PTP_MAX_CLOCKS_PER_STACK; ++i) {
        if (BCM_FAILURE(rv = _bcm_ptp_rx_clock_destroy(unit, ptp_id, i))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_clock_create()");
            return rv;
        }
    }

#if defined (BCM_PTP_EXTERNAL_STACK_SUPPORT)
#ifndef CUSTOMER_CALLBACK

    if (BCM_FAILURE(rv = bcm_rx_unregister(unit, _bcm_ptp_rx_callback, BCM_RX_PRIO_MAX))) {
        PTP_ERROR_FUNC("bcm_rx_unregister()");
    }

#endif /* CUSTOMER_CALLBACK */
#endif /* BCM_PTP_EXTERNAL_STACK_SUPPORT */

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
    if (SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        if (unit_rx_array[unit].stack_array[ptp_id].rx_thread != SAL_THREAD_ERROR) {
            unit_rx_array[unit].stack_array[ptp_id].rx_thread_exit = TRUE;
            /* allow thread to exit */
            for (i=0; i < 50; i++) {
                if (unit_rx_array[unit].stack_array[ptp_id].rx_thread == SAL_THREAD_ERROR) {
                    break;
                }
                sal_usleep(10000);
            }
            if (unit_rx_array[unit].stack_array[ptp_id].rx_thread != SAL_THREAD_ERROR) {
                PTP_ERROR("PTP RX thread did not exit gracefully\n");
            }
        }
    }
#endif

    unit_rx_array[unit].stack_array[ptp_id].memstate = PTP_MEMSTATE_STARTUP;
    sal_free(unit_rx_array[unit].stack_array[ptp_id].clock_data);
    unit_rx_array[unit].stack_array[ptp_id].clock_data = 0;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_rx_clock_create
 * Purpose:
 *      Create the PTP Rx data of a PTP clock.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_clock_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (unit_rx_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_data = 0;
    unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_len = 0;

    unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_ready =
        _bcm_ptp_sem_create("BCM_PTP_resp", sal_sem_BINARY, 0);

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_rx_clock_destroy
 * Purpose:
 *      Destroy the PTP Rx data of a PTP clock.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_clock_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv = BCM_E_NONE;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
        (unit_rx_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    if (unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_ready) {
        _bcm_ptp_sem_destroy(unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_ready);
        unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_ready = 0;
    }

    return rv;
}


/* External version */
int
_bcm_ptp_external_rx_response_free(int unit, int ptp_id, uint8 *resp_data)
{
    return bcm_rx_free(unit, resp_data - PTP_RX_UDP_PAYLOAD_OFFSET);
}

/* Internal version */
int
_bcm_ptp_internal_rx_response_free(int unit, int ptp_id, uint8 *resp_data)
{
    _bcm_ptp_stack_info_t *stack_p;
    _bcm_ptp_info_t *ptp_info_p;
    int i;

    SET_PTP_INFO;
    stack_p = &ptp_info_p->stack_info[ptp_id];
    for (i = 0; i < BCM_PTP_MAX_BUFFERS; ++i) {
        if (stack_p->int_state.mboxes->mbox[i].data == resp_data) {
            if (GET_MBOX_STATUS(stack_p, i) != MBOX_STATUS_PENDING_HOST) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "Invalid mbox status on PTP rx response free (%d)\n"),
                           GET_MBOX_STATUS(stack_p, i)));
            }
            SET_MBOX_STATUS(stack_p, i, MBOX_STATUS_EMPTY);
            return BCM_E_NONE;
        }
    }

    LOG_ERROR(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Invalid PTP rx response free (%p vs %p)\n"),
               (void *)resp_data, (void*)stack_p->int_state.mboxes->mbox[i-1].data));

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      _bcm_ptp_rx_response_flush
 * Purpose:
 *      Flush prior Rx response.
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      ptp_id     - (IN)  PTP stack ID.
 *      clock_num  - (IN)  PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_response_flush(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv = BCM_E_UNAVAIL;
    int spl;

    uint8 *prior_data;
    _bcm_ptp_stack_info_t *stack_p;
    _bcm_ptp_info_t *ptp_info_p;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_rx_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "PTP memory state not initialized\n")));
        return BCM_E_UNAVAIL;
    }

    rv = _bcm_ptp_sem_take(unit_rx_array[unit].stack_array[ptp_id]
            .clock_data[clock_num].response_ready, sal_mutex_NOWAIT);

    if (rv == BCM_E_NONE) {
        LOG_WARN(BSL_LS_BCM_PTP,
            (BSL_META_U(unit,
            "PTP mbox flush found waiting sem.  Rx task blocked?\n")));
    } else {
        LOG_WARN(BSL_LS_BCM_PTP,
            (BSL_META_U(unit,
            "PTP mbox flush: no sem waiting.\n")));
    }

    /*
     * Flush response.
     * NOTICE: Response already waiting is unexpected.
     */
    /* Lock. */
    spl = sal_splhi();

    prior_data = unit_rx_array[unit].stack_array[ptp_id]
        .clock_data[clock_num].response_data;

    unit_rx_array[unit].stack_array[ptp_id]
        .clock_data[clock_num].response_data = 0;

    /* Unlock. */
    sal_spl(spl);

    SET_PTP_INFO;
    stack_p = &ptp_info_p->stack_info[ptp_id];

    if (prior_data) {
        stack_p->rx_free(unit, ptp_id, prior_data);
    }

    /* MBOX 0 is the one that has the response in it */
    SET_MBOX_STATUS(stack_p, 0, MBOX_STATUS_EMPTY);

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_rx_response_get
 * Purpose:
 *      Get Rx response data for a PTP clock.
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      ptp_id     - (IN)  PTP stack ID.
 *      clock_num  - (IN)  PTP clock number.
 *      usec       - (IN)  Semaphore timeout (usec).
 *      data       - (OUT) Response data.
 *      data_len   - (OUT) Response data size (octets).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_rx_response_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int usec,
    uint8 **data,
    int *data_len)
{
    int rv = BCM_E_UNAVAIL;
    int spl;
    sal_usecs_t expiration_time = sal_time_usecs() + usec;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_rx_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    rv = BCM_E_FAIL;
    /* ptp_printf("Await resp @ %d\n", (int)sal_time_usecs()); */

    while (BCM_FAILURE(rv) && (int32) (sal_time_usecs() - expiration_time) < 0) {
        rv = _bcm_ptp_sem_take(unit_rx_array[unit].stack_array[ptp_id].clock_data[clock_num].response_ready, usec);
    }
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Failed management Tx to ToP\n")));
        PTP_ERROR_FUNC("_bcm_ptp_sem_take()");
        return rv;
    }

    /* Lock. */
    spl = sal_splhi();

    *data = unit_rx_array[unit].stack_array[ptp_id]
               .clock_data[clock_num].response_data;

    *data_len = unit_rx_array[unit].stack_array[ptp_id]
                    .clock_data[clock_num].response_len;

    unit_rx_array[unit].stack_array[ptp_id]
        .clock_data[clock_num].response_data = 0;

    /* Unlock. */
    sal_spl(spl);

    return rv;
}


#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)

void _bcm_ptp_send_event_cb(int unit,
        bcm_ptp_stack_id_t      ptp_id,
        int                     clock_num,
        uint8                   *data,
        uint16                  data_len)
{
    bcm_ptp_cb_msg_t cb_msg;
    cb_msg.length = data_len;
    cb_msg.data = data;
    unit_rx_array[unit].event_cb(unit, ptp_id, clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT,
            bcmPTPCallbackTypeEvent, &cb_msg,
            unit_rx_array[unit].event_user_data);
}

static void _bcm_ptp_rx_thread(void *arg)
{
    _bcm_ptp_stack_info_t *stack_p = arg;
    int unit = stack_p->unit;
    bcm_ptp_stack_id_t ptp_id = stack_p->stack_id;
    int rv = 0;
    _bcm_ptp_stack_rx_array_t *stack_rx = &unit_rx_array[unit].stack_array[ptp_id];
    bcm_ptp_cb_msg_t cb_msg;
    int rx_thread_exit = 0;

    /* sal_usecs_t last_time = 0; */
    /* sal_usecs_t this_time = 0; */

    while (1) {
        int mbox;
        
        /* mos_msg_data_t rcv; */
        /* int rv = soc_cmic_uc_msg_receive(stack_p->unit, stack_p->int_state.core_num, MOS_MSG_CLASS_1588, &rcv, sal_sem_FOREVER); */
        /* if (rv) { */
        /*     /\* got error, so wait *\/ */
        /*     sal_usleep(100000); */
        /* } */
        /* The uc_msg is just a signal that there is a message somewhere to get, so look through all mboxes */

        /* rather than getting woken by soc_cmic_uc_msg_receive, just poll */

        /* Note:  on some SAL implementations, a short usleep is done by doing a 'yield' within a loop,
           rather than a true 'sleep'.  If no other tasks are using the CPU, this will effectively be a
           busy-wait, as the yield will immediately return.  This can make it appear that the PTP Rx thread
           is using a lot of CPU, though this is only because no other task is in a running state.
           To avoid this appearance of CPU load, the value of BCM_PTP_RX_POLL_WAIT_USEC can be increased
           via the Make.local file, e.g.:  CFGFLAGS += -DBCM_PTP_RX_POLL_WAIT_USEC=11000
        */
        sal_usleep(BCM_PTP_RX_POLL_WAIT_USEC);

        /* last_time = this_time; */
        /* this_time = sal_time_usecs(); */

        for (mbox = 0; mbox < BCM_PTP_MAX_BUFFERS; ++mbox) {

            if (stack_rx->rx_thread_exit || (_bcm_ptp_running(unit) != BCM_E_NONE)
                || (stack_p->int_state.mboxes == NULL)) {
                /* Existing the rx thread */
                rx_thread_exit = 1;
                break;
            }

            /* Invalidate cache for the shared memory before reading it */
            soc_cm_sinval(unit, (void*)&stack_p->int_state.mboxes->mbox[mbox].status, sizeof(stack_p->int_state.mboxes->mbox[mbox]));

            switch (soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].status)) {
            case MBOX_STATUS_ATTN_HOST_TUNNEL: {
                int cb_flags = 0;        
                unsigned clock_num = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].clock_num);
                int message_type = _bcm_ptp_uint16_read((uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_MSG_TYPE_OFFSET);
                int port_num = _bcm_ptp_uint16_read((uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_PORT_NUM_OFFSET);
                int protocol = _bcm_ptp_uint16_read((uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_PROTOCOL_OFFSET);
                int src_addr_offset = _bcm_ptp_uint16_read((uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_SRC_ADDR_OFFS_OFFSET);
                int ptp_offset = _bcm_ptp_uint16_read((uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_PTP_OFFS_OFFSET);
                uint8 * cb_data = (uint8 *)stack_p->int_state.mboxes->mbox[mbox].data + MBOX_TUNNEL_PACKET_OFFSET;
                int cb_data_len = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].data_len) - MBOX_TUNNEL_PACKET_OFFSET;

                bcm_ptp_clock_identity_t peer_clockIdentity;
                bcm_ptp_clock_port_address_t peer_portAddress;

                /* ptp_printf("Got Tunnel: MsgType:%d, Proto:%d Clk:%d\n", message_type, protocol, clock_num); */
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx,
                                     "Got Tunnel: MsgType:%d, Proto:%d Clk:%d\n",
                                     message_type, protocol, clock_num));

                
                _bcm_ptp_most_recent_clock_num = clock_num;
                _bcm_ptp_most_recent_port = port_num;
                _bcm_ptp_most_recent_protocol = protocol;
                _bcm_ptp_most_recent_src_addr_offset = src_addr_offset;
                _bcm_ptp_most_recent_msg_offset = ptp_offset;
                cb_flags = 0;

                if (message_type == bcmPTP_MESSAGE_TYPE_SIGNALING ||
                    message_type == bcmPTP_MESSAGE_TYPE_MANAGEMENT) {
                    /* Peer information for counts of Rx signaling and external management messages. */
                    sal_memcpy(peer_clockIdentity,
                               cb_data + ptp_offset + PTP_PTPHDR_SRCPORT_OFFSET_OCTETS,
                               sizeof(bcm_ptp_clock_identity_t));

                    peer_portAddress.addr_type = protocol;
                    sal_memset(peer_portAddress.address, 0, BCM_PTP_MAX_NETW_ADDR_SIZE);
                    switch (protocol) {
                    case bcmPTPUDPIPv4:
                        sal_memcpy(peer_portAddress.address,
                                   cb_data + src_addr_offset, PTP_IPV4_ADDR_SIZE_BYTES);
                        break;
                    case bcmPTPUDPIPv6:
                        sal_memcpy(peer_portAddress.address,
                                   cb_data + src_addr_offset, PTP_IPV6_ADDR_SIZE_BYTES);
                        break;
                    case bcmPTPIEEE8023:
                        sal_memcpy(peer_portAddress.address,
                                   cb_data + src_addr_offset, PTP_MAC_ADDR_SIZE_BYTES);
                        break;
                    default:
                        ;
                    }
                }

                switch (message_type) {
                case bcmPTP_MESSAGE_TYPE_SIGNALING:
                    /* Peer dataset counts of Rx signaling messages. */
                    if (BCM_FAILURE(rv = _bcm_ptp_update_peer_counts(unit, ptp_id, clock_num, port_num,
                            &peer_clockIdentity, &peer_portAddress, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL))) {
                        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx,
                                             "Error in _bcm_ptp_update_peer_counts()\n"));
                        PTP_ERROR_FUNC("_bcm_ptp_update_peer_counts()");
                    }

                    if (unit_rx_array[unit].signal_cb) {
                        sal_memset(&cb_msg, 0, sizeof(bcm_ptp_cb_msg_t));
                        cb_msg.length = cb_data_len;
                        cb_msg.data = cb_data;
                        unit_rx_array[unit].signal_cb(unit, ptp_id, clock_num, port_num,
                                                      bcmPTPCallbackTypeSignal, &cb_msg, 
                                                      unit_rx_array[unit].signal_user_data);
                    }
                    break;

                case bcmPTP_MESSAGE_TYPE_MANAGEMENT:
                    /* Peer dataset counts of Rx external management messages. */
                    if (BCM_FAILURE(rv = _bcm_ptp_update_peer_counts(unit, ptp_id, clock_num, port_num,
                            &peer_clockIdentity, &peer_portAddress, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL))) {
                        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx,
                                             "Error in _bcm_ptp_update_peer_counts()\n"));
                        PTP_ERROR_FUNC("_bcm_ptp_update_peer_counts()");
                    }

                    if (unit_rx_array[unit].management_cb) {
                        sal_memset(&cb_msg, 0, sizeof(bcm_ptp_cb_msg_t));

                        cb_msg.flags = cb_flags;
                        cb_msg.protocol = protocol;
                        cb_msg.src_addr_offset = src_addr_offset;
                        cb_msg.msg_offset = ptp_offset;
                        cb_msg.length = cb_data_len;
                        cb_msg.data = cb_data;

                        if (unit_rx_array[unit].management_cb(unit, ptp_id, clock_num, port_num,
                                                              bcmPTPCallbackTypeManagement, &cb_msg,
                                                              unit_rx_array[unit].management_user_data)
                            == bcmPTPCallbackAccept) {
                            if (BCM_FAILURE(rv =_bcm_ptp_tunnel_message_to_top(unit,
                                                     ptp_id, clock_num, port_num,
                                                     cb_data_len, cb_data, 1))) {
                                PTP_ERROR_FUNC("_bcm_ptp_tunnel_message_to_top()");
                            }
                        }
                    }
                    break;
                case bcmPTP_MESSAGE_TYPE_ARP:
                    if (_bcm_ptp_arp_callback) {
                        _bcm_ptp_arp_callback(unit, ptp_id, protocol, src_addr_offset, ptp_offset, cb_data_len, cb_data);
                    }
                    break;

                case (0x0100):
                    
                    if (BCM_FAILURE(rv = bcm_esmc_rx(unit, ptp_id, port_num,
                            protocol, cb_data_len, cb_data))) {
                        PTP_ERROR_FUNC("bcm_esmc_rx()");
                    }
                    break;

                default:
                    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                                (BSL_META_U(unit,
                                            "Improper PTP tunneled message type: %d in mbox %d\n"), message_type, mbox));
                    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx,
                        "Improper PTP tunneled message type: %d in mbox %d\n",
                        message_type, mbox));
                    break;
                }
                /* Free the mbox so the CMICm can use it again */
                SET_MBOX_STATUS(stack_p, mbox, MBOX_STATUS_EMPTY);

                break;
              }

            case MBOX_STATUS_ATTN_HOST_EVENT: {
                
                unsigned clock_num = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].clock_num);
                int cb_flags = 0;
                int cb_data_len = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].data_len);
                uint8 *cb_data = (uint8 *)stack_p->int_state.mboxes->mbox[mbox].data;
                int ev_internal = 0;
#ifdef PTP_RX_CALLBACK_DEBUG
                LOG_CLI((BSL_META_U(unit,
                    "Rx thread : Host event(%d)\n"), cb_data_len));
                _bcm_ptp_dump_hex(cb_data, cb_data_len, 0);
#endif

                if (BCM_FAILURE(rv = _bcm_ptp_event_message_monitor(unit, ptp_id,
                        cb_data_len, cb_data, &ev_internal))) {
                        PTP_ERROR_FUNC("_bcm_ptp_event_message_monitor()");
                }

                if (unit_rx_array[unit].event_cb && !ev_internal) {
                    sal_memset(&cb_msg, 0, sizeof(bcm_ptp_cb_msg_t));
                    cb_msg.length = cb_data_len;
                    cb_msg.data = cb_data;
                    cb_msg.flags = cb_flags;
                    unit_rx_array[unit].event_cb(unit, ptp_id, clock_num,
                                                 PTP_CLOCK_PORT_NUMBER_DEFAULT,
                                                 bcmPTPCallbackTypeEvent, &cb_msg,
                                                 unit_rx_array[unit].event_user_data);
                }
                SET_MBOX_STATUS(stack_p, mbox, MBOX_STATUS_EMPTY);
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx, "Got HOST_EVENT in mbox %d\n", mbox));
                break;
              }

            case MBOX_STATUS_ATTN_HOST_RESP: {
                /* This really should only be in mbox 0, but here for generality */
                int clock_num = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].clock_num);

                /* ptp_printf("Got HOST_RESP in mbox %d\n", mbox); */
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx, "Got HOST_RESP in mbox %d\n", mbox));

                stack_rx->clock_data[clock_num].response_data = (uint8 *)stack_p->int_state.mboxes->mbox[mbox].data;
                stack_rx->clock_data[clock_num].response_len = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].data_len);

                SET_MBOX_STATUS(stack_p, mbox, MBOX_STATUS_PENDING_HOST);

                rv = _bcm_ptp_sem_give(stack_rx->clock_data[clock_num].response_ready);
                if (BCM_FAILURE(rv)) {
                    PTP_ERROR_FUNC("_bcm_ptp_sem_give()");
                }
                break;
              }
             case MBOX_STATUS_ATTN_HOST_TIMESTAMPS: {
                unsigned clock_num = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].clock_num);
                int cb_data_len = soc_ntohl(stack_p->int_state.mboxes->mbox[mbox].data_len);
                uint8 *cb_data = (uint8 *)stack_p->int_state.mboxes->mbox[mbox].data;
                if (BCM_FAILURE(rv =_bcm_ptp_timestamp_queue_add(unit,
                                                     ptp_id, clock_num, cb_data_len, cb_data))) {
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
                   mbox_tsevent_stats.error_queue_add++;
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */
                   PTP_ERROR_FUNC("_bcm_ptp_queue_timestamp_message()");
                } else {
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
                mbox_tsevent_stats.event_queued++;
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */
                }
                SET_MBOX_STATUS(stack_p, mbox, MBOX_STATUS_EMPTY);
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtpRx, "Got HOST_TIMESTAMPS_EVENT in mbox %d\n", mbox));
                break;
              }
           }
        }
        /* exit the thread */
        if (rx_thread_exit) {
            break;
        }

#ifdef BCM_PTP_EXT_SERVO_SUPPORT
        _bcm_ptp_ext_servo_def_evt_hdlr(unit, ptp_id, 0);
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

    } /* while (1) loop */

    stack_rx->rx_thread = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}
#endif  /* defined(BCM_PTP_INTERNAL_STACK_SUPPORT) */

#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)

bcm_rx_t
_bcm_ptp_rx_callback(
    int unit,
    bcm_pkt_t *pkt,
    void *cookie)
{
    int rv = BCM_E_UNAVAIL;
    int spl;

    uint8 *prior_data;
    uint16 udp_dest_port;
    uint16 message_len;

    int wrapClockNumber;
    int wrapPortNumber;
    bcm_ptp_message_type_t wrapMessageType;
    bcm_ptp_protocol_t wrapProtocol;
    uint16 wrapSrcAddrOffset;
    uint16 wrapPtpOffset;

    _bcm_ptp_stack_info_t *stack_p;
    _bcm_ptp_info_t *ptp_info_p;
    bcm_ptp_clock_identity_t cb_clock_identity;
    bcm_ptp_stack_id_t cb_ptp_id = (bcm_ptp_stack_id_t)cookie;
    int cb_clock_num;
    uint32 cb_flags;
    int ev_internal = 0;
    int vlan, tpid;
    int i = 0;

    bcm_ptp_clock_identity_t peer_clockIdentity;
    bcm_ptp_clock_port_address_t peer_portAddress;

    bcm_ptp_cb_msg_t cb_msg;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return BCM_RX_NOT_HANDLED;
    }

    if (unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Rx unit data not initialized\n")));
        return BCM_RX_NOT_HANDLED;
    }

#ifdef PTP_RX_CALLBACK_DEBUG
    LOG_CLI((BSL_META_U(unit,
                        "_bcm_ptp_rx_callback(%d,%d)\n"),
             pkt->pkt_len, BCM_PKT_IEEE_LEN(pkt)));
        _bcm_ptp_dump_hex(BCM_PKT_IEEE(pkt), BCM_PKT_IEEE_LEN(pkt), 0);
#endif

    if (pkt->pkt_data[0].len < pkt->pkt_len ||
            pkt->pkt_len < PTP_RX_PACKET_MIN_SIZE_OCTETS) {
        /*
         * Ignore packet.
         * NOTICE: inconsistent or incompatible packet length.
         */
        return BCM_RX_NOT_HANDLED;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_rx_message_length_get(BCM_PKT_IEEE(pkt),
            &message_len))) {
        PTP_ERROR_FUNC("_bcm_ptp_rx_message_length_get()");
        return BCM_RX_NOT_HANDLED;
    }

    /*
     * Parse packet data.
     */

    /** Get destination port from UDP header, which is a proxy for packet
     * type and subsequent handling.
     */
    if (BCM_FAILURE(rv = _bcm_ptp_rx_message_destination_port_get(
             BCM_PKT_IEEE(pkt), &udp_dest_port))) {
        PTP_ERROR_FUNC("_bcm_ptp_rx_message_destination_port_get()");
        return BCM_RX_NOT_HANDLED;
    }

    switch (udp_dest_port) {
    case (0x0140):
        /* Response message. */

        /* Sanity / Security check: Validate VLAN & MAC information for this clock */
        tpid = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + 12);  /* 12: fixed offset for TPID/VLAN */

        /* Extract only vid, mask out Pri and CFI */
        vlan =  (_bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + 14) & 0x0FFF);

        if (tpid != unit_rx_array[unit].stack_array[cb_ptp_id].tpid ||
            vlan != unit_rx_array[unit].stack_array[cb_ptp_id].vlan) {
            /* This is a PTP message, but not from our stack (since it is on the wrong VLAN). */
            return BCM_RX_NOT_HANDLED;
        }

        if (pkt->pkt_len < PTP_RX_MGMT_MIN_SIZE) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Bad response len\n")));
            return BCM_RX_NOT_HANDLED;
        }

        /*
         * Parse packet data.
         * Lookup the unit number, PTP stack ID, and PTP clock number
         * association of the packet based on the sender's PTP clock
         * identity.
         */
        if (BCM_FAILURE(rv = _bcm_ptp_rx_message_source_clock_identity_get(
                BCM_PKT_IEEE(pkt), &cb_clock_identity))) {
            PTP_ERROR_FUNC("_bcm_ptp_rx_message_source_clock_identity_get()");
            return BCM_RX_NOT_HANDLED;
        }

        if (BCM_FAILURE(rv = _bcm_ptp_clock_lookup(cb_clock_identity, unit,
                                                   cb_ptp_id, &cb_clock_num))) {
            /* Not from one of this stack's clocks, so leave it */
            return BCM_RX_NOT_HANDLED;
        }

        /*
         * Ensure that Rx framework is initialized and Rx data structures are
         * created for the requisite unit and PTP stack.
         */
        if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
                (unit_rx_array[unit].stack_array[cb_ptp_id].memstate !=
                PTP_MEMSTATE_INITIALIZED)) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Rx unit/stack data not initialized\n")));
            return BCM_RX_NOT_HANDLED;
        }

        if (sal_memcmp(BCM_PKT_IEEE(pkt) + sizeof(bcm_mac_t),
                       unit_rx_array[unit].stack_array[cb_ptp_id].top_mac,
                       sizeof(bcm_mac_t)) != 0) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Bad response smac\n")));
        }

        if (sal_memcmp(BCM_PKT_IEEE(pkt),
                       unit_rx_array[unit].stack_array[cb_ptp_id].host_mac,
                       sizeof(bcm_mac_t)) != 0) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Bad response dmac\n")));
        }

        /* Lock. */
        spl = sal_splhi();

        prior_data = unit_rx_array[unit].stack_array[cb_ptp_id]
                                           .clock_data[cb_clock_num]
                                           .response_data;

        unit_rx_array[unit].stack_array[cb_ptp_id]
                              .clock_data[cb_clock_num]
                              .response_data = pkt->pkt_data[0].data + PTP_RX_UDP_PAYLOAD_OFFSET;

        unit_rx_array[unit].stack_array[cb_ptp_id]
                              .clock_data[cb_clock_num]
                              .response_len =  pkt->pkt_len - PTP_RX_UDP_PAYLOAD_OFFSET;

        /* Unlock. */
        sal_spl(spl);

        /*
         * Free unclaimed response.
         * NOTICE: If prior data exists, it must be freed.
         */
        if (prior_data) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Unclaimed response freed\n")));

            SET_PTP_INFO;
            stack_p = &ptp_info_p->stack_info[cb_ptp_id];

            stack_p->rx_free(unit, cb_ptp_id, prior_data);
        }

        rv = _bcm_ptp_sem_give(unit_rx_array[unit].stack_array[cb_ptp_id]
                     .clock_data[cb_clock_num].response_ready);
        if (BCM_FAILURE(rv)) {
            PTP_ERROR_FUNC("_bcm_ptp_sem_give()");
        }

        return BCM_RX_HANDLED_OWNED;
        break;

    case (0x0141):
        /* Forwarded (tunnel) message. */
        if (message_len < PTP_RX_TUNNEL_MSG_MIN_SIZE_OCTETS) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Invalid (too-short) tunnel message received (0x%04x)\n"),
                         message_len));
            return BCM_RX_HANDLED;
        }

        /*
         * Parse wrapping header.
         * Move cursor forward to "remove" wrapping header.
         *
         * NOTICE: Forwarded tunnel message prepends an 11-element header, which
         *         includes PTP message and addressing metadata.
         *         Wrapping Header Octet  0      : Instance number.
         *         Wrapping Header Octets 1...2  : Local (receiving) port number.
         *         Wrapping Header Octets 3...4  : Message type.
         *         Wrapping Header Octets 5...6  : Ethertype.
         *         Wrapping Header Octets 7...8  : Source address offset.
         *         Wrapping Header Octets 9...10 : PTP payload offset.
         */
        i = PTP_PTPHDR_START_IDX;

        wrapClockNumber = *(BCM_PKT_IEEE(pkt) + i);
        ++i;
        --message_len;

        wrapPortNumber = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + i);
        i += sizeof(uint16);
        message_len -= sizeof(uint16);

        wrapMessageType = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + i);
        i += sizeof(uint16);
        message_len -= sizeof(uint16);

        wrapProtocol = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + i);
        i += sizeof(uint16);
        message_len -= sizeof(uint16);

        wrapSrcAddrOffset = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + i);
        i += sizeof(uint16);
        message_len -= sizeof(uint16);

        wrapPtpOffset = _bcm_ptp_uint16_read(BCM_PKT_IEEE(pkt) + i);
        i += sizeof(uint16);
        message_len -= sizeof(uint16);

        
        _bcm_ptp_most_recent_clock_num = wrapClockNumber;
        _bcm_ptp_most_recent_port = wrapPortNumber;
        _bcm_ptp_most_recent_protocol = wrapProtocol;
        _bcm_ptp_most_recent_src_addr_offset = wrapSrcAddrOffset;
        _bcm_ptp_most_recent_msg_offset = wrapPtpOffset;

        /*
         * Parse packet data.
         * Lookup the unit number, PTP stack ID, and PTP clock number
         * association of the packet based on the recipient's PTP clock
         * identity.
         */
        if (wrapMessageType == bcmPTP_MESSAGE_TYPE_SIGNALING ||
            wrapMessageType == bcmPTP_MESSAGE_TYPE_MANAGEMENT) {
            /* PTP tunneled message. */
            sal_memcpy(cb_clock_identity,
                       BCM_PKT_IEEE(pkt) + i + wrapPtpOffset + PTP_PTPHDR_SIZE_OCTETS,
                       sizeof(bcm_ptp_clock_identity_t));

            /* Peer information for counts of Rx signaling and external management messages. */
            sal_memcpy(peer_clockIdentity,
                       BCM_PKT_IEEE(pkt) + i + wrapPtpOffset + PTP_PTPHDR_SRCPORT_OFFSET_OCTETS,
                       sizeof(bcm_ptp_clock_identity_t));

            peer_portAddress.addr_type = wrapProtocol;
            sal_memset(peer_portAddress.address, 0, BCM_PTP_MAX_NETW_ADDR_SIZE);
            switch (wrapProtocol) {
            case bcmPTPUDPIPv4:
                sal_memcpy(peer_portAddress.address,
                           BCM_PKT_IEEE(pkt) + i + wrapSrcAddrOffset, PTP_IPV4_ADDR_SIZE_BYTES);
                break;
            case bcmPTPUDPIPv6:
                sal_memcpy(peer_portAddress.address,
                           BCM_PKT_IEEE(pkt) + i + wrapSrcAddrOffset, PTP_IPV6_ADDR_SIZE_BYTES);
                break;
            case bcmPTPIEEE8023:
                sal_memcpy(peer_portAddress.address,
                           BCM_PKT_IEEE(pkt) + i + wrapSrcAddrOffset, PTP_MAC_ADDR_SIZE_BYTES);
                break;
            default:
                ;
            }
        } else {
            /* Non-PTP tunneled message (e.g. ARP). */

            
            sal_memset(cb_clock_identity, 0xff, sizeof(bcm_ptp_clock_identity_t));
        }

        if (BCM_FAILURE(rv = _bcm_ptp_clock_lookup(cb_clock_identity, unit,
                                                   cb_ptp_id, &cb_clock_num))) {
            /* Not from one of this stack's clocks, so leave it */
            return BCM_RX_NOT_HANDLED;
        }

        /*
         * Ensure that Rx framework is initialized and Rx data structures are
         * created for the requisite unit and PTP stack.
         */
        if ((unit_rx_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
                (unit_rx_array[unit].stack_array[cb_ptp_id].memstate !=
                PTP_MEMSTATE_INITIALIZED)) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Rx unit/stack data not initialized\n")));
            return BCM_RX_NOT_HANDLED;
        }

        switch (wrapMessageType) {
        case bcmPTP_MESSAGE_TYPE_SIGNALING:
            /* Peer dataset counts of Rx signaling messages. */
            if (BCM_FAILURE(rv = _bcm_ptp_update_peer_counts(unit, cb_ptp_id, wrapClockNumber, wrapPortNumber,
                    &peer_clockIdentity, &peer_portAddress, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL))) {
                PTP_ERROR_FUNC("_bcm_ptp_update_peer_counts()");
            }

            
            cb_flags = 0;

            if (unit_rx_array[unit].signal_cb) {
                cb_msg.flags = cb_flags;
                cb_msg.protocol = wrapProtocol;
                cb_msg.src_addr_offset = wrapSrcAddrOffset;
                cb_msg.msg_offset = wrapPtpOffset;
                cb_msg.length = message_len;
                cb_msg.data = BCM_PKT_IEEE(pkt) + i;

                unit_rx_array[unit].signal_cb(unit, cb_ptp_id,
                    cb_clock_num, wrapPortNumber,
                    bcmPTPCallbackTypeSignal, &cb_msg,
                    unit_rx_array[unit].signal_user_data);
            }
            break;

        case bcmPTP_MESSAGE_TYPE_MANAGEMENT:
            /* Peer dataset counts of Rx external management messages. */
            if (BCM_FAILURE(rv = _bcm_ptp_update_peer_counts(unit, cb_ptp_id, wrapClockNumber, wrapPortNumber,
                    &peer_clockIdentity, &peer_portAddress, 0, 0, 0, 0, 0, 0, 1, 0, 0, NULL))) {
                PTP_ERROR_FUNC("_bcm_ptp_update_peer_counts()");
            }

            
            cb_flags = 0;

            if (unit_rx_array[unit].management_cb) {
                cb_msg.flags = cb_flags;
                cb_msg.protocol = wrapProtocol;
                cb_msg.src_addr_offset = wrapSrcAddrOffset;
                cb_msg.msg_offset = wrapPtpOffset;
                cb_msg.length = message_len;
                cb_msg.data = BCM_PKT_IEEE(pkt) + i;

                if (unit_rx_array[unit].management_cb(unit, cb_ptp_id,
                        cb_clock_num, wrapPortNumber,
                        bcmPTPCallbackTypeManagement, &cb_msg,
                        unit_rx_array[unit].management_user_data) == bcmPTPCallbackAccept) {
                    /* Tunnel message to ToP. */
                    if (BCM_FAILURE(rv =_bcm_ptp_tunnel_message_to_top(unit,
                            cb_ptp_id, cb_clock_num, wrapPortNumber,
                            message_len, BCM_PKT_IEEE(pkt) + i, 1))) {
                        PTP_ERROR_FUNC("_bcm_ptp_tunnel_message_to_top()");
                    }
                }
            }
            break;

        case bcmPTP_MESSAGE_TYPE_ARP:
            if (_bcm_ptp_arp_callback) {
                _bcm_ptp_arp_callback(unit, cb_ptp_id, wrapProtocol, wrapSrcAddrOffset,
                    wrapPtpOffset, message_len, BCM_PKT_IEEE(pkt) + i);
            }
            break;

        default:
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Invalid tunnel message received: "
                                    "unknown/unsupported type (0x%02x)\n"), wrapMessageType));
        }
        break;

    case (0x0142):
        /* Event message. */
        if (message_len < PTP_RX_EVENT_MSG_MIN_SIZE_OCTETS) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "Invalid (too-short) event message received (0x%04x)\n"),
                         message_len));
            return BCM_RX_HANDLED;
        }

        
        cb_ptp_id = 0;
        cb_clock_num = 0;
        cb_flags = 0;

        if (BCM_FAILURE(rv = _bcm_ptp_event_message_monitor(unit, cb_ptp_id,
                message_len, BCM_PKT_IEEE(pkt) + PTP_PTPHDR_START_IDX, &ev_internal))) {
                PTP_ERROR_FUNC("_bcm_ptp_event_message_monitor()");
        }

        if (unit_rx_array[unit].event_cb && !ev_internal) {
            i = PTP_PTPHDR_START_IDX;

            cb_msg.flags = cb_flags;
            cb_msg.protocol = bcmPTPUDPIPv4;
            cb_msg.src_addr_offset = 0;
            cb_msg.msg_offset = 0;
            cb_msg.length = message_len;
            cb_msg.data = BCM_PKT_IEEE(pkt) + i;

            unit_rx_array[unit].event_cb(unit, cb_ptp_id,
                cb_clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT,
                bcmPTPCallbackTypeEvent, &cb_msg,
                unit_rx_array[unit].event_user_data);
        }

        break;

    default:
#ifdef PTP_RX_CALLBACK_DEBUG
        LOG_CLI((BSL_META_U(unit,
                            "UDP packet dst port 0x%04x not handled\n"), udp_dest_port));
#endif
        return BCM_RX_NOT_HANDLED;
    }

    return BCM_RX_HANDLED;
}


/*
 * Function:
 *      _bcm_ptp_rx_message_destination_port_get
 * Purpose:
 *      Get destination port number in UDP header.
 * Parameters:
 *      message   - (IN)  PTP management message.
 *      dest_port - (OUT) Destination port number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
static int
_bcm_ptp_rx_message_destination_port_get(
    uint8 *message,
    uint16 *dest_port)
{
    int i = PTP_UDPHDR_START_IDX + PTP_UDPHDR_DESTPORT_OFFSET_OCTETS;

    *dest_port = _bcm_ptp_uint16_read(message + i);
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_ptp_rx_message_source_clock_identity_get
 * Purpose:
 *      Get PTP source clock identity in PTP common header.
 * Parameters:
 *      message        - (IN)  PTP management message.
 *      clock_identity - (OUT) PTP source clock identity.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
static int
_bcm_ptp_rx_message_source_clock_identity_get(
    uint8 *message,
    bcm_ptp_clock_identity_t *clock_identity)
{
    int i = PTP_PTPHDR_START_IDX + PTP_PTPHDR_SRCPORT_OFFSET_OCTETS;

    sal_memcpy(clock_identity, message + i, sizeof(bcm_ptp_clock_identity_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_rx_message_length_get
 * Purpose:
 *      Get the length of a message.
 * Parameters:
 *      message     - (IN)  PTP management message.
 *      message_len - (OUT) Message length (octets).
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      Message length is size of Rx packet excluding headers.
 */
static int
_bcm_ptp_rx_message_length_get(
    uint8 *message,
    uint16 *message_len)
{
    int i = PTP_UDPHDR_START_IDX + PTP_UDPHDR_MSGLEN_OFFSET_OCTETS;

    *message_len = _bcm_ptp_uint16_read(message + i) - PTP_UDPHDR_SIZE_OCTETS;
    return BCM_E_NONE;
}

#endif /* defined(BCM_PTP_EXTERNAL_STACK_SUPPORT) */

/*
 * Function:
 *      _bcm_ptp_register_management_callback
 * Purpose:
 *      Register a management callback function
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_register_management_callback(
    int unit,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].management_cb = cb;
    unit_rx_array[unit].management_user_data = user_data;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_register_event_callback
 * Purpose:
 *      Register a event callback function
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_register_event_callback(
    int unit,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].event_cb = cb;
    unit_rx_array[unit].event_user_data = user_data;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_register_signal_callback
 * Purpose:
 *      Register a signal callback function
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_register_signal_callback(
    int unit,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].signal_cb = cb;
    unit_rx_array[unit].signal_user_data = user_data;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_register_peers_callback
 * Purpose:
 *      Register callback function to periodically get the peer counters
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_register_peers_callback(
    int unit,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].peers_cb = cb;
    unit_rx_array[unit].peers_user_data = user_data;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_register_fault_callback
 * Purpose:
 *      Register a fault callback function
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for the specified PTP events
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_register_fault_callback(
    int unit,
    bcm_ptp_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].fault_cb = cb;
    unit_rx_array[unit].fault_user_data = user_data;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unregister_management_callback
 * Purpose:
 *      Unregister a management callback function
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_unregister_management_callback(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].management_cb = NULL;
    unit_rx_array[unit].management_user_data = NULL;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unregister_event_callback
 * Purpose:
 *      Unregister a event callback function
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_unregister_event_callback(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].event_cb = NULL;
    unit_rx_array[unit].event_user_data = NULL;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unregister_signal_callback
 * Purpose:
 *      Unregister a signal callback function
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_unregister_signal_callback(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].signal_cb = NULL;
    unit_rx_array[unit].signal_user_data = NULL;

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unregister_peers_callback
 * Purpose:
 *      Unregister a check peers callback function
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_unregister_peers_callback(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].peers_cb = NULL;
    unit_rx_array[unit].peers_user_data = NULL;

    return rv;
}


/*
 * Function:
 *      _bcm_ptp_unregister_fault_callback
 * Purpose:
 *      Unregister a fault callback function
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX.
 * Notes:
 *      The unit is already locked by the calling function
 */
int
_bcm_ptp_unregister_fault_callback(
    int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    unit_rx_array[unit].fault_cb = NULL;
    unit_rx_array[unit].fault_user_data = NULL;

    return rv;
}

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT) || defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
/*
 * Function:
 *      _bcm_ptp_event_message_monitor
 * Purpose:
 *      Monitor incoming event messages and perform basic operations req'd
 *      for maintenance of host caches.
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      ptp_id      - (IN)  PTP stack ID.
 *      ev_data_len - (IN)  Event message data length (octets).
 *      ev_data     - (IN)  Event message data.
 *      ev_internal - (OUT) Internal-only event. Do not pass to event callbacks.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
_bcm_ptp_event_message_monitor(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int ev_data_len,
    uint8 *ev_data,
    int *ev_internal)
{
    int rv;

    uint8 event_clock_num;
    uint16 event_port_num;
    _bcm_ptp_event_t event_code;
    _bcm_ptp_port_state_t portState;
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    int event_input_clock_num;
#endif

    int cursor = 0;

    event_code = (_bcm_ptp_event_t)_bcm_ptp_uint16_read(ev_data + cursor);
    cursor += sizeof(uint16);

    if (event_code == _bcm_ptp_state_change_event) {
        /* EVENT MONITOR: portState change. */
        *ev_internal = 0;

        /* Extract clock number and port number of event. */
        event_clock_num = ev_data[cursor++];
        event_port_num = _bcm_ptp_uint16_read(ev_data + cursor);
        cursor += sizeof(uint16);

        /* Advance cursor and extract portState of event. */
        cursor += BCM_PTP_CLOCK_EUID_IEEE1588_SIZE + 2;
        portState = (_bcm_ptp_port_state_t)ev_data[cursor];

        if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_port_state_set(unit, ptp_id,
                event_clock_num, event_port_num, portState))) {
            return rv;
        }
    } else if (event_code == _bcm_ptp_tdev_event) {
        /* EVENT MONITOR: TDEV data analysis. */
        *ev_internal = 1;

        if (BCM_FAILURE(rv = _bcm_ptp_ctdev_gateway(unit, ptp_id, PTP_CLOCK_NUMBER_DEFAULT,
                ev_data_len, ev_data))) {
            return rv;
        }
#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)
    } else if (event_code == _bcm_ptp_top_ready_event) {
        _bcm_ptp_ext_fw_info_set(unit, ptp_id, ev_data + 2);
#endif
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    } else if (event_code == _bcm_ptp_synce_loss_event) {
        /* Handling of SyncE loss for ESMC with external DPLL is done from here.
            So no need to invoke the general handler for events */
        *ev_internal = 1;
        event_input_clock_num = _bcm_ptp_uint16_read(ev_data + cursor);
        bcm_extdpll_esmc_synce_event_handler(unit, ptp_id, event_input_clock_num);
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
    } else if (event_code == _bcm_ptp_ts1_offset_event) {
#if defined (BCM_MONTEREY_SUPPORT) && defined (INCLUDE_GDPLL)
        int64 new_ts1_offset = _bcm_ptp_int64_read(ev_data + cursor);
        LOG_INFO(BSL_LS_BCM_PTP, (BSL_META_U(unit, "_bcm_ptp_ts1_offset_event offset: %ld\n"), (long int)new_ts1_offset));
        if (BCM_FAILURE(rv = _bcm_uc_2a_handle_ts1_offset_change_event(unit, new_ts1_offset))) {
            return rv;
        }
    } else if (event_code == _bcm_ptp_pps_in_state_event) {
        int pps_in_state = ev_data[cursor];
        LOG_INFO(BSL_LS_BCM_PTP, (BSL_META_U(unit, "_bcm_ptp_pps_in_state_event: %d\n"), pps_in_state));
        if (BCM_FAILURE(rv = _bcm_common_tdpll_dpll_handle_1pps_event(unit, ptp_id, pps_in_state))) {
            return rv;
        }
#else
        /*Handling required only for GDPLL. Nothing to do here*/
        return BCM_E_NONE;
#endif
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_ptp_signal_handler_default
 * Purpose:
 *      Default forwarded (tunneled) PTP signaling message callback handler.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_port - (IN) PTP port number.
 *      type       - (IN) Callback function type.
 *      msg        - (IN) Callback message data.
 *      user_data  - (IN) Callback user data.
 * Returns:
 *      BCM_E_XXX (if failure)
 *      bcmPTPCallbackAccept (if success)
 * Notes:
 */
STATIC int
_bcm_ptp_signal_handler_default(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num, 
    uint32 clock_port, 
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
    
    int port_num = _bcm_ptp_most_recent_port;
    bcm_ptp_protocol_t protocol = _bcm_ptp_most_recent_protocol;
    int src_addr_offset = _bcm_ptp_most_recent_src_addr_offset;
    int msg_offset = _bcm_ptp_most_recent_msg_offset;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

#ifdef PTP_RX_CALLBACK_DEBUG
    LOG_CLI((BSL_META_U(unit,
                        "Signaling callback (Unit = %d, PTP Stack = %d)\n"),
             unit, ptp_id));
    _bcm_ptp_dump_hex(msg->data, msg->length,0);
#endif

    return _bcm_ptp_process_incoming_signaling_msg(unit, ptp_id, clock_num, port_num, protocol, src_addr_offset,
                                                   msg_offset, msg->length, msg->data);
}

#endif /* defined(INCLUDE_PTP) */
