/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for RCPU packets encap/decap
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/util.h>
#include <soc/drv.h>

#ifdef INCLUDE_RCPU

#define SOC_RCPU_BENCHMARK     1
#define MAX_RETRY_ATTEMPTS     3
#define SOC_RCPU_F_INITED    0x1

/* The data length of CMIC request packet */
#define CMIC_REQUEST_DATA_LEN  0x8

soc_rcpu_control_t   *soc_rcpu_control[SOC_MAX_NUM_DEVICES];

#define SOC_RCPU_CONTROL(_unit)     soc_rcpu_control[_unit]
#define RCPU_TRANS_PTR_SET(unit)    _rcpu_trans_ptr = CMVEC(unit).rcpu_tp;

/*
 * Data format of CMIC request packet:
 * The first word of each pair is an address/operation word.
 * The bottom 24 bits of the word are the address in CMIC register space. 
 * The top 8 bits encode the operation type and the size of the request:
 * ED00 00SS
 * 0 bits are reserved and must be zero.
 * E is an error bit and must be 0 in a CMIC_REQUEST.
 * D is a direction bit (0 means read, 1 means write)
 * SS are the size of the operation (00 is reserved, 01 is 1 byte,
 *                                   10 is 2 bytes, 11 is 4 bytes)
 */
#if defined(LE_HOST)
#define CMIC_REQ_ADDR_OP_SET(ptr, addr, read_op) \
        (((uint32 *)(ptr))[0] = (soc_htonl(addr) | 0x3 |  \
                                 ((0 == read_op)? (1 << 6) : 0)))

#define CMIC_REQ_VALUE_SET(ptr, read_op, val) \
        (((uint32 *)(ptr))[1] = ((0 == read_op)? soc_htonl(*val): 0))
#else
#define CMIC_REQ_ADDR_OP_SET(ptr, addr, read_op) \
        (((uint32 *)(ptr))[0] = (addr | 0x3000000 |  \
                                 ((0 == read_op)? (1 << 30): 0)))

#define CMIC_REQ_VALUE_SET(ptr, read_op, val) \
        (((uint32 *)(ptr))[1] = ((0 == read_op)? *val: 0))
#endif

/***********************************************************
 *
 * Functions:
 *      _soc_rcpu_tp_rx
 * Purpose:
 *      Main entry for all received RCPU packets.
 *      1. Perform decapsulation to find which unit this packet belongs to.
 *      2. Handle SCHAN or CMIC reply packets.
 *      3. Disptach ToCPU and Interrupt packets.
 *
 * Parameters:
 *      rx_unit     - (IN) Recevie tp unit number
 *      pkt         - (IN) Packet buffer where to decap header
 *      pkt_len     - (IN) Packet length
 *      cookie      - (IN) Callback cookie
 * Returns:
 *      SOC_E_NONE
 *      -1 : Not RCPU packets or error case
 * Notes:
 *      Assumes buffers are valid and contain enough memory space.
 */
STATIC int 
_soc_rcpu_tp_rx(int rx_unit, uint8 *pkt, int pkt_len, void *cookie)
{
    int unit;
    rcpu_encap_info_t info;

    SOC_IF_ERROR_RETURN(soc_rcpu_decap(pkt, &unit, &info));

    if (NULL == SOC_RCPU_CONTROL(unit)) {
        /* Not initialized */
        return -1;
    }

    if ((SOC_RCPU_OP_SCHAN_REPLY == info.operation) ||
        (SOC_RCPU_OP_CMIC_REPLY == info.operation)) {

        if (info.flags & SOC_RCPU_FLAG_BUSY) {
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "****RCPU REPLY BUSY****\n")));
            }
            /* Let it timeout and retry. */
            return -1;
        }

        /* 
         * Check Fail flag 
         *     If FAIL, then set length appropriately
         */
        if (info.flags & SOC_RCPU_FLAG_FAIL) {
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "****RCPU REPLY FAIL****\n")));
                soc_dma_ether_dump(unit, "RCPU_REP ", pkt, pkt_len, 0);
            }
            info.datalen = pkt_len - info.cmic_hdr_size;
        }

        if (++info.transid == SOC_RCPU_CONTROL(unit)->transid) {
            sal_memcpy(SOC_RCPU_CONTROL(unit)->reply_data, 
                       pkt + info.cmic_hdr_size,
                       info.datalen);
            if (SOC_RCPU_CONTROL(unit)->schan_reply) {
                sal_sem_give(SOC_RCPU_CONTROL(unit)->schan_reply);
            }
        }
        return SOC_E_NONE;
    }

    /* Handle ToCPU packets */
    if (SOC_RCPU_OP_TOCPU_PKT == info.operation) {
        if (SOC_RCPU_CONTROL(unit)->tocpu_cb != NULL) {
            SOC_RCPU_CONTROL(unit)->tocpu_cb(unit, pkt, pkt_len, &info);
        }
        return SOC_E_NONE;
    }

#ifdef BCM_CMICM_SUPPORT
    /* Handle Remote Interrupt packets */
    if (SOC_RCPU_OP_INTR_PKT == info.operation) {
        soc_cmicm_rcpu_intr(unit, (soc_rcpu_intr_packet_t *)(pkt + info.cmic_hdr_size));
        return SOC_E_NONE;
    }
#endif /* BCM_CMICM_SUPPORT */

    return -1;

}

/***********************************************************
 *
 * Functions:
 *      soc_rcpu_encap
 * Purpose:
 *      Encap RCPU CMIC header into the buffer
 * 
 * Parameters:
 *      unit        - (IN) device number
 *      buf         - (IN) Buffer where to encap header.
 *      info        - (IN) Encap CMIC packet header information.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_PARAM
 * Notes:
 *      Assumes buffers are valid and contain enough memory space.
 */
int 
soc_rcpu_encap(int unit, uint8 *buf, rcpu_encap_info_t *info)
{
    soc_rcpu_cfg_t rcpu_cfg;
    rcpu1_cmic_pkt_hdr_t *hdr = (rcpu1_cmic_pkt_hdr_t *)buf;

    if (!SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_cm_get_rcpu_cfg(unit, &rcpu_cfg));

    sal_memcpy(hdr->dst_mac, rcpu_cfg.dst_mac, 6);
    sal_memcpy(hdr->src_mac, rcpu_cfg.src_mac, 6);
    hdr->tpid = soc_htons(rcpu_cfg.tpid);
    hdr->vlan = soc_htons(rcpu_cfg.vlan);
    hdr->ethertype = soc_htons(rcpu_cfg.ether_type);
    hdr->signature = soc_htons(rcpu_cfg.signature);

    hdr->operation = info->operation;
    hdr->flags = info->flags;
    hdr->transid = soc_htons(info->transid);
    hdr->datalen = soc_htons(info->datalen);
    hdr->replen = soc_htons(info->replen);
    hdr->reserved = 0;

    return SOC_E_NONE;
}

/***********************************************************
 * RCPU CMIC header Decap
 *
 * Functions:
 *      soc_rcpu_decap
 * Purpose:
 *      Parse input buffer and decide to which device number
 *      it should go.
 * Parameters:
 *      buf   - (IN)  Buffer where to decap message.
 *      runit - (OUT) Device unit number to which this message belogs.
 *      info  - (OUT) Decap CMIC packet header information.
 *
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_NOT_FOUND
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
int
soc_rcpu_decap(uint8 *buf, int *runit, rcpu_encap_info_t *info)
{
    int rv, unit;
    soc_rcpu_cfg_t rcpu_cfg;
    rcpu1_cmic_pkt_hdr_t *hdr = (rcpu1_cmic_pkt_hdr_t *)buf;

    for (unit = 0; unit < soc_cm_get_num_devices(); unit++) {

        if (!SOC_IS_RCPU_ONLY(unit)) {
            continue;
        }

        rv = soc_cm_get_rcpu_cfg(unit, &rcpu_cfg);

        if (SOC_E_NONE != rv) {
            continue;
        }

        if (sal_memcmp(buf+6, rcpu_cfg.dst_mac, 6)) {
            continue;
        }

        if (rcpu_cfg.tpid != soc_ntohs(hdr->tpid) &&
            rcpu_cfg.ether_type != soc_ntohs(hdr->tpid)) {
            continue;
        }

        info->cmic_hdr_size = sizeof(rcpu1_cmic_pkt_hdr_t);

        if (rcpu_cfg.tpid == soc_ntohs(hdr->tpid)) {
            if ((rcpu_cfg.vlan != soc_ntohs(hdr->vlan)) ||
                (rcpu_cfg.ether_type != soc_ntohs(hdr->ethertype))) {
                /* Could be double tagged */
                hdr = (rcpu1_cmic_pkt_hdr_t *)(buf + 4);
                if (rcpu_cfg.ether_type != soc_ntohs(hdr->ethertype)) {
                    continue;
                }
                info->cmic_hdr_size += 4;
            }
        } else {
            /* Could have no vlan tag. */
            info->cmic_hdr_size -= 4;
            hdr = (rcpu1_cmic_pkt_hdr_t *)(buf - 4);
        }

        if (rcpu_cfg.signature != soc_ntohs(hdr->signature)) {
            continue;
        }

        info->operation = hdr->operation;
        info->flags = hdr->flags;
        info->transid = soc_ntohs(hdr->transid);
        info->datalen = soc_ntohs(hdr->datalen);
        if (hdr->operation == SOC_RCPU_OP_TOCPU_PKT) {
            /* Count dcb size */
            info->dcb_size = (SOC_DCB_SIZE(unit) == 64)? 64:32;
        } else {
            info->dcb_size = 0;
        }
        *runit = unit;
        return SOC_E_NONE;
    }
    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_rcpu_init
 * Purpose:
 *      Initialize RCPU master units before BCM module init so each bcm module
 *      can access s-channel registers/tables via RCPU mechanism
 * Parameters:
 *      unit - (IN) unit number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_init(int unit)
{
    int                 rv = SOC_E_NONE;
    rcpu_trans_ptr_t    *_rcpu_trans_ptr;

    if (!SOC_UNIT_VALID(unit)){
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        return SOC_E_NONE;
    }

    if (SOC_RCPU_CONTROL(unit) &&
       (SOC_RCPU_CONTROL(unit)->flags & SOC_RCPU_F_INITED)) {
        return SOC_E_NONE;
    }

    if (SOC_RCPU_CONTROL(unit) == NULL) {
        SOC_RCPU_CONTROL(unit) = sal_alloc(sizeof(soc_rcpu_control_t), 
                                           "soc_rcpu_control");
        if (SOC_RCPU_CONTROL(unit) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_RCPU_CONTROL(unit), 0, sizeof(soc_rcpu_control_t));

        SOC_RCPU_CONTROL(unit)->lock = sal_mutex_create("rcpu_unit_mutex");
        if (SOC_RCPU_CONTROL(unit)->lock == NULL) {
            LOG_ERROR(BSL_LS_SOC_RCPU,
                      (BSL_META_U(unit,
                                  "soc_rcpu_init: Failed to create mutex.\n")));
            return SOC_E_MEMORY;
        }

        if ((SOC_RCPU_CONTROL(unit)->schan_reply =
                sal_sem_create("RCPU SCHAN", sal_sem_BINARY, 0)) == NULL) {
            LOG_ERROR(BSL_LS_SOC_RCPU,
                      (BSL_META_U(unit,
                                  "soc_rcpu_init: Failed to create semaphore.\n")));
            return SOC_E_MEMORY;
        }
    }

    RCPU_TRANS_PTR_SET(unit);
    rv = _rcpu_trans_ptr->rcpu_tp_rx_register(_rcpu_trans_ptr->tp_unit,
                                              _soc_rcpu_tp_rx,
                                              NULL);

    if (SOC_SUCCESS(rv)) {
        _rcpu_trans_ptr->ref_count++;
    }

    if (SOC_IS_RCPU_ONLY(unit)) {
        soc_rcpu_schan_op_register(unit, soc_rcpu_schan_op);
    }

    SOC_RCPU_CONTROL(unit)->flags |= SOC_RCPU_F_INITED;

    return rv;
}
/*
 * Function:
 *      soc_rcpu_deinit
 * Purpose:
 *      Deinitialize resources of RCPU master units
 * Parameters:
 *      unit - (IN) unit number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_deinit(int unit)
{
    rcpu_trans_ptr_t *_rcpu_trans_ptr;

    if (!SOC_UNIT_VALID(unit)){
        return SOC_E_UNIT;
    }

    if (!soc_feature(unit, soc_feature_rcpu_1)) {
        return SOC_E_NONE;
    }

    (void)soc_rcpu_schan_op_unregister(unit);

    if (SOC_RCPU_CONTROL(unit)) {

        RCPU_TRANS_PTR_SET(unit);

        if (_rcpu_trans_ptr->ref_count > 0) {
            _rcpu_trans_ptr->ref_count--;
            if (0 == _rcpu_trans_ptr->ref_count) {
                _rcpu_trans_ptr->rcpu_tp_rx_unregister(_rcpu_trans_ptr->tp_unit,
                                                       _soc_rcpu_tp_rx);
            }
        }

        if (SOC_RCPU_CONTROL(unit)->lock) {
            sal_mutex_destroy(SOC_RCPU_CONTROL(unit)->lock);
        }

        if (SOC_RCPU_CONTROL(unit)->schan_reply) {
            sal_sem_destroy(SOC_RCPU_CONTROL(unit)->schan_reply);
        }

        sal_free(SOC_RCPU_CONTROL(unit));
        SOC_RCPU_CONTROL(unit) = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_rcpu_tocpu_cb_register
 * Purpose:
 *      Registers ToCPU packet handler
 * Parameters:
 *      unit - SOC unit #
 *      f    - (IN) Function to be called for ToCPU packets
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_tocpu_cb_register(int unit, rcpu_tocpu_cb_f f)
{
    if (!SOC_IS_RCPU_UNIT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_RCPU_CONTROL(unit) == NULL) {
        return SOC_E_INIT;
    }

    if (NULL == f) {
        return SOC_E_PARAM;
    }

    SOC_RCPU_CONTROL(unit)->tocpu_cb = f;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_rcpu_tocpu_cb_unregister
 * Purpose:
 *      Unregisters ToCPU packet handler
 *
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_tocpu_cb_unregister(int unit)
{
    if (!SOC_IS_RCPU_UNIT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_UNAVAIL;
    }

    if (SOC_RCPU_CONTROL(unit) == NULL) {
        return SOC_E_INIT;
    }

    SOC_RCPU_CONTROL(unit)->tocpu_cb = NULL;

    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_rcpu_schan_op
 * Purpose:
 *      RCPU version of soc_schan_op function
 *
 * Returns:
 *      SOC_E_XXX
 */
int
soc_rcpu_schan_op(int unit, schan_msg_t *msg, int dwc_write, int dwc_read)
{
    uint8 *tx_buf;
    uint32 *buf_ptr;
    int i, rcpu_schan_pkt_len = 0;
    int rv = SOC_E_NONE, num_retries;
    rcpu_encap_info_t info;
#if defined(SOC_RCPU_BENCHMARK)
    sal_usecs_t         time_start = 0;
    sal_usecs_t         time_end = 0;
#endif /* SOC_RCPU_BENCHMARK */
    rcpu_trans_ptr_t *_rcpu_trans_ptr;

    if (SOC_RCPU_CONTROL(unit) == NULL) {
        return SOC_E_INIT;
    }

#if defined(SOC_RCPU_BENCHMARK)
    if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_INFO)) {
        time_start = sal_time_usecs();
    }
#endif /* SOC_RCPU_BENCHMARK */

    rcpu_schan_pkt_len = sizeof(rcpu1_cmic_pkt_hdr_t) + 4 * dwc_write;
    rcpu_schan_pkt_len = (rcpu_schan_pkt_len < ENET_MIN_PKT_SIZE)? ENET_MIN_PKT_SIZE:rcpu_schan_pkt_len;

    info.datalen = 4 * dwc_write;
    info.flags = SOC_RCPU_FLAG_REPLY;
    info.operation = SOC_RCPU_OP_SCHAN_REQ;
    /* If dwc_read is 0, get a SCHAN_REPLY as an ACK (1 word) */
    info.replen = dwc_read ? 4 * dwc_read : 4;

    SCHAN_LOCK(unit);

    info.transid = SOC_RCPU_CONTROL(unit)->transid++;

    RCPU_TRANS_PTR_SET(unit);

    for (num_retries = 0; num_retries < MAX_RETRY_ATTEMPTS; num_retries++) {

        _rcpu_trans_ptr->rcpu_tp_data_alloc(_rcpu_trans_ptr->tp_unit,
                                        rcpu_schan_pkt_len,
                                        (void*)&tx_buf);

        if (!tx_buf) {
            SCHAN_UNLOCK(unit);
            return SOC_E_MEMORY;
        }

        /* Construct CMIC header */
        soc_rcpu_encap(unit, tx_buf, &info);

        buf_ptr = (uint32 *)(tx_buf + sizeof(rcpu1_cmic_pkt_hdr_t));

        for (i = 0; i < dwc_write; i++) {
            buf_ptr[i] = soc_htonl(msg->dwords[i]);
        }

        _rcpu_trans_ptr->rcpu_tp_tx(_rcpu_trans_ptr->tp_unit,
                                    (void*)tx_buf,
                                    rcpu_schan_pkt_len,
                                    NULL);

        if (sal_sem_take(SOC_RCPU_CONTROL(unit)->schan_reply, 
                         SOC_CONTROL(unit)->schanTimeout) == 0) {
            uint32 *pdata = (uint32 *)SOC_RCPU_CONTROL(unit)->reply_data;
            for (i = 0; i < dwc_read; i++) {
                msg->dwords[i] = soc_ntohl(pdata[i]);
            }
            rv = SOC_E_NONE;
            break;
        } else {
            rv = SOC_E_TIMEOUT;
        }
    }

    SCHAN_UNLOCK(unit);

#if defined(SOC_RCPU_BENCHMARK)
    if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_SCHAN | BSL_INFO)) {
        time_end = sal_time_usecs();
        LOG_CLI((BSL_META_U(unit,
                            "RCPU schan-op (%d, %d) took %d usecs\n"),
                 dwc_write, dwc_read, SAL_USECS_SUB(time_end, time_start)));
    }
#endif /* SOC_RCPU_BENCHMARK */

    return rv;
}

#ifdef BCM_CMICM_SUPPORT
/*
 * Functions:
 *      _soc_rcpu_cmic_op
 * Purpose:
 *      Handling CMIC request packets
 * Parameters:
 *      unit    - (IN) device number
 *      read_op - (IN) 1 = read operation; 0 = write operation
 *      addr    - (IN) CMIC address to access
 *      val32   - (IN/OUT) Input for write operation; Output for read operation
 *
 * Returns:
 *      SOC_E_XXXE
 */
STATIC int 
_soc_rcpu_cmic_op(int unit, int read_op, uint32 addr, uint32 *val32)
{
    uint8  *tx_buf;
    uint32 *addr_ptr;
    int rcpu_cmic_pkt_len = 0;
    int rv = SOC_E_NONE, num_retries;
    rcpu_encap_info_t info;
#if defined(SOC_RCPU_BENCHMARK)
    sal_usecs_t         time_start = 0;
    sal_usecs_t         time_end = 0;
#endif /* SOC_RCPU_BENCHMARK */
    rcpu_trans_ptr_t *_rcpu_trans_ptr;

    if (SOC_RCPU_CONTROL(unit) == NULL) {
        return SOC_E_INIT;
    }

    if (!soc_feature(unit, soc_feature_cmicm)) {
        return SOC_E_UNIT;
    }

#if defined(SOC_RCPU_BENCHMARK)
    if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_PCI | BSL_INFO)) {
        time_start = sal_time_usecs();
    }
#endif /* SOC_RCPU_BENCHMARK */

    /* Use minimum size since the packet length for one operation is 40 bytes. */
    rcpu_cmic_pkt_len = ENET_MIN_PKT_SIZE;

    info.datalen = CMIC_REQUEST_DATA_LEN;
    info.flags = SOC_RCPU_FLAG_REPLY;
    info.operation = SOC_RCPU_OP_CMIC_REQ;
    /* Reply length must be the same value as datalen */
    info.replen = CMIC_REQUEST_DATA_LEN;

    SCHAN_LOCK(unit);

    info.transid = SOC_RCPU_CONTROL(unit)->transid++;

    RCPU_TRANS_PTR_SET(unit);

    for (num_retries = 0; num_retries < MAX_RETRY_ATTEMPTS; num_retries++) {

        _rcpu_trans_ptr->rcpu_tp_data_alloc(_rcpu_trans_ptr->tp_unit,
                                        rcpu_cmic_pkt_len,
                                        (void*)&tx_buf);

        if (!tx_buf) {
            LOG_ERROR(BSL_LS_SOC_RCPU,
                      (BSL_META_U(unit,
                                  "rcpu_cmic_read: Failed to allocate memory.\n")));
            SCHAN_UNLOCK(unit);
            return SOC_E_MEMORY;
        }

        /* Construct CMIC header */
        rv = soc_rcpu_encap(unit, tx_buf, &info);

        addr_ptr = (uint32 *)(tx_buf + sizeof(rcpu1_cmic_pkt_hdr_t));

        CMIC_REQ_ADDR_OP_SET(addr_ptr, addr, read_op);

        CMIC_REQ_VALUE_SET(addr_ptr, read_op, val32);

        rv = _rcpu_trans_ptr->rcpu_tp_tx(_rcpu_trans_ptr->tp_unit,
                                    (void*)tx_buf,
                                    rcpu_cmic_pkt_len,
                                    NULL);

        if (sal_sem_take(SOC_RCPU_CONTROL(unit)->schan_reply,
                         SOC_CONTROL(unit)->schanTimeout) == 0) {
            uint32 *pdata = (uint32 *)SOC_RCPU_CONTROL(unit)->reply_data;
            *val32 = soc_ntohl(pdata[1]);
            rv = SOC_E_NONE;
            break;
        } else {
            rv = SOC_E_TIMEOUT;
        }
    }

    SCHAN_UNLOCK(unit);

#if defined(SOC_RCPU_BENCHMARK)
    if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_PCI | BSL_INFO)) {
        time_end = sal_time_usecs();
        LOG_CLI((BSL_META_U(unit,
                            "RCPU cmic-op took %d usecs\n"), 
                 SAL_USECS_SUB(time_end, time_start)));
    }
#endif /* SOC_RCPU_BENCHMARK */

    return rv;
}
/*
 * Functions:
 *      _soc_rcpu_cmic_op
 * Purpose:
 *      RCPU version for CMIC read operation
 * Parameters:
 *      unit    - (IN) device number
 *      addr    - (IN) CMIC address to access
 *
 * Returns:
 *      Register value or zero
 */
uint32
soc_rcpu_cmic_read(int unit, uint32 addr)
{
    int rv;
    uint32 data;

    rv = _soc_rcpu_cmic_op(unit, 1, addr, &data);

    return (SOC_E_NONE == rv)? data : 0;
}
/*
 * Functions:
 *      soc_rcpu_cmic_write
 * Purpose:
 *      RCPU version for CMIC write operation
 * Parameters:
 *      unit    - (IN) device number
 *      addr    - (IN) CMIC address to access
 *      data    - (IN) Data to write
 *
 * Returns:
 *      SOC_E_XXXE
 */
int
soc_rcpu_cmic_write(int unit, uint32 addr, uint32 data)
{
    _soc_rcpu_cmic_op(unit, 0, addr, &data);
    return 0;
}
#endif /* BCM_CMICM_SUPPORT */

#endif /* INCLUDE_RCPU */
