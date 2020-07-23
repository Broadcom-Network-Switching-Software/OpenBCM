/*! \file bcmlu_tpacket.c
 *
 * TPACKET wrapper.
 *
 * This file is only work fo Packet mmap raw socket.
 * At least TPacket version 1 supported.
 * The goal Linux kernel version is 2.4+. Doesn't support old Packet socket
 * older than Linux 2.2;
 * PF_PACKET is a new feature in Linux 2.2. Earlier Linux versions supported
 * only SOCK_PACKET.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sys/types.h>
#include <sys/mman.h>
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
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_hg3.h>
#include <bcmpkt/flexhdr/bcmpkt_hg3_base_t.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include "bcmlu_tpacket.h"
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlu/bcmlu_ngknet.h>

#define BSL_LOG_MODULE          BSL_LS_SYS_TPACKET

#ifdef TPACKET_HDRLEN

#ifdef TPACKET3_HDRLEN
#define HAVE_TPACKET_V3         3
#define HAVE_TPACKET_V2         2
#elif defined(TPACKET2_HDRLEN)
#define HAVE_TPACKET_V2         2
#else  /* TPACKET2_HDRLEN */
#define TPACKET_V1              0
#endif /* TPACKET2_HDRLEN */

#define BCMPKT_TP_FAMILY        PF_PACKET
#define BCMPKT_TP_PROTOCOL      ETH_P_ALL

#ifndef PACKET_TX_RING
#define PACKET_TX_RING          13
#endif
#ifndef PACKET_RX_RING
#define PACKET_RX_RING          5
#endif

#ifndef POLLRDHUP
#define POLLRDHUP               0
#endif

#define TXPMD_START_HIGIG       3

#define RX_IMMEDIATE            0
#define TX_IMMEDIATE            1
#define RX_BLOCK_TIMEOUT        5
#define TX_BLOCK_TIMEOUT        5
#define TX_NONBLOCK_TIMEOUT     1 /* 1 second */

#define RING_GET_CURRENT_RD(h) (((bcmlu_tpacket_hdr_t **)h->rd)[(h->rd_p)])
#define RING_GET_CURRENT_WD(h) (((bcmlu_tpacket_hdr_t **)h->wd)[(h->wd_p)])

/*!
 * We use this internally as the tpacket version for TPACKET_V1 in
 * 32-bit code on a 64-bit kernel.
 */
#define TPACKET_V1_64           99

/*!
 * This is what the header structure looks like in a 64-bit kernel;
 * we use this, rather than struct tpacket_hdr, if we're using
 * TPACKET_V1 in 32-bit code running on a 64-bit kernel.
 */
struct tpacket_hdr_64 {
    uint64_t tp_status;
    unsigned int tp_len;
    unsigned int tp_snaplen;
    unsigned short tp_mac;
    unsigned short tp_net;
    unsigned int tp_sec;
    unsigned int tp_usec;
};

typedef union bcmlu_tpacket_hdr_s {
    struct tpacket_hdr *h1;
    struct tpacket_hdr_64 *h1_64;
#if defined(HAVE_TPACKET_V2) || defined(HAVE_TPACKET_V3)
    struct tpacket2_hdr *h2;
#ifdef HAVE_TPACKET_V3
    struct tpacket_block_desc *h3;
#endif
#endif
    void *raw;
} bcmlu_tpacket_hdr_t;

/*!
 * \brief PF_PACKET ring handler structure.
 */
typedef struct bcmlu_tpacket_ring_s{

    /*! Tpacket version for the ring. */
    int tp_version;

    /*! The tpacket_hdr length. */
    int tp_hdrlen;

    /*! The memory-mapped buffer handle. */
    uint8_t *buf;

    /*! The memory-mapped buffer size. */
    unsigned int buf_len;

    /*! Maximum frame size in the block. */
    unsigned int frame_size;

    /*! Read descriptor. */
    uint8_t *rd;

    /*! Read descriptor count. */
    int rd_cnt;

    /*! Read descriptor position. */
    int rd_p;

    /*! Write descriptor. */
    uint8_t *wd;

    /*! Write descriptor count. */
    int wd_cnt;

    /*! Write descriptor position. */
    int wd_p;

#ifdef HAVE_TPACKET_V3
    /*!
     * Maximum filling ring block time, timeout will release the block
     * to userland.
     */
    uint8_t block_timeout;

    /*!
     * Current packet within the TPACKET_V3 block. Move to next block
     * if NULL.
     */
    uint8_t *current_packet;

    /*!
     * Unhandled packets left within the block from previous call to
     * pcap_read_linux_mmap_v3 in case of TPACKET_V3.
     */
    int packets_left;
#endif

    /*! Received valid packets on the ring. */
    uint32_t rx_packets;

    /*! Dropped invalid packets on the ring. */
    uint32_t rx_drops;

    /*! Packet dropped due to data buf error. */
    uint32_t pkt_buf_err;

    /*! Packet dropped due to invalid packet length. */
    uint32_t pkt_len_err;

    /*! Non-RCPU packet dropped due to RCPU encap configured on Netif. */
    uint32_t non_rcpu_pkt;

    /*! RCPU packet dropped due to header check fail. */
    uint32_t rcpu_hdr_err;

    /*! RCPU packet dropped due to length check fail. */
    uint32_t rcpu_len_err;

    /*! Received bytes on the ring. */
    uint32_t rx_bytes;

    /*! Transmit packets on the ring. */
    uint32_t tx_packets;

    /*! Dropped packets to the ring. */
    uint32_t tx_drops;

    /*! Transmit bytes no the ring. */
    uint32_t tx_bytes;

    /*! Socket address link-level header information. */
    struct sockaddr_ll sll;
} bcmlu_tpacket_ring_t;

typedef struct bcmlu_tpacket_s bcmlu_tpacket_t;

typedef int  (*bcmlu_tpacket_read_f)(bcmlu_tpacket_t *tp);
typedef int  (*bcmlu_tpacket_send_f)(bcmlu_tpacket_t *tp, uint8_t *data,
                                     uint32_t len);

/*!
 * \brief Tpacket handler structure.
 */
struct bcmlu_tpacket_s {

    /*! Host unit number. */
    int unit;

    /*! File descriptor of socket. */
    int fd;

    /*! Tpacket private data stucture - ring. */
    void *priv;

    /*! Host network interface configuration. */
    bcmpkt_netif_t netif;

    /*! RX thread handler. */
    shr_thread_t rx_tc;

    /*! TX thread handler. */
    shr_thread_t tx_tc;

    /*! Break RX loop to terminate RX thread. */
    bool rx_break;

    /*! Immediate or delay for receive. (For Tpacket V1 & V2) */
    int rx_immediate;

    /*! Immediate or delay for transmit. (For Tpacket V1 & V2) */
    int tx_immediate;

    /*! RX configuration buffer size for mmap. */
    uint32_t rx_buf_size;

    /*! TX configuration buffer size for mmap. */
    uint32_t tx_buf_size;

    /*!
     * For V3 only, kernel returns the RX block to userland when the block is
     * receving more packets but this timeout happened.
     */
    uint32_t rx_block_timeout;

    /*!
     * For V1/V2 batch send only, notify kernel to send packets when this
     * timeout happened.
     */
    uint32_t tx_block_timeout;

    /*! Timeout for poll function. */
    uint32_t poll_timeout;

    /*! Receive function. */
    bcmlu_tpacket_read_f read;

    /*! Transmit function. */
    bcmlu_tpacket_send_f send;

    /*
     * RX callback handle.
     */
    bcmpkt_rx_cb_info_t *cb_info;
} ;

static const char *tp_ver_str[] = {
    "TPACKET_V1",
    "TPACKET_V2",
    "TPACKET_V3",
};

/*!
 * RX default buffer size is 16M, and TX is 2M.
 *
 * The frame size for Tpacket_V1 and Tpacket_V2 must be fix, each
 * frame may hold one packet and hence should be enable to hold a
 * maximum size packet plus overhead (e.g. RCPU header). For example,
 * if frame size is 2048 and overhead is 256, the packet_mmap ring can
 * deliver up to 1792B size packets. 16M memory in the case can buffer
 * 8k packets. If RX data rate is 5G bps, the buffer can buffer ~1 ms.

 * TPacket_V3 supports flexible frame size buffer. Hence, in the case,
 * 16M memory can hold ~52k packets, and so buffer ~10 ms 5G bps
 * traffic.
 */
#define TP_BUF_SIZE_DEFAULT_RX    (8*1024*2048)
#define TP_BUF_SIZE_DEFAULT_TX    (1024*2048)
#define TP_BUF_V3_BLOCK_MIN_SIZE  (256*1024)

/* Netif 0 is reserved. Supported netif is 1 ~ BCMLU_NETIF_MAX_IDS. */
static bcmlu_tpacket_t *tpacket_handles[BCMDRD_CONFIG_MAX_UNITS][BCMLU_NETIF_MAX_IDS + 1];

#if defined(HAVE_TPACKET_V2) || defined(HAVE_TPACKET_V3)
static int
bcmlu_tpacket_version_set(bcmlu_tpacket_t *tp, int version)
{
    bcmlu_tpacket_ring_t *ring;
    int val = version;
    socklen_t len = sizeof(val);
    int err;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    /*
     * Probe whether kernel supports the specified TPACKET version;
     * And get the length of the header for that version.
     */
    err = getsockopt(tp->fd, SOL_PACKET, PACKET_HDRLEN, &val, &len);
    if (err < 0) {
        if (errno == ENOPROTOOPT || errno == EINVAL) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_UNAVAIL,
                 (BSL_META_U(tp->unit, "getsockopt failed (%s) on %s\n"),
                  strerror(errno), tp_ver_str[version]));
        }
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "getsockopt failed (%s) on %s\n"),
              strerror(errno), tp_ver_str[version]));
    }
    ring->tp_hdrlen = val;

    val = version;
    err = setsockopt(tp->fd, SOL_PACKET, PACKET_VERSION, &val, sizeof(val));
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
             (SHR_E_FAIL,
              (BSL_META_U(tp->unit, "setsockopt failed (%s) on %s\n"),
                        strerror(errno), tp_ver_str[version]));
    }
    ring->tp_version = version;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit, "Set (%s)\n"), tp_ver_str[version]));

exit:
    SHR_FUNC_EXIT();
}
#endif /*! defined(HAVE_TPACKET_V2) || defined(HAVE_TPACKET_V3) */

static int
bcmlu_tpacket_version_select(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
#ifdef HAVE_TPACKET_V2
    int rv;
#endif

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

#ifdef HAVE_TPACKET_V3
    /*
     * Packets might not be delivered immediately in TPACKET_V3 mode.
     */
    if (!tp->rx_immediate) {
        rv = bcmlu_tpacket_version_set(tp, TPACKET_V3);
        if (rv == SHR_E_NONE) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(tp->unit, "Select version %s\n"),
                         tp_ver_str[TPACKET_V3]));
            SHR_EXIT();
        } else if (rv != SHR_E_UNAVAIL) {
            SHR_ERR_EXIT(rv);
        }
    }
#endif /* HAVE_TPACKET_V3 */

#ifdef HAVE_TPACKET_V2
    rv = bcmlu_tpacket_version_set(tp, TPACKET_V2);
    if (rv == SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(tp->unit, "Select version %s\n"),
                     tp_ver_str[TPACKET_V2]));
        SHR_EXIT();
    } else if (rv != SHR_E_UNAVAIL) {
        SHR_ERR_EXIT(rv);
    }
#endif /* HAVE_TPACKET_V2 */

    ring->tp_version = TPACKET_V1;
    ring->tp_hdrlen = sizeof(struct tpacket_hdr);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit, "Set %s\n"), tp_ver_str[TPACKET_V1]));

#ifdef ISA_64_BIT
    /*
     * 32-bit userspace + 64-bit kernel + TPACKET_V1 are not compatible with
     * each other due to platform-dependent data type size differences.
     *
     * If we have a 32-bit userland and a 64-bit kernel, use an
     * internally-defined TPACKET_V1_64, with which we use a 64-bit
     * version of the data structures.
     */
    if (sizeof(long) == 4) {
        /*
         * This is 32-bit code.
         */
        struct utsname utsname;

        if (uname(&utsname) == -1) {
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                  BSL_META_U(tp->unit, "uname failed (%s)\n"), strerror(errno));
        }
        if (sal_strcmp(utsname.machine, ISA_64_BIT) == 0) {
            /*
             * uname() tells us the machine is 64-bit,
             * so we presumably have a 64-bit kernel.
             *
             * XXX - this presumes that uname() won't lie
             * in 32-bit code and claim that the machine
             * has the 32-bit version of the ISA.
             */
            ring->tp_version = TPACKET_V1_64;
            ring->tp_hdrlen = sizeof(struct tpacket_hdr_64);
        }
    }
#endif

exit:
    SHR_FUNC_EXIT();
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
bcmlu_tpacket_netif_bind(int sock_fd, bcmlu_tpacket_t *tp)
{
    int err;
    socklen_t errlen = sizeof(err);
    bcmlu_tpacket_ring_t *ring;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    sal_memset(&(ring->sll), 0, sizeof(ring->sll));
    ring->sll.sll_family    = BCMPKT_TP_FAMILY;
    ring->sll.sll_protocol  = htons(BCMPKT_TP_PROTOCOL);
    ring->sll.sll_ifindex   = iface_get_id(sock_fd, tp->netif.name);
    if (ring->sll.sll_ifindex < 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    ring->sll.sll_halen = 6;
    sal_memcpy(ring->sll.sll_addr, tp->netif.mac_addr, 6);

    err = bind(sock_fd, (struct sockaddr *) &(ring->sll), sizeof(ring->sll));
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "bind failed (%s)\n"), strerror(errno)));
    }
    err = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit,
                         "getsockopt failed (%s)\n"), strerror(errno)));
    }

    if (err > 0) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit,
                         "getsockopt SO_ERROR (%s)\n"), strerror(err)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit,
                            "Bind socket (%d) to network interface (%s) succeeded\n"),
                 sock_fd, tp->netif.name));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_socket_create(bcmlu_tpacket_t *tp)
{
    int err;
    int sock_fd = -1;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);

    /* Create socket */
    sock_fd = socket(PF_PACKET, SOCK_RAW, htons(BCMPKT_TP_PROTOCOL));
    if (sock_fd == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "Create socket failed (%s)\n"),
              strerror(errno)));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit, "Create socket (%d) successfully\n"),
                 sock_fd));

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, tp->netif.name, sizeof(ifr.ifr_name));
    err = ioctl(sock_fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Set interface configuration. */
    ifr.ifr_flags |= (IFF_PROMISC | IFF_UP | IFF_RUNNING);
    err = ioctl(sock_fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit, "Open network interface %s\n"),
                 tp->netif.name));
#endif

    /*! Bind socket to the netif. */
    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_netif_bind(sock_fd, tp));
    tp->fd = sock_fd;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit,
                            "Bound socket %d with network interface %s\n"),
                 tp->fd, tp->netif.name));

exit:
    if (SHR_FUNC_ERR() && sock_fd >= 0) {
        close(sock_fd);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_socket_destroy(bcmlu_tpacket_t *tp)
{
    int err;
    struct ifreq ifr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);

    /* Get interface configuration. */
    sal_memset(&ifr, 0, sizeof(ifr));
    sal_strlcpy(ifr.ifr_name, tp->netif.name, sizeof(ifr.ifr_name));
    err = ioctl(tp->fd, SIOCGIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "Ioctl SIOCGIFFLAGS error\n")));
    }

#if defined(IFF_PROMISC) && defined(IFF_UP) && defined(IFF_RUNNING)
    /* Disable netif. */
    ifr.ifr_flags &= (~IFF_PROMISC) & (~IFF_RUNNING) & (~IFF_UP);
    err = ioctl(tp->fd, SIOCSIFFLAGS, &ifr);
    if (err == -1) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit, "Ioctl SIOCSIFFLAGS error\n")));
    }
#endif

exit:
    if (tp->fd >= 0) {
        close(tp->fd);
        tp->fd = -1;
    }
    SHR_FUNC_EXIT();
}
static int
bcmlu_tpacket_ring_destroy(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    struct tpacket_req req;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    sal_memset(&req, 0, sizeof(req));

    (void)setsockopt(tp->fd, SOL_PACKET, PACKET_RX_RING, (void *) &req,
                     sizeof(req));
    if (tp->tx_immediate == 0) {
        (void)setsockopt(tp->fd, SOL_PACKET, PACKET_TX_RING, (void *) &req,
                         sizeof(req));
    }
    if (ring->buf) {
        (void)munmap(ring->buf, ring->buf_len);
        ring->buf = NULL;
    }
    SHR_FREE(ring->rd);
    SHR_FREE(ring->wd);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_ring_create(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    unsigned i, j, frames_per_block, wd_start;
    unsigned int tp_hdrlen;
    unsigned int frame_size;
#ifdef HAVE_TPACKET_V3
    struct tpacket_req3 req, req_tx;
#else
    struct tpacket_req req, req_tx;
#endif

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    switch (ring->tp_version) {
    case TPACKET_V1:
    case TPACKET_V1_64:
#ifdef HAVE_TPACKET_V2
    case TPACKET_V2:
#endif
        /* Not support reserve space. */
        frame_size = tp->netif.max_frame_size + BCMPKT_RCPU_MAX_ENCAP_SIZE;
        tp_hdrlen = TPACKET_ALIGN(ring->tp_hdrlen) + sizeof(struct sockaddr_ll);
        req.tp_frame_size = TPACKET_ALIGN(tp_hdrlen + frame_size);
        req.tp_frame_nr = tp->rx_buf_size / req.tp_frame_size;
        /* Not support reserve space. */
        req_tx.tp_frame_size = TPACKET_ALIGN(tp_hdrlen + frame_size);
        req_tx.tp_frame_nr = tp->tx_buf_size  /req_tx.tp_frame_size;
        break;

#ifdef HAVE_TPACKET_V3
    case TPACKET_V3:
        /* No reserve space. */
        tp_hdrlen = TPACKET_ALIGN(ring->tp_hdrlen) + sizeof(struct sockaddr_ll);
        frame_size = TPACKET_ALIGN(tp_hdrlen + tp->netif.max_frame_size + BCMPKT_RCPU_MAX_ENCAP_SIZE);
        req.tp_frame_size = sysconf(_SC_PAGE_SIZE);
        while (req.tp_frame_size < TP_BUF_V3_BLOCK_MIN_SIZE ||
               req.tp_frame_size < frame_size) {
            req.tp_frame_size <<= 1;
        }
        req.tp_frame_nr = tp->rx_buf_size / req.tp_frame_size;
        ring->block_timeout = tp->rx_block_timeout;
        /* TPacket_V3 doesnot support batch send. */
        tp->tx_immediate = 1;
        break;
#endif
    default:
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(tp->unit, "Unknown TPACKET version %u\n"),
                   ring->tp_version));

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Compute the minumum block size that will handle this frame.
     * The block has to be page size aligned.
     */
    req.tp_block_size = sysconf(_SC_PAGE_SIZE);
    while (req.tp_block_size < req.tp_frame_size) {
        req.tp_block_size <<= 1;
    }

    frames_per_block = req.tp_block_size / req.tp_frame_size;
    req.tp_block_nr = req.tp_frame_nr / frames_per_block;
    req.tp_frame_nr = req.tp_block_nr * frames_per_block;

#ifdef HAVE_TPACKET_V3
        /* Timeout value to retire block - maximum buffer delay in TP. */
        req.tp_retire_blk_tov =
            (ring->block_timeout >= 0) ? ring->block_timeout : 0;
        /* Not use private data. */
        req.tp_sizeof_priv = 0;
        /* Rx ring - feature request bits - none (rxhash will not be filled). */
        req.tp_feature_req_word = 0;
#endif

    /* The req.tp_frame_nr is requested to match frames_per_block*req.tp_block_nr. */
    while (setsockopt(tp->fd, SOL_PACKET, PACKET_RX_RING, (void *) &req,
                      sizeof(req))) {
        if ((errno == ENOMEM) && (req.tp_block_nr > 1)) {
            /* Memory failure; try to reduce the requested ring size.*/
            req.tp_block_nr--;
            req.tp_frame_nr = req.tp_block_nr * frames_per_block;
            continue;
        }
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(tp->unit, "Create rx ring failed %s\n"),
                   strerror(errno)));

        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (!tp->tx_immediate) {
        req_tx.tp_block_size = sysconf(_SC_PAGE_SIZE);
        while (req_tx.tp_block_size < req_tx.tp_frame_size ||
               tp->tx_buf_size / req_tx.tp_block_size > 256) {
            req_tx.tp_block_size <<= 1;
        }

        frames_per_block = req_tx.tp_block_size / req_tx.tp_frame_size;
        req_tx.tp_block_nr = req_tx.tp_frame_nr / frames_per_block;

#ifdef HAVE_TPACKET_V3
        req_tx.tp_retire_blk_tov = 0;
        req_tx.tp_sizeof_priv = 0;
        req_tx.tp_feature_req_word = 0;
#endif
        while (setsockopt(tp->fd, SOL_PACKET, PACKET_TX_RING,
               (void *) &req_tx, sizeof(req_tx))) {
            if ((errno == ENOMEM) && (req_tx.tp_block_nr > 1)) {
                /* Memory failure; try to reduce the requested ring size. */
                req_tx.tp_block_nr--;
                req_tx.tp_frame_nr = req_tx.tp_block_nr * frames_per_block;
                continue;
            }
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(tp->unit, "Create tx ring failed %s\n"),
                       strerror(errno)));

            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /* Memory map the rx ring. */
    ring->buf_len = req.tp_block_nr * req.tp_block_size;
    if (!tp->tx_immediate) {
        ring->buf_len += req_tx.tp_block_nr * req_tx.tp_block_size;
    }

    ring->buf = mmap(0, ring->buf_len, PROT_READ|PROT_WRITE, MAP_SHARED,
                     tp->fd, 0);
    if (ring->buf == MAP_FAILED) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(tp->unit, "Mmap failed %s\n"), strerror(errno)));

        /* Clear the allocated ring on error. */
        bcmlu_tpacket_ring_destroy(tp);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Allocate a ring for each frame header pointer. */
    ring->rd_cnt = req.tp_frame_nr;
    SHR_ALLOC(ring->rd, ring->rd_cnt * sizeof(union thdr *),
              "bcmluTPktRingCreate");
    if (!ring->rd) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(tp->unit, "Allocate ring rd failed %s\n"),
                   strerror(errno)));

        bcmlu_tpacket_ring_destroy(tp);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Fill the header ring with proper frame ptr. */
    ring->rd_p = 0;
    for (i = 0; i < req.tp_block_nr; ++i) {
        void *base = &ring->buf[i * req.tp_block_size];
        for (j = 0; j < frames_per_block; ++j, ++ring->rd_p) {
            RING_GET_CURRENT_RD(ring) = base;
            base = (uint8_t*)base + req.tp_frame_size;
        }
    }
    if (!tp->tx_immediate) {
        ring->wd_cnt = req_tx.tp_frame_nr;
        SHR_ALLOC(ring->wd, ring->wd_cnt * sizeof(union thdr *),
                  "bcmluTPktRingCreate");
        if (!ring->wd) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(tp->unit, "Allocate ring wd failed %s\n"),
                       strerror(errno)));

            bcmlu_tpacket_ring_destroy(tp);
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        wd_start = i * req.tp_block_size;
        ring->wd_p = 0;
        for (i = 0; i < req_tx.tp_block_nr; ++i) {
            void *base = &ring->buf[wd_start + i * req_tx.tp_block_size];
            for (j = 0; j < frames_per_block; ++j, ++ring->wd_p) {
                RING_GET_CURRENT_WD(ring) = base;
                base = (uint8_t*)base + req_tx.tp_frame_size;
            }
        }
    }

    ring->frame_size = req.tp_frame_size;
    ring->rd_p = 0;
    ring->wd_p = 0;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit,
                            "Create ring on network interface %d done\n"),
                 tp->netif.id));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Block waiting for frames to be available.
 */
static int bcmlu_tpacket_ring_poll(bcmlu_tpacket_t *tp)
{
    char c;
    struct pollfd pfd;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);

    sal_memset(&pfd, 0, sizeof(pfd));
    pfd.fd = tp->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    do {
        rv = poll(&pfd, 1, tp->rx_block_timeout);

        if (rv < 0 && errno != EINTR) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(tp->unit, "Poll failed: %s\n"),
                       strerror(errno)));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & (POLLHUP | POLLRDHUP)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(tp->unit, "Hangup on packet socket\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLERR) {
            /*
             * Get actual error code from recv().
             */
            if (recv(tp->fd, &c, sizeof(c), MSG_PEEK) != -1)
                continue;    /* No error. */
            if (errno == ENETDOWN) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(tp->unit, "The interface went down\n")));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(tp->unit, "Error: %s\n"),
                           strerror(errno)));
            }
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if (pfd.revents & POLLNVAL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(tp->unit, "Invalid polling request\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* Check for break loop condition on interrupted syscall. */
        if (tp->rx_break) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
    } while (rv < 0);

exit:
    SHR_FUNC_EXIT();
}

/* Handle a single memory mapped packet */
static int
bcmlu_tpacket_packet_handle(bcmlu_tpacket_t *tp,
                            bcmpkt_packet_t *packet,
                            uint8_t *frame, unsigned int tp_len,
                            unsigned int tp_mac, unsigned int tp_snaplen,
                            unsigned int tp_sec, unsigned int tp_usec)
{
    int rv;
    bcmpkt_rcpu_hdr_t *rhdr = bcmpkt_rcpu_hdr(tp->unit);
    bcmlu_tpacket_ring_t *ring;
    uint8_t *data = frame + tp_mac;
    unsigned int pkt_len, meta_size = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    assert(rhdr);
    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb;
        pb = shr_pb_create();
        shr_pb_printf(pb, "Received packet (len=%u):\n", tp_snaplen);
        bcmpkt_data_dump(pb, data, tp_snaplen);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(tp->unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    /* Perform sanity check. */
    if (tp_snaplen > ring->frame_size + BCMPKT_RCPU_HDR_LEN ||
        tp_snaplen < bcmpkt_framesize_min_get()) {
        ring->pkt_len_err++;
        ring->rx_bytes += tp_snaplen;
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(tp->unit,
                         "Corrupted: tp_mac %u, tp_snaplen %u, max_frame_size %u\n"),
              tp_mac, tp_snaplen, ring->frame_size));
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
            ring->rcpu_hdr_err++;
            ring->rx_bytes += tp_snaplen;
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(tp->unit, "RCPU validation failure:\n"
                                       "Signature %04x, OPCODE %02x, Flags %02x\n"),
                  (data[18] << 8) + data[19], data[20], data[21]));
        }
        pkt_len = data[25] + (data[24] << 8);
        meta_size = data[28];
        if (pkt_len + meta_size != tp_snaplen - sizeof(*rhdr)) {
            ring->rcpu_len_err++;
            ring->rx_bytes += tp_snaplen;
            SHR_IF_ERR_MSG_EXIT
                (SHR_E_FAIL,
                 (BSL_META_U(tp->unit, "Length error: packet length %u bytes\n"
                                       "meta length %u bytes\n"
                                       "RCPU header length %u bytes\n"
                                       "Snap length %u bytes\n"),
                  pkt_len, meta_size, BCMPKT_RCPU_HDR_LEN, tp_snaplen));
        }

        if (!packet->data_buf) {
            /* Allocate buffer from shared buffer pool. */
            rv = bcmpkt_data_buf_alloc(BCMPKT_BPOOL_SHARED_ID, tp_snaplen,
                                       &packet->data_buf);
            if (SHR_FAILURE(rv)) {
                ring->pkt_buf_err++;
                ring->rx_bytes += tp_snaplen;
                SHR_ERR_EXIT(rv);
            }
        } else {
            packet->data_buf->data = packet->data_buf->head;
            packet->data_buf->data_len = 0;
        }
        /* Reserve headroom for possible TX request, e.g. loopback test. */
        packet->data_buf->data = packet->data_buf->head + sizeof(*rhdr) +
                                 meta_size;
        data += sizeof(*rhdr);
        /* Copy RXPMD data. */
        sal_memcpy(packet->pmd.rxpmd, data, meta_size);
        data += meta_size;
    } else { /* Handle as non-RCPU packets. */
        pkt_len = tp_snaplen;

        /*
         * Drop non-RCPU packets if BCMPKT_NETIF_F_RCPU_ENCAP flag was
         * configured.
         */
        if (tp->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP) {
            ring->non_rcpu_pkt++;
            SHR_EXIT();
        }

        if (!packet->data_buf) {
            /* Allocate buffer from shared buffer pool. */
            rv = bcmpkt_data_buf_alloc(BCMPKT_BPOOL_SHARED_ID, tp_snaplen,
                                       &packet->data_buf);
            if (SHR_FAILURE(rv)) {
                ring->pkt_buf_err++;
                ring->rx_bytes += tp_snaplen;
                SHR_ERR_EXIT(rv);
            }
        } else {
            packet->data_buf->data = packet->data_buf->head;
            packet->data_buf->data_len = 0;
        }

        /* Reserve headroom for possible TX request, e.g. loopback test. */
        if ((pkt_len + BCMPKT_TX_HDR_RSV) <= packet->data_buf->len) {
            packet->data_buf->data = packet->data_buf->head + BCMPKT_TX_HDR_RSV;
        }
    }

    if (!bcmpkt_put(packet->data_buf, pkt_len)) {
        ring->pkt_buf_err++;
        ring->rx_bytes += tp_snaplen;
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Copy received packet data. */
    sal_memcpy(BCMPKT_PACKET_DATA(packet), data, BCMPKT_PACKET_LEN(packet));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        shr_pb_t *pb;
        uint32_t dev_type;
        uint32_t *flexdata = NULL;
        uint32_t flexdata_len = 0;
        uint32_t flex_profile = 0;
        bcmlrd_variant_t variant;

        variant = bcmlrd_variant_get(tp->unit);
        if (variant == BCMLRD_VARIANT_T_NONE) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        pb = shr_pb_create();

        if (SHR_SUCCESS(bcmpkt_dev_type_get(tp->unit, &dev_type))) {
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
                shr_pb_printf(pb, "RX reasons:\n");
                bcmpkt_rxpmd_flex_reason_dump(variant, flexdata, pb);
            } else {
                shr_pb_printf(pb, "RX reasons:\n");
                bcmpkt_rx_reason_dump(dev_type, packet->pmd.rxpmd, pb);
            }
        }

        shr_pb_printf(pb, "RX Data buf:\n");
        bcmpkt_data_buf_dump(packet->data_buf, pb);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(tp->unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    ring->rx_packets++;
    ring->rx_bytes += BCMPKT_PACKET_LEN(packet);
    /* BCMPKT API only supports one callback. */
    if (tp->cb_info != NULL && tp->cb_info->cb_func &&
        !tp->cb_info->cb_pending) {
        tp->cb_info->cb_func(tp->unit, tp->netif.id, packet,
                             tp->cb_info->cb_data);
    }

exit:
    /*!
     * If this packet is cloned somewhere,
     * release the data buf and alloc new one for receiving
     */
    if (packet->data_buf && packet->data_buf->ref_count > 1) {
        bcmpkt_data_buf_free(packet->unit, packet->data_buf);
        packet->data_buf = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_read_v1(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    bcmlu_tpacket_hdr_t h;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID,
                      bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    /* Pointer must be initialized to avoid compiler warnings */
    h.raw = NULL;

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    while (1) {
        h.raw = RING_GET_CURRENT_RD(ring);
        if (h.h1->tp_status == TP_STATUS_KERNEL) {
            int rv = bcmlu_tpacket_ring_poll(tp);
            if (rv == SHR_E_DISABLED) {
                SHR_EXIT();
            } else if (rv) {
                SHR_ERR_EXIT(rv);
            }
            continue;
        }

        SHR_IF_ERR_CONT
            (bcmlu_tpacket_packet_handle(
                                         tp,
                                         packet,
                                         h.raw,
                                         h.h1->tp_len,
                                         h.h1->tp_mac,
                                         h.h1->tp_snaplen,
                                         h.h1->tp_sec,
                                         h.h1->tp_usec));
        h.h1->tp_status = TP_STATUS_KERNEL;

        if (++ring->rd_p >= ring->rd_cnt)
            ring->rd_p = 0;

        if (tp->rx_break) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
    }

exit:
    if (packet) {
        bcmpkt_free(packet->unit, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_read_v1_64(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    bcmlu_tpacket_hdr_t h;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID,
                      bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    /* Pointer must be initialized to avoid compiler warnings */
    h.raw = NULL;

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    while (1) {
        h.raw = RING_GET_CURRENT_RD(ring);
        if (h.h1_64->tp_status == TP_STATUS_KERNEL){
            int rv = bcmlu_tpacket_ring_poll(tp);
            if (rv == SHR_E_DISABLED) {
                SHR_EXIT();
            } else if (rv) {
                SHR_ERR_EXIT(rv);
            }
            continue;
        }

        SHR_IF_ERR_CONT
            (bcmlu_tpacket_packet_handle(
                                         tp,
                                         packet,
                                         h.raw,
                                         h.h1_64->tp_len,
                                         h.h1_64->tp_mac,
                                         h.h1_64->tp_snaplen,
                                         h.h1_64->tp_sec,
                                         h.h1_64->tp_usec));
        h.h1_64->tp_status = TP_STATUS_KERNEL;

        if (++ring->rd_p >= ring->rd_cnt)
            ring->rd_p = 0;

        if (tp->rx_break) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
    }

exit:
    if (packet) {
        bcmpkt_free(packet->unit, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}

#ifdef HAVE_TPACKET_V2
static int
bcmlu_tpacket_read_v2(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    bcmlu_tpacket_hdr_t h;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID,
                      bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    /* Pointer must be initialized to avoid compiler warnings */
    h.raw = NULL;

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    while (1) {
        h.raw = RING_GET_CURRENT_RD(ring);
        if (h.h2->tp_status == TP_STATUS_KERNEL){
            int rv = bcmlu_tpacket_ring_poll(tp);
            if (rv == SHR_E_DISABLED) {
                SHR_EXIT();
            } else if (rv) {
                SHR_ERR_EXIT(rv);
            }
            continue;
        }

        SHR_IF_ERR_CONT
            (bcmlu_tpacket_packet_handle(
                                         tp,
                                         packet,
                                         h.raw,
                                         h.h2->tp_len,
                                         h.h2->tp_mac,
                                         h.h2->tp_snaplen,
                                         h.h2->tp_sec,
                                         h.h2->tp_nsec));
        h.h2->tp_status = TP_STATUS_KERNEL;

        if (++ring->rd_p >= ring->rd_cnt)
            ring->rd_p = 0;

        if (tp->rx_break) {
            SHR_ERR_EXIT(SHR_E_DISABLED);
        }
    }

exit:
    if (packet) {
        bcmpkt_free(packet->unit, packet);
        packet = NULL;
    }
    SHR_FUNC_EXIT();
}
#endif /* HAVE_TPACKET_V2 */

#ifdef HAVE_TPACKET_V3
static int
bcmlu_tpacket_read_v3(bcmlu_tpacket_t *tp)
{
    bcmlu_tpacket_ring_t *ring;
    bcmlu_tpacket_hdr_t h;
    bcmpkt_bpool_status_t bpool_status;
    bcmpkt_packet_t *packet = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmpkt_bpool_status_get(BCMPKT_BPOOL_SHARED_ID, &bpool_status));

    /* Allocate buffer from shared buffer pool. */
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID,
                      bpool_status.buf_size - sizeof(bcmpkt_data_buf_t),
                      0, &packet));

    /* Pointer must be initialized to avoid compiler warnings */
    h.raw = NULL;

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    while (1) {
        int packets_to_read;

        if (ring->current_packet == NULL) {
            h.raw = RING_GET_CURRENT_RD(ring);
            if ((h.h3->hdr.bh1.block_status & TP_STATUS_USER) == 0) {
                int rv = bcmlu_tpacket_ring_poll(tp);
                if (rv == SHR_E_DISABLED) {
                    SHR_EXIT();
                } else if (rv) {
                    SHR_ERR_EXIT(rv);
                }

                continue;
            }

            ring->current_packet =
                (uint8_t *)h.raw + h.h3->hdr.bh1.offset_to_first_pkt;
            ring->packets_left = h.h3->hdr.bh1.num_pkts;

        }
        packets_to_read = ring->packets_left;

        while (packets_to_read--) {
            struct tpacket3_hdr *tp3_hdr = (struct tpacket3_hdr *)ring->current_packet;
            SHR_IF_ERR_CONT
                (bcmlu_tpacket_packet_handle(
                                             tp,
                                             packet,
                                             ring->current_packet,
                                             tp3_hdr->tp_len,
                                             tp3_hdr->tp_mac,
                                             tp3_hdr->tp_snaplen,
                                             tp3_hdr->tp_sec,
                                             tp3_hdr->tp_nsec));
            ring->current_packet += tp3_hdr->tp_next_offset;
            ring->packets_left--;
        }

        if (ring->packets_left <= 0) {
            h.h3->hdr.bh1.block_status = TP_STATUS_KERNEL;
            if (++ring->rd_p >= ring->rd_cnt)
                ring->rd_p = 0;

            ring->current_packet = NULL;
        }

        if (tp->rx_break) {
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
#endif /* HAVE_TPACKET_V3 */

static int
bcmlu_tpacket_send_v1(bcmlu_tpacket_t *tp, uint8_t *data, uint32_t len)
{
    return SHR_E_NONE;
}

static int
bcmlu_tpacket_send_v1_64(bcmlu_tpacket_t *tp, uint8_t *data, uint32_t len)
{

    return SHR_E_NONE;
}

#ifdef HAVE_TPACKET_V2
static int
bcmlu_tpacket_send_v2(bcmlu_tpacket_t *tp, uint8_t *data, uint32_t len)
{

    return SHR_E_NONE;
}
#endif

static int
bcmlu_tpacket_send(bcmlu_tpacket_t *tp, uint8_t *data, uint32_t len)
{
    int bytes;
    bcmlu_tpacket_ring_t *ring;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = TX_NONBLOCK_TIMEOUT * SECOND_USEC;
    int min_polls = 1;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    shr_timeout_init(&to, timeout_usec, min_polls);
    while (TRUE) {
        /* Send the message with non-blocking sendto */
        bytes = sendto(tp->fd, data, len, MSG_DONTWAIT,
                       (struct sockaddr *) &ring->sll, sizeof(ring->sll));
        if (bytes == -1) {
            if (shr_timeout_check(&to)) {
                SHR_IF_ERR_MSG_EXIT
                    (SHR_E_FAIL,
                     (BSL_META_U(tp->unit,
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
                 (BSL_META_U(tp->unit,
                             "Call sendto failed (%s)\n"),
                  strerror(errno)));
        }
        break;
    }
    LOG_VERBOSE(BSL_LS_SYS_TX,
                (BSL_META_U(tp->unit,
                            "Send: len=%"PRIu32"(%d)\n"),
                 len, bytes));

exit:
    SHR_FUNC_EXIT();
}

static void
bcmlu_tpacket_rx_thread(shr_thread_t tc, void *p)
{
    bcmlu_tpacket_t *tp = (bcmlu_tpacket_t *)p;

    if (tp == NULL) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL pointer\n")));
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(tp->unit, "Enter RX thread\n")));
    tp->read(tp);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit,
                            "RX thread closed (network interface %d)\n"),
                 tp->netif.id));
}

/*
 * TX thread for batch send (v1 & v2).
 * This feature's implementation leaves for future depends on requirement.
 */
static void
bcmlu_tpacket_tx_thread(shr_thread_t tc, void *p)
{
    bcmlu_tpacket_t *tp = (bcmlu_tpacket_t *)p;

    if (tp == NULL) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("NULL pointer\n")));
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(tp->unit, "Enter TX thread\n")));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(tp->unit,
                            "TX thread closed (unit %d network interface %d)\n"),
                 tp->unit, tp->netif.id));
}

static int
bcmlu_tpacket_created(int unit, int netif_id, bool *created)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(created, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    *created = (tpacket_handles[unit][netif_id] != NULL);

exit:
    SHR_FUNC_EXIT();
}

static bcmlu_tpacket_t *
bcmlu_tpacket_handle_get(int unit, int netif_id)
{
    return tpacket_handles[unit][netif_id];
}

/* Create TP on a netif. */
static int
bcmlu_tpacket_setup(bcmlu_tpacket_t *tp)
{
    char name[128];
    bcmlu_tpacket_ring_t *ring;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(tp, SHR_E_PARAM);
    SHR_NULL_CHECK(tp->priv, SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    if (tp->rx_buf_size <= 0 ||
        tp->tx_buf_size <= 0 ||
        tp->netif.max_frame_size < 64) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(tp->unit,
                         "rx_buf_size=%"PRIu32" tx_buf_size=%"PRIu32" max_frame_size=%"PRIu32"\n"),
              tp->rx_buf_size, tp->tx_buf_size, tp->netif.max_frame_size));
    }

    /* Create socket and bind to netif. */
    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_socket_create(tp));

    /* Configure TP version version. */
    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_version_select(tp));

    /* Create ring with mmap. */
    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_ring_create(tp));

    /* Hung up access functions. */
    tp->send = bcmlu_tpacket_send;
    switch (ring->tp_version) {
    case TPACKET_V1:
        tp->read = bcmlu_tpacket_read_v1;
        if (!tp->tx_immediate) {
            tp->send = bcmlu_tpacket_send_v1;
        }
        break;
    case TPACKET_V1_64:
        tp->read = bcmlu_tpacket_read_v1_64;
        if (!tp->tx_immediate) {
            tp->send = bcmlu_tpacket_send_v1_64;
        }
        break;
#ifdef HAVE_TPACKET_V2
    case TPACKET_V2:
        tp->read = bcmlu_tpacket_read_v2;
        if (!tp->tx_immediate) {
            tp->send = bcmlu_tpacket_send_v2;
        }
        break;
#endif
#ifdef HAVE_TPACKET_V3
    case TPACKET_V3:
        tp->read = bcmlu_tpacket_read_v3;
        break;
#endif
    }

    sal_sprintf(name, "Bcmpkt_tp_rx_%s", tp->netif.name);
    tp->rx_tc = shr_thread_start(name, -1, bcmlu_tpacket_rx_thread,
                                 (void*)tp);
    if (!tp->rx_tc) {
        SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                            (BSL_META_U(tp->unit,
                                        "Could not start RX thread\n")));
    }

    /*
     * Currently not use tpacket TX.
     */
    if (!tp->tx_immediate) {
        sal_sprintf(name, "Bcmpkt_tp_tx_%s", tp->netif.name);
        tp->tx_tc = shr_thread_start(name, -1,
                                     bcmlu_tpacket_tx_thread, (void*)tp);
        if (!tp->tx_tc) {
            SHR_IF_ERR_MSG_EXIT(SHR_E_RESOURCE,
                                (BSL_META_U(tp->unit,
                                            "Could not start TX thread\n")));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_create(int unit, int netif_id, bcmpkt_socket_cfg_t *cfg)
{
    bcmlu_tpacket_t *tp = NULL;
    bcmlu_tpacket_ring_t *ring = NULL;
    bcmpkt_netif_t netif;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (bcmlu_tpacket_handle_get(unit, netif_id) != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "SOCKET exists on the network interface\n")));
    }

    SHR_IF_ERR_EXIT
        (bcmpkt_netif_get(unit, netif_id, &netif));

    /* RCPU encapsulation is requested for socket. */
    if (!(netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit,
                         "The network interface should support RCPU header\n")));
    }

    SHR_ALLOC(tp, sizeof(*tp), "bcmluTpktCreate");
    SHR_NULL_CHECK(tp, SHR_E_MEMORY);

    SHR_ALLOC(ring, sizeof(*ring), "bcmluTpktCreate");
    SHR_NULL_CHECK(ring, SHR_E_MEMORY);

    sal_memset(tp, 0, sizeof(*tp));
    sal_memset(ring, 0, sizeof(*ring));
    tp->priv = ring;

    tp->unit = unit;
    /* Init to invalid value. */
    tp->fd = -1;
    tp->rx_buf_size     =
        (!cfg->rx_buf_size) ? TP_BUF_SIZE_DEFAULT_RX : cfg->rx_buf_size;
    tp->tx_buf_size     =
        (!cfg->tx_buf_size) ? TP_BUF_SIZE_DEFAULT_TX : cfg->tx_buf_size;
    tp->rx_immediate    = RX_IMMEDIATE;
    tp->tx_immediate    = TX_IMMEDIATE;
    tp->poll_timeout    = cfg->rx_poll_timeout;
    tp->rx_block_timeout = RX_BLOCK_TIMEOUT;
    tp->tx_block_timeout = TX_BLOCK_TIMEOUT;
    sal_memcpy(&tp->netif, &netif, sizeof(netif));

    SHR_IF_ERR_EXIT
        (bcmlu_tpacket_setup(tp));

    tpacket_handles[unit][netif.id] = tp;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(tp);
        SHR_FREE(ring);
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_destroy(int unit, int netif_id)
{
    bcmlu_tpacket_t *tp;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Not found Tpacket on network interface %d\n"),
                     netif_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (tp->rx_tc) {
        tp->rx_break = true;
        rv = shr_thread_stop(tp->rx_tc, 100000);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Tpacket RX thread did not exit\n")));
        }
        tp->rx_tc = NULL;
        tp->rx_break = false;
    }

    bcmlu_tpacket_ring_destroy(tp);
    bcmlu_tpacket_socket_destroy(tp);

    SHR_FREE(tp->priv);
    SHR_FREE(tp->cb_info);
    SHR_FREE(tp);

    /* Set to NULL after free. */
    tpacket_handles[unit][netif_id] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_rx_register(int unit, int netif_id, uint32_t flags,
                          bcmpkt_rx_cb_f cb_func, void *cb_data)
{
    bcmlu_tpacket_t *tp;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Network interface %d does not support Tpacket\n"),
              netif_id));
    }
    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    if (tp->cb_info != NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_EXISTS,
             (BSL_META_U(unit, "Overwriting callback function is prohibit\n")));
    }
    SHR_ALLOC(tp->cb_info, sizeof(bcmpkt_rx_cb_info_t), "bcmluTpktRxReg");
    SHR_NULL_CHECK(tp->cb_info, SHR_E_MEMORY);

    tp->cb_info->flags   = flags;
    tp->cb_info->cb_func = cb_func;
    tp->cb_info->cb_data = cb_data;
    tp->cb_info->cb_pending = FALSE;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                            void *cb_data)
{
    bcmlu_tpacket_t *tp;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Network interface %d does not support Tpacket\n"),
              netif_id));
    }

    if (tp->cb_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback not registered\n")));
        SHR_EXIT();
    }

    if (tp->cb_info->cb_func != cb_func) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "RX callback does not match\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tp->cb_info->cb_pending = TRUE;
    sal_usleep(100000);
    SHR_FREE(tp->cb_info);
    LOG_VERBOSE(BSL_LS_SYS_RX,
                (BSL_META_U(unit, "Unregister succeed\n")));

exit:
    SHR_FUNC_EXIT();
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
        SHR_IF_ERR_EXIT(bcmpkt_hg3_base_t_field_get(variant, packet->pmd.higig,
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
        /* Encapsulate Higig header. Continue only for HG/HG2 hdr
         * insertion since this step is not valid for HG3
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
bcmlu_tpacket_tx(int unit, int netif_id, bcmpkt_packet_t *packet)
{
    bcmlu_tpacket_t *tp;
    bcmlu_tpacket_ring_t *ring = NULL;
    uint32_t data_len, pktsize_min;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_BADID,
             (BSL_META_U(unit, "Network interface %d does not support Tpacket\n"),
              netif_id));
    }
    SHR_NULL_CHECK(packet, SHR_E_PARAM);
    SHR_NULL_CHECK(packet->data_buf, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMPKT_PACKET_DATA(packet), SHR_E_PARAM);

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    data_len = BCMPKT_PACKET_LEN(packet);
    pktsize_min = bcmpkt_framesize_min_get();
    if (data_len > tp->netif.max_frame_size ||
        data_len < pktsize_min) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_PARAM,
             (BSL_META_U(unit,
                         "Packet size %"PRIu32" is out of valid range "
                         "[%d, %"PRIu32"]\n"),
              BCMPKT_PACKET_LEN(packet), pktsize_min,
              tp->netif.max_frame_size));
    }

    if (packet->type == BCMPKT_FWD_T_NORMAL &&
        (tp->netif.flags & BCMPKT_NETIF_F_RCPU_ENCAP)) {
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
        (tp->send(tp, BCMPKT_PACKET_DATA(packet), BCMPKT_PACKET_LEN(packet)));

exit:
    if (ring != NULL) {
        if (SHR_FUNC_ERR()) {
            ring->tx_drops++;
        } else {
            ring->tx_packets++;
            ring->tx_bytes += data_len;
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_stats_dump(int unit, int netif_id, shr_pb_t *pb)
{
    bcmlu_tpacket_t *tp;
    bcmlu_tpacket_ring_t *ring;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMLU_NETIF_MAX_IDS) {
            tp = bcmlu_tpacket_handle_get(unit, id++);
            if (tp == NULL || tp->priv == NULL) {
                continue;
            }

            ring = (bcmlu_tpacket_ring_t *)tp->priv;
            ring->rx_drops = ring->pkt_buf_err +
                             ring->pkt_len_err +
                             ring->non_rcpu_pkt +
                             ring->rcpu_hdr_err +
                             ring->rcpu_len_err;

            shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tRX dropped %"PRIu32"\n"
                          "\t    Packet data buf fail %"PRIu32"\n"
                          "\t    Packet length check fail %"PRIu32"\n"
                          "\t    Non RCPU packet %"PRIu32"\n"
                          "\t    RCPU header check fail %"PRIu32"\n"
                          "\t    RCPU length check fail %"PRIu32"\n"
                          "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                          "\tTX dropped %"PRIu32"\n",
                          ring->rx_packets, ring->rx_bytes, ring->rx_drops,
                          ring->pkt_buf_err, ring->pkt_len_err,
                          ring->non_rcpu_pkt, ring->rcpu_hdr_err,
                          ring->rcpu_len_err, ring->tx_packets,
                          ring->tx_bytes, ring->tx_drops);
        }
        SHR_EXIT();
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL || tp->priv == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Network interface %d does not support Tpacket\n"),
              netif_id));
    }

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    shr_pb_printf(pb, "\tRX received %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tRX dropped %"PRIu32"\n"
                  "\t    Packet data buf fail %"PRIu32"\n"
                  "\t    Packet length check fail %"PRIu32"\n"
                  "\t    Non RCPU packet %"PRIu32"\n"
                  "\t    RCPU header check fail %"PRIu32"\n"
                  "\t    RCPU length check fail %"PRIu32"\n"
                  "\tTX Transmit %"PRIu32" (%"PRIu32" bytes)\n"
                  "\tTX dropped %"PRIu32"\n",
                  ring->rx_packets, ring->rx_bytes, ring->rx_drops,
                  ring->pkt_buf_err, ring->pkt_len_err, ring->non_rcpu_pkt,
                  ring->rcpu_hdr_err, ring->rcpu_len_err, ring->tx_packets,
                  ring->tx_bytes, ring->tx_drops);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmlu_tpacket_stats_clear(int unit, int netif_id)
{
    bcmlu_tpacket_t *tp;
    bcmlu_tpacket_ring_t *ring;

    SHR_FUNC_ENTER(unit);
    if (!bcmdrd_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* The netif_id = -1 means for all TXRX stats.  */
    if (netif_id == -1) {
        int id = 0;
        while (id < BCMLU_NETIF_MAX_IDS) {
            tp = bcmlu_tpacket_handle_get(unit, id++);
            if (tp == NULL || tp->priv == NULL) {
                continue;
            }

            ring = (bcmlu_tpacket_ring_t *)tp->priv;

            ring->rx_packets = 0;
            ring->rx_bytes = 0;
            ring->rx_drops = 0;
            ring->tx_packets = 0;
            ring->tx_bytes = 0;
            ring->tx_drops = 0;
        }
        SHR_EXIT();
    }

    if (!BCMLU_NETIF_VALID(netif_id)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    tp = bcmlu_tpacket_handle_get(unit, netif_id);
    if (tp == NULL || tp->priv == NULL) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_UNAVAIL,
             (BSL_META_U(unit, "Network interface %d does not support Tpacket\n"),
              netif_id));
    }

    ring = (bcmlu_tpacket_ring_t *)tp->priv;

    ring->rx_packets = 0;
    ring->rx_bytes = 0;
    ring->rx_drops = 0;
    ring->tx_packets = 0;
    ring->tx_bytes = 0;
    ring->tx_drops = 0;

exit:
    SHR_FUNC_EXIT();
}

#endif /*! TPACKET_HDRLEN */

int
bcmlu_tpacket_attach(void)
{
#ifdef TPACKET_HDRLEN
    bcmpkt_socket_t socket_vect = {
        .initialized = 1,
        .driver_name = "TPACKET",
        .driver_type = BCMPKT_SOCKET_DRV_T_TPKT,
        .create = bcmlu_tpacket_create,
        .destroy = bcmlu_tpacket_destroy,
        .stats_dump = bcmlu_tpacket_stats_dump,
        .stats_clear = bcmlu_tpacket_stats_clear,
        .created = bcmlu_tpacket_created
    };
    bcmpkt_net_t net_vect = {
        .initialized = 1,
        .driver_name = "TPACKET",
        .driver_type = BCMPKT_NET_DRV_T_TPKT,
        .rx_register = bcmlu_tpacket_rx_register,
        .rx_unregister = bcmlu_tpacket_rx_unregister,
        .tx = bcmlu_tpacket_tx
    };

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_socket_drv_register(&socket_vect), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpkt_net_drv_register(&net_vect), SHR_E_EXISTS);

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

int
bcmlu_tpacket_detach(void)
{
#ifdef TPACKET_HDRLEN
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmpkt_net_drv_unregister(BCMPKT_NET_DRV_T_TPKT));

    SHR_IF_ERR_EXIT
        (bcmpkt_socket_drv_unregister(BCMPKT_SOCKET_DRV_T_TPKT));

exit:
    SHR_FUNC_EXIT();
#else
    return SHR_E_UNAVAIL;
#endif
}

