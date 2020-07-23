/*! \file bcmlu_rawsock.c
 *
 * Raw socket wrapper.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <shr/shr_thread.h>
#include <sal/sal_types.h>
#include <sal/sal_sleep.h>
#include <sal/sal_assert.h>
#include <shr/shr_types.h>
#include <shr/shr_timeout.h>
#include <bsl/bsl.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_hg3.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmlrd/bcmlrd_conf.h>

#include "bcmlu_rawsock.h"
#include <bcmlu/bcmlu_ngknet.h>

#define BSL_LOG_MODULE          BSL_LS_SYS_SOCKET

#define TXPMD_START_HIGIG       3
#define TX_NONBLOCK_TIMEOUT     1 /* 1 second */

typedef struct bcmlu_rawsock_s bcmlu_rawsock_t;

typedef int  (*bcmlu_rawsock_read_f)(bcmlu_rawsock_t *rawsock);
typedef int  (*bcmlu_rawsock_send_f)(bcmlu_rawsock_t *rawsock, uint8_t *data,
                                     uint32_t len);
/*!
 * \brief Rawsock handler structure.
 */
struct bcmlu_rawsock_s {

    /*! Host unit number. */
    int unit;

    /*! File descriptor of socket. */
    int fd;

    /*! Host network interface configuration. */
    bcmpkt_netif_t netif;

    /*! RX thread handler. */
    shr_thread_t *rx_th;

    /*! Break RX loop to terminate RX thread. */
    bool rx_break;

    /*! Timeout for poll function. */
    uint32_t poll_timeout;

    /*! Receive function. */
    bcmlu_rawsock_read_f read;

    /*! Transmit function. */
    bcmlu_rawsock_send_f send;

    /*! RX callback handle. */
    bcmpkt_rx_cb_info_t *cb_info;

    /*! Received valid packets on the socket. */
    uint32_t rx_packets;

    /*! Dropped invalid packets on the socket. */
    uint32_t rx_drops;

    /*! Packet dropped due to invalid packet length. */
    uint32_t pkt_len_err;

    /*! Non-RCPU packet dropped due to RCPU encap configured on Netif. */
    uint32_t non_rcpu_pkt;

    /*! RCPU packet dropped due to header check fail. */
    uint32_t rcpu_hdr_err;

    /*! RCPU packet dropped due to length check fail. */
    uint32_t rcpu_len_err;

    /*! Received bytes on the socket. */
    uint32_t rx_bytes;

    /*! Transmit packets on the socket. */
    uint32_t tx_packets;

    /*! Dropped packets to the socket. */
    uint32_t tx_drops;

    /*! Transmit bytes no the socket. */
    uint32_t tx_bytes;

} ;

/* Netif 0 is reserved. Supported netif is 1 ~ BCMLU_NETIF_MAX_IDS. */
static bcmlu_rawsock_t *rawsock_handles[BCMDRD_CONFIG_MAX_UNITS][BCMLU_NETIF_MAX_IDS + 1];

static bcmlu_rawsock_t *
bcmlu_rawsock_handle_get(int unit, int netif_id)
{
    return rawsock_handles[unit][netif_id];
}

/* This function inserts higig3 header at the correct location within the packet
 * header. Higig3 packet header is specified by caller (hg_size, in bytes)
 */
static int
higig3_header_insert(int unit, bcmpkt_packet_t *packet, int hg_size)
{
    uint8_t *data_ptr;
    uint8_t *mac_ptr;
    uint32_t *hdr_ptr;

    SHR_FUNC_ENTER(unit);

    if (hg_size <= 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Start of packet has eth header (dm-sm-ethtype) */
    mac_ptr = BCMPKT_PACKET_DATA(packet);

    /* Adjust data ptr and data len to accomodate hg3 header bytes (currently
     * 8 or 16)
     */
    if (!bcmpkt_push(packet->data_buf, hg_size)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get data ptr which points to a new location */
    data_ptr = BCMPKT_PACKET_DATA(packet);
    /* Copy dmac and smac first */
    sal_memmove(data_ptr, mac_ptr, 12);

    /* Next copy higig3 header. See d2 calculation */
    hdr_ptr = (uint32_t *)(BCMPKT_PACKET_DATA(packet) + 12); /* d2 */
    shr_util_host_to_be32(hdr_ptr, packet->pmd.higig, hg_size / 4);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT()
}

static int
header_generate(int unit, bcmpkt_packet_t *packet)
{
    uint32_t hg_size = 0, pstart = 0, hdr_size = 0;
    uint32_t lbhdr_size = 0, lbstart = 0;
    uint32_t gihhdr_size = 0, gihstart = 0;
    bcmpkt_data_buf_t *dbuf = NULL;
    HIGIG_t *hghdr = NULL;
    uint32_t dev_type;
    uint8_t *data;
    uint32_t *hdr = NULL;
    uint32_t data_size, meta_len = 0;
    uint8_t *mac_addr = NULL;
    bcmpkt_rcpu_hdr_t *rhdr;
    uint32_t is_higig3_hdr = FALSE;
    /* New var used for Higig3 since hg_size is used for module header
     * calculation/processing. Higig3 is seen as a regular ethernet packet
     */
    uint32_t hg3_size = 0;
    bool hg3_support = false;
    bool glb_support = false;
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    rhdr = bcmpkt_rcpu_hdr(unit);

    mac_addr = BCMPKT_PACKET_DATA(packet);
    hghdr = (HIGIG_t *)packet->pmd.higig;

    if ((hghdr != NULL) && (packet->pmd.higig[0] != 0)) {
        uint32_t hstart = HIGIG_STARTf_GET(*hghdr);
        if (hstart == BCMPKT_HIGIG2_SOF) {
            hg_size = HIGIG2_SIZE;
        } else if (hstart == BCMPKT_HIGIG_SOF) {
            hg_size = HIGIG_SIZE;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_hg3_base_t_is_supported(variant, &hg3_support));
    SHR_IF_ERR_EXIT
        (bcmpkt_generic_loopback_t_is_supported(variant, &glb_support));

    /* If not HG/HG2, check for HG3 */
    if ((hg3_support == true) && (packet->pmd.higig != NULL) &&
        (hg_size == 0)) {
        uint32_t hstart;
        uint32_t ext_hdrs;
        int fid = BCMPKT_HG3_BASE_T_FID_INVALID;

        SHR_IF_ERR_EXIT(bcmpkt_hg3_base_t_field_id_get("RESERVED_ETYPE", &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_hg3_base_t_field_get(variant, packet->pmd.higig,
                                         fid, &hstart));
        /* Extract 2-bit field for determining presence of ext hdr */
        SHR_IF_ERR_EXIT
            (bcmpkt_hg3_base_t_field_id_get("EXT_HDR_PRESENT", &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_hg3_base_t_field_get(variant, packet->pmd.higig,
                                         fid, &ext_hdrs));

        if (hstart == BCMPKT_HG3_ETHER_TYPE) {
            is_higig3_hdr = TRUE;
            hg3_size = 0;

            switch (ext_hdrs) {
                case 0x1:
                    /* Only base header present */
                    hg3_size = BCMPKT_HG3_BASE_HEADER_SIZE_BYTES;
                    break;

                case 0x2:
                    /* Base header and extension header 0 present */
                    hg3_size = BCMPKT_HG3_BASE_HEADER_SIZE_BYTES +
                               BCMPKT_HG3_EXT0_HEADER_SIZE_BYTES;
                    break;

                case 0x3:
                    /* Extension header 1 is currently not supported.
                     * This code should be updated when extension header 1
                     * is used by any future device
                     */
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Error: Unsupported Higig3"
                              " version %u\n"), ext_hdrs));
                    return (SHR_E_UNAVAIL);

                case 0x0:
                    /* Reserved value */
                default:
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Error: Reserved/Invalid"
                              " Higig3 version passed %u\n"), ext_hdrs));
                    return (SHR_E_CONFIG);
            }
        } else {
            /* If hstart is 0, it means a normal(non-higig) packet
             * is being sent. Any other value means an invalid value was
             * passed
             */
            if (hstart != 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Error: Invalid"
                          " Higig3 ethertype 0x%X\n"), hstart));
                return (SHR_E_CONFIG);
            }
        }
    }

    /* If Higig3 header is present, insert it before any other pmds/headers are
     * processed. So rest of the code sees the packet as a regular
     * ethernet packet
     */
    if ((hg3_support == true) && (is_higig3_hdr) && (hg3_size > 0)) {
        SHR_IF_ERR_EXIT(higig3_header_insert(unit, packet, hg3_size));
        /* HG3 header insertion modifies packet's 'data' pointer, so we adjust
         * 'mac_addr' as per to HG3 header size
         */
        mac_addr -= hg3_size;
    }

    if ((packet->pmd.txpmd != NULL) && (packet->pmd.txpmd[0] != 0)) {
        SHR_IF_ERR_EXIT
            (bcmpkt_txpmd_field_get(dev_type, packet->pmd.txpmd,
                                    BCMPKT_TXPMD_START, &pstart));

        if ((packet->pmd.lbhdr != NULL) && (packet->pmd.lbhdr[0] != 0)) {
            if (glb_support == true) {
                int fid = BCMPKT_GENERIC_LOOPBACK_T_FID_INVALID;
                SHR_IF_ERR_EXIT
                    (bcmpkt_generic_loopback_t_field_id_get("START_BYTE", &fid));
                SHR_IF_ERR_EXIT
                    (bcmpkt_generic_loopback_t_field_get(variant, packet->pmd.lbhdr,
                                                         fid, &lbstart));
            } else {
                SHR_IF_ERR_EXIT
                    (bcmpkt_lbhdr_field_get(dev_type, packet->pmd.lbhdr,
                                            BCMPKT_LBHDR_START, &lbstart));
            }

            if (lbstart > 0) {
                lbhdr_size = BCMPKT_LBHDR_SIZE_BYTES;
            }
        }
    } else if ((glb_support == true) &&
               (packet->pmd.lbhdr != NULL) && (packet->pmd.lbhdr[0] != 0)) {
        int fid = BCMPKT_GENERIC_LOOPBACK_T_FID_INVALID;
        SHR_IF_ERR_EXIT
            (bcmpkt_generic_loopback_t_field_id_get("START_BYTE", &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_generic_loopback_t_field_get(variant,packet->pmd.lbhdr, fid,
                                                 &gihstart));
        if (gihstart > 0) {
            gihhdr_size = BCMPKT_LBHDR_SIZE_BYTES;
        }
    }

    /* Calculate request space for RCPU, TXPMD and Higig header. */
    if (pstart > 0) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len + hg_size + lbhdr_size;
    } else if ((hg_size > 0) || (gihstart > 0)) {
        hdr_size = sizeof(*rhdr) + rhdr->meta_len;
    } else {
        hdr_size = sizeof(*rhdr);
    }

    /* If the head room is not enough, try to re-allocate data buffer. */
    if (BCMPKT_PACKET_DATA(packet) < packet->data_buf->head + hdr_size) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Not enough headroom, re-allocate packet buffer\n")));
        SHR_IF_ERR_EXIT
            (bcmpkt_data_buf_alloc(packet->unit,
                                   BCMPKT_PACKET_LEN(packet) + hdr_size,
                                   &dbuf));
        if (!bcmpkt_reserve(dbuf, hdr_size)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (!bcmpkt_put(dbuf, BCMPKT_PACKET_LEN(packet))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_memcpy(dbuf->data, BCMPKT_PACKET_DATA(packet),
                   BCMPKT_PACKET_LEN(packet));

        SHR_IF_ERR_CONT
            (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
        packet->data_buf = dbuf;
        mac_addr = BCMPKT_PACKET_DATA(packet);
    }

    data_size = BCMPKT_PACKET_LEN(packet);

    /* Encapsulate TXPMD and Higig header. */
    if (pstart > 0) {
        /* Encapsulate Higig header. Continue only for HG/HG2 hdr insertion
         * since this step is not valid for HG3
         */
        if ((!is_higig3_hdr) && (hg_size > 0)) {
            if (!bcmpkt_push(packet->data_buf, hg_size)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        }

        /* Encapsulate Loopback header. */
        if (lbhdr_size > 0) {
            if (!bcmpkt_push(packet->data_buf, lbhdr_size)) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
            shr_util_host_to_be32(hdr, packet->pmd.lbhdr, lbhdr_size / 4);
        }
        data_size = BCMPKT_PACKET_LEN(packet);

        /* Set TXPMD metadata. */
        if ((!is_higig3_hdr) && (hg_size > 0)) {
            /* Set Higig flag. */
            SHR_IF_ERR_EXIT
                (bcmpkt_txpmd_field_set(dev_type, packet->pmd.txpmd,
                                        BCMPKT_TXPMD_START, TXPMD_START_HIGIG));
        }
        if (!bcmpkt_push(packet->data_buf, rhdr->meta_len)) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.txpmd, rhdr->meta_len / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.txpmd, rhdr->meta_len);
        }
        meta_len = rhdr->meta_len;
    } else if ((!is_higig3_hdr) && (hg_size > 0)) {
        /* The code under this 'else-if' is used for Higig/Higig2 only */
        if (rhdr->meta_len < hg_size ||
            (!bcmpkt_push(packet->data_buf, hg_size))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.higig, hg_size / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.higig, hg_size);
        }
        meta_len = rhdr->meta_len;
    } else if (gihstart > 0) {
        /* Encapsulate GIH header */
        if ((rhdr->meta_len < gihhdr_size) ||
            (!bcmpkt_push(packet->data_buf, gihhdr_size))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        hdr = (uint32_t *)BCMPKT_PACKET_DATA(packet);
        if (bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM)) {
            shr_util_host_to_be32(hdr, packet->pmd.lbhdr, gihhdr_size / 4);
        } else {
            sal_memcpy(hdr, packet->pmd.lbhdr, gihhdr_size);
        }
        meta_len = rhdr->meta_len;
    }

    /* Encapsulate RCPU header. */
    if (!bcmpkt_push(packet->data_buf, sizeof(*rhdr))) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    data = BCMPKT_PACKET_DATA(packet);
    sal_memset(data, 0, sizeof(*rhdr));
    sal_memcpy(data, mac_addr, 12);
    data[12] = rhdr->tpid >> 8;
    data[13] = rhdr->tpid & 0xFF;
    data[14] = rhdr->vlan >> 8;
    data[15] = rhdr->vlan & 0xFF;
    data[16] = rhdr->ethertype >> 8;
    data[17] = rhdr->ethertype & 0xFF;
    data[18] = rhdr->signature >> 8;
    data[19] = rhdr->signature & 0xFF;
    data[20] = BCMPKT_RCPU_OP_TX;
    if ((meta_len > 0) || is_higig3_hdr) {
        data[21] = BCMPKT_RCPU_F_MODHDR;
    }
    data[24] = data_size >> 8;
    data[25] = data_size & 0xFF;
    data[28] = meta_len;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_send(bcmlu_rawsock_t *rawsock, uint8_t *data, uint32_t len)
{
    int bytes;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = TX_NONBLOCK_TIMEOUT * SECOND_USEC;
    int min_polls = 1;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    shr_timeout_init(&to, timeout_usec, min_polls);
    while (TRUE) {
        /* Send the message with non-blocking send */
        bytes = send(rawsock->fd, data, len, MSG_DONTWAIT);
        if (bytes == -1) {
            if (shr_timeout_check(&to)) {
                SHR_IF_ERR_MSG_EXIT
                    (SHR_E_FAIL,
                     (BSL_META_U(rawsock->unit,
                                 "Call send failed, "
                                 "timeout reached (%d s).\n"),
                      TX_NONBLOCK_TIMEOUT));
            }
            if (errno == ENOBUFS || errno == ENOMEM ||
                errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(rawsock->unit,
                             "Call send failed (%s)\n"),
                  strerror(errno)));
        }
        break;
    }
    LOG_VERBOSE(BSL_LS_SYS_TX,
                (BSL_META_U(rawsock->unit,
                            "Send: len=%"PRIu32"(%d)\n"),
                 len, bytes));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    bcmlu_rawsock_t *rawsock;
    uint32_t data_len, pktsize_min;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->data_buf, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMPKT_PACKET_DATA(packet), SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Network interface %d does not support raw socket\n"),
              netif_id));
    }

    data_len = BCMPKT_PACKET_LEN(packet);
    pktsize_min = bcmpkt_framesize_min_get();
    if (data_len > rawsock->netif.max_frame_size ||
        data_len < pktsize_min) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit,
                         "Packet size %"PRIu32" is out of valid range "
                         "[%d, %"PRIu32"]\n"),
              BCMPKT_PACKET_LEN(packet), pktsize_min,
              rawsock->netif.max_frame_size));
    }

    if (packet->type == BCMPKT_FWD_T_NORMAL &&
        (rawsock->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
        SHR_IF_ERR_EXIT
            (header_generate(unit, packet));
    }

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb;
        uint32_t dev_type;
        pb = shr_pb_create();
        if (SHR_SUCCESS(bcmpkt_dev_type_get(unit, &dev_type))) {
            shr_pb_printf(pb, "TXPMD:\n");
            (void)bcmpkt_txpmd_dump(dev_type, packet->pmd.txpmd,
                                    BCMPKT_TXPMD_DUMP_F_ALL, pb);
        }
        shr_pb_printf(pb, "TX Data buf:\n");
        bcmpkt_data_buf_dump(packet->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    SHR_IF_ERR_EXIT
        (rawsock->send(rawsock, BCMPKT_PACKET_DATA(packet),
                        BCMPKT_PACKET_LEN(packet)));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_packet_validate(bcmlu_rawsock_t *rawsock, bcmpkt_packet_t *packet)
{
    uint32_t pkt_len, meta_size = 0;
    uint8_t *data = NULL;

    bcmpkt_rcpu_hdr_t *rhdr = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    rhdr = bcmpkt_rcpu_hdr(rawsock->unit);
    assert(rhdr);

    data = BCMPKT_PACKET_DATA(packet);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    /* Perform sanity check. */
    if (BCMPKT_PACKET_LEN(packet) > rawsock->netif.max_frame_size +
                                    BCMPKT_RCPU_RX_ENCAP_SIZE ||
        BCMPKT_PACKET_LEN(packet) < bcmpkt_framesize_min_get()) {
        rawsock->pkt_len_err++;
        rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
    }
    /* RCPU validation. */
    if (data[12] == (rhdr->tpid >> 8) &&
        data[13] == (rhdr->tpid & 0xFF) &&
        data[14] == (rhdr->vlan >> 8) &&
        data[15] == (rhdr->vlan & 0xFF) &&
        data[16] == (rhdr->ethertype >> 8) &&
        data[17] == (rhdr->ethertype & 0xFF)) {
        if (data[18] != (rhdr->signature >> 8) ||
            data[19] != (rhdr->signature & 0xFF) ||
            data[20] != BCMPKT_RCPU_OP_RX ||
            data[21] != BCMPKT_RCPU_F_MODHDR) {
            rawsock->rcpu_hdr_err++;
            rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(rawsock->unit,
                             "RCPU validation failure:\n"
                             "Signature %04x, OPCODE %02x, Flags %02x\n"),
                  (data[18] << 8) + data[19], data[20], data[21]));
        }
        pkt_len = data[25] + (data[24] << 8);
        meta_size = data[28];
        if (pkt_len + meta_size != BCMPKT_PACKET_LEN(packet) - sizeof(*rhdr)) {
            rawsock->rcpu_len_err++;
            rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(rawsock->unit,
                             "Length error: packet length %u bytes\n"
                             "meta length %u bytes\n"
                             "RCPU header length %u bytes\n"
                             "Snap length %u bytes\n"),
                  pkt_len, meta_size, BCMPKT_RCPU_HDR_LEN,
                  BCMPKT_PACKET_LEN(packet)));
        }

        data += sizeof(*rhdr);
        /* Copy RXPMD data. */
        sal_memcpy(packet->pmd.rxpmd, data, meta_size);
        data += meta_size;
        bcmpkt_pull(packet->data_buf, meta_size + sizeof(*rhdr));
    } else { /* Handle as non-RCPU packets. */
        /*
         * Drop non-RCPU packets if BCMPKT_NETIF_F_RCPU_ENCAP flag was
         * configured.
         */
        if (rawsock->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP) {
            rawsock->non_rcpu_pkt++;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_packet_handle(bcmlu_rawsock_t *rawsock, bcmpkt_packet_t *packet)
{
    int bytes;
    uint32_t max_frame_size;
    bcmpkt_bpool_status_t bpool_status;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));
    max_frame_size = bpool_status.buf_size - sizeof(bcmpkt_data_buf_t);

    do {
        if (!packet->data_buf) {
            /* Allocate buffer from shared buffer pool. */
            SHR_IF_ERR_EXIT
                (bcmpkt_data_buf_alloc(BCMPKT_BPOOL_SHARED_ID, max_frame_size, &packet->data_buf));
        } else {
            packet->data_buf->data = packet->data_buf->head;
            packet->data_buf->data_len = 0;
        }
        bytes = recv(rawsock->fd, BCMPKT_PACKET_DATA(packet),
                     max_frame_size, MSG_DONTWAIT);
        if (bytes == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                SHR_EXIT();
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(rawsock->unit, "%s\n"),
                           strerror(errno)));
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        } else if (bytes == 0) {
            SHR_EXIT();
        }
        BCMPKT_PACKET_LEN(packet) = bytes;
        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb;
            pb = shr_pb_create();
            shr_pb_printf(pb, "Received packet (len=%u):\n", bytes);
            bcmpkt_data_dump(pb, BCMPKT_PACKET_DATA(packet), bytes);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(rawsock->unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }

        if (SHR_FAILURE(bcmlu_rawsock_packet_validate(rawsock, packet))){
            continue;
        }

        if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
            shr_pb_t *pb;
            uint32_t dev_type;
            uint32_t *flexdata = NULL;
            uint32_t flexdata_len = 0;
            uint32_t flex_profile = 0;
            bcmlrd_variant_t variant;

            variant = bcmlrd_variant_get(rawsock->unit);
            if (variant == BCMLRD_VARIANT_T_NONE) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }

            pb = shr_pb_create();

            if (SHR_SUCCESS(bcmpkt_dev_type_get(rawsock->unit, &dev_type))) {
                SHR_IF_ERR_EXIT_EXCEPT_IF(
                    bcmpkt_rxpmd_flexdata_get(dev_type, packet->pmd.rxpmd,
                                              &flexdata, &flexdata_len),
                    SHR_E_UNAVAIL);
                shr_pb_printf(pb, "RXPMD:\n");
                bcmpkt_rxpmd_dump(dev_type, packet->pmd.rxpmd,
                                  BCMPKT_RXPMD_DUMP_F_ALL, pb);
                if (flexdata_len) {
                    bcmpkt_rxpmd_field_get(dev_type, packet->pmd.rxpmd,
                                           BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE,
                                           &flex_profile);
                    shr_pb_printf(pb, "RXPMD FLEX fields:\n");
                    bcmpkt_rxpmd_flex_dump(variant, flexdata,
                                           flex_profile,
                                           BCMPKT_RXPMD_FLEX_DUMP_F_ALL, pb);
                }
                shr_pb_printf(pb, "RX reasons:\n");
                bcmpkt_rxpmd_flex_reason_dump(variant,
                                              flexdata, pb);
            } else {
                shr_pb_printf(pb, "RX reasons:\n");
                bcmpkt_rx_reason_dump(dev_type, packet->pmd.rxpmd, pb);
            }

            shr_pb_printf(pb, "RX Data buf:\n");
            bcmpkt_data_buf_dump(packet->data_buf, pb);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(rawsock->unit, "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }

        rawsock->rx_packets++;
        rawsock->rx_bytes += BCMPKT_PACKET_LEN(packet);

        /* BCMPKT API only supports one callback. */
        if (rawsock->cb_info != NULL && rawsock->cb_info->cb_func &&
            !rawsock->cb_info->cb_pending) {
            rawsock->cb_info->cb_func(rawsock->unit, rawsock->netif.id, packet,
                                      rawsock->cb_info->cb_data);
        }

        /*!
         * If this packet is cloned somewhere,
         * release the data buf and alloc new one for receiving
         */
        if (packet->data_buf && packet->data_buf->ref_count > 1) {
            SHR_IF_ERR_EXIT
                (bcmpkt_data_buf_free(packet->unit, packet->data_buf));
            packet->data_buf = NULL;
        }

        if (rawsock->rx_break) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
    } while (TRUE);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_read(bcmlu_rawsock_t *rawsock)
{
    int rv;
    struct pollfd pfd;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID, bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    sal_memset(&pfd, 0, sizeof(pfd));
    pfd.fd = rawsock->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    while (!rawsock->rx_break) {

        pfd.revents = 0;
        rv = poll(&pfd, 1, rawsock->poll_timeout);
        if (rv == -1  && errno != EINTR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit, "Polling failed: %s\n"),
                       strerror(errno)));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLHUP) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Hangup on packet socket\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLNVAL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Invalid polling request\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLERR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(rawsock->unit,
                                  "Polling error\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLIN) {
            rv = bcmlu_rawsock_packet_handle(rawsock, packet);
            if (rv == SHR_E_DISABLED) {
                SHR_EXIT();
            } else if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }
        }
        if (rawsock->rx_break) {
            SHR_EXIT();
        }
    }

exit:
    if (packet) {
        bcmpkt_free(packet->unit, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}

static void
bcmlu_rawsock_rx_thread(shr_thread_t th, void *p)
{
    bcmlu_rawsock_t *rawsock = (bcmlu_rawsock_t *)p;

    if (rawsock == NULL) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL pointer\n")));
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(rawsock->unit, "Enter RX thread\n")));
    rawsock->read(rawsock);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "RX thread closed (network interface %d)\n"),
                 rawsock->netif.id));
}

static int
iface_get_id(int sock_fd, const char *device)
{
    struct ifreq  ifr;

    sal_memset(&ifr, 0, sizeof(struct ifreq));
    sal_strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));

    if (ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("SIOCGIFINDEX failure: %s"), strerror(errno)));
        return -1;
    }

    return ifr.ifr_ifindex;
}

static int
bcmlu_rawsock_created(int unit, int netif_id, bool *created)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(created, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    *created = (rawsock_handles[unit][netif_id] != NULL);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_netif_bind(int sock_fd, bcmlu_rawsock_t *rawsock)
{
    int err;
    socklen_t errlen = sizeof(err);
    struct sockaddr_ll sll;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    sal_memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family    = PF_PACKET;
    sll.sll_protocol  = htons(ETH_P_ALL);
    sll.sll_ifindex   = iface_get_id(sock_fd, rawsock->netif.name);
    if (sll.sll_ifindex < 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    sll.sll_halen = 6;
    sal_memcpy(sll.sll_addr, rawsock->netif.mac_addr, 6);

    err = bind(sock_fd, (struct sockaddr *) &sll, sizeof(struct sockaddr_ll));
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "bind failed (%s)\n"), strerror(errno)));
    }
    err = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "getsockopt failed (%s)\n"), strerror(errno)));
    }

    if (err > 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit,
                         "getsockopt SO_ERROR (%s)\n"), strerror(err)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "Bind socket (%d) to network interface (%s) succeeded\n"),
                 sock_fd, rawsock->netif.name));

exit:
    SHR_FUNC_EXIT();
}

/* Create raw socket on a netif. */
static int
bcmlu_rawsock_setup(bcmlu_rawsock_t *rawsock)
{
    char name[128];
    int err;
    int sock_fd = -1;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    /* Create socket */
    sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_fd == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Create socket failed (%s)\n"),
              strerror(errno)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "Create socket (%d) successfully\n"),
                 sock_fd));

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, rawsock->netif.name, sizeof(ifr.ifr_name));
    err = ioctl(sock_fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Set interface configuration. */
    ifr.ifr_flags |= (IFF_PROMISC | IFF_UP | IFF_RUNNING);
    err = ioctl(sock_fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit, "Open network interface %s\n"),
                 rawsock->netif.name));
#endif

    /*! Bind socket to the netif. */
    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_netif_bind(sock_fd, rawsock));

    rawsock->fd = sock_fd;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(rawsock->unit,
                            "Bound socket %d with network interface %s\n"),
                 rawsock->fd, rawsock->netif.name));

    rawsock->send = bcmlu_rawsock_send;
    rawsock->read = bcmlu_rawsock_read;

    sal_sprintf(name, "Bcmpkt_raw_socket_rx_%s", rawsock->netif.name);
    rawsock->rx_th = shr_thread_start(name, -1,
                                      bcmlu_rawsock_rx_thread,
                                      (void*)rawsock);
    if (!rawsock->rx_th) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(rawsock->unit,
                                        "Could not start RX thread!")));
    }

    exit:
    if (SHR_FUNC_ERR() && sock_fd >= 0) {
        close(sock_fd);
    }
    SHR_FUNC_EXIT();

}

static int
bcmlu_rawsock_socket_destroy(bcmlu_rawsock_t *rawsock)
{
    int err;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(rawsock, SHR_E_PARAM);

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, rawsock->netif.name, sizeof(ifr.ifr_name));
    err = ioctl(rawsock->fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Disable netif. */
    ifr.ifr_flags &= (~IFF_PROMISC) & (~IFF_RUNNING) & (~IFF_UP);
    err = ioctl(rawsock->fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(rawsock->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
#endif

exit:
    if (rawsock->fd >= 0) {
        close(rawsock->fd);
        rawsock->fd = -1;
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_create(int unit, int netif_id, bcmpkt_socket_cfg_t *cfg)
{
    bcmlu_rawsock_t *rawsock = NULL;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (bcmlu_rawsock_handle_get(unit, netif_id) != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "SOCKET exists on the network interface\n")));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_netif_get(unit, netif_id, &netif));

    /* RCPU encapsulation is requested for SOCKET. */
    if (!(netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit,
                         "The network interface should support RCPU header\n")));
    }

    SHR_ALLOC(rawsock, sizeof(*rawsock), "bcmluRawSocketCreate");
    SHR_NULL_CHECK(rawsock, SHR_E_MEMORY);

    sal_memset(rawsock, 0, sizeof(*rawsock));

    rawsock->unit = unit;
    /* Init to invalid value. */
    rawsock->fd = -1;
    rawsock->poll_timeout = cfg->rx_poll_timeout;
    sal_memcpy(&rawsock->netif, &netif, sizeof(netif));

    SHR_IF_ERR_EXIT
        (bcmlu_rawsock_setup(rawsock));

    rawsock_handles[unit][netif.id] = rawsock;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(rawsock);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_destroy(int unit, int netif_id)
{
    bcmlu_rawsock_t *rawsock;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Not found Raw Socket on network interface %d\n"),
                     netif_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (rawsock->rx_th) {
        rawsock->rx_break = TRUE;
        rv = shr_thread_stop(rawsock->rx_th, 1000000);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Rawsock RX thread did not exit\n")));
        }
        rawsock->rx_th = NULL;
        rawsock->rx_break = FALSE;
    }

    bcmlu_rawsock_socket_destroy(rawsock);

    SHR_FREE(rawsock->cb_info);
    SHR_FREE(rawsock);

    /* Set to NULL after free. */
    rawsock_handles[unit][netif_id] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_stats_dump(int unit, int netif_id, shr_pb_t *pb)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMLU_NETIF_MAX_IDS) {
            rawsock = bcmlu_rawsock_handle_get(unit, id++);
            if (rawsock == NULL ) {
                continue;
            }
            rawsock->rx_drops = rawsock->pkt_len_err  +
                                rawsock->non_rcpu_pkt +
                                rawsock->rcpu_hdr_err +
                                rawsock->rcpu_len_err;

            shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tRX dropped %"PRIu32"\n"
                          "\t    Packet length check fail %"PRIu32"\n"
                          "\t    Non RCPU packet %"PRIu32"\n"
                          "\t    RCPU header check fail %"PRIu32"\n"
                          "\t    RCPU length check fail %"PRIu32"\n"
                          "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tTX dropped %"PRIu32"\n",
                          rawsock->rx_packets, rawsock->rx_bytes,
                          rawsock->rx_drops, rawsock->pkt_len_err,
                          rawsock->non_rcpu_pkt, rawsock->rcpu_hdr_err,
                          rawsock->rcpu_len_err, rawsock->tx_packets,
                          rawsock->tx_bytes, rawsock->tx_drops);
        }
        SHR_EXIT();
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Network interface %d does not support raw socket\n"),
              netif_id));
    }

    rawsock->rx_drops = rawsock->pkt_len_err  +
                        rawsock->non_rcpu_pkt +
                        rawsock->rcpu_hdr_err +
                        rawsock->rcpu_len_err;

    shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tRX dropped %"PRIu32"\n"
                  "\t    Packet length check fail %"PRIu32"\n"
                  "\t    Non RCPU packet %"PRIu32"\n"
                  "\t    RCPU header check fail %"PRIu32"\n"
                  "\t    RCPU length check fail %"PRIu32"\n"
                  "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tTX dropped %"PRIu32"\n",
                  rawsock->rx_packets, rawsock->rx_bytes, rawsock->rx_drops,
                  rawsock->pkt_len_err, rawsock->non_rcpu_pkt,
                  rawsock->rcpu_hdr_err, rawsock->rcpu_len_err,
                  rawsock->tx_packets, rawsock->tx_bytes, rawsock->tx_drops);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_stats_clear(int unit, int netif_id)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMLU_NETIF_MAX_IDS) {
            rawsock = bcmlu_rawsock_handle_get(unit, id++);
            if (rawsock == NULL) {
                continue;
            }

            rawsock->rx_packets = 0;
            rawsock->rx_bytes = 0;
            rawsock->rx_drops = 0;
            rawsock->tx_packets = 0;
            rawsock->tx_bytes = 0;
            rawsock->tx_drops = 0;
        }
        SHR_EXIT();
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Network interface %d does not support Raw Socket\n"),
              netif_id));
    }

    rawsock->rx_packets = 0;
    rawsock->rx_bytes = 0;
    rawsock->rx_drops = 0;
    rawsock->tx_packets = 0;
    rawsock->tx_bytes = 0;
    rawsock->tx_drops = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_rx_register(int unit, int netif_id, uint32_t flags,
                          bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit,
                         "Network interface %d does not support Raw Socket\n"),
              netif_id));
    }
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (rawsock->cb_info != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Overwriting callback function is prohibit\n")));
    }
    SHR_ALLOC(rawsock->cb_info, sizeof(bcmpkt_rx_cb_info_t), "bcmluRawSocketRxReg");
    SHR_NULL_CHECK(rawsock->cb_info, SHR_E_MEMORY);

    rawsock->cb_info->flags   = flags;
    rawsock->cb_info->cb_func = cb_func;
    rawsock->cb_info->cb_data = cb_data;
    rawsock->cb_info->cb_pending = FALSE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_rawsock_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                            void *cb_data)
{
    bcmlu_rawsock_t *rawsock;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    rawsock = bcmlu_rawsock_handle_get(unit, netif_id);
    if (rawsock == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit,
                         "Network interface %d does not support RawSocket\n"),
              netif_id));
    }

    if (rawsock->cb_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback not registered\n")));
        SHR_EXIT();
    }

    if (rawsock->cb_info->cb_func != cb_func) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback does not match\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rawsock->cb_info->cb_pending = TRUE;
    sal_usleep(100000);
    SHR_FREE(rawsock->cb_info);
    LOG_VERBOSE(BSL_LS_SYS_RX,
                (BSL_META_U(unit, "Unregister succeed\n")));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_rawsock_attach(void)
{
    bcmpkt_socket_t socket_vect = {
        .initialized = 1,
        .driver_name = "RAWSOCK",
        .driver_type = BCMPKT_SOCKET_DRV_T_RAWSOCK,
        .create = bcmlu_rawsock_create,
        .destroy = bcmlu_rawsock_destroy,
        .stats_dump = bcmlu_rawsock_stats_dump,
        .stats_clear = bcmlu_rawsock_stats_clear,
        .created = bcmlu_rawsock_created
    };
    bcmpkt_net_t net_vect = {
        .initialized = 1,
        .driver_name = "RAWSOCK",
        .driver_type = BCMPKT_SOCKET_DRV_T_RAWSOCK,
        .rx_register = bcmlu_rawsock_rx_register,
        .rx_unregister = bcmlu_rawsock_rx_unregister,
        .tx = bcmlu_rawsock_tx
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_socket_drv_register(&socket_vect), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_net_drv_register(&net_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
}

int
bcmlu_rawsock_detach(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_unregister(BCMPKT_NET_DRV_T_RAWSOCK));

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_unregister(BCMPKT_SOCKET_DRV_T_RAWSOCK));

exit:
    SHR_FUNC_EXIT();
}

