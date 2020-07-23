/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    internal_stack.c
 *
 * Purpose: 
 *
 * Functions:
 *      _bcm_kt_ptp_hw_init
 *      _bcm_ptp_internal_stack_create
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <bcm_int/common/rx.h>
#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/ptp_common.h>
#include <bcm/error.h>
#include <bcm_int/common/PTP_feature.h>

/* Global variables */
/* PTP FW version for feature compatibility check */
uint32 ptp_firmware_version = PTP_UC_MIN_VERSION;
#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)

#include <soc/uc_msg.h>
#include <soc/uc_dbg.h>


/*
 * Function:
 *      bcm_kt_ptp_hw_init
 * Purpose:
 *      Initialize the HW for PTP packet processing.
 *      Configure:
 *      - FP rules to copy L2, IPv4, and IPv6 PTP traffic to CPU
 *      - Assign RX DMA channel to PTP CPU COS Queue
 *      - Map RX PTP packets to PTP CPU COS Queue
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_kt_ptp_hw_init(int unit, int uc_num)
{
    int rv = BCM_E_NONE;
    bcm_rx_chan_t chan_id;
    int num_cosq = 0;
    int min_cosq, max_cosq, ptp_cosq;
    int i;
    int rx_channel = 1;  /* For now, PTP is always on uC by itself, so always use Rx channel 1 */

    /*
     * PTP COS Queue
     *
     * - The default PTP COS queue.  This is set to the highest queue
     *   available within the queue range assigned to the uC where
     *   the PTP application is running.
     */
    

    /* Get valid range of COS queues for uC where PTP is loaded */
    min_cosq = 0;
    for (i = 0; i < SOC_CMCS_NUM(unit); i++) {
        num_cosq = NUM_CPU_ARM_COSQ(unit, i);
        if (i == uc_num + 1) {
            break;
        }
        min_cosq += num_cosq;
    }

    max_cosq = min_cosq + num_cosq - 1;

    /* Check that there is at least one COS queue assigned to the CMC */
    if (max_cosq < min_cosq) {
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                     "PTP unable to assign COSQ, none are assigned to FW core.\n")));
        return BCM_E_CONFIG;
    }

    /* Get PTP COSq soc property */
    ptp_cosq = soc_property_get(unit, spn_PTP_COSQ, max_cosq);

    /*
     * Assign RX DMA channel to PTP CPU COS Queue
     * (This is the RX channel to listen on for PTP packets).
     *
     * DMA channels (12) are assigned 4 per processor:
     * (see /src/bcm/common/rx.c)
     *   channels 0..3  --> PCI host
     *   channels 4..7  --> uController 0
     *   chnanels 8..11 --> uController 1
     *
     * The uControllers designate the 4 local DMA channels as follows:
     *   local channel  0     --> TX
     *   local channel  1..3  --> RX
     *
     * Each uController application needs to use a different
     * RX DMA channel to listen on.
     */
    chan_id = (BCM_RX_CHANNELS * (SOC_ARM_CMC(unit, uc_num))) + rx_channel;

    if (ptp_cosq != max_cosq) 
    {
       if ((ptp_cosq < min_cosq) || (ptp_cosq > max_cosq)) {
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                 "PTP COSQ %d is out of bounds for FW core [%d - %d].\n"),
                 ptp_cosq, min_cosq, max_cosq));

          return BCM_E_RESOURCE;    /* Could not find an available COSQ */
       }

       rv = _bcm_common_rx_queue_channel_set(unit, ptp_cosq, chan_id);

       if (!BCM_SUCCESS(rv)) {
           LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                 "PTP unable to assign COSQ %d.\n"),
                 ptp_cosq));

           return rv;
       }
    }
    else
    {
       for (i = max_cosq; i >= min_cosq; i--) {
          rv = _bcm_common_rx_queue_channel_set(unit, i, chan_id);
          if (BCM_SUCCESS(rv)) {
             break;
          }
       }

       if (i < min_cosq) {
           LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                 "PTP unable to assign COSQ from range %d - %d.\n"),
                 min_cosq, max_cosq));

           return BCM_E_RESOURCE;    /* Could not find an available COSQ */
       }

       ptp_cosq = i;
    }

    LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit,
        "1588 COSQ for unit %d is %d chan_id: %d\n"), unit, ptp_cosq, chan_id));

    return rv;
}


/*
 * Function:
 *      _bcm_ptp_internal_stack_create
 * Purpose:
 *      Create a PTP stack instance
 * Parameters:
 *      unit - (IN) Unit number.
 *      ptp_info - (IN/OUT) Pointer to an PTP Stack Info structure
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
_bcm_ptp_internal_stack_create(
    int unit, 
    bcm_ptp_stack_info_t *info,
    bcm_ptp_stack_id_t ptp_id)
{
    _bcm_ptp_info_t *ptp_info_p;
    _bcm_ptp_stack_info_t *stack_p;
    mos_msg_data_t start_msg;
    sal_paddr_t buf_paddr;
    int timeout_usec = 1900000;
    int result;
    int c;
    int i;
    int rv;

    /* log storage size is the size of the structure without the placeholder */
    /* space for debug->buf, plus the real space for it                      */
    int log_storage_size = sizeof(_bcm_ptp_internal_stack_log_t) -
        sizeof(stack_p->int_state.log->buf) + BCM_PTP_MAX_LOG;

    SET_PTP_INFO;
    if (!SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        return BCM_E_UNAVAIL;
    }

    stack_p = &ptp_info_p->stack_info[ptp_id];

    /* Set up dispatch for internal transport */
    stack_p->transport_init = _bcm_ptp_internal_transport_init;
    stack_p->tx = _bcm_ptp_internal_tx;
    stack_p->tx_completion = _bcm_ptp_internal_tx_completion;
    stack_p->rx_free = _bcm_ptp_internal_rx_response_free;
    stack_p->transport_terminate = _bcm_ptp_internal_transport_terminate;

    /* allocate space for mboxes */
    stack_p->int_state.mboxes = soc_cm_salloc(unit, sizeof(_bcm_ptp_internal_stack_mboxes_t), "ptp msg");

    if (!stack_p->int_state.mboxes) {
        return BCM_E_MEMORY;
    }

    /* clear state of message mboxes */
    stack_p->int_state.mboxes->num_mboxes = soc_htonl(BCM_PTP_MAX_BUFFERS);
    stack_p->int_state.mboxes->mbox_size = soc_htonl(sizeof(_bcm_ptp_internal_stack_mbox_t));
    stack_p->int_state.mboxes->mbox_offset = soc_htonl((uint8 *)&stack_p->int_state.mboxes->mbox[0] - (uint8 *)stack_p->int_state.mboxes);
    stack_p->int_state.mboxes->rfu = soc_htonl(0);

    for (i = 0; i < BCM_PTP_MAX_BUFFERS; ++i) {
        stack_p->int_state.mboxes->mbox[i].status = soc_htonl(MBOX_STATUS_EMPTY);
        stack_p->int_state.mboxes->mbox[i].clock_num = soc_htonl(0);
        stack_p->int_state.mboxes->mbox[i].data_len = soc_htonl(0);
        stack_p->int_state.mboxes->mbox[i].rfu_mbox = soc_htonl(0);
    }

    /* allocate space for debug log */
    stack_p->int_state.log = soc_cm_salloc(unit, log_storage_size, "ptp log");
    if (!stack_p->int_state.log) {
        soc_cm_sfree(unit, stack_p->int_state.mboxes);
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                     "PTP Failed to allocate shared memory\n")));
        return BCM_E_MEMORY;
    }

    /* initialize debug */
    stack_p->int_state.log->size = soc_htonl(BCM_PTP_MAX_LOG);
    stack_p->int_state.log->head = soc_htonl(0);
    stack_p->int_state.log_tail = 0;

    /* set up the network-byte-order pointers so that CMICm can access the shared memory */
    stack_p->int_state.mbox_ptr = soc_htonl(soc_cm_l2p(unit, (void*)stack_p->int_state.mboxes));
    stack_p->int_state.log_ptr = soc_htonl(soc_cm_l2p(unit, (void*)stack_p->int_state.log));

    /* LOG_CLI((BSL_META_U(unit,
                           "DEBUG SPACE: %p\n"), (void *)stack_p->int_state.log->buf)); */

    /* flush memory on platforms that need it */
    soc_cm_sflush(unit, stack_p, sizeof(_bcm_ptp_stack_info_t));
    soc_cm_sflush(unit, stack_p->int_state.mboxes, sizeof(_bcm_ptp_internal_stack_mboxes_t));
    soc_cm_sflush(unit, stack_p->int_state.log, log_storage_size);

    rv = BCM_E_UNAVAIL;
    for (c = (SOC_INFO(unit).num_ucs - 1); c >= 0; c--) {
        LOG_VERBOSE(BSL_LS_BCM_PTP, (BSL_META_U(unit,
            "Trying PTP on core %d\n"), c));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtp,
            "Trying PTP on core %d\n", c));
        result = soc_cmic_uc_appl_init(unit, c,
                                       MOS_MSG_CLASS_1588,
                                       timeout_usec,
                                       PTP_SDK_VERSION, PTP_UC_MIN_VERSION,
                                       &_bcm_ptp_stack_deinit_callback, INT_TO_PTR(ptp_id));
        if (SOC_E_NONE == result){
            /* uKernel communications started successfully */
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtp,
                "PTP -> uKernel communications started successfully\n"));
            soc_cmic_ucdebug_core_communication_status(unit,
                CMIC_UC_DBG_PTP_CORE_COMM_SUCCESS);
            buf_paddr = soc_cm_l2p(unit, (void*)&stack_p->int_state);
            start_msg.s.mclass = MOS_MSG_CLASS_1588;
            start_msg.s.subclass = MOS_MSG_SUBCLASS_MBOX_CONFIG;
            start_msg.s.len = 0;
            start_msg.s.data = bcm_htonl(PTR_TO_INT(buf_paddr));

            if (BCM_FAILURE(rv = soc_cmic_uc_msg_send(unit, c, &start_msg, timeout_usec))) {
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtp,
                    "Error in Initializing PTP application\n"));
                PTP_ERROR_FUNC("soc_cmic_uc_msg_send()");
            }
            stack_p->int_state.core_num = c;
            break;
        }
        LOG_WARN(BSL_LS_BCM_PTP, (BSL_META_U(unit,
                            "No response on core %d\n"), c));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostPtp,
                            "No response on core %d\n", c));
    }

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_cmic_ucdebug_core_communication_status(unit,
        CMIC_UC_DBG_PTP_STACK_INIT_SUCCESS);

    rv = _bcm_kt_ptp_hw_init(unit, stack_p->int_state.core_num);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_internal_stack_cleanup
 * Purpose:
 *      Cleans up internal allocations of an internal stack
 * Parameters:
 *      unit - (IN) Unit number.
 *      ptp_id - (IN) Stack ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_ptp_internal_stack_cleanup(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    _bcm_ptp_stack_info_t *stack_p;

    if (!SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_common_ptp_info[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_UNAVAIL;
    }

    stack_p = &_bcm_common_ptp_info[unit].stack_info[ptp_id];

    soc_cm_sfree(unit, stack_p->int_state.log);
    stack_p->int_state.log = NULL;

    soc_cm_sfree(unit, stack_p->int_state.mboxes);
    stack_p->int_state.mboxes = NULL;

    return BCM_E_NONE;
}

int
_bcm_ptp_internal_stack_shutdown(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_NONE;

    uint8 payload[PTP_MGMTMSG_PAYLOAD_MIN_PROPRIETARY_MSG_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;
    int i = 0;
    bcm_ptp_port_identity_t portid;

    const bcm_ptp_port_identity_t portid_all =
        {{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, PTP_IEEE1588_ALL_PORTS};

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id,
            0, PTP_IEEE1588_ALL_PORTS, &portid))) {
        /* On error, target message to (all clocks, all ports) portIdentity. */
        portid = portid_all;
    }
    portid = portid_all;

    /*
     * Make payload.
     *    Octet 0...5 : Custom management message key/identifier.
     *                  BCM<null><null><null>.
     */
    i = 0;
    payload[i++] = 'B';
    payload[i++] = 'C';
    payload[i++] = 'M';

    if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, ptp_id, 0,
                    &portid, PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_SHUTDOWN,
                    payload, PTP_MGMTMSG_PAYLOAD_MIN_PROPRIETARY_MSG_SIZE_OCTETS,
                    resp, &resp_len))) {
        return rv;
    }
    LOG_INFO(BSL_LS_BCM_PTP, (BSL_META_U(unit, "\n Shutdown msg sent\n")));
    return BCM_E_NONE;
}
#endif /* defined(BCM_PTP_INTERNAL_STACK_SUPPORT) */
#endif /* defined(INCLUDE_PTP) */
