/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INT - In-band Telemetry Turnaround Embedded Application APP interface
 * Purpose: API to configure In-band Telemetry Turnaround embedded app interface.
 */

#if defined(INCLUDE_INT) && defined(BCM_TOMAHAWK3_SUPPORT)

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/bslenum.h>
#include <shared/idxres_fl.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/int.h>

#include <bcm_int/common/int_feature.h>
#include <bcm_int/common/int.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/tomahawk3_dispatch.h>

#include <soc/shared/int.h>
#include <soc/shared/int_msg.h>
#include <soc/shared/int_pack.h>
#include <soc/shared/shr_pkt.h>

/*Global to hold the INT Turnaround firmware version for feature compatibility check */
uint32 int_firmware_version = 0;

/* INT Turnaround global information */
_bcm_int_turnaround_info_t *int_global_info[BCM_MAX_NUM_UNITS] = {0};
static sal_mutex_t mutex[BCM_MAX_NUM_UNITS];

/* List of INT Turnaround configuration information */
bcm_int_turnaround_config_t int_turnaround_config[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_int_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_int_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_INT_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_INT_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_INT_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_INT_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_INT_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_INT_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_INT_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_INT_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_INT_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_INT_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_INT_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_INT_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_INT_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_INT_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_INT_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_INT_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_INT_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_INT_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_INT_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

STATIC int
_bcm_int_lb_header_get(
        shr_lb_header_t *lb)
{
    /* LB Header below fields fixed in case of INT Turnaround eapps */
    sal_memset(lb, 0, sizeof(*lb));
    lb->start               = 0xFB;
    lb->common_hdr0         = 1;
    lb->common_hdr1         = 1;
    lb->source_type         = 1;

    /* LB header other fields are update by R5 */
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_int_eapp_lb_encap_build_pack
 * Purpose:
 *      Builds and packs the INT LB Header
 * Parameters:
 *      config_info     - (IN) Pointer to config info structure.
 *      config_info_msg - (OUT) Message which contains the int header encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only INT headers
 */
STATIC int
_bcm_int_eapp_lb_encap_build_pack(int unit,
        bcm_int_turnaround_config_t *config_info,
        shr_int_turnaround_msg_ctrl_config_t *config_info_msg)
{
    uint8               *buffer  = config_info_msg->lb_encap;
    uint8               *cur_ptr;
    shr_lb_header_t     lb;

    sal_memset(&lb, 0, sizeof(lb));
    BCM_IF_ERROR_RETURN
        (_bcm_int_lb_header_get(&lb));

    /*
     * Pack header into given buffer (network packet format).
     */
    cur_ptr = buffer;

    /* LoopBack Header is always present */
    cur_ptr = shr_lb_header_pack(cur_ptr, &lb);

    /* Set INT export pkt encapsulation length */
    config_info_msg->lb_encap_length = cur_ptr - buffer;

    if (config_info_msg->lb_encap_length != SHR_LB_HEADER_LENGTH) {
        LOG_DEBUG(BSL_LS_BCM_INT, (BSL_META_U(unit,
                        "INT(unit %d) Info: update LB header"), unit));
        return (BCM_E_INTERNAL);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_int_msg_send_receive(int unit, uint8 s_subclass,
        uint16 s_len, uint32 s_data,
        uint8 r_subclass, uint16 *r_len,
        sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;
    uint32 uc_rv;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    uint8 *dma_buffer;
    int dma_buffer_len;


    dma_buffer = int_turnaround_info->dma_buffer;
    dma_buffer_len = int_turnaround_info->dma_buffer_len;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_INT;
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

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, int_turnaround_info->uc_num,
            &send, &reply,
            timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert INT Turnaround uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_int_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_INT) ||
                (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

STATIC int
_bcm_int_eapp_send_ctrl_init_msg (int unit)
{
    shr_int_turnaround_msg_ctrl_init_t ctrl_init_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    sal_memset(&ctrl_init_msg, 0, sizeof(ctrl_init_msg));
    ctrl_init_msg.rx_channel = BCM_INT_EAPP_RX_CHANNEL;
    ctrl_init_msg.tx_channel = BCM_INT_EAPP_TX_CHANNEL;

    buffer_req      = int_turnaround_info->dma_buffer;
    buffer_ptr      = shr_int_turnaround_msg_ctrl_init_pack(buffer_req, &ctrl_init_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_int_msg_send_receive(unit, MOS_MSG_SUBCLASS_INT_INIT,
                buffer_len, 0,
                MOS_MSG_SUBCLASS_INT_INIT_REPLY, &reply_len,
                SHR_INT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*******************************************************
 *                                                     *
 *           INT TURNAROUND EMBEDDED APP APIs          *
 */

STATIC int
bcm_int_eapp_detach(int unit)
{
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    int                          rv = BCM_E_NONE;
    int                          status = 0;
    uint16                       reply_len = 0;

    if (!SOC_WARM_BOOT(unit)) {
        /* Un Init the app */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, int_turnaround_info->uc_num, &status));
        if (status) {
            rv = _bcm_int_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_INT_SHUTDOWN,
                    0, 0,
                    MOS_MSG_SUBCLASS_INT_SHUTDOWN_REPLY,
                    &reply_len,
                    SHR_INT_MAX_UC_MSG_TIMEOUT);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                return BCM_E_INTERNAL;
            }
        }
    }

    /*
     * Free DMA buffer
 */
    if (int_turnaround_info->dma_buffer != NULL) {
        soc_cm_sfree(unit, int_turnaround_info->dma_buffer);
        int_turnaround_info->dma_buffer = NULL;
    }

    return BCM_E_NONE;
}

STATIC int
bcm_int_turnaround_eapp_config_get_msg(int unit,
        bcm_int_turnaround_config_t *config_info)
{
    shr_int_turnaround_msg_ctrl_config_t config_info_msg;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    if (config_info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(config_info, 0, sizeof(bcm_int_turnaround_config_t));
    sal_memset(&config_info_msg, 0, sizeof(config_info_msg));

    BCM_IF_ERROR_RETURN(_bcm_int_msg_send_receive(unit, MOS_MSG_SUBCLASS_INT_CONFIG_GET,
                buffer_len, 0,
                MOS_MSG_SUBCLASS_INT_CONFIG_GET_REPLY, &reply_len,
                SHR_INT_MAX_UC_MSG_TIMEOUT));
    buffer_req      = int_turnaround_info->dma_buffer;
    buffer_ptr      = shr_int_turnaround_msg_ctrl_config_unpack(buffer_req, &config_info_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }

    sal_memcpy(config_info->src_mac, config_info_msg.src_mac, SHR_MAC_ADDR_LENGTH);

    return BCM_E_NONE;
}

STATIC int
bcm_int_eapp_init(int unit)
{
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int uc = 0;
    int status;
    bcm_int_turnaround_config_t *config_info = (&(int_turnaround_config[unit]));

    /* Init the app */
    /*
     * Start INT Turnaround application in BTE (Broadcom Task Engine) uController,
     * if not already running (warm boot).
     * Determine which uController is running INT Turnaround  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++) {
        rv = soc_uc_status(unit, uc, &status);
        if ((rv == SOC_E_NONE) && (status != 0)) {
            rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_INT,
                    SHR_INT_MAX_UC_MSG_TIMEOUT,
                    INT_SDK_VERSION,
                    INT_UC_MIN_VERSION,
                    NULL, NULL);
            if (SOC_E_NONE == rv) {
                /* INT Turnaround started successfully */
                break;
            }
        }
    }

    if (uc >= CMICM_NUM_UCS) {
        /* Could not find or start INT Turnaround appl */
        LOG_WARN(BSL_LS_BCM_INT,
                (BSL_META_U(unit,
                            "uKernel INT Turnaround application not available\n")));
        return BCM_E_NONE;
    }
    int_turnaround_info->uc_num                 = uc;
    int_turnaround_info->cosq_turnaround_index  = -1;
    int_turnaround_info->cosq_hoplimit_index    = -1;

    /* Detach if already initialized */
    if (!soc_feature(unit, soc_feature_uc_int_turnaround)) {
        BCM_IF_ERROR_RETURN(bcm_int_eapp_detach(unit));
        return BCM_E_INIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the init config to UKERNEL */
        rv = _bcm_int_eapp_send_ctrl_init_msg(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_int_eapp_detach(unit));
            return rv;
        }
        sal_memset(config_info, 0, sizeof(bcm_int_turnaround_config_t));
    } else {
        /* Get the Config info */
        rv = bcm_int_turnaround_eapp_config_get_msg(unit, config_info);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcm_int_turnaround_eapp_config_msg(int unit,
        bcm_int_turnaround_config_t *config_info)
{
    shr_int_turnaround_msg_ctrl_config_t config_info_msg;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    sal_memset(&config_info_msg, 0, sizeof(config_info_msg));

    sal_memcpy(config_info_msg.src_mac, config_info->src_mac, SHR_MAC_ADDR_LENGTH);
    rv = _bcm_int_eapp_lb_encap_build_pack(unit,
            config_info, &config_info_msg);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    buffer_req      = int_turnaround_info->dma_buffer;
    buffer_ptr      = shr_int_turnaround_msg_ctrl_config_pack(buffer_req, &config_info_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_int_msg_send_receive(unit, MOS_MSG_SUBCLASS_INT_CONFIG_SET,
                buffer_len, 0,
                MOS_MSG_SUBCLASS_INT_CONFIG_SET_REPLY, &reply_len,
                SHR_INT_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_int_tunraround_eapp_stat_get_msg(int unit,
        bcm_int_turnaround_stat_t *stat_data)
{
    shr_int_turnaround_msg_ctrl_stat_t stat_data_msg;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(stat_data, 0, sizeof(bcm_int_turnaround_stat_t));
    sal_memset(&stat_data_msg, 0, sizeof(stat_data_msg));

    BCM_IF_ERROR_RETURN(_bcm_int_msg_send_receive(unit, MOS_MSG_SUBCLASS_INT_STAT_GET,
                buffer_len, 0,
                MOS_MSG_SUBCLASS_INT_STAT_GET_REPLY, &reply_len,
                SHR_INT_MAX_UC_MSG_TIMEOUT));
    buffer_req      = int_turnaround_info->dma_buffer;
    buffer_ptr      = shr_int_turnaround_msg_ctrl_stat_unpack(buffer_req, &stat_data_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }

    COMPILER_64_SET(stat_data->rx_pkt_cnt, stat_data_msg.rx_pkt_cnt_hi,
                    stat_data_msg.rx_pkt_cnt_lo);
    COMPILER_64_SET(stat_data->tx_pkt_cnt, stat_data_msg.tx_pkt_cnt_hi,
                    stat_data_msg.tx_pkt_cnt_lo);
    COMPILER_64_SET(stat_data->drop_pkt_cnt, stat_data_msg.drop_pkt_cnt_hi,
                    stat_data_msg.drop_pkt_cnt_lo);
    COMPILER_64_SET(stat_data->int_init_config_drop, stat_data_msg.int_init_config_drop_hi,
                    stat_data_msg.int_init_config_drop_lo);
    COMPILER_64_SET(stat_data->int_hop_cnt_invalid_drop, stat_data_msg.int_hop_cnt_invalid_drop_hi,
                    stat_data_msg.int_hop_cnt_invalid_drop_lo);
    COMPILER_64_SET(stat_data->int_hdr_len_invalid_drop, stat_data_msg.int_hdr_len_invalid_drop_hi,
                    stat_data_msg.int_hdr_len_invalid_drop_lo);
    COMPILER_64_SET(stat_data->int_pkt_size_invalid_drop, stat_data_msg.int_pkt_size_invalid_drop_hi,
                    stat_data_msg.int_pkt_size_invalid_drop_lo);

    return BCM_E_NONE;
}

/* Shut down the INT TURNAROUND app. */
STATIC int
_bcm_th3_int_detach(int unit)
{
    /* De-init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    if (int_turnaround_info == NULL) {
        return BCM_E_NONE;
    }

    rv = bcm_int_eapp_detach(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_INT, (BSL_META_U(unit,
                        "INT TURNAROUND: Failed to Detach")));
        return rv;
    }

    /* Delete CPU COS queue mapping entries for INT TURNAROUND packets */
    if (int_turnaround_info->cosq_turnaround_index >= 0) {
        (void) bcm_esw_rx_cosq_mapping_delete(unit,
                int_turnaround_info->cosq_turnaround_index);
        int_turnaround_info->cosq_turnaround_index = -1;
    }
    if (int_turnaround_info->cosq_hoplimit_index >= 0) {
        (void) bcm_esw_rx_cosq_mapping_delete(unit,
                int_turnaround_info->cosq_hoplimit_index);
        int_turnaround_info->cosq_hoplimit_index = -1;
    }

    sal_free(int_turnaround_info);
    int_turnaround_info = NULL;
    int_global_info[unit] = NULL;

    return rv;
}

/*
 * Function:
 *      bcm_int_turnaround_hw_init
 * Purpose:
 *      Initialize the HW for INT Turnaround packet processing.
 *      Configure:
 *      - Copy to CPU INT Turnaround error packets
 *      - Assign RX DMA channel to CPU COS Queue
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcm_int_turnaround_hw_init(int unit)
{
    int rv = BCM_E_NONE;
    int index, turnaround_index, hoplimit_index;
    int cosq_map_size;
    bcm_rx_reasons_t reasons, reasons_mask, reasons_all;
    uint8 int_prio, int_prio_mask;
    uint32 packet_type, packet_type_mask;
    bcm_cos_queue_t cosq, cpu_cosq;
    bcm_rx_chan_t chan_id;
    int num_cosq = 0;
    int min_cosq, max_cosq;
    int i;
    uint32 max_rx_channels = 0;
    uint32 num_of_cpu_queues = 0;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    /*
     * INT Turnaround COS Queue
     *
     * INT Turnaround COSQ will be set to one of the following:
     * - User configured INT Turnaround COSq, if provided
     * - The default INT Turnaround COS queue.  This is set to the highest queue
     *   available within the queue range assigned to the uC where
     *   the INT Turnaround application is running.
     */

    /* Get valid range of COS queues for uC where INT Turnaround is loaded */
    min_cosq = 0;
    num_of_cpu_queues = SOC_CMCS_NUM(unit);

    for (i = 0; i < num_of_cpu_queues; i++) {
        num_cosq = NUM_CPU_ARM_COSQ(unit, i);
        if (i == int_turnaround_info->uc_num + 1) {
            break;
        }
        min_cosq += num_cosq;
    }
    max_cosq = min_cosq + num_cosq - 1;

    /* Check that there is at least one COS queue assigned to the CMC */
    if (max_cosq < min_cosq) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "ERROR: No INT Turnaround COS Queue available from uC%d\n"),
                  int_turnaround_info->uc_num));
        return BCM_E_CONFIG;
    }

    /*
     * Assign RX DMA channel to INT Turnaround CPU COS Queue
     * (This is the RX channel to listen on for INT Turnaround packets).
     */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        max_rx_channels = BCM_CMICX_RX_CHANNELS;
    }
#endif /* BCM_CMICX_SUPPORT */

    chan_id = (max_rx_channels * (SOC_ARM_CMC(unit, int_turnaround_info->uc_num))) +
        BCM_INT_EAPP_RX_CHANNEL;
    for (i = max_cosq; i >= min_cosq; i--) {
        rv = _bcm_common_rx_queue_channel_set(unit, i, chan_id);
        if (BCM_SUCCESS(rv)) {
            cpu_cosq = i;  /* COSQ found */
            break;
        }
    }

    if (i < min_cosq) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "ERROR: INT Turnaround COS Queue minimum uC%d\n"),
                  int_turnaround_info->uc_num));
        return BCM_E_RESOURCE;    /* Could not find an available COSQ */
    }

    /*
     * Direct INT Turnaround packets to designated RX CPU COS Queue
     * Reasons:
     *   - bcmRxReasonIntTurnAround
     *         INT TurnAround
     *   - bcmRxReasonIntHopLimit
     *         INT Hop Limit
     */

    /* Find available entries in CPU COS queue map table */
    turnaround_index   = -1;   /* COSQ map index for TurnAround */
    hoplimit_index    = -1;   /* COSQ map index for Hop Limit */
    BCM_IF_ERROR_RETURN
        (bcm_esw_rx_cosq_mapping_size_get(unit, &cosq_map_size));

    for (index = 0; index < cosq_map_size; index++) {
        rv = bcm_esw_rx_cosq_mapping_get(unit, index,
                                         &reasons, &reasons_mask,
                                         &int_prio, &int_prio_mask,
                                         &packet_type, &packet_type_mask,
                                         &cosq);
        if (rv == BCM_E_NOT_FOUND) {
            /* Find available indexes for INT Turnaround reason codes */
            rv = BCM_E_NONE;
            if (turnaround_index == -1) {
                turnaround_index = index;
            } else {
                hoplimit_index = index;
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                      "ERROR: INT Turnaround COS mapping uC%d\n"),
                      int_turnaround_info->uc_num));
            return rv;
        }
    }
    if ((turnaround_index == -1) || (hoplimit_index == -1)) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "ERROR: INT Turnaround Index uC%d\n"),
                  int_turnaround_info->uc_num));
        return BCM_E_RESOURCE;
    }

    /*
     * Map RX reason code to RX CPU COS Queue
     */
    BCM_IF_ERROR_RETURN
        (bcm_esw_rx_cosq_mapping_reasons_get(unit, &reasons_all));

    /* INT TurnAround hit */
    BCM_RX_REASON_CLEAR_ALL(reasons);
    BCM_RX_REASON_SET(reasons, bcmRxReasonIntTurnAround);
    rv = bcm_esw_rx_cosq_mapping_set(unit, turnaround_index,
                                     reasons, reasons,
                                     0, 0, /* Any priority */
                                     0, 0, /* Any packet type */
                                     cpu_cosq);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "ERROR: INT Turnaround index mapping uC%d\n"),
                  int_turnaround_info->uc_num));
        return rv;
    }
    int_turnaround_info->cosq_turnaround_index = turnaround_index;

    /* INT Hop Limit */
    BCM_RX_REASON_CLEAR_ALL(reasons);
    BCM_RX_REASON_SET(reasons, bcmRxReasonIntHopLimit);
    rv = bcm_esw_rx_cosq_mapping_set(unit, hoplimit_index,
                                     reasons, reasons,
                                     0, 0, /* Any priority */
                                     0, 0, /* Any packet type */
                                     cpu_cosq);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                  "ERROR: INT Turnaround Hop limit mapping uC%d\n"),
                  int_turnaround_info->uc_num));
        return rv;
    }
    int_turnaround_info->cosq_hoplimit_index = hoplimit_index;
    return rv;
}


/* Initialize the  INT Turnaround app. */
STATIC int
_bcm_th3_int_init(int unit)
{
    /* Init the embedded app */
    int rv = BCM_E_NONE;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    int int_turnaround_enable = 0;
    int_turnaround_enable = soc_property_get(unit, spn_INT_TURNAROUND_ENABLE, 0);

    if (!int_turnaround_enable) {
        /* Silently return since INT Turnaround is not enabled */
        return BCM_E_NONE;
    }

    if (int_turnaround_info != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_th3_int_detach(unit));
        int_turnaround_info = NULL;
    }

    _BCM_INT_ALLOC(int_turnaround_info, _bcm_int_turnaround_info_t,
            sizeof(_bcm_int_turnaround_info_t), "INT TURNAROUND INFO");
    if (int_turnaround_info == NULL) {
        LOG_ERROR(BSL_LS_BCM_INT, (BSL_META_U(unit,
                        "INT TURNAROUND: Failed to allocate memory")));
        return BCM_E_MEMORY;
    }
    int_global_info[unit] = int_turnaround_info;
    int_turnaround_info->uc_num   = -1;

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    int_turnaround_info->dma_buffer_len = sizeof(shr_int_turnaround_msg_ctrl_init_t);
    int_turnaround_info->dma_buffer = soc_cm_salloc(unit, int_turnaround_info->dma_buffer_len,
            "INT Turnaround DMA buffer allocate");
    if (!int_turnaround_info->dma_buffer) {
        BCM_IF_ERROR_RETURN(bcm_int_eapp_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(int_turnaround_info->dma_buffer, 0, int_turnaround_info->dma_buffer_len);

    rv = bcm_int_eapp_init(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_INT, (BSL_META_U(unit,
                        "INT TURNAROUND App Config set uc message communication failed \n")));
        return rv;
    }

    if (int_turnaround_info->uc_num == -1) {
        /* Could not start INT Turnaround appl */
        soc_cm_sfree(unit, int_turnaround_info->dma_buffer);
        int_turnaround_info->dma_buffer = NULL;

        sal_free(int_turnaround_info);
        int_turnaround_info = NULL;
        int_global_info[unit] = NULL;
        return BCM_E_NONE;
    } else {
        rv = bcm_int_turnaround_hw_init(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_INT, (BSL_META_U(unit,
                            "INT TURNAROUND App hardware issue failed \n")));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_int_turnaround_config_set
 * Purpose:
 *      Create a INT Turnaround Configuration informatio
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      config_info     - (IN)  Configuration information
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_int_turnaround_config_set(int unit,
        bcm_int_turnaround_config_t *config_info)
{
    int rv = BCM_E_NONE;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    if (int_turnaround_info == NULL) {
        return BCM_E_INIT;
    }

    bcm_int_turnaround_config_t *config_info_node = (&(int_turnaround_config[unit]));
    sal_memcpy(config_info_node, config_info,
            sizeof(bcm_int_turnaround_config_t));

    /* Inform INT TURNAROUND EAPP of the change/addition in/of configuration information */
    rv = bcm_int_turnaround_eapp_config_msg(unit, config_info);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_INT, (BSL_META_U(unit,
                        "INT TURNAROUND App Config set uc message communication failed \n")));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_int_turnaround_config_get
 * Purpose:
 *      Get INT Turnaround configuration information
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      config_info      - (OUT)  Configuration information info
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_int_turnaround_config_get(int unit,
        bcm_int_turnaround_config_t *config_info)
{
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);
    bcm_int_turnaround_config_t *config_info_data = NULL;

    if (int_turnaround_info == NULL) {
        return BCM_E_INIT;
    }
    config_info_data = (bcm_int_turnaround_config_t *)(&(int_turnaround_config[unit]));

    /* Ensure configuration_info is not NULL */
    if (config_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the configuration info */
    sal_memcpy(config_info, config_info_data,
            sizeof(bcm_int_turnaround_config_t));
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th3_int_turnaround_stat_get
 * Purpose:
 *      Get INT Turnaround statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_int_turnaround_stat_get(int unit,
        bcm_int_turnaround_stat_t *stat_data)
{
    int rv = BCM_E_NONE;
    _bcm_int_turnaround_info_t *int_turnaround_info = INT_TURNAROUND_INFO_GET(unit);

    if (int_turnaround_info == NULL) {
        return BCM_E_NONE;
    }

    /* Ensure stat_data is not NULL */
    if (stat_data == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the Stat info */
    rv = bcm_int_tunraround_eapp_stat_get_msg(unit, stat_data);
    return rv;
}

int bcm_tomahawk3_int_lock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(mutex[unit], sal_mutex_FOREVER);
    return BCM_E_NONE;
}

int bcm_tomahawk3_int_unlock(int unit)
{
    if (mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(mutex[unit]) != 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/* Initialize the INT Turnaround app. */
int bcm_tomahawk3_int_init(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Create mutex */
    if (mutex[unit] == NULL) {
        mutex[unit] = sal_mutex_create("int.mutex");
        if (mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        result = _bcm_th3_int_init(unit);
    }

    /* If init itself fails, there is no point in having the mutex.
     * Destroy it.
     */
    if (BCM_FAILURE(result)) {
        sal_mutex_destroy(mutex[unit]);
        mutex[unit] = NULL;
    }
    return result;
}

/* Shut down the INT Turnaround app. */
int bcm_tomahawk3_int_detach(int unit)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_lock(unit));

    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        result = _bcm_th3_int_detach(unit);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_unlock(unit));
    return result;
}

int bcm_tomahawk3_int_turnaround_config_set(int unit,
        bcm_int_turnaround_config_t *config_data)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_lock(unit));

    /* INT Turnaround app APIs.  */
    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        result = _bcm_th3_int_turnaround_config_set(unit,
                config_data);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_unlock(unit));
    return result;
}

int bcm_tomahawk3_int_turnaround_config_get(int unit,
        bcm_int_turnaround_config_t *config_data)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_lock(unit));

    /* INT Turnaround app APIs.  */
    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        result = _bcm_th3_int_turnaround_config_get(unit,
                config_data);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_unlock(unit));
    return result;
}

/*
 * Function:
 *      bcm_tomahawk3_int_turnaround_stat_get
 * Purpose:
 *      Get INT Turnaround statistics information data
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      stat_data       - (OUT) Statistics information data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_int_turnaround_stat_get(int unit,
        bcm_int_turnaround_stat_t *stat_data)
{
    int result = BCM_E_UNAVAIL;
    /* Take lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_lock(unit));

    /*  INT Turnaround app APIs.  */
    if (soc_feature(unit, soc_feature_uc_int_turnaround)) {
        result = _bcm_th3_int_turnaround_stat_get(unit,
                stat_data);
    }

    /* Release lock */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_int_unlock(unit));
    return result;
}
#else /* INCLUDE_INT and BCM_TOMAHAWK3_SUPPORT */
typedef int _tomahawk3_int_turnaround_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_INT and BCM_TOMAHAWK3_SUPPORT*/
