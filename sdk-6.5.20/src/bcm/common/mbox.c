/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Shared Memory Mailbox infrastruction - SHM mbox communications & debug
 * logging
 */

#include <shared/bsl.h>

#include <bcm_int/common/mbox.h>

#include <soc/drv.h>
#include <soc/uc_msg.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>

#include <bcm/types.h>
#include <bcm/error.h>

#define MBOX_STATUS_LOCK_TIME (1000)

#define _BCM_MBOX_HEXDUMP_LINE_WIDTH_OCTETS   (100)

#if defined(BCM_CMICM_SUPPORT)
#define _BCM_MBOX_LOCAL_DEBUGBUFSIZE          (1024)
static _bcm_bs_internal_comm_info_t mbox_info;

static char local_debugbuf[_BCM_MBOX_LOCAL_DEBUGBUFSIZE];
static int local_head = 0;

static char output_debugbuf[_BCM_MBOX_LOCAL_DEBUGBUFSIZE * 2];
static int local_tail = 0;
#endif /* BCM_CMICM_SUPPORT */

static uint32 _cmicm_debug_flags = 0;
/* Global variables */
uint32 bs_firmware_version = BS_MIN_UC_VERSION;

#if defined(INCLUDE_GDPLL)
extern int _bcm_time_host_notification(int, int);
#endif

#if defined(BCM_CMICM_SUPPORT)
STATIC void _bcm_mbox_rx_thread(void *arg);

STATIC void _bcm_mbox_debug_poll(void* owner, void* time_as_ptr,
                                 void *unit_as_ptr, void *unused_2,
                                 void* unused_3);
#endif /* BCM_CMICM_SUPPORT */

int
_bcm_mbox_status_read(int unit, int idx, uint32 *status)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_CMICM_SUPPORT)
    rv = sal_mutex_take(mbox_info.unit_state[unit].status_access_lock,
                        MBOX_STATUS_LOCK_TIME);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "Failed to get mbox status lock")));
        return BCM_E_UNAVAIL;
    }
    soc_cm_sinval(unit, (void*)&mbox_info.unit_state[unit].mboxes->status[idx],
                  sizeof(mbox_info.unit_state[unit].mboxes->status[0]));
    *status = soc_htonl(mbox_info.unit_state[unit].mboxes->status[idx]);
    sal_mutex_give(mbox_info.unit_state[unit].status_access_lock);
#endif /* BCM_CMICM_SUPPORT */

    return rv;
}

int
_bcm_mbox_status_write(int unit, int idx, uint32 status)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_CMICM_SUPPORT)
    rv = sal_mutex_take(mbox_info.unit_state[unit].status_access_lock,
                        MBOX_STATUS_LOCK_TIME);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "Failed to get mbox status lock")));
        return BCM_E_UNAVAIL;
    }
    mbox_info.unit_state[unit].mboxes->status[idx] = soc_htonl(status);
    soc_cm_sflush(unit, (void*)&mbox_info.unit_state[unit].mboxes->status[idx],
                  sizeof(mbox_info.unit_state[unit].mboxes->status[0]));
    sal_mutex_give(mbox_info.unit_state[unit].status_access_lock);
#endif /* BCM_CMICM_SUPPORT */

    return rv;
}
/* ************************************ Transport *****************************/

/*
 * Function:
 *      _bcm_mbox_bs_wb_prep
 * Purpose:
 *      Send the WarmBoot message to Host.
 * Parameters:
 *      Unit- Unit to send the warmboot message
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_mbox_bs_wb_prep(int unit)
{
#if defined(BCM_CMICM_SUPPORT)
    int timeout_usec = 1900000;
    int c;
    int rv = BCM_E_NONE;
    mos_msg_data_t wb_msg, rcv;
    /* If the mbox_comm_init is not done then no need to intimate about warmboot
     * to BS-FW
     */
    if (!mbox_info.unit_state) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "MBOX not initialised \n")));
        return BCM_E_NONE;
    }

    c = mbox_info.unit_state[unit].core_num;

    wb_msg.s.mclass     = MOS_MSG_CLASS_BS;
    wb_msg.s.subclass   = MOS_MSG_SUBCLASS_MBOX_WB_PREP;
    rv = soc_cmic_uc_msg_send(unit, c, &wb_msg, timeout_usec);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                   "soc_cmic_uc_msg_send failed for warmboot message\n")));
        rv = BCM_E_UNAVAIL;
    }

    /* Response/handshaking from Broadsync-FW */
    if (BCM_E_NONE == rv) {

        /* Read the response from the FW */
        uint16 rlen;
        rv = soc_cmic_uc_msg_receive(unit, c, MOS_MSG_CLASS_BS,
                                     &rcv, timeout_usec);
        rlen = rcv.s.len;

        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "soc_cmic_uc_msg_receive failed for warmboot ack\n")));
            rv = BCM_E_UNAVAIL;
        }

        /* The length value is just for proper handshaking!! */
        if (0xF0F0 != rlen) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "_bcm_mbox_bs_wb_prep: wrong length from FW\n")));
            rv = BCM_E_UNAVAIL;
        }
        else {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit, "_bcm_mbox_bs_wb_prep: FW done with WB prep\n")));
        }
    }

    return rv;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      _bcm_mbox_bs_cosq_port_set
 * Purpose:
 *      Send the cosq port (to be used in PTCH) to firmware.
 * Parameters:
 *      Unit- Unit to send the warmboot message
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_mbox_bs_cosq_port_set(int unit)
{
#if defined(BCM_CMICM_SUPPORT)
    int timeout_usec = 1900000;
    int c;
    int rv = BCM_E_NONE;
    int cosq_port=0;
    mos_msg_data_t msg;

    if (!mbox_info.unit_state) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "MBOX not initialised \n")));
        return BCM_E_NONE;
    }

    /* Read the config property custom_feature_ptp_cosq_port */
    cosq_port = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ptp_cosq_port", 0);

    if (!cosq_port) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "Error: bs get cosq port failed.\n")));
        return BCM_E_RESOURCE;
    }

    c = mbox_info.unit_state[unit].core_num;

    msg.s.mclass = MOS_MSG_CLASS_BS;
    msg.s.subclass = MOS_MSG_SUBCLASS_PTP_COSQ_PORT;
    msg.s.len = 0;
    msg.s.data = bcm_htonl(cosq_port);
    rv = soc_cmic_uc_msg_send(unit, c, &msg, timeout_usec);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                        "soc_cmic_uc_msg_send failed for cosq port set\n")));
        rv = BCM_E_UNAVAIL;
    }

    return rv;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */

}

#if defined(BCM_CMICM_SUPPORT)
/* Callback from MCS messaging layer: called before and after uC shutdown */
int _bcm_bs_deinit_callback(int unit,
                            int uC,
                            soc_cmic_uc_shutdown_stage_t stage,
                            void *user_data)
{
    if (stage == SOC_CMIC_UC_SHUTDOWN_NOHALT) {
        _bcm_mbox_bs_wb_prep(unit);
    }

    return BCM_E_NONE;
}
#endif /* BCM_CMICM_SUPPORT */

int
_bcm_mbox_comm_init(int unit, int appl_type)
{
#if defined(BCM_CMICM_SUPPORT)
    int rv = BCM_E_NONE;
    int timeout_usec = 1900000;
    int max_num_cores = 2;
    int result;
    int c;
    int i;
    sal_thread_t thread_id;
    mos_msg_data_t start_msg;
    sal_paddr_t buf_paddr;

    /* Init the system if this is the first time in */
    if (mbox_info.unit_state == NULL) {
        mbox_info.unit_state = soc_cm_salloc(unit,
                                             sizeof(_bcm_bs_internal_stack_state_t) *
                                             BCM_MAX_NUM_UNITS,
                                             "mbox_info_unit_state");
        if (!mbox_info.unit_state) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "MBOX unit_state no memory\n")));
            return BCM_E_MEMORY;
        }
        
	    mbox_info.comm_available = sal_sem_create("BCM BS comms",
	                                              sal_sem_BINARY, 0);
	    if (mbox_info.comm_available == NULL) {
	        soc_cm_sfree(unit, mbox_info.unit_state);
	        mbox_info.unit_state = NULL;
	        LOG_ERROR(BSL_LS_BCM_COMMON,
	                  (BSL_META_U(unit, "BCM BS comms semaphore create failed\n")));
	        return BCM_E_MEMORY;
	    }
        sal_memset(mbox_info.unit_state, 0,
                   sizeof(_bcm_bs_internal_stack_state_t) * BCM_MAX_NUM_UNITS);
        rv = sal_sem_give(mbox_info.comm_available);
        if (BCM_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit, "sal_sem_give failed\n")));
            return BCM_E_INTERNAL;
        }
    }

    /* Init the unit if this is the first time for the unit */
    if (mbox_info.unit_state[unit].mboxes == NULL) {
        /* allocate space for mboxes */
        mbox_info.unit_state[unit].mboxes =
                soc_cm_salloc(unit, sizeof(_bcm_bs_internal_stack_mboxes_t), "bs msg");

        if (!mbox_info.unit_state[unit].mboxes) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "MBOX mboxes no memory\n")));
            return BCM_E_MEMORY;
        }

        /* clear state of message mboxes */
        mbox_info.unit_state[unit].mboxes->num_buffers =
                              soc_ntohl(_BCM_MBOX_MAX_BUFFERS);
        for (i = 0; i < _BCM_MBOX_MAX_BUFFERS; ++i) {
            mbox_info.unit_state[unit].mboxes->status[i] =
                           soc_htonl(_BCM_MBOX_MS_EMPTY);
        }
        soc_cm_sflush(unit, (void*)&mbox_info.unit_state[unit].mboxes->status,
                      _BCM_MBOX_MAX_BUFFERS *
                      sizeof(mbox_info.unit_state[unit].mboxes->status[0]));



        /* allocate space for debug log */
        /* size is the size of the structure without the placeholder space for
         * debug->buf, plus the real space for it
         */
        mbox_info.unit_state[unit].log =
                    soc_cm_salloc(unit,
                                  sizeof(_bcm_bs_internal_stack_log_t)
                                  - sizeof(mbox_info.unit_state[unit].log->buf)
                                  + _BCM_MBOX_MAX_LOG, "bs log");
        if (!mbox_info.unit_state[unit].log) {
            soc_cm_sfree(unit, mbox_info.unit_state[unit].mboxes);
            mbox_info.unit_state[unit].mboxes = NULL;
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit, "MBOX log no memory\n")));
            return BCM_E_MEMORY;
        }

        mbox_info.unit_state[unit].status_access_lock =
                        sal_mutex_create("CMICM_status");
        if (mbox_info.unit_state[unit].status_access_lock == NULL) {
            soc_cm_sfree(unit, mbox_info.unit_state[unit].mboxes);
            mbox_info.unit_state[unit].mboxes = NULL;
            soc_cm_sfree(unit, mbox_info.unit_state[unit].log);
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit, "CMICM_status mutex create failed\n")));
            return BCM_E_MEMORY;
        }

        mbox_info.unit_state[unit].response_ready =
                        sal_sem_create("CMICM_resp", sal_sem_BINARY, 0);
        if (mbox_info.unit_state[unit].response_ready == NULL) {
            soc_cm_sfree(unit, mbox_info.unit_state[unit].mboxes);
            mbox_info.unit_state[unit].mboxes = NULL;
            soc_cm_sfree(unit, mbox_info.unit_state[unit].log);
            sal_mutex_destroy(mbox_info.unit_state[unit].status_access_lock);
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit, "CMICM_resp semaphore create failed\n")));
            return BCM_E_MEMORY;
        }
        thread_id = sal_thread_create("CMICM Rx", SAL_THREAD_STKSZ,
                                      soc_property_get(unit, spn_UC_MSG_THREAD_PRI, 50) + 1,
                                      _bcm_mbox_rx_thread,  INT_TO_PTR(unit));
        if (thread_id == SAL_THREAD_ERROR) {
            soc_cm_sfree(unit, mbox_info.unit_state[unit].mboxes);
            mbox_info.unit_state[unit].mboxes = NULL;
            soc_cm_sfree(unit, mbox_info.unit_state[unit].log);
            sal_mutex_destroy(mbox_info.unit_state[unit].status_access_lock);
            sal_sem_destroy(mbox_info.unit_state[unit].response_ready);
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit, "CMICM Rx thread create failed\n")));
            return BCM_E_INTERNAL;
        }

        /* initialize debug */
        mbox_info.unit_state[unit].log->size = soc_htonl(_BCM_MBOX_MAX_LOG);
        mbox_info.unit_state[unit].log->head = 0;
        mbox_info.unit_state[unit].log->tail = 0;

        /* set up the network-byte-order pointers so that CMICm can access
         * the shared memory
         */
        mbox_info.unit_state[unit].mbox_ptr =
            soc_htonl(soc_cm_l2p(unit, (void*)mbox_info.unit_state[unit].mboxes));
        mbox_info.unit_state[unit].log_ptr =
            soc_htonl(soc_cm_l2p(unit, (void*)mbox_info.unit_state[unit].log));

        _bcm_mbox_debug_poll(INT_TO_PTR(unit),
                             INT_TO_PTR(soc_property_get(unit, spn_BS_POLL_INTERVAL, 1000)),
                             INT_TO_PTR(unit), 0, 0);
    }

    /* Allow BS application on uC to get initialized on every call */
    rv = BCM_E_UNAVAIL;
    for (c = max_num_cores - 1; c >= 0; c--) {
        /* LOG_CLI((BSL_META_U(unit,
                               "Trying BS on core %d\n"), c)); */
        result = soc_cmic_uc_appl_init(unit, c,
                                       MOS_MSG_CLASS_BS, timeout_usec,
                                       BS_SDK_VERSION,
                                       BS_MIN_UC_VERSION,
                                       _bcm_bs_deinit_callback, NULL);

        if (SOC_E_NONE == result){
            /* uKernel communcations started successfully, so run the init */
            /* Note: the length of this message is unused, and can be
             * overloaded */
            buf_paddr = soc_cm_l2p(unit, (void*)&mbox_info.unit_state[unit]);

            start_msg.s.mclass = MOS_MSG_CLASS_BS;
            start_msg.s.subclass = MOS_MSG_SUBCLASS_MBOX_CONFIG;
            _shr_uint16_write((uint8*)(&(start_msg.s.len)), (uint16) appl_type);
            start_msg.s.data = bcm_htonl(buf_paddr);
            rv = soc_cmic_uc_msg_send(unit, c, &start_msg, timeout_usec);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                           (BSL_META_U(unit, "soc_cmic_uc_msg_send failed\n")));
            }

            mbox_info.unit_state[unit].core_num = c;

            /* Configure custom_feature_ptp_cosq_port required for DNX platforms */
            if (SOC_IS_JERICHO(unit)) {
                _bcm_mbox_bs_cosq_port_set(unit);
            }
            break;
        }

        /* LOG_CLI((BSL_META_U(unit,
                               "No response on core %d\n"), c)); */
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit, "No response from CMICm core(s)\n")));
        return rv;
    }

    return rv;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


int
_bcm_mbox_tx(
    int unit,
    uint32 node_num,
    _bcm_mbox_transport_type_t transport,
    uint8 *message,
    int message_len)
{
#if defined(BCM_CMICM_SUPPORT)
    int rv = BCM_E_NONE;

    mos_msg_data_t uc_msg;

    int wait_iter = 0;
    uint32 status = _BCM_MBOX_MS_ERR;
    uint32 status_out = _BCM_MBOX_MS_EMPTY;

    /* LOG_CLI((BSL_META_U(unit,
                           "cmic_tx Len:%d\n"), message_len)); */

    if (_bcm_mbox_status_read(unit, 0, &status) != BCM_E_NONE ||
        status != _BCM_MBOX_MS_EMPTY) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Contention\n")));
    }

    /* wait for to-TOP buffer to be free, if it is not already */
    while ((_bcm_mbox_status_read(unit, 0, &status) != BCM_E_NONE ||
            status != _BCM_MBOX_MS_EMPTY) && wait_iter < 100000) {
        ++wait_iter;
        sal_usleep(1);
    }

    if (status != _BCM_MBOX_MS_EMPTY) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "TOP message buffer in use on Tx, re-pinging\n")));

        rv = soc_cmic_uc_msg_send(unit, mbox_info.unit_state[unit].core_num,
                                  &uc_msg, 1000000);

        return BCM_E_FAIL;
    }

    /* load the mailbox */
    sal_memcpy((uint8 *)mbox_info.unit_state[unit].mboxes->mbox[0].data,
                        message, message_len);
    mbox_info.unit_state[unit].mboxes->mbox[0].data_len =
                                    soc_htonl(message_len);
    mbox_info.unit_state[unit].mboxes->mbox[0].node_num = node_num;

    /* finish mbox load by setting status */
    switch (transport) {
    case _BCM_MBOX_MESSAGE:
        status_out = _BCM_MBOX_MS_CMD;
        break;
    case _BCM_MBOX_TUNNEL_TO:
        status_out = _BCM_MBOX_MS_TUNNEL_TO;
        break;
    case _BCM_MBOX_TUNNEL_OUT:
        status_out = _BCM_MBOX_MS_TUNNEL_OUT;
        break;
    default:
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "mbox_tx failed: Unknown transport type\n")));
    }

    soc_cm_sflush(unit, (void*)&mbox_info.unit_state[unit].mboxes->mbox[0],
                  sizeof(mbox_info.unit_state[unit].mboxes->mbox[0]));
    _bcm_mbox_status_write(unit, 0, status_out);

    /* Send a notification to the CMICm */
    sal_memset(&uc_msg, 0, sizeof(uc_msg));
    uc_msg.s.mclass = MOS_MSG_CLASS_BS;
    uc_msg.s.subclass = MOS_MSG_SUBCLASS_MBOX_CMDRESP;
    uc_msg.s.len = message_len;
    uc_msg.s.data = 0;
    rv = soc_cmic_uc_msg_send(unit, mbox_info.unit_state[unit].core_num,
                              &uc_msg, 1000000);

    return rv;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


int
_bcm_mbox_tx_completion(
    int unit,
    uint32 node_num)
{
#if defined(BCM_CMICM_SUPPORT)
    int iter;
    uint32 status = _BCM_MBOX_MS_ERR;

    for (iter = 0; iter < 10000; ++iter) {
        if (_bcm_mbox_status_read(unit, 0, &status) == BCM_E_NONE &&
            status == _BCM_MBOX_MS_EMPTY) {
            /* sal_strcpy(holding_thread_name, "none"); */
            return BCM_E_NONE;
        }
        sal_usleep(1);
    }

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "Failed async Tx to ToP.  No clear\n")));
    return BCM_E_TIMEOUT;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


int
_bcm_mbox_rx_response_free(int unit, uint8 *resp_data)
{
#if defined(BCM_CMICM_SUPPORT)
    unsigned i;

    for (i = 0; i < _BCM_MBOX_MAX_BUFFERS; ++i) {
        if (mbox_info.unit_state[unit].mboxes->mbox[i].data == resp_data) {
            _bcm_mbox_status_write(unit, i, _BCM_MBOX_MS_EMPTY);

            return BCM_E_NONE;
        }
    }

    LOG_CLI((BSL_META_U(unit,
             "Invalid CMICM rx response free (%p)\n"), (void *)resp_data));

    return BCM_E_NOT_FOUND;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
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
_bcm_mbox_rx_response_get(
    int unit,
    int node_num,
    int usec,
    uint8 **data,
    int *data_len)
{
#if defined(BCM_CMICM_SUPPORT)
    int rv = BCM_E_UNAVAIL;
    int spl;
    sal_usecs_t expiration_time = sal_time_usecs() + usec;

    /* LOG_CLI((BSL_META_U(unit,
                           "cmic_rx_get\n"))); */

    rv = BCM_E_FAIL;
    /* ptp_printf("Await resp @ %d\n", (int)sal_time_usecs()); */

    while (BCM_FAILURE(rv) && (int32) (sal_time_usecs() - expiration_time) < 0) {
        rv = sal_sem_take(mbox_info.unit_state[unit].response_ready, usec);
    }

    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit, "sal_sem_take failed\n")));
        return rv;
    }

    /* Lock. */
    spl = sal_splhi();

    *data     = mbox_info.unit_state[unit].response_data;
    *data_len = mbox_info.unit_state[unit].response_len;

    mbox_info.unit_state[unit].response_data = 0;

    /* Unlock. */
    sal_spl(spl);

    return rv;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


#if defined(BCM_CMICM_SUPPORT)

STATIC
void
_bcm_mbox_rx_thread(void *arg)
{
    int rv = 0;

    int unit = PTR_TO_INT(arg);

    while (1) {
        int mbox;


        /* The uc_msg is just a signal that there is a message somewhere to get,
         * so look through all mboxes
         */
        /*
        Note:  on some SAL implementations, a short usleep is done by doing a
        'yield' within a loop, rather than a true 'sleep'.  If no other tasks are
        using the CPU, this will effectively be a busy-wait, as the yield will
        immediately return.  This can make it appear that the PTP Rx thread is
        using a lot of CPU, though this is only because no other task is in a
        running state. To avoid this appearance of CPU load, the value can be
        increased to larger than 20000us=20ms.
        */
        sal_usleep(20000);

        for (mbox = 0; mbox < _BCM_MBOX_MAX_BUFFERS; ++mbox) {
            uint32 status;

            if (_bcm_mbox_status_read(unit, mbox, &status) != BCM_E_NONE) {
                continue;
            }

            switch (status) {
            case _BCM_MBOX_MS_TUNNEL_IN:
                break;

            case _BCM_MBOX_MS_EVENT:
#if defined(INCLUDE_GDPLL)
                {
                    uint16 event_id;
                    soc_cm_sinval(unit,
                                  (void*)&mbox_info.unit_state[unit].mboxes->mbox[mbox],
                                  sizeof(mbox_info.unit_state[unit].mboxes->mbox[mbox]));
                    mbox_info.unit_state[unit].response_data =
                        (uint8*)mbox_info.unit_state[unit].mboxes->mbox[mbox].data;
                    mbox_info.unit_state[unit].response_len  = soc_ntohl(
                        mbox_info.unit_state[unit].mboxes->mbox[mbox].data_len);

                    event_id = _shr_uint16_read((uint8*)mbox_info.unit_state[unit].response_data);

                    _bcm_mbox_status_write(unit, mbox, _BCM_MBOX_MS_EMPTY);

                    _bcm_time_host_notification(unit, (int)event_id);
                }
#endif /* defined(INCLUDE_GDPLL) */
                break;

            case _BCM_MBOX_MS_RESP:
                {
                    soc_cm_sinval(unit,
                                  (void*)&mbox_info.unit_state[unit].mboxes->mbox[mbox],
                                  sizeof(mbox_info.unit_state[unit].mboxes->mbox[mbox]));
                    mbox_info.unit_state[unit].response_data =
                        (uint8*)mbox_info.unit_state[unit].mboxes->mbox[mbox].data;
                    mbox_info.unit_state[unit].response_len  = soc_ntohl(
                        mbox_info.unit_state[unit].mboxes->mbox[mbox].data_len);

                    rv = sal_sem_give(mbox_info.unit_state[unit].response_ready);
                    if (BCM_FAILURE(rv)) {
                        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                                    (BSL_META_U(unit, "sal_sem_give failed\n")));
                    }
                }
                break;
            }
        }
    }
}

#endif /* BCM_CMICM_SUPPORT */


int
_bcm_mbox_txrx(
    int unit,
    uint32 node_num,
    _bcm_mbox_transport_type_t transport,
    uint8 *out_data,
    int out_len,
    uint8 *in_data,
    int *in_len)
{
#if defined(BCM_CMICM_SUPPORT)
    int rv;
    uint8 *response_data;
    int response_len;

    /* LOG_CLI((BSL_META_U(unit,
                           "cmic_txrx tx Len:%d\n"), out_len)); */

    int max_response_len = (in_len) ? *in_len : 0;
    if (in_len) {
        *in_len = 0;
    }

    rv = sal_sem_take(mbox_info.comm_available, _BCM_MBOX_RESPONSE_TIMEOUT_US);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "mbox_txrx failed: sal_sem_take\n")));
        return rv;
    }

    rv = _bcm_mbox_tx(unit, node_num, _BCM_MBOX_MESSAGE, out_data, out_len);

    if (rv != BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "mbox_txrx failed: Tx error\n")));
        goto release_mgmt_lock;
    }

    /*
     * Get rx buffer, either from rx callback or from cmicm wait task
     * NOTICE: This call will return an rx response buffer that we will need to
     *         release by notifying the Rx section
     */
    rv = _bcm_mbox_rx_response_get(unit, node_num, _BCM_MBOX_RESPONSE_TIMEOUT_US,
                                   &response_data, &response_len);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "mbox_txrx failed: No response\n")));
        goto release_mgmt_lock;
    }

    
    if (in_data && in_len) {
        if (response_len > max_response_len) {
            response_len = max_response_len;
        }

        *in_len = response_len;
        sal_memcpy(in_data, response_data, response_len);
    }

    /* LOG_CLI((BSL_META_U(unit,
                           "cmic_txrx rx Len:%d\n"), *in_len)); */

    rv = BCM_E_NONE;

/* dispose_of_resp: */
    _bcm_mbox_rx_response_free(unit, response_data);

release_mgmt_lock:
    rv = sal_sem_give(mbox_info.comm_available);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "mbox_txrx failed: sal_sem_give\n")));
    }

    return rv;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

#if defined(BCM_CMICM_SUPPORT)
/* Dump anything new in the debug buffers.
 *  owner:         The owner if this is being called as a DPC.
 *  time_as_ptr:   Recurrence time if a follow-up DPC to this func should
 *                  be scheduled.
 *  unit_as_ptr:   unit
 */
STATIC void
_bcm_mbox_debug_poll(void* owner, void* time_as_ptr, void *unit_as_ptr,
                        void *unused_2, void* unused_3)
{
    int callback_time = (int)(size_t)time_as_ptr;
    int out_idx = 0;

    /* output the local debug first */
    while (local_tail != local_head) {
        char c = local_debugbuf[local_tail++];

        if (c) {
            output_debugbuf[out_idx++] = c;
        }

        if (local_tail == _BCM_MBOX_LOCAL_DEBUGBUFSIZE) {
            local_tail = 0;
        }
    }

    /* CMICM shared-mem output */
    {
        int unit = (int)(size_t)unit_as_ptr;
        uint32 head, size;
        if ((SOC_CONTROL(unit) != NULL ) &&
            (soc_feature(unit, soc_feature_cmicm) ||
            soc_feature(unit, soc_feature_iproc))) {
            /* head is written in externally, so will be in network byte order
             * tail is local, so we'll keep it in local endianness
             * size is read externally, so is also network byte order
             */
            head = soc_htonl(mbox_info.unit_state[unit].log->head);
            size = soc_htonl(mbox_info.unit_state[unit].log->size);

            while (mbox_info.unit_state[unit].log->tail != head) {
                char c =
                    mbox_info.unit_state[unit].log->buf[mbox_info.unit_state[unit].log->tail++];
                if (c) {
                    output_debugbuf[out_idx++] = c;
                }
                if (mbox_info.unit_state[unit].log->tail == size) {
                    mbox_info.unit_state[unit].log->tail = 0;
                }
            }
        }
    }


    if (out_idx) {
        output_debugbuf[out_idx] = 0;
        if (_cmicm_debug_flags) {
            LOG_CLI((BSL_META("%s"), output_debugbuf));
        }
    }

    if (callback_time) {
        sal_dpc_time(callback_time, &_bcm_mbox_debug_poll,
                     owner, time_as_ptr, unit_as_ptr, 0, 0);
    }
}

#endif /* BCM_CMICM_SUPPORT */
int
_bcm_mbox_debug_flag_set(uint32 flags)
{
    _cmicm_debug_flags = flags;

    return BCM_E_NONE;
}


int
_bcm_mbox_debug_flag_get(uint32 *flags)
{
    *flags = _cmicm_debug_flags;

    return BCM_E_NONE;
}
