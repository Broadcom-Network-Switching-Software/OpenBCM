/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packet receive test that uses the BCM api.
 *
 * December, 2002:  Options added to support RX APIs.
 *    This also adds:
 *        interrupt/non-interrupt handling;
 *        packet stealing;
 *        if stealing, freeing packets with DPC (always with interrupt,
 *               optionally with non-interrupt) or immediately;
 *
 *    When stealing packets, the system will crash if the rate is
 *    high enough:  Over 4K pkts/sec on Mousse, and 6K pkts/sec on BMW.
 *    This does not appear to be related to DPC.
 *
 *    Current max rates for RX API, no packet stealing:
 *       BMW (8245) on 5690, interrupt:           11.6 K pkts/sec
 *       BMW, non-interrupt:                      11.6 K pkts/sec
 *       Mousse, non-interrupt:                   10.4 K pkts/sec
 *
 *    Update on max rates for RX API:
 */

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/stats.h>
#include <sal/appl/sal.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/util.h>
#include <soc/drv.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/shell.h>

#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/mcast.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm/field.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
#include <bcm/stack.h>
#include <bcm/vlan.h>
#if defined(INCLUDE_PKTIO)
#include <bcm/pktio.h>
#include <bcm/pktio_defs.h>
#endif

#include <bcm_int/common/rx.h>
#include <bcm_int/common/tx.h>
#if defined (BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif
#if defined (BCM_DNX_SUPPORT)
#include <src/appl/reference/dnx/appl_ref_rx_init.h>
#endif

#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
#include <bcm/knet.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/mman.h>
    
#include <sched.h>
#include <sal/core/thread.h>
#endif


#include "testlist.h"

#if defined(BCM_FIELD_SUPPORT) || defined(BCM_FE2000_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT)

typedef struct p_s {
    int                         p_init; /* TRUE --> initialized */

    /* parameters changed or watched by other threads -- must be volatile */
    volatile uint32             p_count_packets; /* Finished test - just drain */
    volatile uint32             p_received;
    volatile uint32             p_drained;
    volatile uint32             p_time_us;
    volatile uint32             p_error_num;

    /* internal parameters */
    int                         p_num_test_ports;
    int                         p_use_fp_action;
    int                         p_port[2]; /* Active port */
    int                         p_time; /* Duration (seconds) */
    bcm_port_info_t             p_port_info[2]; /* saved port config */
    int                         p_pkts_per_sec;
    int                         p_hw_rate;  /* use hw rate limiting */
    int                         p_max_q_len;
    int                         p_per_cos; /* Run tests with per-cos settings */
    int                         p_burst;
    uint32                      reg_flags;  /* Flags used for register */
    int                         p_intr_cb;
    int                         p_dump_rx;
#if defined(BCM_FIELD_SUPPORT)
    bcm_field_entry_t           p_field_entry[2];
#endif /* BCM_FIELD_SUPPORT */
    int                         p_l_start; /* Length start */
    int                         p_l_end; /* Length end */
    int                         p_l_inc; /* Length increment */
    int                         p_free_buffer;
    bcm_pkt_t                   *p_pkt; /* Tx - Packet buffer */
    bcm_rx_cfg_t                p_rx_cfg;
    int                         rx_mode;
    int                         p_txrx_unit;
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
    int                         p_use_socket;
    int                         p_sock;
    int                         p_netid;
    int                         p_filterid;
    sal_thread_t                p_threadid;
    int                         p_ringbuf_size;
    char *                      p_ringbuf;
    int                         p_use_socket_send;
#endif
    int                         hdr_chk; /* Flag to check EP_TO_CPU header on CMICx */
    int                         diff_len; /* Acceptable difference between s_len and r_len */
    int                         diff_len_chk; /* Flag to check difference between s_len and r_len */
#if defined(INCLUDE_PKTIO)
    uint8                       *p_data_buf;
#endif
} p_t;

static sal_mac_addr_t rp_mac_dest = {0x00, 0x11, 0x22, 0x33, 0x44, 0x99};
static sal_mac_addr_t rp_mac_src  = {0x00, 0x11, 0x22, 0x33, 0x44, 0x66};

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
int is_force_local = 0;
int is_force_fabric = 0;
static uint8 rp_ptch_header[2]  = {0xd0, 0x00};
static uint8 mypacket[] = {


/*
                          0x0,0x0,0x0,0x0,       0x0,0x1,0x0,0x0,
                          0x0,0x0,0x0,0xe2,      0x81,0x00,0x00,0x00,
                          
                          0x90,0x00,*/0x1,0x2, 




/*                          0x0,0x0,0x0,0x0,       0x0,0x1,0x0,0x0,
                          0x0,0x0,0x0,0xe2,      0x90,0x00,0x1,0x2,
                          
                          0x03,0x4,0x5,0x6, */     0x7,0x8,0x9,0xa,
                          0xb,0xc,0xd,0xe,       0xf,0x10,0x11,0x12,
                          
                          0x13,0x14,0x15,0x16,   0x17,0x18,0x19,0x1a,
                          0x1b,0x1c,0x1d,0x1e,   0x1f,0x20,0x21,0x22,
                          
                          0x23,0x24,0x25,0x26,   0x27,0x28,0x29,0x2a,
                          0x2b,0x2c,0x2d,0x2e,   0x2f,0x30,0x31,0x32,
                          
                          0x33,0x34,0x35,0x36,   0x37,0x38,0x39,0x3a,
                          0x3b,0x3c,0x3d,0x3e,   0x3f,0x40,0x41,0x42,

                          0x43,0x44,0x45,0x46,   0x47,0x48,0x49,0x4a,
                          0x4b,0x4c,0x4d,0x4e,   0x4f,0x50,0x51,0x52,


                          0x53,0x54,0x55,0x56,   0x57,0x58,0x59,0x5a,
                          0x5b,0x5c,0x5d,0x5e,   0x5f,0x60,0x61,0x62,

                          0x63,0x64,0x65,0x66,   0x67,0x68,0x69,0x6a,
                          0x6b,0x6c,0x6d,0x6e,   0x6f,0x70,0x71,0x72,

                          0x73,0x74,0x75,0x76,   0x77,0x78,0x79,0x7a,
                          0x7b,0x7c,0x7d,0x7e,   0x7f,0x80,0x81,0x82 };
#endif

#ifdef BCM_DNX_SUPPORT
static uint8 rp_mh_header[16]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif


static p_t *p_control[SOC_MAX_NUM_DEVICES];

#define RP_RECEIVER_PRIO                255    /* Priority of rx handler */
#define RP_MAX_PKT_LENGTH               2048
#define RP_RX_CHANNEL                   1      /* DMA channel */


#define RP_CHK(rv, f)                      \
   if (BCM_FAILURE((rv))) {                \
       cli_out("call to %s line %d failed:%d %s\n", #f, __LINE__,\
               (rv), bcm_errmsg((rv))); \
    }


#define RP_MAX_PPC SOC_DV_PKTS_MAX

#ifndef DNX_MODULE_HEADER_SIZE
/** Define length of Module Header */
#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
/** Size of module header on DNX devices, it should be written at the start of the packet */
#define DNX_MODULE_HEADER_SIZE         16
#else
/** No Module Header on ADAPTER and non DNX*/
#define DNX_MODULE_HEADER_SIZE         0
#endif
#endif

#ifdef BCM_DNX_SUPPORT
/** Define length PTCH-2 */
#define RP_DNX_HDR_PTCH_TYPE2_LEN       2
#define RP_DNX_MAC_ADDR_LEN             6

/* Helper macro for appending Module Header to ethernet packets */
#define RP_DNX_PKT_MH_HDR_SET(pkt, mh)  \
    sal_memcpy((pkt)->_pkt_data.data, (mh), DNX_MODULE_HEADER_SIZE)

/* Helper macro for appending PTCH2 header to ethernet packets with Module Header */
#define RP_DNX_PKT_PTCH_HDR_SET_WITH_MH(pkt, ptch)  \
    sal_memcpy((pkt)->_pkt_data.data + DNX_MODULE_HEADER_SIZE, (ptch), RP_DNX_HDR_PTCH_TYPE2_LEN)

/* Helper macro for setting DMAC to ethernet packets with Module Header and PTCH2 header */
#define RP_DNX_PKT_HDR_DMAC_SET_WITH_MH_PTCH(pkt, dmac)  \
    sal_memcpy((pkt)->_pkt_data.data + DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN, (dmac), RP_DNX_MAC_ADDR_LEN)

/* Helper macro for setting SMAC to ethernet packets with Module Header and PTCH2 header */
#define RP_DNX_PKT_HDR_SMAC_SET_WITH_MH_PTCH(pkt, smac)  \
    sal_memcpy((pkt)->_pkt_data.data + DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN + 6, (smac), RP_DNX_MAC_ADDR_LEN)

/* Helper macro for setting uint16 to specified position of packet val in network byte order */
#define RP_DNX_HTONS_CVT_SET(pkt, val, posn)  \
    do { \
         uint16 _tmp; \
         _tmp = soc_htons(val); \
         sal_memcpy((pkt)->_pkt_data.data + (posn), &_tmp, sizeof(uint16)); \
    } while (0)

/* Helper macro for setting TPID to ethernet packets with Module Header and PTCH header */
#define RP_DNX_PKT_HDR_TPID_SET_WITH_MH_PTCH(pkt, tpid)  \
    RP_DNX_HTONS_CVT_SET(pkt, tpid, DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN + RP_DNX_MAC_ADDR_LEN + RP_DNX_MAC_ADDR_LEN)

/* Helper macro for setting vlan tag to ethernet packets with Module Header and PTCH header */
#define RP_DNX_PKT_HDR_VTAG_CONTROL_SET_WITH_MH_PTCH(pkt, vtag)  \
    RP_DNX_HTONS_CVT_SET(pkt, vtag, DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN + RP_DNX_MAC_ADDR_LEN + RP_DNX_MAC_ADDR_LEN + sizeof(uint16))

/* Helper macro for single buffer Ethernet packet with PTCH header(not HiGig). */
#define RP_DNX_PKT_HDR_TAGGED_LEN_SET_WITH_MH_PTCH(pkt, len)  \
    RP_DNX_HTONS_CVT_SET(pkt, len, DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN + RP_DNX_MAC_ADDR_LEN + RP_DNX_MAC_ADDR_LEN + sizeof(uint32))
#endif

STATIC void 
debug_dump(int len, uint8 *data)
{
    int i;
    char tbuf[128];

    sal_memset(tbuf, 0, 128);
    for (i = 0; i< len; i++) {
           sal_sprintf(tbuf+(i % 16)*3, "%02x ", data[i]);
           if (((i+1) % 16) == 0) {
               cli_out("%s\n", tbuf);
               sal_memset(tbuf, 0, 128);
           }
    }
    cli_out("%s\n", tbuf);
}

STATIC void
packet_measure(p_t *p, int s_len, int r_len, uint8 *buf, int len_adj)
{
    static sal_usecs_t  time_start = 0, hdr_len = 0;
    sal_usecs_t  time_end = 0;

    /* keep accumulating stats */
    p->p_received++;
    /* one packet has been received; track stats */
    if (p->p_received == 1) {
        time_start = sal_time_usecs();
    } else {
        time_end = sal_time_usecs();
        /* again overkill to calculate on each packet, but
           the main test is going to make off with this value
           at any time, so we have to keep it up to date */
        p->p_time_us = (int)(time_end - time_start);
    }

    if (!p->hdr_chk) {
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(p->p_txrx_unit, soc_feature_cmicx)) {
            hdr_len = soc_cmicx_pktdma_header_size_get(p->p_txrx_unit);
        }
#endif
        p->hdr_chk = 1;
    }

    if (len_adj) {
        r_len -= hdr_len;
    }

    if (p->diff_len_chk && (abs(r_len - s_len) > p->diff_len)) {
        /* packet error */
        p->p_error_num++;
        if (p->p_error_num == 1) {
            cli_out("S:%d R:%d\n", s_len, r_len);
            debug_dump(96, buf);
        }
    }
}

#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))


#define RXRING_FRAME_SIZE  (2048)
#define MAX_RXRING_FRAMES  (1024)

static int idx = 0;


STATIC void
_set_thread_priority(int prio)
{
    struct sched_param param;
    param.sched_priority = prio;
    if (sched_setscheduler(0, SCHED_RR, &param)) {       
        cli_out("priority set: \n");
    }
}

STATIC int 
knet_stats_get(long unsigned int *ic, long unsigned int *tc)
{
    FILE* f;
    unsigned int tmp, int_cnt, pkt_cnt0, pkt_cnt1, pkt_cnt2;

    if ((f=fopen("/proc/bcm/knet/stats", "rw" )) == NULL) {
        *tc = 0;
        return -1;
    }
    (void)fscanf(f, "Device stats (unit %d):\n"\
                    "  Interrupts  %10u\n"\
                    "  Tx packets  %10u\n"\
                    "  Rx0 packets %10u\n"\
                    "  Rx1 packets %10u\n"\
                    "  Rx2 packets %10u\n"\
                    "  Rx0 pkts/intr %8u\n"\
                    "  Rx1 pkts/intr %8u\n"\
                    "  Rx2 pkts/intr %8u\n",
                    (int *)&tmp, &int_cnt, &tmp,
                    &pkt_cnt0, &pkt_cnt1, &pkt_cnt2,
                    &tmp, &tmp, &tmp);
    * ic = int_cnt;
    * tc = pkt_cnt0 + pkt_cnt1 + pkt_cnt2;
    fclose(f);
    
    return 0;
    
}

STATIC int 
knet_stats_clear(void)
{
    FILE* f;

    if ((f=fopen("/proc/bcm/knet/stats", "w+" )) == NULL) {
        cli_out("fail to open stats for writing\n");
        return -1;
    }
    fwrite("clear", 5, 1, f);
    fclose(f);
    return 0;
    
}

#ifdef FIX_IDX_ORDER

STATIC int 
fix_idx(int idx ,volatile p_t* p, int *len) 
{
    int max = p-> p_ringbuf_size;
    int i =  (idx+1) % max;
    
    while (i != idx) {
#ifdef PACKET_VERSION
        volatile struct tpacket2_hdr *head = (volatile struct tpacket2_hdr *)
            (p->p_ringbuf + i * RXRING_FRAME_SIZE);
#else 
        volatile struct tpacket_hdr *head = (volatile struct tpacket_hdr *)
            (p->p_ringbuf + i * RXRING_FRAME_SIZE);        
#endif 
        if (head->tp_status & TP_STATUS_USER) {
            * len = head->tp_len;
            head->tp_len = 0;
            head->tp_snaplen = 0;
            head->tp_status = TP_STATUS_KERNEL;
            
            return i;
        }
        i=(i+1) % max;
    }
    return i;
}
#endif

STATIC int 
sock_read_one_pkt (volatile p_t* p, uint8 ** pbuf)
{
    static char sock_buf[RXRING_FRAME_SIZE];
    int len = 0;

    if (p->p_ringbuf) {   /* ring buffer receive */                 

#ifdef PACKET_VERSION
        volatile struct tpacket2_hdr *head = (volatile struct tpacket2_hdr *)
            (p->p_ringbuf + idx * RXRING_FRAME_SIZE);
#else 
        volatile struct tpacket_hdr *head = (volatile struct tpacket_hdr *)
            (p->p_ringbuf + idx * RXRING_FRAME_SIZE);        
#endif
        
        * pbuf = (uint8 *)head;

        if (head->tp_status & TP_STATUS_USER) { /* it's our packet */
            len = head->tp_len;
            /* release the ringbuf entry back to the kernel */
            head->tp_len = 0;
            head->tp_snaplen = 0;
            head->tp_status = TP_STATUS_KERNEL;
        
            /* next packet */
            idx = (idx + 1) % (p->p_ringbuf_size);
        } else {
#ifdef FIX_IDX_ORDER
            int fixed_idx = fix_idx(idx, p, &len);
            if (fixed_idx !=  idx) {
                idx = (fixed_idx+1) %(p->p_ringbuf_size);
                
            } 
#endif
        }

    } else {  /* standard socket receive */
        * pbuf = (uint8 *)sock_buf;
        len = recvfrom(p->p_sock, sock_buf, RXRING_FRAME_SIZE, MSG_DONTWAIT, NULL, NULL);  
        if (len < 0) {
            len = 0;
        }
    }

    return len;
} /* sock_read_one_pkt*/


STATIC void 
socket_receive(void *arg)
{
    p_t *p = (p_t *)arg;
    int          len = 0;
    struct       pollfd pfd;    
    uint8 * buf;
   
    _set_thread_priority(50);
       
    if (p->p_sock <= 0) {
        cli_out("bad socket %d\n", p->p_sock);
        return;
    }

    sal_memset(&pfd, 0, sizeof(pfd));
    pfd.fd = p->p_sock;
    pfd.events = POLLIN /*| POLLERR*/;  
    pfd.revents = 0;

    while (TRUE) {
        pfd.revents = 0;
        /* wait for data to arrive*/
        len = poll(&pfd, 1, -1);

        if (len == -1) {
            if (errno != EINTR) {
                cli_out("poll error: errno %d\n", errno);
            }
            continue;   
        } else if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            cli_out("poll exception; revents 0x%x\n", pfd.revents);
            continue;
        } else if ((pfd.revents & POLLIN) == 0) {
            cli_out("poll without data; revents %x\n", pfd.revents);
            continue;
        }
  
        /* pull packet from socket until empty*/
        while ((len = sock_read_one_pkt (p, &buf)) > 0) {
            if (p->p_count_packets) {
                packet_measure(p, p->p_pkt->_pkt_data.len, len, buf, 0);
            } else {
                p->p_drained++;
            }
        } /* while packet len > 0 */

    } /* while processing packets */

    cli_out("socket_receive thread done.\n");
    /* spin forever, waiting to be killed */
    for (;;) {
    }

} /* socket_receive */


STATIC int 
bind_device(int sock, char* dev)
{
    int rv;
    struct sockaddr_ll addr;    
    struct ifreq req;
    
    sal_memset(&req, 0, sizeof(req));
    sal_strncpy(req.ifr_name, dev, sizeof(req.ifr_name));
    if ((rv = ioctl(sock, SIOCGIFINDEX, &req)) < 0) {
        cli_out("ioctl error %d\n", errno);
        return rv;
    }  

    if ((rv = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, 
                        (void *)&req, sizeof(req))) < 0) {
        cli_out("bind_device setsockopt error %d\n", errno); 
        return rv;
    }

    sal_memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(0x8100);
    addr.sll_ifindex = req.ifr_ifindex;
    addr.sll_hatype = 0;
    addr.sll_pkttype = 0;
    addr.sll_halen = 0;
    if ((rv = bind(sock, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        cli_out("bind error %d\n", errno);
        return rv;
    }
    return 0;
}

STATIC int
set_promisc_up(int sock,  char* dev)
{
    struct ifreq req;       
    sal_strncpy(req.ifr_name, dev, sizeof(req.ifr_name));
    
    if (ioctl(sock, SIOCGIFFLAGS, &req)==-1) {
        cli_out("ioctl error");
        return -1;
    }
    req.ifr_flags |= (IFF_PROMISC | IFF_UP | IFF_RUNNING);
    if (ioctl(sock, SIOCSIFFLAGS, &req)==-1) {
        cli_out("ioctl error");
        return -1;
    }
    return 0;
}
STATIC int
knetif_setup(int unit, p_t *p) 
{  
    int rv;
    bcm_knet_netif_t netif;
    bcm_knet_filter_t filter;
    struct tpacket_req  req;
    
    bcm_knet_netif_t_init(&netif);
    netif.type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
    sal_memcpy(netif.mac_addr, rp_mac_dest, 6);
    
    p->p_netid = 0;
    rv = bcm_knet_netif_create(unit, &netif);
    if (BCM_SUCCESS(rv)) {
        p->p_netid = netif.id;
    } else {
        cli_out("bcm_knet_netif_create failed: %d\n", rv);
        return rv;
    }
    
    p->p_filterid = 0;
    bcm_knet_filter_t_init(&filter);
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    if (p->p_use_socket) {
        filter.dest_type = BCM_KNET_DEST_T_NETIF;
        filter.dest_id = p->p_netid; 
    } else {
        filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
    }
    rv = bcm_knet_filter_create(unit, &filter);
    if (BCM_SUCCESS(rv)) {
        p->p_filterid = filter.id;
    } else {
        cli_out("bcm_knet_filter_create:%d\n", rv);
        return rv;
    }
    
    p->p_sock = socket(AF_PACKET, SOCK_RAW, htons(0x8100));

    set_promisc_up(p->p_sock, "bcm0");
    set_promisc_up(p->p_sock, netif.name);

        /* set up ring buffer, if needed */
    if ((p->p_use_socket) && (p->p_ringbuf_size > 0)) {
        int buf_size = p->p_ringbuf_size * RXRING_FRAME_SIZE; 
#ifdef PACKET_VERSION 
        int tpacket_version = TPACKET_V2;
        /* set tpacket hdr version. */
        if (-1 == setsockopt (p->p_sock, SOL_PACKET, PACKET_VERSION, 
            &tpacket_version, sizeof (int))) {
            cli_out("set tpacket version failure.\n");
        }
#endif        
        idx = 0;

        req.tp_block_size = buf_size;
        req.tp_block_nr = 1;
        req.tp_frame_size = RXRING_FRAME_SIZE;
        req.tp_frame_nr = p->p_ringbuf_size;
        
        if (-1 == setsockopt(p->p_sock, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req))) {
            cli_out("setsockopt PACKET_RX_RING error\n");  
        }
        p->p_ringbuf = (char*)mmap(0, buf_size, PROT_READ|PROT_WRITE, MAP_SHARED, p->p_sock, 0);
        if (MAP_FAILED == p->p_ringbuf) {
            cli_out("mmap error\n");
            p->p_ringbuf = NULL;
            p->p_ringbuf_size = 0;
        } else {
            memset(p->p_ringbuf, 0, buf_size);
        }
    }
    bind_device(p->p_sock, netif.name);
    if (p->p_use_socket) {     
        p->p_threadid = sal_thread_create("sock_rx", 8192, 50, socket_receive, p);
    }
    return rv;
}

STATIC void
knetif_clean(int unit, p_t *p) 
{
    if (p->p_threadid != NULL) {
        sal_thread_destroy(p->p_threadid);
        p->p_threadid = NULL;
    }
    
    if (p->p_ringbuf != NULL) {
        struct tpacket_req  req;
        int buf_size = p->p_ringbuf_size * RXRING_FRAME_SIZE;
        sal_memset(&req, 0, sizeof(req));
        setsockopt(p->p_sock, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req));
        munmap(p->p_ringbuf, buf_size);
        p->p_ringbuf = NULL;
    }
    if (p->p_sock > 0) {
        struct ifreq req;
        bcm_knet_netif_t netif;
        bcm_knet_netif_get(unit, p->p_netid, &netif);
       
        sal_strncpy(req.ifr_name, netif.name, sizeof(req.ifr_name));
        
        if (ioctl(p->p_sock, SIOCGIFFLAGS, &req)==-1) {
            cli_out("ioctl SIOCGIFFLAGS error");
        }
        req.ifr_flags &= (~(IFF_UP | IFF_RUNNING));
        if (ioctl(p->p_sock, SIOCSIFFLAGS, &req)==-1) {
            cli_out("ioctl SIOCSIFFLAGS error");
        }
        close(p->p_sock);
        p->p_sock = 0;
    }
    if (p->p_filterid > 0) {
        bcm_knet_filter_destroy(unit, p->p_filterid);
        p->p_filterid = 0;
    }
    
    if (p->p_netid > 0) {
        bcm_knet_netif_destroy(unit, p->p_netid);
        p->p_netid = 0;
    }
}

#endif

/*
 * The following function may be called in interrupt context.
 * Does no packet classification; assume we're to see all pkts.
 */

STATIC bcm_rx_t
rpacket_rx_receive(int unit, bcm_pkt_t *pkt, void *vp)
{
    p_t         *p = (p_t *)vp;
    int          send_len = p->p_pkt->_pkt_data.len;
    int          len = pkt->tot_len;
    if (!p->p_count_packets) {
        /*
         * Just drain packets.
         */
        p->p_drained++;
        return BCM_RX_HANDLED;
    }

    packet_measure(p, send_len, len, (uint8 *)pkt->pkt_data->data, 0);

    if (p->p_free_buffer) {
        if (p->p_intr_cb) { /* Queue in interrupt context */
            bcm_rx_free_enqueue(unit, pkt->_pkt_data.data);
        } else {
            bcm_rx_free(unit, pkt->alloc_ptr);
        }
        return BCM_RX_HANDLED_OWNED;
    }

    return BCM_RX_HANDLED;
}

#ifdef INCLUDE_PKTIO
STATIC bcm_pktio_rx_t
rpacket_pktio_receive(int unit, bcm_pktio_pkt_t *pkt, void *cookie)
{
    p_t         *p = (p_t *)cookie;
    static sal_usecs_t  time_start = 0;
    sal_usecs_t  time_end = 0;

    /* keep accumulating stats */
    p->p_received++;
    /* one packet has been received; track stats */
    if (p->p_received == 1) {
        time_start = sal_time_usecs();
    } else {
        time_end = sal_time_usecs();
        /* again overkill to calculate on each packet, but
           the main test is going to make off with this value
           at any time, so we have to keep it up to date */
        p->p_time_us = (int)(time_end - time_start);
    }

    return BCM_PKTIO_RX_HANDLED;
}
#endif /* INCLUDE_PKTIO */

STATIC INLINE int
rpacket_register(int unit, p_t *p)
{
    if (SOC_IS_ARAD(unit)) {
        return _bcm_common_rx_register(unit, "rpkt-rx", rpacket_rx_receive, RP_RECEIVER_PRIO, p, p->reg_flags);

    } else {
#ifdef INCLUDE_PKTIO
        if (soc_feature(unit, soc_feature_sdklt_pktio)) {
            return bcm_pktio_rx_register(unit, "rpkt-rx", rpacket_pktio_receive,
                                         RP_RECEIVER_PRIO, p, p->reg_flags);
        } else
#endif /* INCLUDE_PKTIO */
        {
            return bcm_rx_register(unit, "rpkt-rx", rpacket_rx_receive,
                                   RP_RECEIVER_PRIO, p, p->reg_flags);
        }
    }
}

STATIC INLINE int
rpacket_unregister(int unit, p_t *p)
{
    if (SOC_IS_ARAD(unit)) {
        return _bcm_common_rx_unregister(unit, rpacket_rx_receive, RP_RECEIVER_PRIO);
    } else {
#ifdef INCLUDE_PKTIO
        if (soc_feature(unit, soc_feature_sdklt_pktio)) {
            return bcm_pktio_rx_unregister(unit, rpacket_pktio_receive,
                                           RP_RECEIVER_PRIO);
        } else
#endif /* INCLUDE_PKTIO */
        {
            return bcm_rx_unregister(unit, rpacket_rx_receive,
                                     RP_RECEIVER_PRIO);
        }
    }
}

STATIC int
rpacket_receiver_activate(int unit, p_t *p)
{
    int rv;

    /* Set up common attributes first */
    if (bcm_rx_active(unit)) {
        cli_out("Stopping active RX.\n");
        rv = bcm_rx_stop(unit, NULL);
        if (!BCM_SUCCESS(rv)) {
            cli_out("Unable to stop RX: %s\n", bcm_errmsg(rv));
            return -1;
        }
    }

    if (!soc_feature(unit, soc_feature_packet_rate_limit)) {
        /* Only set the burst size if the unit doesn't have
         * CPU packet rate limiting feature in HW. Otherwise,
         * packets are dropped causing the test to fail.
         */
        if (SOC_IS_ARAD(unit) || SOC_IS_DNX(unit)) {
            rv = _bcm_common_rx_burst_set(unit, p->p_burst);
        } else {
            rv = bcm_rx_burst_set(unit, p->p_burst);
        }
        if (BCM_FAILURE(rv)) {
            cli_out("Unable to set RX burst limit: %s\n",
                    bcm_errmsg(rv));
        }
    }

    if (p->p_hw_rate) { 
        if (p->p_per_cos) {
            if (!SOC_IS_KATANA(unit)) {
                bcm_rx_cos_rate_set(unit, 0, p->p_pkts_per_sec);
                bcm_rx_cos_burst_set(unit, 0, p->p_burst);
                p->p_rx_cfg.global_pps = 0;
                p->p_rx_cfg.max_burst = 0;
            }
        } else {
            rv = bcm_port_rate_egress_pps_set (unit, CMIC_PORT(unit), 
                              p->p_pkts_per_sec, p->p_burst);
        } 
    } else {
            bcm_rx_cos_rate_set(unit, BCM_RX_COS_ALL, 0);
            bcm_rx_cos_burst_set(unit, BCM_RX_COS_ALL, 0);
            p->p_rx_cfg.global_pps = p->p_pkts_per_sec;
            p->p_rx_cfg.max_burst = p->p_burst;
    }  
    if (p->p_max_q_len >= 0) {
        cli_out("Setting MAX Q length to %d\n", p->p_max_q_len);
        bcm_rx_cos_max_len_set(unit, BCM_RX_COS_ALL, p->p_max_q_len);
    }
    if (SOC_IS_ARAD(unit)) {
        rv = _bcm_common_rx_start(unit, &p->p_rx_cfg  );
    } else {
        rv = bcm_rx_start(unit, &p->p_rx_cfg  );
    }
    if (!BCM_SUCCESS(rv)) {
        cli_out("Unable to Start RX: %s\n", bcm_errmsg(rv));
        return -1;
    }
    p->reg_flags = BCM_RCO_F_ALL_COS +
        (p->p_intr_cb ? BCM_RCO_F_INTR : 0);

    return 0;
}

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
static int
rp_dune_port_enable_set (int unit, p_t *p, int enable)
{
    bcm_port_config_t config;
    pbmp_t      tmp_pbmp;
    int         port_idx;
    soc_port_t  port;
    int         rv = BCM_E_NONE;

    /** Disable all other ports */
    BCM_PBMP_ASSIGN(tmp_pbmp, PBMP_E_ALL(unit));
    /** Remove CPU port*/
    BCM_PBMP_REMOVE(tmp_pbmp, PBMP_CMIC(unit));

    /** Remove special ports that are not included in SOC PBMPCPU */
    rv = bcm_port_config_get(unit, &config);
    RP_CHK(rv, bcm_port_config_get);
    if (SOC_IS_DNX(unit))
    {
        BCM_PBMP_REMOVE(tmp_pbmp, config.erp);
        BCM_PBMP_REMOVE(tmp_pbmp, config.oamp);
        BCM_PBMP_REMOVE(tmp_pbmp, config.olp);
        BCM_PBMP_REMOVE(tmp_pbmp, config.eventor);
        BCM_PBMP_REMOVE(tmp_pbmp, config.sat);
        BCM_PBMP_REMOVE(tmp_pbmp, config.rcy_mirror);
    }
    else if (SOC_IS_ARAD(unit))
    {
        BCM_PBMP_REMOVE(tmp_pbmp, config.sat);
        BCM_PBMP_REMOVE(tmp_pbmp, config.ipsec);
    }

    /** Remove loopback ports */
    for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
        BCM_PBMP_PORT_REMOVE(tmp_pbmp, p->p_port[port_idx]);
    }

    PBMP_ITER(tmp_pbmp, port) {
        rv = bcm_port_enable_set(unit, port, enable);
        RP_CHK(rv, bcm_port_enable_set);
    }

    return rv;
}
static int
rp_dune_get_ssp_and_cpu_channel(int unit, bcm_pkt_t *pkt, p_t *p)
{
    int rv = BCM_E_NONE;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t port_ndx;
    bcm_port_t ptch_ssp = 0;
    bcm_core_t ptch_ssp_core = 0;

    /** Get corresponding core of PTCH.SSP */
    rv = bcm_port_get(unit, p->p_port[0], &flags, &interface_info, &mapping_info);
    RP_CHK(rv, bcm_port_get);

    ptch_ssp = mapping_info.pp_port;
    ptch_ssp_core = mapping_info.core;

    /** Always set BCM_PKT_F_HGHDR to indicate MH is present*/
    p->p_pkt->flags |= BCM_PKT_F_HGHDR;

    /** Get first CPU port with Same core as in TM port*/
    BCM_PBMP_ITER(PBMP_CMIC(unit), port_ndx)
    {
        rv = bcm_port_get(unit, port_ndx, &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE)
        {
            test_error(unit, "bcm_port_get failed: %s\n", bcm_errmsg(rv));
        }
        if (mapping_info.core == ptch_ssp_core)
        {
            p->p_pkt->_dpp_hdr[0] = (uint8)(mapping_info.channel & 0xff);
            break;
        }
    }

    pkt->_dpp_hdr[1] = 0;
    pkt->_dpp_hdr[2] = 0;
    pkt->_dpp_hdr[3] = 0;

    /* Set SSP to PTCH */
    rp_ptch_header[1] = ptch_ssp;

    return rv;
}

#endif

#ifdef INCLUDE_PKTIO
STATIC int
rpacket_pktio_setup(int unit, p_t *p)
{
    enet_hdr_t *enet_hdr;
    uint8 *data;
    uint8 *fill_addr;

    if (p->p_data_buf != NULL) {
        sal_free(p->p_data_buf);
        p->p_data_buf = NULL;
    }
    /* Allocate packet data buffer */
    data = sal_alloc(p->p_l_end, "rxtest_pktio");
    if (data == NULL) {
        test_error(unit, "Failed to allocate TX packet buffer\n");
        return -1;
    }
    p->p_data_buf = data;
    /* Fill in buffer according to request */
    enet_hdr = (enet_hdr_t *)data;
    ENET_SET_MACADDR(enet_hdr->en_dhost, rp_mac_dest);
    ENET_SET_MACADDR(enet_hdr->en_shost, rp_mac_src);
    enet_hdr->en_tag_tpid = soc_htons(ENET_DEFAULT_TPID);
    enet_hdr->en_tag_ctrl = soc_htons(VLAN_CTRL(0, 0, 1));

    fill_addr = (uint8 *)enet_hdr + 2 * sizeof(bcm_mac_t);
    fill_addr += sizeof(uint32);
    sal_memset(fill_addr, 0xFF,
               p->p_l_end - (fill_addr - (uint8 *)enet_hdr));

    return 0;
}
#endif

STATIC int
rpacket_setup(int unit, p_t *p)
{
    int         got_port;
    int         rv = BCM_E_UNAVAIL;
    uint8       *fill_addr;
    soc_port_t  port;
    int         port_idx;
    pbmp_t      gxe_pbm;
    pbmp_t      tmp_pbmp;
    int         speed = 0;
    int         txrx_modid = 0;

    p->p_txrx_unit = unit;
    p->p_use_fp_action = 0;
    p->p_num_test_ports = 1;

    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2(unit) || SOC_IS_HELIX4(unit)){
        p->p_use_fp_action = 1;
        p->p_num_test_ports = 2;
    }

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        p->p_num_test_ports = 2;
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        p->p_num_test_ports = 2;
    }
#endif

    /*
     * Pick a port, and redirect the packet back out that port with the
     * FFP. Single port redirection drops packets on ingress due to the
     * split horizon check
     */

    got_port = FALSE;

    BCM_PBMP_ASSIGN(gxe_pbm, PBMP_GE_ALL(unit));
    BCM_PBMP_OR(gxe_pbm, PBMP_XE_ALL(unit));
    BCM_PBMP_OR(gxe_pbm, PBMP_CE_ALL(unit));

    port_idx = 0;
    PBMP_ITER(gxe_pbm, port) {
        if (IS_ST_PORT(unit, port) || IS_PON_PORT(unit, port)) {
            continue;
        }
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            rv = bcm_port_linkscan_get(unit, port, &p->p_port_info[port_idx].linkscan);
            RP_CHK(rv, bcm_port_linkscan_get);
            if (BCM_SUCCESS(rv))
            {
                rv = bcm_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE);
                RP_CHK(rv, bcm_port_linkscan_set);
            }
            if (BCM_SUCCESS(rv))
            {
                rv = bcm_port_loopback_get(unit, port, &p->p_port_info[port_idx].loopback);
                RP_CHK(rv, bcm_port_loopback_get);
            }
        }
        else
#endif
        {
            rv = bcm_port_info_save(unit, port, &p->p_port_info[port_idx]);
            RP_CHK(rv, bcm_port_info_save);

            if (BCM_SUCCESS(rv)) {
                rv = bcm_port_linkscan_set(unit, port, BCM_LINKSCAN_MODE_NONE);
                RP_CHK(rv, bcm_port_linkscan_set);
            }
#ifdef BCM_PETRA_SUPPORT
            if ((SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) || (!SOC_IS_ARAD(unit)))
#endif
            {
                if (BCM_SUCCESS(rv))
                {
                    rv = bcm_port_speed_max(unit, port, &speed);
                    RP_CHK(rv, bcm_port_speed_max);
                }
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit) && IS_CD_PORT(unit, port) &&
                                             BCM_SUCCESS(rv))
                {
                    bcm_port_resource_t pr;
                    BCM_IF_ERROR_RETURN(bcm_port_resource_get(unit, port, &pr));
                    if (pr.speed != speed) {
                        LOG_ERROR(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                 "port speed (%d) does NOT match "
                                 " current speed (%d).\n"),
                                 pr.speed, speed));
                    }
                    pr.speed = speed;
                    BCM_IF_ERROR_RETURN(bcm_port_resource_speed_set(unit, port, &pr));
                }
                else
#endif
                {
                    if (BCM_SUCCESS(rv))
                    {
                        rv = bcm_port_speed_set(unit, port, speed);
                        RP_CHK(rv, bcm_port_speed_set);
                    }
                }
            }

            if (BCM_SUCCESS(rv)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_FE2000_SUPPORT)
               if (SOC_IS_HELIX4(unit)) {
                   rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
               } else {
                   rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC);
               }
#endif /* BCM_ESW_SUPPORT  || defined(BCM_FE2000_SUPPORT) */
               RP_CHK(rv, bcm_port_loopback_set);
            }
            if (BCM_SUCCESS(rv)) {
                rv = bcm_port_pause_set(unit, port, 0, 0);
                RP_CHK(rv, bcm_port_pause_set);
            }
        }

        if ((got_port = BCM_SUCCESS(rv))) {
            p->p_port[port_idx++] = port;

            if (port_idx >= p->p_num_test_ports) {
                break;
            }
            got_port = BCM_E_NOT_FOUND;
        }
    }

    if (!got_port) {
        test_error(unit, "Unable to find suitable XE/GE port.\n");
        return -1;
    }

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    if (SOC_IS_ARAD(unit) || SOC_IS_DNX(unit))
    {
        /* Disable all other ports */
        rv = rp_dune_port_enable_set(unit, p, FALSE);
        RP_CHK(rv, rp_dune_port_enable_set);
    }
    else
#endif
    {
        /* Disable all other ports (can run with active links plugged in) */
        BCM_PBMP_ASSIGN(tmp_pbmp, PBMP_E_ALL(unit));

        /* Remove CPU port, it's not real ports for phy driver */
        BCM_PBMP_REMOVE(tmp_pbmp, PBMP_CMIC(unit));
        for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
            BCM_PBMP_PORT_REMOVE(tmp_pbmp, p->p_port[port_idx]);
        }

        PBMP_ITER(tmp_pbmp, port) {
            rv = bcm_port_enable_set(unit, port, FALSE);
            RP_CHK(rv, bcm_port_enable_set);
        }
    }

#if defined(BCM_FIELD_SUPPORT)
    if (soc_feature(unit, soc_feature_field)) {
        if (SOC_IS_ESW(unit)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_FE2000_SUPPORT)
            bcm_field_qset_t        qset;
            bcm_field_group_t       fg;
            bcm_field_entry_t       fent[2];
            int                     stat_id = -1;
            bcm_field_stat_t        stat_type = bcmFieldStatPackets;

            BCM_FIELD_QSET_INIT(qset);
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

            rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &fg);

            for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                if (BCM_SUCCESS(rv)) {
                    rv = bcm_field_entry_create(unit, fg, &fent[port_idx]);
                }

                if (BCM_SUCCESS(rv)) {
                    p->p_field_entry[port_idx] = fent[port_idx];

                    rv = bcm_field_qualify_InPort(unit, fent[port_idx],
                                                  p->p_port[port_idx],
                                                  BCM_FIELD_EXACT_MATCH_MASK);
                }
            }

            if (BCM_SUCCESS(rv)) {
                for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                    rv = bcm_field_stat_create(unit, fg, 1, &stat_type, &stat_id);
                    RP_CHK(rv, bcm_field_stat_create);

                    rv = bcm_field_entry_stat_attach(unit, fent[port_idx], stat_id);
                    RP_CHK(rv, bcm_field_entry_stat_attach);
                    stat_id = -1;
                }
            }

            for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                if (p->p_use_fp_action) {
                    if (port_idx == 1) {
                        if (BCM_SUCCESS(rv)) {
                            rv = bcm_field_action_add(unit, fent[port_idx], 
                                                      bcmFieldActionCopyToCpu,
                                                      0, 0);
                            RP_CHK(rv, bcm_field_action_add);
                        }
#ifdef BCM_TOMAHAWK_SUPPORT
                        if (SOC_IS_TOMAHAWKX(unit) && BCM_SUCCESS(rv)) {
                            rv = bcm_field_action_add(unit, fent[port_idx], 
                                                      bcmFieldActionDrop,
                                                      0, 0);
                            RP_CHK(rv, bcm_field_action_add);
                        }
#endif /* BCM_TOMAHAWK_SUPPORT */
                    }
                } else {
                    if (BCM_SUCCESS(rv)) {
                        rv = bcm_field_action_add(unit, fent[port_idx], 
                                                  bcmFieldActionCopyToCpu,
                                                  0, 0);
                        RP_CHK(rv, bcm_field_action_add);
                    }
                }
                if (BCM_SUCCESS(rv)) {

                    int port = p->p_port[port_idx];
                    if (p->p_use_fp_action) {
                        if (port_idx == 0) {
                            rv = bcm_field_action_add(unit, fent[port_idx], 
                                                  bcmFieldActionRedirect,
                                                  0, port);
                            RP_CHK(rv, bcm_field_action_add);

                            rv = bcm_field_action_add(unit, fent[port_idx], 
                                                  bcmFieldActionMirrorEgress,
                                                  0, p->p_port[1]);
                            RP_CHK(rv, bcm_field_action_add);

                        }
                    } else {
                        rv = bcm_field_action_add(unit, fent[port_idx], 
                                                  bcmFieldActionRedirect,
                                                  0, port);
                        RP_CHK(rv, bcm_field_action_add);
                    }
                }
            }
#endif /* BCM_ESW_SUPPORT || BCM_FE2000_SUPPORT */
        } else {
        }
    }

#endif /* BCM_FIELD_SUPPORT */
    if (BCM_FAILURE(rv)) {
        test_error(unit, "Unable to configure filter: %s\n", bcm_errmsg(rv));
        return -1;
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
            /* coverity [returned_value] */
            rv = bcm_port_learn_set(unit, p->p_port[port_idx], BCM_PORT_LEARN_FWD);
            RP_CHK(rv, bcm_port_learn_set);
        }
    }
    else
#endif     
    {
        /* Disable learning on that port */
#ifdef BCM_PETRA_SUPPORT
        if ((SOC_IS_ARAD(unit) && SOC_DPP_PP_ENABLE(unit)) || (!SOC_IS_ARAD(unit)))
#endif
        {
            for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                /* coverity [returned_value] */
                rv = bcm_port_learn_set(unit, p->p_port[port_idx], BCM_PORT_LEARN_FWD);
                RP_CHK(rv, bcm_port_learn_set);
            }
        }
    }

#ifdef INCLUDE_PKTIO
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        return rpacket_pktio_setup(unit, p);
    }
#endif

    /* Create Maximum size packet requested */
    BCM_IF_ERROR_RETURN(bcm_pkt_alloc(p->p_txrx_unit , p->p_l_end, 0, &(p->p_pkt)));
    if (p->p_pkt == NULL) {
        test_error(unit, "Failed to allocate Tx packet\n");
        return -1;
    }

    p->p_pkt->flags = BCM_TX_CRC_REGEN;

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit))
    {
        int len = 0;
        rv = rp_dune_get_ssp_and_cpu_channel(unit, p->p_pkt, p);
        RP_CHK(rv, rp_dune_get_ssp_and_cpu_channel);

        p->p_pkt->flags |= BCM_TX_CRC_APPEND;

        /* Fill in buffer */
        SOC_DPP_PKT_PTCH_HDR_SET(p->p_pkt, rp_ptch_header);
        SOC_DPP_PKT_HDR_DMAC_SET_WITH_PTCH(p->p_pkt, rp_mac_dest);
        SOC_DPP_PKT_HDR_SMAC_SET_WITH_PTCH(p->p_pkt, rp_mac_src);
        SOC_DPP_PKT_HDR_TPID_SET_WITH_PTCH(p->p_pkt, ENET_DEFAULT_TPID);
        SOC_DPP_PKT_HDR_VTAG_CONTROL_SET_WITH_PTCH(p->p_pkt, VLAN_CTRL(0, 0, 1));
        /* Fill address starts at PTCH(2) + DMAC(6) + SMAC(6) + VTAG(4) */
        fill_addr = BCM_PKT_DMAC(p->p_pkt) + 2 + 6 + 6 + 4;

        if (sizeof(mypacket)  < (p->p_l_end - (fill_addr - BCM_PKT_DMAC(p->p_pkt))))
        {
            len = sizeof(mypacket);
        }
        else
        {
            len = p->p_l_end - (fill_addr - BCM_PKT_DMAC(p->p_pkt));
        }

        sal_memcpy(fill_addr, mypacket, len);
    }
    else
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        int len = 0;
        rv = rp_dune_get_ssp_and_cpu_channel(unit, p->p_pkt, p);
        RP_CHK(rv, rp_dune_get_ssp_and_cpu_channel);

        /** Disable visibility resume which lower down PPS/BPS */
        p->p_pkt->flags |= BCM_TX_NO_VISIBILITY_RESUME;

        /* Fill in buffer */
        RP_DNX_PKT_MH_HDR_SET(p->p_pkt, rp_mh_header);
        RP_DNX_PKT_PTCH_HDR_SET_WITH_MH(p->p_pkt, rp_ptch_header);
        RP_DNX_PKT_HDR_DMAC_SET_WITH_MH_PTCH(p->p_pkt, rp_mac_dest);
        RP_DNX_PKT_HDR_SMAC_SET_WITH_MH_PTCH(p->p_pkt, rp_mac_src);
        RP_DNX_PKT_HDR_TPID_SET_WITH_MH_PTCH(p->p_pkt, ENET_DEFAULT_TPID);
        RP_DNX_PKT_HDR_VTAG_CONTROL_SET_WITH_MH_PTCH(p->p_pkt, VLAN_CTRL(0, 0, 1));
        /* Fill address starts at MH(16) + PTCH(2) + DMAC(6) + SMAC(6) + VTAG(4) */
        fill_addr = BCM_PKT_DMAC(p->p_pkt) + DNX_MODULE_HEADER_SIZE + RP_DNX_HDR_PTCH_TYPE2_LEN + RP_DNX_MAC_ADDR_LEN + RP_DNX_MAC_ADDR_LEN + sizeof(uint32);
        if (sizeof(mypacket) < (p->p_l_end - (fill_addr - BCM_PKT_DMAC(p->p_pkt))))
        {
            len = sizeof(mypacket);
        }
        else
        {
            len = p->p_l_end - (fill_addr - BCM_PKT_DMAC(p->p_pkt));
        }
        sal_memcpy(fill_addr, mypacket, len);
    }
    else 
#endif
    {
        /* Fill in buffer */
        BCM_PKT_HDR_DMAC_SET(p->p_pkt, rp_mac_dest);
        BCM_PKT_HDR_SMAC_SET(p->p_pkt, rp_mac_src);
        BCM_PKT_HDR_TPID_SET(p->p_pkt, ENET_DEFAULT_TPID);
        BCM_PKT_HDR_VTAG_CONTROL_SET(p->p_pkt, VLAN_CTRL(0, 0, 1));
        /* Fill address starts 2 * MAC + TPID + VTAG */
        fill_addr = BCM_PKT_DMAC(p->p_pkt) + 6 + 6 + 2 + 2;
        sal_memset(fill_addr, 0xff, p->p_l_end -
                   (fill_addr - BCM_PKT_DMAC(p->p_pkt)));
    }

    /* Set the dest port for the packet 
     *  For all devices, the packet will target the first port
     */
    BCM_PKT_PORT_SET(p->p_pkt, p->p_port[0], FALSE, FALSE);
    p->p_pkt->dest_mod = txrx_modid;
    p->p_pkt->opcode = BCM_PKT_OPCODE_UC;

    /* Set up the proper configuration */
    rv = rpacket_receiver_activate(p->p_txrx_unit, p);
    if (!BCM_SUCCESS(rv)) {
        test_error(unit, "Could not setup receiver\n");
        return -1;
    }

    return 0;
}

#ifdef INCLUDE_PKTIO
static int
rpacket_pktio_tx(int unit, p_t *p, int l)
{
    bcm_pktio_pkt_t *pkt = NULL;
    uint8   *data;
    int rv;
    uint32 pkt_len;
    bcm_pktio_txpmd_t pmd = {0};

    pkt_len = l;

    rv = bcm_pktio_alloc(unit, pkt_len, BCM_PKTIO_BUF_F_TX, &pkt);
    if (BCM_FAILURE(rv)) {
        cli_out("Packet alloc failed.\n");
        return rv;
    }
    if (pkt == NULL) {
        cli_out("Packet alloc failed.\n");
        return -1;
    }
    rv = bcm_pktio_put(unit, pkt, pkt_len, (void *)&data);
    if (BCM_FAILURE(rv)) {
        goto exit;

    }
    rv = bcm_pktio_pkt_data_get(unit, pkt, (void *)&data, NULL);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }
    sal_memcpy(data, p->p_data_buf, l);

    pmd.tx_port = p->p_port[0];
    rv = bcm_pktio_pmd_set(unit, pkt, &pmd);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }
    /* Send it */
    rv = bcm_pktio_tx(unit, pkt);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

exit:
    if (pkt != NULL) {
        bcm_pktio_free(unit, pkt);
        pkt = NULL;
    }
    return rv;
}
#endif /* INCLUDE_PKTIO */

/*
 * Function:
 *      rpacket_storm_start
 * Purpose:
 *      Start packet storm in switch to serve as packet generator
 * Parameters:
 *      u - unit #.
 *      p - pointer to test parameters
 *      l - length of packet to send
 * Returns:
 *      0 on success, -1 on failure
 * Notes:
*/

int
rpacket_storm_start (int unit, p_t *p, int l)
{
    int rv = 0;
    int tx_pkt_idx;
    int tx_pkt_cnt = 1;

#if defined(BCM_FIELD_SUPPORT)
    if (soc_feature(unit, soc_feature_field)) {
        int port_idx = 0;
        if (SOC_IS_ESW(unit)) {
            for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                rv = bcm_field_entry_install(unit, p->p_field_entry[port_idx]);
                RP_CHK(rv, bcm_field_entry_install);
            }
        } else {
        }
    }
#endif

    if (BCM_FAILURE(rv)) {
        test_error(unit, "Unable to install filter: %s\n", bcm_errmsg(rv));
        return rv;
    }

#if defined(INCLUDE_PKTIO)
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        return rpacket_pktio_tx(unit, p, l);
    }
#endif
    /* determine num packets needed to test this architecture */

    /* set up test ports */

    /* Send packet(s) to kick off packet storm */
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit))
    {
        /** include 2 bytes PTCH */
        BCM_PKT_TX_LEN_SET(p->p_pkt, (l + 2));
    }
    else
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        /** exclude 16bytes MH and 2 bytes PTCH */
        BCM_PKT_TX_LEN_SET(p->p_pkt, (l + DNX_MODULE_HEADER_SIZE + 2));
    }
    else
#endif
    {
        BCM_PKT_TX_LEN_SET(p->p_pkt, l);
    }
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        int port_idx;
        SOC_DPP_PKT_HDR_TAGGED_LEN_SET_WITH_PTCH(p->p_pkt, l); /* Set length */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            uint32 reg_val = 0;
            READ_IPT_FORCE_LOCAL_OR_FABRICr(unit,  &reg_val);
            is_force_fabric = soc_reg_field_get(unit, IPT_FORCE_LOCAL_OR_FABRICr, reg_val, FORCE_FABRICf);
            is_force_local = soc_reg_field_get(unit, IPT_FORCE_LOCAL_OR_FABRICr, reg_val, FORCE_LOCALf);
            soc_reg_field_set(unit, IPT_FORCE_LOCAL_OR_FABRICr, &reg_val, FORCE_FABRICf, 0);
            soc_reg_field_set(unit, IPT_FORCE_LOCAL_OR_FABRICr, &reg_val, FORCE_LOCALf, 1);
            WRITE_IPT_FORCE_LOCAL_OR_FABRICr(unit,  reg_val);
        }
        /** Set Loopback to make packet storm  */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++)
        {
            rv = bcm_port_loopback_set(unit, p->p_port[port_idx], BCM_PORT_LOOPBACK_MAC);
            RP_CHK(rv, bcm_port_loopback_set);
        }
    }
    else 
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        int port_idx;
        /** Set Loopback to make packet storm  */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++)
        {
        rv = bcm_port_loopback_set(unit, p->p_port[port_idx], BCM_PORT_LOOPBACK_MAC);
        RP_CHK(rv, bcm_port_loopback_set);
        }
    }
    else
#endif
    {
        BCM_PKT_HDR_TAGGED_LEN_SET(p->p_pkt, l); /* Set length */
    }

    for (tx_pkt_idx = 0; tx_pkt_idx < tx_pkt_cnt; tx_pkt_idx++) {
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        if (SOC_KNET_MODE(unit) && p->p_use_socket_send) {
            rv = send (p->p_sock, p->p_pkt->_pkt_data.data, l, 0);
            if (rv < 0) { 
                cli_out("Send to socket %d returned len %d errno %d\n", 
                p->p_sock, rv, errno);
                rv = BCM_E_FAIL;
            }
        } else
#endif 
        {
            rv = bcm_tx(p->p_txrx_unit, p->p_pkt, NULL);
        }
    }

    return rv;
} /* rpacket_storm_start */

 /*
  * Function:
  *      rpacket_storm_stop
  * Purpose:
  *      Stop packet storm; clear out remaining packets
  * Parameters:
  *      u - unit #.
  *      p - pointer to test parameters
  * Returns:
  *      0 on success, -1 on failure
  * Notes:
  */
 int
 rpacket_storm_stop (int unit, p_t *p)
{
    int rv = 0;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    int port_index;
#endif

/*
 * Stop the packets from flowing.
 */
#if defined(BCM_FIELD_SUPPORT)
    if (soc_feature(unit, soc_feature_field)) {
        int port_idx = 0;
        if (SOC_IS_ESW(unit)) {
            for (port_idx=0; port_idx < p->p_num_test_ports; port_idx++) {
                rv = bcm_field_entry_remove(unit, p->p_field_entry[port_idx]);
                RP_CHK(rv, bcm_field_entry_remove);
            }
        } else {
        }
        }
#endif
    if (BCM_FAILURE(rv)) {
        test_error(unit, "Unable to remove filter: %s\n", bcm_errmsg(rv));
        return rv;
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        uint32 reg_val = 0;
        READ_IPT_FORCE_LOCAL_OR_FABRICr(unit,  &reg_val);
        soc_reg_field_set(unit, IPT_FORCE_LOCAL_OR_FABRICr, &reg_val, FORCE_FABRICf, is_force_fabric);
        soc_reg_field_set(unit, IPT_FORCE_LOCAL_OR_FABRICr, &reg_val, FORCE_LOCALf, is_force_local);
        WRITE_IPT_FORCE_LOCAL_OR_FABRICr(unit,  reg_val);
    }
    /** Clear Loopback to stop packet storm  */
    for (port_index = 0; port_index < p->p_num_test_ports; port_index++)
    {
        rv = bcm_port_loopback_set(unit, p->p_port[port_index], BCM_PORT_LOOPBACK_NONE);
        RP_CHK(rv, bcm_port_loopback_set);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        /** Clear Loopback to stop packet storm  */
        for (port_index = 0; port_index < p->p_num_test_ports; port_index++)
        {
            rv = bcm_port_loopback_set(unit, p->p_port[port_index], BCM_PORT_LOOPBACK_NONE);
            RP_CHK(rv, bcm_port_loopback_set);
        }
    }
#endif

    return rv;
} /* rpacket_storm_stop */

/*
 * Function:
 *      rpacket_test
 * Purpose:
 *      Test packet reception interface.
 * Parameters:
 *      u - unit #.
 *      a - pointer to arguments.
 *      pa - ignored cookie.
 * Returns:
 *      0 on success, -1 on failure
 * Notes:
 *      There remains an issue when stealing packets that the
 *      system will crash when the rate is high enough.  This has
 *      been experimentally determined to be between 4-5K pkts/sec
 *      on Mousse (8240) and 6-7K pkts/sec on BMW (8245).
 */

int
rpacket_test(int unit, args_t *a, void *pa)
{
    p_t *p = (p_t *)pa;
    int l, rv = BCM_E_UNAVAIL;
    int test_rv = 0;
    int use_socket  = 0;
    int ringbuf_size = 0;
    sal_cpu_stats_t  cpu_start;
    sal_cpu_stats_t  cpu_end;

#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__))) 
    long unsigned int knet_pkt=0;
    long unsigned int knet_ic =0;
#endif
    soc_control_t    *soc = SOC_CONTROL(unit);
    int desc_intr_count = 0;
    int chain_intr_count = 0;    
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__))) 
    if (SOC_KNET_MODE(unit)) {
        use_socket = p->p_use_socket;
        ringbuf_size = p->p_ringbuf_size ;
    }
#endif    

#ifdef INCLUDE_PKTIO
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        /* coverity[dead_error_condition] */
        cli_out("\n"
                "  Packet |          Rate           |  Total    |         | CPU %%             \n"
                "   Len   |   Pkt/s   |    MB/s     |  packets  |  Time   | Idle /user /kern  \n"
                " --------+-----------+-------------+-----------+---------+-------------------\n");

    } else
#endif
    {
        /* coverity[dead_error_condition] */
        cli_out("\n"
                "Rate: %d/%d (%s). %s %s. %s %d PPC. Packets%s freed.\n",
                p->p_pkts_per_sec,
                p->p_burst,
                p->p_hw_rate  ? "HW" : "SW",
                "IF:",
                use_socket ?
                (ringbuf_size ? "ring" : "socket") :
                "bcm_rx",
                (use_socket) ? "" : ((p->p_intr_cb) ? "Intr CB." : "Task CB."),
                p->p_rx_cfg.pkts_per_chain,
                p->p_free_buffer ? "" : " not"
                );
    #if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        if (SOC_KNET_MODE(unit)) {
            cli_out("\n"
                    "  Packet |          Rate           |  Total    |         | CPU %%             | Knet tot  Knet Rate |\n"
                    "   Len   |   Pkt/s   |    MB/s     |  packets  |  Time   | Idle /user /kern  |  packets | Pkt/s    |Interrupts\n"
                    " --------+-----------+-------------+-----------+---------+-------------------+----------+----------+----------\n");
        } else
    #endif
        {
            cli_out("\n"
                    "  Packet |          Rate           |  Total    |         | CPU %%             |    Interrupts\n"
                    "   Len   |   Pkt/s   |    MB/s     |  packets  |  Time   | Idle /user /kern  | Desc_Intr|Chain_Intr\n"
                    " --------+-----------+-------------+-----------+---------+-------------------+----------+----------\n");
        }
    }

    if (!use_socket) {
            /* Register the callback handler */
            rv = rpacket_register(p->p_txrx_unit, p);
            if (BCM_FAILURE(rv)) {
                test_error(unit, "Unable to register handler, %s\n",
                            bcm_errmsg(rv));
                test_rv = -1;
                goto done;
            }
    }

    for (l = p->p_l_start; l <= p->p_l_end; l += p->p_l_inc) {

        if (SOC_IS_KATANA(unit) && p->p_hw_rate)  {
            bcm_port_rate_egress_set(unit, CMIC_PORT(unit), 
                                      p->p_pkts_per_sec * l / 125, p->p_burst * l /125);
        }

        rv = rpacket_storm_start (unit, p, l);
        if (BCM_FAILURE(rv)) {
            test_error(unit, "Failed to start packet storm: %s\n", bcm_errmsg(rv));
            test_rv = -1;
            goto done;
        }

        /* Start clean */
        p->p_received = 0;
        p->p_drained = 0;
        p->p_time_us = 0;
        p->p_error_num = 0;

        /* measure rx packets over known interval*/
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        if (SOC_KNET_MODE(unit)) {
            knet_stats_clear();
        }
#endif
        sal_cpu_stats_get (&cpu_start);
        desc_intr_count = soc->stat.intr_desc;
        chain_intr_count =  soc->stat.intr_chain;
        p->p_count_packets = TRUE;
        sal_sleep(p->p_time);
        p->p_count_packets = FALSE;
        desc_intr_count = soc->stat.intr_desc - desc_intr_count;
        chain_intr_count = soc->stat.intr_chain - chain_intr_count;
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        if (SOC_KNET_MODE(unit)) {
            knet_stats_get(&knet_ic, &knet_pkt);
        }
#endif
        sal_cpu_stats_get (&cpu_end);

        rv = rpacket_storm_stop (unit, p);
        if (BCM_FAILURE(rv)) {
            test_error(unit, "Failed to stop packet storm: %s\n", bcm_errmsg(rv));
            test_rv = -1;
            goto done;
        }

        /* display test results */
        {
            int td, bps, pps, idle, user,kernel ;
            int idle_diff, user_diff, kernel_diff, total_diff;

            td = (p->p_time_us + 500UL) / 1000UL; /* round to ms */
            if (td == 0) { /* avoid divide by zero */
                td = 1;
            }
            if (p->p_received == 0) {
                pps = bps = 0;
            } else {
                pps = (1000UL * p->p_received) / td;
                bps = (p->p_received * l) / td;
            }

            COMPILER_64_SUB_64(cpu_end.total, cpu_start.total);
            COMPILER_64_TO_32_LO(total_diff, cpu_end.total);
            COMPILER_64_SUB_64(cpu_end.idle, cpu_start.idle);
            COMPILER_64_TO_32_LO(idle_diff,cpu_end.idle);
            COMPILER_64_SUB_64(cpu_end.user, cpu_start.user);
            COMPILER_64_TO_32_LO(user_diff,cpu_end.user);
            COMPILER_64_SUB_64(cpu_end.kernel, cpu_start.kernel);
            COMPILER_64_TO_32_LO(kernel_diff,cpu_end.kernel);

            if (total_diff != 0) {
                idle = idle_diff * 10000UL / total_diff;
                user = user_diff * 10000UL / total_diff;
                kernel = kernel_diff * 10000UL / total_diff;
            } else {
                idle = 0;
                user = 0;
                kernel = 0;
            }

#if INCLUDE_PKTIO
            if (soc_feature(unit, soc_feature_sdklt_pktio)) {
                cli_out("  %5u  | %8u  | %5u.%03u   | %8u  | %3u.%03u | %2u.%02u/%2u.%02u/%2u.%02u ",
                        l,
                        (int)pps,
                        (int)(bps / 1000), (int)(bps % 1000),
                        p->p_received,
                        td / 1000, td % 1000,
                        idle / 100, idle % 100,
                        user / 100, user % 100,
                        kernel / 100, kernel % 100);
            } else
#endif
            {
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
                if (SOC_KNET_MODE(unit)) {
                    cli_out("  %5u  | %8u  | %5u.%03u   | %8u  | %3u.%03u | %2u.%02u/%2u.%02u/%2u.%02u | %8lu | %8u | %8lu ",
                            l,
                            (int)pps,
                            (int)(bps / 1000), (int)(bps % 1000),
                            p->p_received,
                            td / 1000, td % 1000,
                            idle / 100, idle % 100,
                            user / 100, user % 100,
                            kernel / 100, kernel % 100,
                            knet_pkt, (td < p->p_time*1000) ? (int)(knet_pkt/p->p_time): (int)((knet_pkt * 1000) / td), knet_ic);
                } else
#endif
                {
                    cli_out("  %5u  | %8u  | %5u.%03u   | %8u  | %3u.%03u | %2u.%02u/%2u.%02u/%2u.%02u | %8u | %8u ",
                            l,
                            (int)pps,
                            (int)(bps / 1000), (int)(bps % 1000),
                            p->p_received,
                            td / 1000, td % 1000,
                            idle / 100, idle % 100,
                            user / 100, user % 100,
                            kernel / 100, kernel % 100,
                            desc_intr_count, chain_intr_count);
                }
            }
            if (p->p_error_num > 0) {
                cli_out(" e:%d \n", p->p_error_num);
            } else {
                cli_out("\n");
            }
        }
         /* Sleep a bit to let current backlog of packets drain */
        sal_sleep(2);

    }

    if (!use_socket) {
            /* Unregister the handler, let discard take over */
            rv = rpacket_unregister(p->p_txrx_unit, p);
            if (BCM_FAILURE(rv)) {
                test_error(unit, "Unable to unregister handler, %s\n",
                           bcm_errmsg(rv));
                test_rv = -1;
                goto done;
            }
    }

done:
    if (p->p_dump_rx) {
#ifdef  BROADCOM_DEBUG
        bcm_rx_show(unit);
#endif  /* BROADCOM_DEBUG */
    }
    return test_rv;
}


STATIC int
rpacket_receiver_deactivate(int unit, p_t *p)
{
#ifdef BCM_DNX_SUPPORT
    int rv;
    if(SOC_IS_DNX(unit)) {
    if (bcm_rx_active(unit)) {
        cli_out("Stopping active RX.\n");
        rv = bcm_rx_stop(unit, NULL);
        if (!BCM_SUCCESS(rv)) {
            cli_out("Unable to stop RX: %s\n", bcm_errmsg(rv));
            return -1;
        }
    }

    rv = appl_dnx_rx_init(unit);
    if (!BCM_SUCCESS(rv)) {
        cli_out("Unable to active RX: %s\n", bcm_errmsg(rv));
        return -1;
    }
    }
#endif
    return 0;
}


/*ARGSUSED*/
int
rpacket_done(int unit, void *pa)
/*
 * Function:    rpacket_done
 * Purpose:     Clean up after rpacket test.
 * Parameters:  unit - unit #
 *              pa - cookie (as returned by rpacket_init();
 * Returns:     0 - OK
 *              -1 - failed
 */
{
    p_t         *p = p_control[unit];
    int         rv;
    int         port_idx;

    if (p == NULL) {
        return 0;
    }

    if (p->p_pkt != NULL) {
        bcm_pkt_free(unit, p->p_pkt);
        p->p_pkt = NULL;
    }

#ifdef INCLUDE_PKTIO
    if (p->p_data_buf != NULL) {
        sal_free(p->p_data_buf);
        p->p_data_buf = NULL;
    }
#endif

    rv = rpacket_receiver_deactivate(p->p_txrx_unit, p);
    if (BCM_FAILURE(rv)) {
        test_error(unit, "Unable to deactivate receiver.\n");
        return -1;
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        /* Restore port */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++)
        {
            rv = bcm_port_linkscan_set(unit, p->p_port[port_idx], p->p_port_info[port_idx].linkscan);
            RP_CHK(rv, bcm_port_loopback_set);
            rv = bcm_port_loopback_set(unit, p->p_port[port_idx], p->p_port_info[port_idx].loopback);
            RP_CHK(rv, bcm_port_loopback_set);
        }

        /* Enable all other ports */
        rv = rp_dune_port_enable_set(unit, p, TRUE);
        RP_CHK(rv, rp_dune_port_enable_set);
    }
    else
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_JERICHO(unit))
    {
        int         ms;
        /* Restore port */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++) {
            /* If the Master mode is not changed, remove phy_master attributes from action_mask */
            ms = 0;
            if ((rv = bcm_port_master_get(unit, p->p_port[port_idx], &ms)) != BCM_E_NONE) {
                cli_out("rpacket_done: port %s: could not get port Master mode: %s\n",
                        SOC_PORT_NAME(unit, p->p_port[port_idx]),
                        bcm_errmsg(rv));
                return -1;
            }
            if (ms == p->p_port_info[port_idx].phy_master) {
               p->p_port_info[port_idx].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
            }

            rv = bcm_port_info_restore(unit, p->p_port[port_idx],
                                       &p->p_port_info[port_idx]);
            if (BCM_FAILURE(rv)) {
                test_error(unit, "Unable to restore port %d: %s\n",
                           p->p_port[port_idx], bcm_errmsg(rv));
                return -1;
            }
        }
        /* Enable all other ports */
        rv = rp_dune_port_enable_set(unit, p, TRUE);
        RP_CHK(rv, rp_dune_port_enable_set);
    }
    else if (SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        /* Restore port */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++)
        {
            rv = bcm_port_info_restore(unit, p->p_port[port_idx],
                                       &p->p_port_info[port_idx]);
            if (BCM_FAILURE(rv)) {
                test_error(unit, "Unable to restore port %d: %s\n",
                           p->p_port[port_idx], bcm_errmsg(rv));
                return -1;
            }
        }
        /* Enable all other ports */
        rv = rp_dune_port_enable_set(unit, p, TRUE);
        RP_CHK(rv, rp_dune_port_enable_set);
    }
    else
#endif
    {
        int         ms;
        /* Restore port */
        for (port_idx = 0; port_idx < p->p_num_test_ports; port_idx++) {
            /* If the Master mode is not changed, remove phy_master attributes from action_mask */
            ms = 0;
            if ((rv = bcm_port_master_get(unit, p->p_port[port_idx], &ms)) != BCM_E_NONE) {
                cli_out("rpacket_done: port %s: could not get port Master mode: %s\n",
                        SOC_PORT_NAME(unit, p->p_port[port_idx]),
                        bcm_errmsg(rv));
                return -1;
            }
            if (ms == p->p_port_info[port_idx].phy_master) {
               p->p_port_info[port_idx].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
            }

            rv = bcm_port_info_restore(unit, p->p_port[port_idx],
                                       &p->p_port_info[port_idx]);
            if (BCM_FAILURE(rv)) {
                test_error(unit, "Unable to restore port %d: %s\n",
                           p->p_port[port_idx], bcm_errmsg(rv));
                return -1;
            }
        }

    }

    /*
     * Try to remove filter, ignore error since we don't know how far
     * we got in initialization.
     */
#if defined(BCM_FIELD_SUPPORT)
    if (soc_feature(unit, soc_feature_field)) {
        int i = 0;
        if (SOC_IS_ESW(unit)) {
            for (i = 0; i < p->p_num_test_ports; i++) {
                rv = bcm_field_entry_remove(unit, p->p_field_entry[i]);
                RP_CHK(rv, bcm_field_entry_remove);
                rv = bcm_field_entry_destroy(unit, p->p_field_entry[i]);
                RP_CHK(rv, bcm_field_entry_destroy);
            }
        } else {
        }
    }
#endif


    /*
     * Don't free the p_control entry,
     * keep it around to save argument state
     */
     
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
    if (SOC_KNET_MODE(unit)) {
        knetif_clean(unit, p);
    }
#endif
    return 0;
}

char rpacket_usage[] = 
"Receive Packet Test Usage:\n"
#ifndef COMPILER_STRING_CONST_LIMIT
"  Time=<value>         : Specify the test duration for each packet length. (default=2)\n"
"  LengthStart=<value>  : Specify the start pakcet length. (default=64)\n"
"  LengthEnd=<value>    : Specify the end pakcet length. (default=1522)\n"
"  LengthInc=<value>    : Specify the increasing step of pakcet length. (default=64)\n"
"  FreeBuffer=<value>   : Indicate if packet buffer will be freed. (default=FALSE)\n"
"  QLen=<value>         : Specify Max number of packets permitted in cos queue. (default=200)\n"
"  PERCos=<value>       : Indicate if test is running with per-cos settings. (default=FALSE)\n"
"  Rate=<value>         : Specify packets per second for a cos queue. (default=0)\n"
"  Burst=<value>        : Specify packets to be received in a single burst. (default=100)\n"
"  HWrate=<value>       : Indicate if hw rate limiting is used. (default=FALSE)\n"
"  PktsPerChain=<value> : Specify number of packets per DMA chain. (default=16)\n"
"  Chains=<value>       : Specify number of chains (DVs) set up.. (default=4)\n"
"  useINTR=<value>      : Indicate if packets are received in interrupt context. (default=TRUE)\n"
"  DumpRX=<value>       : Indicate if to show packet rx setting information. (default=FALSE)\n"
"  RxMode=<value>       : Indicate if rx mode is set via flags (parse received packets). (default=FALSE)\n"
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
"  SOCKet=<value>       : Indicate if socket is used for packet rx. (default=FALSE)\n"
"  RingBuf=<value>      : Specify ring buffer size for socket. (default=0)\n"
"  SocketTx=<value>     : Indicate if socket is used for packet tx. (default=FALSE)\n"
#endif
#ifdef INCLUDE_PKTIO
"NOTE: In streamlined packet I/O driver mode, "
"only Time, LengthStart, LengthEnd and LengthInc are applicable.\n"
#endif
#endif
;

#ifdef INCLUDE_PKTIO
static int
rpacket_arg_parse(int unit, args_t *a, parse_table_t *pt, p_t *p)
{
    parse_table_init(unit, pt);
    parse_table_add(pt, "Time", PQ_DFL|PQ_INT, 0, &p->p_time, 0);
    parse_table_add(pt, "LengthStart", PQ_DFL|PQ_INT, 0, &p->p_l_start, 0);
    parse_table_add(pt, "LengthEnd", PQ_DFL|PQ_INT, 0, &p->p_l_end, 0);
    parse_table_add(pt, "LengthInc", PQ_DFL|PQ_INT, 0, &p->p_l_inc, 0);

    if (parse_arg_eq(a, pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit, "%s: Invalid option: %s\n",
                   ARG_CMD(a), ARG_CUR(a) ? ARG_CUR(a) : "*");
        cli_out("%s\n", rpacket_usage);
        parse_arg_eq_done(pt);
        return -1;
    }
    parse_arg_eq_done(pt);
    return 0;
}

#endif

/*
 * Function:
 *      rpacket_init
 * Purpose:
 *      Initialize the rpacket test.
 * Parameters:
 *      u - unit #
 *      a - pointer to args
 *      pa - Pointer to cookie
 * Returns:
 *      0 - success, -1 - failed.
 */

int
rpacket_init(int u, args_t *a, void **pa)
{
    p_t                 *p = p_control[u];
    parse_table_t       pt;


    if (p == NULL) {
        p = sal_alloc(sizeof(p_t), "rpacket");
        if (p == NULL) {
            test_error(u, "ERROR: cannot allocate memory\n");
            return -1;
        }
        sal_memset(p, 0, sizeof(p_t));
        p_control[u] = p;
    }

    if (!p->p_init) {                   /* Init defaults first time */
        p->hdr_chk = 0;
        p->diff_len = 4;
        p->diff_len_chk = 1;
        p->p_l_start = 64;
        p->p_l_end = 1522;
        p->p_l_inc = 64;
        p->p_time  = 2;

        p->p_pkts_per_sec = 0;
        p->p_max_q_len = -1;
        p->p_per_cos = FALSE;
        p->p_burst = 100;
        p->p_hw_rate = FALSE;

        p->p_intr_cb = TRUE;
        p->p_dump_rx = FALSE;
        p->p_count_packets = FALSE;
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        if (SOC_KNET_MODE(u)) {
            p->p_use_socket = FALSE;
            p->p_use_socket_send = FALSE;
        }
#endif
        /* Init the RX cfg here.  Not much exposed */
       p->p_rx_cfg.pkt_size = 8 * 1024;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
       p->p_rx_cfg.pkts_per_chain = 16; 
#endif /* BCM_ESW_SUPPORT */
#ifdef BCM_FE2000_SUPPORT
       p->p_rx_cfg.pkts_per_chain = 16;
#endif /* BCM_FE2000_SUPPORT */
       p->p_rx_cfg.global_pps = p->p_pkts_per_sec;
       p->p_rx_cfg.max_burst = p->p_burst;
       p->p_rx_cfg.chan_cfg[1].chains = 4;
       p->p_rx_cfg.chan_cfg[1].flags = 0;
       p->p_rx_cfg.chan_cfg[1].cos_bmp = 0xff;
       p->rx_mode = 0;
#ifdef INCLUDE_PKTIO
       p->p_data_buf = NULL;
#endif
       /* Not initializing alloc/free functions */
        p->p_init  = TRUE;
    }
#ifdef INCLUDE_PKTIO
    if (soc_feature(u, soc_feature_sdklt_pktio)) {
        rpacket_arg_parse(u, a, &pt, p);
    } else
#endif
    {
        parse_table_init(u, &pt);
        parse_table_add(&pt, "Time", PQ_DFL|PQ_INT, 0, &p->p_time, 0);
        parse_table_add(&pt, "LengthStart", PQ_DFL|PQ_INT, 0, &p->p_l_start, 0);
        parse_table_add(&pt, "LengthEnd", PQ_DFL|PQ_INT, 0, &p->p_l_end, 0);
        parse_table_add(&pt, "LengthInc", PQ_DFL|PQ_INT, 0, &p->p_l_inc, 0);
        parse_table_add(&pt, "FreeBuffer", PQ_DFL|PQ_BOOL, 0, &p->p_free_buffer,
                        0);

        parse_table_add(&pt, "QLen", PQ_DFL|PQ_INT, 0, &p->p_max_q_len, 0);
        parse_table_add(&pt, "PERCos", PQ_DFL|PQ_INT, 0, &p->p_per_cos, 0);
        parse_table_add(&pt, "Rate", PQ_DFL|PQ_INT, 0, &p->p_pkts_per_sec, 0);
        parse_table_add(&pt, "Burst", PQ_DFL|PQ_INT, 0, &p->p_burst, 0);
        parse_table_add(&pt, "HWrate", PQ_DFL|PQ_INT, 0, &p->p_hw_rate, 0);

        parse_table_add(&pt, "PktsPerChain", PQ_DFL|PQ_INT, 0,
                        &p->p_rx_cfg.pkts_per_chain, 0);

        parse_table_add(&pt, "Chains", PQ_DFL|PQ_INT, 0,
                        &p->p_rx_cfg.chan_cfg[1].chains, 0);
        parse_table_add(&pt, "useINTR", PQ_DFL|PQ_BOOL, 0, &p->p_intr_cb, 0);
        parse_table_add(&pt, "DumpRX", PQ_DFL|PQ_BOOL, 0, &p->p_dump_rx, 0);
        parse_table_add(&pt, "RxMode", PQ_DFL|PQ_INT, 0, &p->rx_mode, 0);
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
        parse_table_add(&pt, "SOCKet", PQ_DFL|PQ_INT, 0, &p->p_use_socket, 0);
        parse_table_add(&pt, "RingBuf", PQ_DFL|PQ_INT, 0, &p->p_ringbuf_size, 0);
        parse_table_add(&pt, "SocketTx", PQ_DFL|PQ_INT, 0, &p->p_use_socket_send, 0);
#endif

        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
            test_error(u, "%s: Invalid option: %s\n",
                       ARG_CMD(a), ARG_CUR(a) ? ARG_CUR(a) : "*");
            cli_out("%s\n", rpacket_usage);
            parse_arg_eq_done(&pt);
            return -1;
        }
        parse_arg_eq_done(&pt);
    }

    if (p->p_time < 1) {
        test_error(u, "%s: Invalid duration: %d (must be 1 <= time)\n",
                   ARG_CMD(a), p->p_time);
        return -1;
    }
    
    if (p->p_per_cos) {
        p->p_hw_rate = 1;
    }
    
    if (p->p_rx_cfg.pkts_per_chain > RP_MAX_PPC) {
        cli_out("Too many pkts/chain (%d).  Setting to max (%d)\n",
                p->p_rx_cfg.pkts_per_chain, RP_MAX_PPC);
        p->p_rx_cfg.pkts_per_chain = RP_MAX_PPC;
    }
    if (p->rx_mode) {
        p->p_rx_cfg.flags |= BCM_RX_F_PKT_UNPARSED; 
    }
#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__))) 
    if (SOC_KNET_MODE(u)) {
        if (!p->p_use_socket) {
            p->p_ringbuf_size = 0;
        }
        p->p_ringbuf = NULL;
        if (p->p_ringbuf_size > MAX_RXRING_FRAMES) {
            p->p_ringbuf_size = MAX_RXRING_FRAMES;
        }
        if (p->p_ringbuf_size > 0) {
            p->p_use_socket = TRUE;  /* ringbuf implies raw socket */
        } else {
            p->p_ringbuf_size = 0;
        }
    } else {
        if (p->p_use_socket) {
            cli_out("KNET not online, please insert the kernel module firstly.\n");
            return -1;
        }
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(u))
    {
        if (soc_feature(u, soc_feature_no_ep_to_cpu))
        {
            /** Set to 1 to avoid to check length of EP_TO_CPU header on DNX */
            p->hdr_chk = 1;
        }
        p->diff_len += DNX_MODULE_HEADER_SIZE;
        p->diff_len += 2;
        /** Bypass length check */
        p->diff_len_chk = 0;
    }

#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(u))
    {
        /** Set to 1 to avoid to check length of EP_TO_CPU header on DNX */
        p->hdr_chk = 1;
        p->diff_len += 2;
    }
#endif

    if (rpacket_setup(u, p) < 0) {
        (void)rpacket_done(u, p);
        return -1;
    }

    *pa = (void *)p;


#if (defined(INCLUDE_KNET) && defined(LINUX) && (!defined(__KERNEL__)))
    if (SOC_KNET_MODE(u)) {
        (void)knetif_setup(u, p);
    }
#endif
    return 0;
}

#endif /* BCM_FIELD_SUPPORT || BCM_FE2000_SUPPORT || BCM_PETRA_SUPPORT || BCM_DNX_SUPPORT */
