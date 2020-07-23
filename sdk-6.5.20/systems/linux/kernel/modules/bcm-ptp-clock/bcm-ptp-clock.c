/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Copyright 2017 Broadcom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation (the "GPL").
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 (GPLv2) for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 (GPLv2) along with this source code.
 */

/*
 * This module implements a Linux PTP Clock driver for Broadcom
 * XGS switch devices.
 *
 * For a list of supported module parameters, please see below.
 *   debug: Debug level (default 0)
 *   network_transport : Transport Type (default 0 - Raw)
 *   base_dev_name: Base device name (default ptp0, ptp1, etc.)
 *
 * - All the data structures and functions work on the physical port.
 *   For array indexing purposes, we use (phy_port - 1).
 */

#include <gmodule.h> /* Must be included first */
/* Module Information */
#define MODULE_MAJOR 125
#define MODULE_NAME "linux-bcm-ptp-clock"

MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("PTP Clock Driver for Broadcom XGS Switch");
MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
#include <linux-bde.h>
#include <kcom.h>
#include <bcm-knet.h>
#include <linux/time64.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>

#include <linux/random.h>
#include <linux/seq_file.h>
#include <linux/if_vlan.h>
#include <linux/ptp_clock_kernel.h>

/* Configuration Parameters */
static int debug;
LKM_MOD_PARAM(debug, "i", int, 0);
MODULE_PARM_DESC(debug,
        "Debug level (default 0)");

static int pci_cos;

static int network_transport;
LKM_MOD_PARAM(network_transport, "i", int, 0);
MODULE_PARM_DESC(network_transport,
        "Transport Type (default - Detect from packet)");

static char *base_dev_name = "ptp0";
LKM_MOD_PARAM(base_dev_name, "s", charp, 0);
MODULE_PARM_DESC(base_dev_name,
        "Base device name (default ptp0, ptp1, etc.)");

static int fw_core;
LKM_MOD_PARAM(fw_core, "i", int, 0);
MODULE_PARM_DESC(fw_core,
        "Firmware core (default 0)");

/* Debug levels */
#define DBG_LVL_VERB    0x1
#define DBG_LVL_WARN    0x2

#define DBG_VERB(_s)    do { if (debug & DBG_LVL_VERB) gprintk _s; } while (0)
#define DBG_WARN(_s)    do { if (debug & DBG_LVL_WARN) gprintk _s; } while (0)
#define DBG_ERR(_s)     do { if (1) gprintk _s; } while (0)


#ifdef LINUX_BDE_DMA_DEVICE_SUPPORT
#define DMA_DEV         device
#define DMA_ALLOC_COHERENT(d,s,h)       dma_alloc_coherent(d,s,h,GFP_ATOMIC|GFP_DMA32)
#define DMA_FREE_COHERENT(d,s,a,h)      dma_free_coherent(d,s,a,h)
#else
#define DMA_DEV         pci_dev
#define DMA_ALLOC_COHERENT(d,s,h)       pci_alloc_consistent(d,s,h)
#define DMA_FREE_COHERENT(d,s,a,h)      pci_free_consistent(d,s,a,h)
#endif

/* Type length in bytes */
#define BKSYNC_PACKLEN_U8     1
#define BKSYNC_PACKLEN_U16    2
#define BKSYNC_PACKLEN_U24    3
#define BKSYNC_PACKLEN_U32    4

#define BKSYNC_UNPACK_U8(_buf, _var)         \
    _var = *_buf++

#define BKSYNC_UNPACK_U16(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 8) |        \
                  (_buf)[1]);               \
        (_buf) += BKSYNC_PACKLEN_U16;         \
    } while (0)

#define BKSYNC_UNPACK_U24(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 16) |       \
                  ((_buf)[1] << 8)  |       \
                  (_buf)[2]);               \
        (_buf) += BKSYNC_PACKLEN_U24;         \
    } while (0)

#define BKSYNC_UNPACK_U32(_buf, _var)         \
    do {                                    \
        (_var) = (((_buf)[0] << 24) |       \
                  ((_buf)[1] << 16) |       \
                  ((_buf)[2] << 8)  |       \
                  (_buf)[3]);               \
        (_buf) += BKSYNC_PACKLEN_U32;         \
    } while (0)


#define CMICX_DEV_TYPE   ((ptp_priv->dcb_type == 38) || \
                          (ptp_priv->dcb_type == 36))

#define HOSTCMD_USE_REGS ((ptp_priv->dcb_type == 38) || \
                          (ptp_priv->dcb_type == 36) || \
                          (ptp_priv->dcb_type == 32))

/* CMIC MCS-0 SCHAN Messaging registers */
/* Core0:CMC1 Core1:CMC2 */
#define CMIC_CMC_BASE \
            (CMICX_DEV_TYPE ? (fw_core ? 0x10400 : 0x10300) : \
                              (fw_core ? 0x33000 : 0x32000))

#define CMIC_CMC_SCHAN_MESSAGE_10r(BASE) (BASE + 0x00000034)
#define CMIC_CMC_SCHAN_MESSAGE_11r(BASE) (BASE + 0x00000038)
#define CMIC_CMC_SCHAN_MESSAGE_12r(BASE) (BASE + 0x0000003c)
#define CMIC_CMC_SCHAN_MESSAGE_13r(BASE) (BASE + 0x00000040)
#define CMIC_CMC_SCHAN_MESSAGE_14r(BASE) (BASE + 0x00000044)
#define CMIC_CMC_SCHAN_MESSAGE_15r(BASE) (BASE + 0x00000048)
#define CMIC_CMC_SCHAN_MESSAGE_16r(BASE) (BASE + 0x0000004c)
#define CMIC_CMC_SCHAN_MESSAGE_17r(BASE) (BASE + 0x00000050)
#define CMIC_CMC_SCHAN_MESSAGE_18r(BASE) (BASE + 0x00000054)
#define CMIC_CMC_SCHAN_MESSAGE_19r(BASE) (BASE + 0x00000058)
#define CMIC_CMC_SCHAN_MESSAGE_20r(BASE) (BASE + 0x0000005c)
#define CMIC_CMC_SCHAN_MESSAGE_21r(BASE) (BASE + 0x00000060)

u32 hostcmd_regs[5] = { 0 };

#define BCM_NUM_PORTS           128    /* NUM_PORTS where 2-step is supported. */
#define BCM_MAX_NUM_PORTS       256    /* Max ever NUM_PORTS in the system */

/* Service request commands to R5 */
enum {
    BCM_KSYNC_DONE = 0x0,
    BCM_KSYNC_INIT = 0x1,
    BCM_KSYNC_DEINIT = 0x2,
    BCM_KSYNC_GETTIME = 0x3,
    BCM_KSYNC_SETTIME = 0x4,
    BCM_KSYNC_FREQCOR = 0x5,
    BCM_KSYNC_PBM_UPDATE = 0x6,
    BCM_KSYNC_ADJTIME = 0x7,
    BCM_KSYNC_GET_TSTIME = 0x8,
};

/* Usage macros */
#define ONE_BILLION (1000000000)

#define SKB_U16_GET(_skb, _pkt_offset) \
        ((_skb->data[_pkt_offset] << 8) | _skb->data[_pkt_offset + 1])

#define BKSYNC_PTP_EVENT_MSG(_ptp_msg_type) \
        ((_ptp_msg_type == DELAY_REQ) || (_ptp_msg_type == SYNC))


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
#define HWTSTAMP_TX_ONESTEP_SYNC 2
#else
#include <linux/net_tstamp.h>
#endif


/* Values for the messageType field */
#define SYNC                  0x0
#define DELAY_REQ             0x1

/*
 *  Hardware specific information.
 *  4 words of information used from this data set.
 *       0 -  3: 2-step untagged.
 *       4 -  7: 2-step tagged.
 *       8 - 11: 1-step untagged.
 *      12 - 15: 1-step tagged.
 *      16 - 19: 1-step untagged with ITS-set.
 *      20 - 23: 1-step tagged with ITS-set.
 */
uint32_t sobmhrawpkts_dcb26[24] = {0x00000000, 0x00020E00, 0x00000000, 0x00000000, 0x00000000, 0x00021200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00100E00, 0x00000000, 0x00000000, 0x00000000, 0x00101200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00140E00, 0x00000000, 0x00000000, 0x00000000, 0x00141200, 0x00000000, 0x00000000};

uint32_t sobmhudpipv4_dcb26[24] = {0x00000000, 0x00022A00, 0x00000000, 0x00000000, 0x00000000, 0x00022E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x00102A00, 0x00000000, 0x00000000, 0x00000000, 0x00102E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x00142A00, 0x00000000, 0x00000000, 0x00000000, 0x00142E00, 0x00000000, 0x00000000};

uint32_t sobmhudpipv6_dcb26[24] = {0x00000000, 0x00023E00, 0x00000000, 0x00000000, 0x00000000, 0x00024200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00103E00, 0x00000000, 0x00000000, 0x00000000, 0x00104200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00143E00, 0x00000000, 0x00000000, 0x00000000, 0x00144200, 0x00000000, 0x00000000};

uint32_t sobmhrawpkts_dcb32[24] = {0x00000000, 0x00010E00, 0x00000000, 0x00000000, 0x00000000, 0x00011200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000};

uint32_t sobmhudpipv4_dcb32[24] = {0x00000000, 0x00012A00, 0x00000000, 0x00000000, 0x00000000, 0x00012E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000};

uint32_t sobmhudpipv6_dcb32[24] = {0x00000000, 0x00013E00, 0x00000000, 0x00000000, 0x00000000, 0x00014200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000};

uint32_t sobmhrawpkts_dcb36[24] = {0x00000000, 0x00010E00, 0x00000000, 0x00000000, 0x00000000, 0x00011200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000};

uint32_t sobmhudpipv4_dcb36[24] = {0x00000000, 0x00012A00, 0x00000000, 0x00000000, 0x00000000, 0x00012E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000};

uint32_t sobmhudpipv6_dcb36[24] = {0x00000000, 0x00013E00, 0x00000000, 0x00000000, 0x00000000, 0x00014200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000};
/* th3: onestep only */
uint32_t sobmhrawpkts_dcb38[24] = {0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000,
                                   0x00000000, 0x00080E00, 0x00000000, 0x00000000, 0x00000000, 0x00081200, 0x00000000, 0x00000000};

uint32_t sobmhudpipv4_dcb38[24] = {0x00000000, 0x00082A00, 0x00000000, 0x00000000, 0x00000000, 0x00082E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C2A00, 0x00000000, 0x00000000, 0x00000000, 0x000C2E00, 0x00000000, 0x00000000};

uint32_t sobmhudpipv6_dcb38[24] = {0x00000000, 0x00083E00, 0x00000000, 0x00000000, 0x00000000, 0x00084200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000,
                                   0x00000000, 0x000C3E00, 0x00000000, 0x00000000, 0x00000000, 0x000C4200, 0x00000000, 0x00000000};

/* Driver Proc Entry root */
static struct proc_dir_entry *bksync_proc_root = NULL;

/* Shared data structures with R5 */
typedef struct _bksync_tx_ts_data_s
{
    u32 ts_valid;   /* Timestamp valid indication */
    u32 port_id;    /* Port number */
    u32 ts_seq_id;  /* Sequency Id */
    u32 ts_cnt;
    u64 timestamp;  /* Timestamp */
} bksync_tx_ts_data_t;

typedef struct _bksync_uc_linux_ipc_s
{
    u32 ksyncinit;
    u32 dev_id;
    s64 freqcorr;
    u64 portmap[BCM_NUM_PORTS/64];  /* Two-step enabled ports */
    u64 ptptime;
    u64 reftime;
    s64 phase_offset;
    bksync_tx_ts_data_t port_ts_data[BCM_NUM_PORTS];
} bksync_uc_linux_ipc_t;

typedef struct bksync_port_stats_s {
    u32 pkt_rxctr;             /* All ingress packets */
    u32 pkt_txctr;             /* All egress packets  */
    u32 pkt_txonestep;         /* 1-step Tx packet counter */
    u32 tsts_match;            /* 2-Step tstamp req match */
    u32 tsts_timeout;          /* 2-Step tstamp req timeouts */
    u32 tsts_discard;          /* 2-Step tstamp req discards */
    u32 osts_event_pkts;       /* 1-step event packet counter */
    u32 osts_tstamp_reqs;      /* 1-step events with tstamp request */
} bksync_port_stats_t;

/* Clock Private Data */
struct bksync_ptp_priv {
    struct device           dev;
    int                     dcb_type;
    struct ptp_clock        *ptp_clock;
    struct ptp_clock_info   ptp_caps;
    struct mutex            ptp_lock;
    struct mutex            ptp_pair_lock;
    volatile void           *base_addr;   /* address for PCI register access */
    volatile bksync_uc_linux_ipc_t    *shared_addr; /* address for shared memory access */
    uint64_t                dma_mem;
    int                     dma_mem_size;
    struct DMA_DEV          *dma_dev;    /* Required for DMA memory control */
    int                     num_pports;
    u32                     pkt_rxctr[BCM_NUM_PORTS];
    u32                     pkt_txctr[BCM_NUM_PORTS];
    u32                     pkt_txonestep[BCM_NUM_PORTS];
    u32                     ts_match[BCM_NUM_PORTS];
    u32                     ts_timeout[BCM_NUM_PORTS];
    u32                     ts_discard[BCM_NUM_PORTS];
    int                     timekeep_status;
    struct delayed_work     time_keep;
    bksync_port_stats_t     *port_stats;
};

static struct bksync_ptp_priv *ptp_priv;
volatile bksync_uc_linux_ipc_t *linuxPTPMemory = (bksync_uc_linux_ipc_t*)(0);
static volatile int module_initialized;
static int retry_count = 10;   /* Default retry for 10 jiffies */

static void bksync_ptp_time_keep_init(void);
static void bksync_ptp_time_keep_deinit(void);

#if defined(CMIC_SOFT_BYTE_SWAP)

#define CMIC_SWAP32(_x)   ((((_x) & 0xff000000) >> 24) \
        | (((_x) & 0x00ff0000) >>  8) \
        | (((_x) & 0x0000ff00) <<  8) \
        | (((_x) & 0x000000ff) << 24))

#define DEV_READ32(_d, _a, _p) \
    do { \
        uint32_t _data; \
        _data = (((volatile uint32_t *)(_d)->base_addr)[(_a)/4]); \
        *(_p) = CMIC_SWAP32(_data); \
    } while(0)

#define DEV_WRITE32(_d, _a, _v) \
    do { \
        uint32_t _data = CMIC_SWAP32(_v); \
        ((volatile uint32_t *)(_d)->base_addr)[(_a)/4] = (_data); \
    } while(0)

#else

#define DEV_READ32(_d, _a, _p) \
    do { \
        *(_p) = (((volatile uint32_t *)(_d)->base_addr)[(_a)/4]); \
    } while(0)

#define DEV_WRITE32(_d, _a, _v) \
    do { \
        ((volatile uint32_t *)(_d)->base_addr)[(_a)/4] = (_v); \
    } while(0)
#endif  /* defined(CMIC_SOFT_BYTE_SWAP) */

static void
ptp_usleep(int usec)
{
    usleep_range(usec,usec+1);
}

static void
ptp_sleep(int jiffies)
{
    wait_queue_head_t wq;
    init_waitqueue_head(&wq);

    wait_event_timeout(wq, 0, jiffies);

}


static int bksync_cmd_cmicm_go(u32 cmd, void *data0, void *data1)
{
    int ret = -1;
    int retry_cnt = retry_count;
    u32 cmd_status;

    mutex_lock(&ptp_priv->ptp_lock);
    ptp_priv->shared_addr->ksyncinit = cmd;
    switch (cmd) {
        case BCM_KSYNC_INIT:
            ptp_priv->shared_addr->phase_offset  = 0;
            ret = 0;
            break;
        case BCM_KSYNC_FREQCOR:
            ptp_priv->shared_addr->freqcorr  = *((s32 *)data0);
            break;
        case BCM_KSYNC_ADJTIME:
            ptp_priv->shared_addr->phase_offset  = *((s64 *)data0);
            break;
        case BCM_KSYNC_GETTIME:
            break;
        case BCM_KSYNC_SETTIME:
            ptp_priv->shared_addr->ptptime   = *((s64 *)data0);
            ptp_priv->shared_addr->phase_offset = 0;
            break;
        default:
            break;
    }

    do {
        cmd_status = ptp_priv->shared_addr->ksyncinit;
        if (cmd_status == BCM_KSYNC_DONE) {
            switch (cmd) {
                case BCM_KSYNC_GETTIME:
                    *((s64 *)data0) = ptp_priv->shared_addr->ptptime;
                    *((s64 *)data1) = ptp_priv->shared_addr->reftime; /* ptp counter */
                    break;
                default:
                    break;
            }
            ret = 0;
            break;
        }
        ptp_sleep(1);
        retry_cnt--;
    } while (retry_cnt);
    mutex_unlock(&ptp_priv->ptp_lock);

    if (retry_cnt == 0) {
        DBG_ERR(("Timeout on response from R5\n"));
    }


    return ret;
}



static void bksync_hostcmd_data_op(int setget, u64 *d1, u64 *d2)
{
    u32 w0, w1;
    u64 data;

    if (!d1) {
        return;
    }

    if (setget) {
        data = *d1;
        w0 = (data & 0xFFFFFFFF);
        w1 = (data >> 32);
        DEV_WRITE32(ptp_priv, hostcmd_regs[1], w0);
        DEV_WRITE32(ptp_priv, hostcmd_regs[2], w1);
    } else {
        DEV_READ32(ptp_priv, hostcmd_regs[1], &w0);
        DEV_READ32(ptp_priv, hostcmd_regs[2], &w1);
        data = (((u64)w1 << 32) | (w0));
        *d1 = data;

        if (d2) {
        DEV_READ32(ptp_priv, hostcmd_regs[3], &w0);
        DEV_READ32(ptp_priv, hostcmd_regs[4], &w1);
        data = (((u64)w1 << 32) | (w0));
        *d2 = data;
        }
    }
}


static int bksync_cmd_cmicx_go(u32 cmd, void *data0, void *data1)
{
    int ret = -1;
    int retry_cnt = (retry_count * 100);
    u32 cmd_status;
    char cmd_str[20];

    mutex_lock(&ptp_priv->ptp_lock);
    ptp_priv->shared_addr->ksyncinit = cmd;

    /* init data */
    DEV_WRITE32(ptp_priv, hostcmd_regs[1], 0x0);
    DEV_WRITE32(ptp_priv, hostcmd_regs[2], 0x0);
    DEV_WRITE32(ptp_priv, hostcmd_regs[3], 0x0);
    DEV_WRITE32(ptp_priv, hostcmd_regs[4], 0x0);

    switch (cmd) {
        case BCM_KSYNC_INIT:
            sprintf(cmd_str, "KSYNC_INIT");
            ptp_priv->shared_addr->phase_offset  = 0;
            bksync_hostcmd_data_op(1, (u64 *)&(ptp_priv->shared_addr->phase_offset), 0);
            break;
        case BCM_KSYNC_FREQCOR:
            sprintf(cmd_str, "KSYNC_FREQCORR");
            ptp_priv->shared_addr->freqcorr  = *((s32 *)data0);
            bksync_hostcmd_data_op(1, (u64 *)&(ptp_priv->shared_addr->freqcorr), 0);
            break;
        case BCM_KSYNC_ADJTIME:
            sprintf(cmd_str, "KSYNC_ADJTIME");
            ptp_priv->shared_addr->phase_offset  = *((s64 *)data0);
            bksync_hostcmd_data_op(1, (u64 *)&(ptp_priv->shared_addr->phase_offset), 0);
            break;
        case BCM_KSYNC_GETTIME:
            sprintf(cmd_str, "KSYNC_GETTIME");
            break;
        case BCM_KSYNC_GET_TSTIME:
            retry_cnt = (retry_cnt * 2);
            sprintf(cmd_str, "KSYNC_GET_TSTIME");
            bksync_hostcmd_data_op(1, data0, data1);
            break;
        case BCM_KSYNC_SETTIME:
            sprintf(cmd_str, "KSYNC_SETTIME");
            ptp_priv->shared_addr->ptptime   = *((s64 *)data0);
            ptp_priv->shared_addr->phase_offset = 0;
            bksync_hostcmd_data_op(1, (u64 *)&(ptp_priv->shared_addr->ptptime), (u64 *)&(ptp_priv->shared_addr->phase_offset));
            break;
        case BCM_KSYNC_DEINIT:
            sprintf(cmd_str, "KSYNC_DEINIT");
            break;
        default:
            sprintf(cmd_str, "KSYNC_XXX");
            break;
    }
    DEV_WRITE32(ptp_priv, hostcmd_regs[0], ptp_priv->shared_addr->ksyncinit);

    do {
        DEV_READ32(ptp_priv, hostcmd_regs[0], &cmd_status);
        ptp_priv->shared_addr->ksyncinit = cmd_status;

        if (cmd_status == BCM_KSYNC_DONE) {
            ret = 0;
            switch (cmd) {
                case BCM_KSYNC_GET_TSTIME:
                case BCM_KSYNC_GETTIME:
                    bksync_hostcmd_data_op(0, (u64 *)data0, (u64 *)data1);
                    break;
                default:
                    break;
            }
            break;
        }
        ptp_usleep(100);
        retry_cnt--;
    } while (retry_cnt);

    mutex_unlock(&ptp_priv->ptp_lock);

    if (retry_cnt == 0) {
        DBG_ERR(("Timeout on response from R5 to cmd %s\n", cmd_str));
    }


    return ret;
}


static int bksync_cmd_go(u32 cmd, void *data0, void *data1)
{
    int ret = -1;

    if (ptp_priv == NULL || ptp_priv->shared_addr == NULL) {
        return ret;
    }

    switch (ptp_priv->dcb_type) {
        case 26:
            ret = bksync_cmd_cmicm_go(cmd, data0, data1);
            break;
        case 32:
        case 36:
        case 38:
            ret = bksync_cmd_cmicx_go(cmd, data0, data1);
            break;
        default:
            break;
    }

    return ret;
}


/**
 * bksync_ptp_adjfreq
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ppb: frequency correction value
 *
 * Description: this function will set the frequency correction
 */
static int bksync_ptp_adjfreq(struct ptp_clock_info *ptp, s32 ppb)
{
    int ret = -1;
    u32 cmd_status = BCM_KSYNC_FREQCOR;

    ret = bksync_cmd_go(cmd_status, &ppb, NULL);
    DBG_VERB(("applying freq correction: %x\n", ppb));

    return ret;
}

/**
 * bksync_ptp_adjtime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @delta: desired change in nanoseconds
 *
 * Description: this function will shift/adjust the hardware clock time.
 */
static int bksync_ptp_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
    u32 cmd_status = BCM_KSYNC_ADJTIME;
    int ret = -1;

    ret = bksync_cmd_go(cmd_status, (void *)&delta, NULL);
    DBG_VERB(("ptp adjtime: 0x%llx \n", delta));

    return ret;
}

/**
 * bksync_ptp_gettime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: pointer to hold time/result
 *
 * Description: this function will read the current time from the
 * hardware clock and store it in @ts.
 */
static int bksync_ptp_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
    int ret = -1;
    u32 cmd_status = BCM_KSYNC_GETTIME;
    s64 reftime = 0;
    s64 refctr = 0;

    ret = bksync_cmd_go(cmd_status, (void *)&reftime, (void *)&refctr);
    DBG_VERB(("ptp gettime: 0x%llx refctr:0x%llx\n", reftime, refctr));
    mutex_lock(&ptp_priv->ptp_pair_lock);
    ptp_priv->shared_addr->ptptime = reftime;
    ptp_priv->shared_addr->reftime = refctr;
    mutex_unlock(&ptp_priv->ptp_pair_lock);

    *ts = ns_to_timespec64(reftime);
    return ret;
}


/**
 * bksync_ptp_settime
 *
 * @ptp: pointer to ptp_clock_info structure
 * @ts: time value to set
 *
 * Description: this function will set the current time on the
 * hardware clock.
 */
static int bksync_ptp_settime(struct ptp_clock_info *ptp,
        const struct timespec64 *ts)
{
    s64 reftime, phaseadj;
    int ret = -1;
    u32 cmd_status = BCM_KSYNC_SETTIME;

    phaseadj = 0;
    reftime = timespec64_to_ns(ts);

    ret = bksync_cmd_go(cmd_status, (void *)&reftime, (void *)&phaseadj);
    DBG_VERB(("ptp settime: 0x%llx \n", reftime));

    return ret;
}

static int bksync_ptp_enable(struct ptp_clock_info *ptp,
        struct ptp_clock_request *rq, int on)
{
    return 0;
}

/* structure describing a PTP hardware clock */
static struct ptp_clock_info bksync_ptp_caps = {
    .owner = THIS_MODULE,
    .name = "bksync_ptp_clock",
    .max_adj = 200000,
    .n_alarm = 0,
    .n_ext_ts = 0,
    .n_per_out = 0, /* will be overwritten in bksync_ptp_register */
    .n_pins = 0,
    .pps = 0,
    .adjfreq = bksync_ptp_adjfreq,
    .adjtime = bksync_ptp_adjtime,
    .gettime64 = bksync_ptp_gettime,
    .settime64 = bksync_ptp_settime,
    .enable = bksync_ptp_enable,
};

/**
 * bksync_ptp_hw_tstamp_enable
 *
 * @dev_no: device number
 * @port: port number
 *
 * Description: this is a callback function to enable the timestamping on
 * a given port
 */
int bksync_ptp_hw_tstamp_enable(int dev_no, int port, int tx_type)
{
    uint64_t portmap = 0;
    int map = 0;
    int ret = 0;

    if (!module_initialized) {
        ret = -1;
        goto exit;
    }

    if (tx_type == HWTSTAMP_TX_ONESTEP_SYNC) {
        bksync_ptp_time_keep_init();
        goto exit;
    }

    DBG_VERB(("Enable timestamp on a given port:%d\n", port));
    if (port <= 0) {
        DBG_ERR(("Error enabling timestamp on port:%d\n", port));
        ret = -1;
        goto exit;
    }

    port -= 1;
    map = port/64; port = port%64;

    /* Update the shared structure member */
    if (ptp_priv->shared_addr) {
        portmap = ptp_priv->shared_addr->portmap[map];
        portmap |= (uint64_t)0x1 << port;
        ptp_priv->shared_addr->portmap[map] = portmap;
        /* Command to R5 for the update */
        ptp_priv->shared_addr->ksyncinit=BCM_KSYNC_PBM_UPDATE;
    }

exit:
    return ret;
}

/**
 * bksync_ptp_hw_tstamp_disable
 *
 * @dev_no: device number
 * @port: port number
 *
 * Description: this is a callback function to disable the timestamping on
 * a given port
 */
int bksync_ptp_hw_tstamp_disable(int dev_no, int port, int tx_type)
{
    uint64_t portmap = 0;
    int map = 0;
    int ret = 0;

    if (!module_initialized) {
        ret = -1;
        goto exit;
    }

    if (tx_type == HWTSTAMP_TX_ONESTEP_SYNC) {
        goto exit;
    }

    DBG_VERB(("Disable timestamp on a given port:%d\n", port));
    if (port <= 0) {
        DBG_ERR(("Error disabling timestamp on port:%d\n", port));
        ret = -1;
        goto exit;
    }

    port -= 1;
    map = port/64;  port = port%64;

    /* Update the shared structure member */
    if (ptp_priv->shared_addr) {
        portmap = ptp_priv->shared_addr->portmap[map];
        portmap &= ~((uint64_t)0x1 << port);
        ptp_priv->shared_addr->portmap[map]= portmap;

        /* Command to R5 for the update */
        ptp_priv->shared_addr->ksyncinit = BCM_KSYNC_PBM_UPDATE;
    }
exit:
    return ret;
}

int bksync_ptp_transport_get(uint8_t *pkt)
{
    int         transport = 0;
    uint16_t    ethertype;
    uint16_t    tpid;
    int         tpid_offset, ethype_offset;

    /* Need to check VLAN tag if packet is tagged */
    tpid_offset = 12;
    tpid = pkt[tpid_offset] << 8 | pkt[tpid_offset + 1];
    if (tpid == 0x8100) {
        ethype_offset = tpid_offset + 4;
    } else {
        ethype_offset = tpid_offset;
    }

    ethertype = pkt[ethype_offset] << 8 | pkt[ethype_offset+1];

    switch (ethertype) {
        case 0x88f7:    /* ETHERTYPE_PTPV2 */
            transport = 2;
            break;

        case 0x0800:    /* ETHERTYPE_IPV4 */
            transport = 4;
            break;

        case 0x86DD:    /* ETHERTYPE_IPV6 */
            transport = 6;
            break;

        default:
            transport = 0;
    }

    return transport;
}

static int
bksync_txpkt_tsts_tsamp_get(int port, uint32_t pkt_seq_id, uint32_t *ts_valid, uint32_t *seq_id, uint64_t *timestamp)
{
    int ret = 0;
    uint64_t tmp;

    tmp = (port & 0xFFFF) | (pkt_seq_id << 16);

    if (HOSTCMD_USE_REGS) {
        ret = bksync_cmd_go(BCM_KSYNC_GET_TSTIME, &tmp, timestamp);

        if (ret >= 0) {
            *seq_id = ((tmp >> 16) & 0xFFFF);
            *ts_valid = (tmp & 0x1);
        }
#if 0
        if (tmp & 0x1) gprintk("in_port: %d in_seq_id: %d out_port: %lld ts_valid: %lld seq_id: %lld ts: %llx\n", port, pkt_seq_id, ((tmp & 0xFFFF) >> 1), (tmp & 0x1), (tmp >> 16), *timestamp);
#endif
    } else {
        *ts_valid = ptp_priv->shared_addr->port_ts_data[port].ts_valid;
        *seq_id    = ptp_priv->shared_addr->port_ts_data[port].ts_seq_id;
        *timestamp = ptp_priv->shared_addr->port_ts_data[port].timestamp;
    }


    return ret;
}


/**
 * bksync_ptp_hw_tstamp_tx_time_get
 *
 * @dev_no: device number
 * @port: port number
 * @pkt: packet address
 * @ts: timestamp to be retrieved
 *
 * Description: this is a callback function to retrieve the timestamp on
 * a given port
 */
int bksync_ptp_hw_tstamp_tx_time_get(int dev_no, int port, uint8_t *pkt, uint64_t *ts)
{
    /* Get Timestamp from R5 or CLMAC */
    uint32_t ts_valid = 0;
    uint32_t seq_id = 0;
    uint32_t pktseq_id = 0;
    uint64_t timestamp = 0;
    uint16_t tpid = 0;
    int retry_cnt = retry_count;
    int seq_id_offset, tpid_offset;
    int transport = network_transport;

    if (!ptp_priv || !pkt || !ts || port < 1 || port > 255 || ptp_priv->shared_addr == NULL) {
        return -1;
    }

    *ts = 0;

    tpid_offset = 12;

    /* Parse for nw transport */
    if (transport == 0) {
        transport = bksync_ptp_transport_get(pkt);
    }

    switch(transport)
    {
        case 2:
            seq_id_offset = 0x2c;
            break;
        case 4:
            seq_id_offset = 0x48;
            break;
        case 6:
            seq_id_offset = 0x5c;
            break;
        default:
            seq_id_offset = 0x2c;
            break;
    }

    /* Need to check VLAN tag if packet is tagged */
    tpid = pkt[tpid_offset] << 8 | pkt[tpid_offset + 1];
    if (tpid == 0x8100) {
        seq_id_offset += 4;
    }


    pktseq_id = pkt[seq_id_offset] << 8 | pkt[seq_id_offset + 1];

    port -= 1;

    /* Fetch the TX timestamp from shadow memory */
    do {
        bksync_txpkt_tsts_tsamp_get(port, pktseq_id, &ts_valid, &seq_id, &timestamp);
        if (ts_valid) {

            /* Clear the shadow memory to get next entry */
            ptp_priv->shared_addr->port_ts_data[port].timestamp = 0;
            ptp_priv->shared_addr->port_ts_data[port].port_id = 0;
            ptp_priv->shared_addr->port_ts_data[port].ts_seq_id = 0;
            ptp_priv->shared_addr->port_ts_data[port].ts_valid = 0;

            if (seq_id == pktseq_id) {
                *ts = timestamp;
                if (HOSTCMD_USE_REGS) {
                    ptp_priv->port_stats[port].tsts_match += 1;
                } else {
                    ptp_priv->ts_match[port] += 1;
                }

                DBG_VERB(("Port: %d Skb_SeqID %d FW_SeqId %d and TS:%llx\n",
                          port, pktseq_id, seq_id, timestamp));

                break;
            } else {
                DBG_ERR(("discard timestamp on port %d Skb_SeqID %d FW_SeqId %d\n",
                          port, pktseq_id, seq_id));

                if (HOSTCMD_USE_REGS) {
                    ptp_priv->port_stats[port].tsts_discard += 1;
                } else {
                    ptp_priv->ts_discard[port] += 1;
                }
                continue;
            }
        }
        ptp_sleep(1);
        retry_cnt--;
    } while(retry_cnt);


    if (HOSTCMD_USE_REGS) {
        ptp_priv->port_stats[port].pkt_txctr += 1;
    } else {
        ptp_priv->pkt_txctr[port] += 1;
    }

    if (retry_cnt == 0) {
        if (HOSTCMD_USE_REGS) {
            ptp_priv->port_stats[port].tsts_timeout += 1;
        } else {
            ptp_priv->ts_timeout[port] += 1;
        }
        DBG_ERR(("FW Response timeout: Tx TS on phy port:%d Skb_SeqID: %d\n", port, seq_id));
    }


    return 0;
}


enum {
    bxconCustomEncapVersionInvalid = 0,
    bxconCustomEncapVersionOne  = 1,

    bxconCustomEncapVersionCurrent = bxconCustomEncapVersionOne,
    bxconCustomEncapVersionReserved = 255 /* last */
} bxconCustomEncapVersion;

enum {
    bxconCustomEncapOpcodeInvalid = 0,
    bxconCustomEncapOpcodePtpRx = 1,
    bxconCustomEncapOpcodeReserved = 255 /* last */
} bxconCustomEncapOpcode;

enum {
    bxconCustomEncapPtpRxTlvInvalid = 0,
    bxconCustomEncapPtpRxTlvPtpRxTime = 1,
    bxconCustomEncapPtpRxTlvReserved = 255 /* last */
} bxconCustomEncapPtpRxTlvType;

void
bksync_dump_pkt(uint8_t *data, int size)
{
    int idx;
    char str[128];

    for (idx = 0; idx < size; idx++) {
        if ((idx & 0xf) == 0) {
            sprintf(str, "%04x: ", idx);
        }
        sprintf(&str[strlen(str)], "%02x ", data[idx]);
        if ((idx & 0xf) == 0xf) {
            sprintf(&str[strlen(str)], "\n");
            gprintk(str);
        }
    }
    if ((idx & 0xf) != 0) {
        sprintf(&str[strlen(str)], "\n");
        gprintk(str);
    }
}


static inline int
bksync_pkt_custom_encap_ptprx_get(uint8_t *pkt, uint64_t *ing_ptptime)
{
    uint8_t  *custom_hdr;
    uint8_t   id[4];
    uint8_t   ver, opc;
    uint8_t   nh_type, nh_rsvd;
    uint16_t  len, tot_len;
    uint16_t  nh_len;
    uint32_t  seq_id = 0;
    uint32_t  ptp_rx_time[2];
    uint64_t  u64_ptp_rx_time = 0;

    custom_hdr = pkt;

    BKSYNC_UNPACK_U8(custom_hdr, id[0]);
    BKSYNC_UNPACK_U8(custom_hdr, id[1]);
    BKSYNC_UNPACK_U8(custom_hdr, id[2]);
    BKSYNC_UNPACK_U8(custom_hdr, id[3]);
    if (!((id[0] == 'B') && (id[1] == 'C') && (id[2] == 'M') && (id[3] == 'C'))) {
        /* invalid signature */
        return -1;
    }

    BKSYNC_UNPACK_U8(custom_hdr, ver);
    switch (ver) {
        case bxconCustomEncapVersionCurrent:
            break;
        default:
            gprintk("invalid ver\n");
            return -1;
    }

    BKSYNC_UNPACK_U8(custom_hdr, opc);
    switch (opc) {
        case bxconCustomEncapOpcodePtpRx:
            break;
        default:
            gprintk("invalid opcode\n");
            return -1;
    }


    BKSYNC_UNPACK_U16(custom_hdr, len);
    BKSYNC_UNPACK_U32(custom_hdr, seq_id);
    tot_len = len;

    /* remaining length of custom encap */
    len = len - (custom_hdr - pkt);


    /* process tlv */
    while (len > 0) {
        BKSYNC_UNPACK_U8(custom_hdr, nh_type);
        BKSYNC_UNPACK_U8(custom_hdr, nh_rsvd);
        BKSYNC_UNPACK_U16(custom_hdr, nh_len);
        len = len - (nh_len);
        if (nh_rsvd != 0x0) {
            continue; /* invalid tlv */
        }

        switch (nh_type) {
            case bxconCustomEncapPtpRxTlvPtpRxTime:
                BKSYNC_UNPACK_U32(custom_hdr, ptp_rx_time[0]);
                BKSYNC_UNPACK_U32(custom_hdr, ptp_rx_time[1]);
                u64_ptp_rx_time = ((uint64_t)ptp_rx_time[1] << 32) | (uint64_t)ptp_rx_time[0];
                *ing_ptptime = u64_ptp_rx_time;
                break;
            default:
                custom_hdr += nh_len;
                break;
        }
    }

#if 0
if (!(seq_id % 100)) {
    gprintk("****** seq_id = %d ptp time = 0x%llx\n", seq_id, u64_ptp_rx_time);
    bksync_dump_pkt(pkt, tot_len);
}
#endif

    DBG_VERB(("Custom encap header: ver=%d opcode=%d seq_id=0x%x\n", ver, opc, seq_id));

    return (tot_len);
}



/**
 * bksync_ptp_hw_tstamp_rx_time_upscale
 *
 * @dev_no: device number
 * @ts: timestamp to be retrieved
 *
 * Description: this is a callback function to retrieve 64b equivalent of
 *   rx timestamp
 */
int bksync_ptp_hw_tstamp_rx_time_upscale(int dev_no, int port, struct sk_buff *skb, uint32_t *meta, uint64_t *ts)
{
    int ret = 0;
    int custom_encap_len = 0;

    switch (KNET_SKB_CB(skb)->dcb_type) {
        case 26:
        case 32:
            if (pci_cos != (meta[4] & 0x3F)) {
                return -1;
            }
            break;
        case 38:
            if (pci_cos != ((meta[12] >> 22) & 0x2F)) {
                return -1;
            }
            break;
        case 36:
            if (pci_cos != ((meta[6] >> 22) & 0x2F)) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    /* parse custom encap header in pkt for ptp rxtime */
    custom_encap_len = bksync_pkt_custom_encap_ptprx_get((skb->data), ts);

    /* Remove the custom encap header from pkt */
    if (custom_encap_len > 0) {
        skb_pull(skb, custom_encap_len);

        DBG_VERB(("###### ptp message type: %d\n", skb->data[42]));
    }

    if (port > 0){
        port -= 1;
        if (HOSTCMD_USE_REGS) {
            ptp_priv->port_stats[port].pkt_rxctr += 1;
        } else {
            ptp_priv->pkt_rxctr[port] += 1;
        }
    }

    return ret;
}


void bksync_hton64(u8 *buf, const uint64_t *data)
{
#ifdef __LITTLE_ENDIAN
  /* LITTLE ENDIAN */
  buf[0] = (*(((uint8_t*)(data)) + 7u));
  buf[1] = (*(((uint8_t*)(data)) + 6u));
  buf[2] = (*(((uint8_t*)(data)) + 5u));
  buf[3] = (*(((uint8_t*)(data)) + 4u));
  buf[4] = (*(((uint8_t*)(data)) + 3u));
  buf[5] = (*(((uint8_t*)(data)) + 2u));
  buf[6] = (*(((uint8_t*)(data)) + 1u));
  buf[7] = (*(((uint8_t*)(data)) + 0u));
#else
  memcpy(buf, data, 8);
#endif
}



int bksync_ptp_hw_tstamp_tx_meta_get(int dev_no,
                                     int hwts, int hdrlen,
                                     struct sk_buff *skb,
                                     uint64_t *tstamp,
                                     u32 **md)
{
    uint16_t tpid = 0;
    int md_offset = 0;
    int pkt_offset = 0;
    int ptp_hdr_offset = 0;
    int transport = network_transport;
    s64 ptptime  = 0;
    s64 ptpcounter = 0;
    int64_t corrField;
    int32_t negCurTS32;
    int64_t negCurTS64;

    if(!ptp_priv || ptp_priv->shared_addr == NULL) {
        return 0;
    }

    mutex_lock(&ptp_priv->ptp_pair_lock);
    ptptime = ptp_priv->shared_addr->ptptime;
    ptpcounter = ptp_priv->shared_addr->reftime;
    mutex_unlock(&ptp_priv->ptp_pair_lock);

    negCurTS32 = - (int32_t) ptpcounter;
    negCurTS64 = - (int64_t)(ptpcounter);

    if (CMICX_DEV_TYPE) {
        pkt_offset = ptp_hdr_offset = hdrlen;
    }

    /* Need to check VLAN tag if packet is tagged */
    tpid = SKB_U16_GET(skb, (pkt_offset + 12));
    if (tpid == 0x8100) {
        md_offset = 4;
        ptp_hdr_offset += 4;
    }

    /* One Step Meta Data */
    if (hwts == HWTSTAMP_TX_ONESTEP_SYNC) {
        md_offset += 8;
        if (KNET_SKB_CB(skb)->dcb_type == 26) {
            corrField = (((int64_t)negCurTS32) << 16);
            if (negCurTS32 >= 0) {
                md_offset += 8;
            }
        } else {
            corrField = (((int64_t)negCurTS64) << 16);
        }
    }


    /* Parse for nw transport */
    if (transport == 0) {
        transport = bksync_ptp_transport_get(skb->data + pkt_offset);
    }

    switch(transport)
    {
        case 2: /* IEEE 802.3 */
            ptp_hdr_offset += 14;
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                if (md) *md = &sobmhrawpkts_dcb32[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                if (md) *md = &sobmhrawpkts_dcb26[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 36) {
                if (md) *md = &sobmhrawpkts_dcb36[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 38) {
                if (md) *md = &sobmhrawpkts_dcb38[md_offset];
            }
            break;
        case 4: /* UDP IPv4   */
            ptp_hdr_offset += 42;
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                if (md) *md = &sobmhudpipv4_dcb32[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                if (md) *md = &sobmhudpipv4_dcb26[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 36) {
                if (md) *md = &sobmhudpipv4_dcb36[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 38) {
                if (md) *md = &sobmhudpipv4_dcb38[md_offset];
            }
            break;
        case 6: /* UDP IPv6   */
            ptp_hdr_offset += 62;
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                if (md) *md = &sobmhudpipv6_dcb32[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                if (md) *md = &sobmhudpipv6_dcb26[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 36) {
                if (md) *md = &sobmhudpipv6_dcb36[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 38) {
                if (md) *md = &sobmhudpipv6_dcb38[md_offset];
            }
            break;
        default:
            ptp_hdr_offset += 42;
            if (KNET_SKB_CB(skb)->dcb_type == 32) {
                if (md) *md = &sobmhudpipv4_dcb32[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 26) {
                if (md) *md = &sobmhudpipv4_dcb26[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 36) {
                if (md) *md = &sobmhudpipv4_dcb36[md_offset];
            } else if(KNET_SKB_CB(skb)->dcb_type == 38) {
                if (md) *md = &sobmhudpipv4_dcb38[md_offset];
            }
            break;
    }


    if ((hwts == HWTSTAMP_TX_ONESTEP_SYNC) &&
        BKSYNC_PTP_EVENT_MSG(skb->data[ptp_hdr_offset])) {
        /* One Step Timestamp Field updation */
        int corr_offset = ptp_hdr_offset + 8;
        int origin_ts_offset = ptp_hdr_offset + 34;
        u32 tmp;
        struct timespec64 ts;
        int udp_csum_regen;
        u32 udp_csum20;
        u16 udp_csum;
        int port;

        udp_csum = SKB_U16_GET(skb, (ptp_hdr_offset - 2));

        switch (transport) {
            case 2:
                udp_csum_regen = 0;
                break;
            case 6:
                udp_csum_regen = 1;
                break;
            default:
                udp_csum_regen = (udp_csum != 0x0);
                break;
        }

        /* Fill the correction field */
        bksync_hton64(&(skb->data[corr_offset]), (const u64 *)&corrField);

        /* Fill the Origin Timestamp Field */
        ts = ns_to_timespec64(ptptime);

        tmp = (ts.tv_sec >> 32);
        skb->data[origin_ts_offset + 0] = ((tmp >>  8) & 0xFF);
        skb->data[origin_ts_offset + 1] = ((tmp      ) & 0xFF);

        tmp = (ts.tv_sec & 0xFFFFFFFFLL);
        skb->data[origin_ts_offset + 2] = ((tmp >> 24) & 0xFF);
        skb->data[origin_ts_offset + 3] = ((tmp >> 16) & 0xFF);
        skb->data[origin_ts_offset + 4] = ((tmp >>  8) & 0xFF);
        skb->data[origin_ts_offset + 5] = ((tmp      ) & 0xFF);

        tmp = (ts.tv_nsec & 0xFFFFFFFFLL);
        skb->data[origin_ts_offset + 6] = ((tmp >> 24) & 0xFF);
        skb->data[origin_ts_offset + 7] = ((tmp >> 16) & 0xFF);
        skb->data[origin_ts_offset + 8] = ((tmp >>  8) & 0xFF);
        skb->data[origin_ts_offset + 9] = ((tmp      ) & 0xFF);

        if (udp_csum_regen) {
            udp_csum20 = (~udp_csum) & 0xFFFF;

            udp_csum20 += SKB_U16_GET(skb, (corr_offset + 0));
            udp_csum20 += SKB_U16_GET(skb, (corr_offset + 2));
            udp_csum20 += SKB_U16_GET(skb, (corr_offset + 4));
            udp_csum20 += SKB_U16_GET(skb, (corr_offset + 6));

            udp_csum20 += SKB_U16_GET(skb, (origin_ts_offset + 0));
            udp_csum20 += SKB_U16_GET(skb, (origin_ts_offset + 2));
            udp_csum20 += SKB_U16_GET(skb, (origin_ts_offset + 4));
            udp_csum20 += SKB_U16_GET(skb, (origin_ts_offset + 6));
            udp_csum20 += SKB_U16_GET(skb, (origin_ts_offset + 8));

            /* Fold 20bit checksum into 16bit udp checksum */
            udp_csum20 = ((udp_csum20 & 0xFFFF) + (udp_csum20 >> 16));
            udp_csum = ((udp_csum20 & 0xFFFF) + (udp_csum20 >> 16));

            /* invert again to get final checksum. */
            udp_csum = ~udp_csum;
            if (udp_csum == 0) {
                udp_csum = 0xFFFF;
            }

            skb->data[ptp_hdr_offset - 2] = ((udp_csum >>  8) & 0xFF);
            skb->data[ptp_hdr_offset - 1] = ((udp_csum      ) & 0xFF);
        }

        if (skb->data[ptp_hdr_offset] == DELAY_REQ) {
            *tstamp = ptptime;

            DBG_VERB(("ptp delay req packet tstamp : 0x%llx corrField: 0x%llx\n", ptptime, corrField));
        }

        port = KNET_SKB_CB(skb)->port;
        port -= 1;
        if (HOSTCMD_USE_REGS) {
            ptp_priv->port_stats[port].pkt_txonestep += 1;
        } else {
            ptp_priv->pkt_txonestep[port] += 1;
        }
    }

    return 0;
}


int bksync_ptp_hw_tstamp_ptp_clock_index_get(int dev_no)
{
    int phc_index = -1;
    if (ptp_priv && ptp_priv->ptp_clock)
        phc_index =  ptp_clock_index(ptp_priv->ptp_clock);
    return phc_index;
}


/**
* bcm_ptp_time_keep - call timecounter_read every second to avoid timer overrun
*                 because  a 32bit counter, will timeout in 4s
*/
static void bksync_ptp_time_keep(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    struct bksync_ptp_priv *priv =
                        container_of(dwork, struct bksync_ptp_priv, time_keep);
    struct timespec64 ts;

    /* Call bcm_ptp_gettime function to keep the ref_time_64 and ref_counter_48 in sync */
    bksync_ptp_gettime(&(priv->ptp_caps), &ts);
    schedule_delayed_work(&priv->time_keep, HZ);
}

static void bksync_ptp_time_keep_init(void)
{
    if (!ptp_priv->timekeep_status) {
        INIT_DELAYED_WORK(&(ptp_priv->time_keep), bksync_ptp_time_keep);
        schedule_delayed_work(&ptp_priv->time_keep, HZ);

        ptp_priv->timekeep_status = 1;
    }

    return;
}

static void bksync_ptp_time_keep_deinit(void)
{
    if (ptp_priv->timekeep_status) {
        /* Cancel delayed work */
        cancel_delayed_work_sync(&(ptp_priv->time_keep));

        ptp_priv->timekeep_status = 0;
    }

    return;
}



static int bksync_ptp_init(struct ptp_clock_info *ptp)
{
    return bksync_cmd_go(BCM_KSYNC_INIT, NULL, NULL);
}

static int bksync_ptp_deinit(struct ptp_clock_info *ptp)
{
    bksync_ptp_time_keep_deinit();

    return bksync_cmd_go(BCM_KSYNC_DEINIT, NULL, NULL);
}

/*
 * Device Debug Statistics Proc Entry
 */
/**
* This function is called at the beginning of a sequence.
* ie, when:
*    - the /proc/bcm/ksync/stats file is read (first time)
*   - after the function stop (end of sequence)
 */
static void *bksync_proc_seq_start(struct seq_file *s, loff_t *pos)
{
   /* beginning a new sequence ? */
   if ( (int)*pos == 0 && ptp_priv->shared_addr != NULL)
   {
        seq_printf(s, "TwoStep Port Bitmap : %08llx%08llx\n",
                      (uint64_t)(ptp_priv->shared_addr->portmap[1]),
                      (uint64_t)(ptp_priv->shared_addr->portmap[0]));
        seq_printf(s,"%4s| %9s| %9s| %9s| %9s| %9s| %9s| %9s|\n",
                     "Port", "RxCounter", "TxCounter", "TxOneStep", "TSTimeout", "TSRead", "TSMatch", "TSDiscard");
   }

   if ((int)*pos < (ptp_priv->num_pports))
        return (void *)(unsigned long)(*pos + 1);
   /* End of the sequence, return NULL */
   return NULL;
}

/**
* This function is called after the beginning of a sequence.
* It's called untill the return is NULL (this ends the sequence).
 */
static void *bksync_proc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    (*pos)++;
    return bksync_proc_seq_start(s, pos);
}
/**
* This function is called at the end of a sequence
 */
static void bksync_proc_seq_stop(struct seq_file *s, void *v)
{
    /* nothing to do, we use a static value in bksync_proc_seq_start() */
}

/**
* This function is called for each "step" of a sequence
 */
static int bksync_proc_seq_show(struct seq_file *s, void *v)
{
    unsigned long port = (unsigned long)v;
    port = port - 1;
    if (HOSTCMD_USE_REGS) {
        if (ptp_priv->port_stats[port].pkt_rxctr || ptp_priv->port_stats[port].pkt_txctr ||
            ptp_priv->port_stats[port].pkt_txonestep||
            ptp_priv->port_stats[port].tsts_discard || ptp_priv->port_stats[port].tsts_timeout ||
            ptp_priv->shared_addr->port_ts_data[port].ts_cnt || ptp_priv->port_stats[port].tsts_match) {
              seq_printf(s, "%4lu | %9d| %9d| %9d| %9d| %9d| %9d| %9d| %s\n", (port + 1),
                    ptp_priv->port_stats[port].pkt_rxctr,
                    ptp_priv->port_stats[port].pkt_txctr,
                    ptp_priv->port_stats[port].pkt_txonestep,
                    ptp_priv->port_stats[port].tsts_timeout,
                    ptp_priv->shared_addr->port_ts_data[port].ts_cnt,
                    ptp_priv->port_stats[port].tsts_match,
                    ptp_priv->port_stats[port].tsts_discard,
                    ptp_priv->port_stats[port].pkt_txctr != ptp_priv->port_stats[port].tsts_match ? "***":"");
        }
     } else {
        if (ptp_priv->pkt_rxctr[port] || ptp_priv->pkt_txctr[port] ||
            ptp_priv->pkt_txonestep[port] ||
            ptp_priv->ts_discard[port] || ptp_priv->ts_timeout[port] ||
            ptp_priv->shared_addr->port_ts_data[port].ts_cnt || ptp_priv->ts_match[port]) {
                seq_printf(s, "%4lu | %9d| %9d| %9d| %9d| %9d| %9d| %9d| %s\n", (port + 1),
                    ptp_priv->pkt_rxctr[port],
                    ptp_priv->pkt_txctr[port],
                    ptp_priv->pkt_txonestep[port],
                    ptp_priv->ts_timeout[port],
                    ptp_priv->shared_addr->port_ts_data[port].ts_cnt,
                    ptp_priv->ts_match[port],
                    ptp_priv->ts_discard[port],
                    ptp_priv->pkt_txctr[port] != ptp_priv->ts_match[port] ? "***":"");
        }
     }
    return 0;
}

/**
* seq_operations for bsync_proc_*** entries
 */
static struct seq_operations bksync_proc_seq_ops = {
    .start = bksync_proc_seq_start,
    .next  = bksync_proc_seq_next,
    .stop  = bksync_proc_seq_stop,
    .show  = bksync_proc_seq_show
};
static int bksync_proc_txts_open(struct inode * inode, struct file * file)
{
    return seq_open(file, &bksync_proc_seq_ops);
}

static ssize_t
bksync_proc_txts_write(struct file *file, const char *buf,
                      size_t count, loff_t *loff)
{
    char debug_str[40];
    char *ptr;
    int port;

    if (copy_from_user(debug_str, buf, count)) {
        return -EFAULT;
    }

    if ((ptr = strstr(debug_str, "clear")) != NULL) {
        if (HOSTCMD_USE_REGS) {
            for (port = 0; port < ptp_priv->num_pports; port++) {
                ptp_priv->port_stats[port].pkt_rxctr = 0;
                ptp_priv->port_stats[port].pkt_txctr = 0;
                ptp_priv->port_stats[port].pkt_txonestep = 0;
                ptp_priv->port_stats[port].tsts_timeout = 0;
                ptp_priv->port_stats[port].tsts_match = 0;
                ptp_priv->port_stats[port].tsts_discard = 0;
                if (ptp_priv->shared_addr)
                    ptp_priv->shared_addr->port_ts_data[port].ts_cnt = 0;
            }
        } else {
            for (port = 0; port < ptp_priv->num_pports; port++) {
                ptp_priv->pkt_rxctr[port] = 0;
                ptp_priv->pkt_txctr[port] = 0;
                ptp_priv->pkt_txonestep[port] = 0;
                ptp_priv->ts_discard[port] = 0;
                ptp_priv->ts_timeout[port] = 0;
                ptp_priv->ts_match[port] = 0;
                if (ptp_priv->shared_addr)
                    ptp_priv->shared_addr->port_ts_data[port].ts_cnt = 0;
            }
        }
    } else {
        gprintk("Warning: unknown input\n");
    }

    return count;
}

struct file_operations bksync_proc_txts_file_ops = {
    owner:      THIS_MODULE,
    open:       bksync_proc_txts_open,
    read:       seq_read,
    llseek:     seq_lseek,
    write:      bksync_proc_txts_write,
    release:    seq_release,
};

static int
bksync_proc_init(void)
{
    struct proc_dir_entry *entry;

    PROC_CREATE(entry, "stats", 0666, bksync_proc_root, &bksync_proc_txts_file_ops);
    if (entry == NULL) {
        return -1;
    }
    return 0;
}

static int
bksync_proc_cleanup(void)
{
    remove_proc_entry("stats", bksync_proc_root);
    return 0;
}

static void bksync_ptp_cmicm_dma_init(int dcb_type)
{
    int endianess;
    int num_pports = 128;
    dma_addr_t dma_mem = 0;

    /* Initialize the Base address for CMIC and shared Memory access */
    ptp_priv->base_addr = lkbde_get_dev_virt(0);
    ptp_priv->dma_dev = lkbde_get_dma_dev(0);

    ptp_priv->num_pports = num_pports;
    ptp_priv->dma_mem_size = 16384;/*sizeof(bksync_uc_linux_ipc_t);*/

    if (ptp_priv->shared_addr == NULL) {
        DBG_ERR(("Allocate shared memory with R5\n"));
        ptp_priv->shared_addr = DMA_ALLOC_COHERENT(ptp_priv->dma_dev,
                                                   ptp_priv->dma_mem_size,
                                                   &dma_mem);
        ptp_priv->dma_mem = (uint64_t)dma_mem;
        ptp_priv->port_stats = kzalloc((sizeof(bksync_port_stats_t) * num_pports), GFP_KERNEL);
    }

    if (ptp_priv->shared_addr != NULL) {
        /* Reset memory */
        memset((void *)ptp_priv->shared_addr, 0, ptp_priv->dma_mem_size);

        DBG_ERR(("Shared memory allocation (%d bytes) successful at 0x%016lx.\n",
                ptp_priv->dma_mem_size, (long unsigned int)ptp_priv->dma_mem));
#ifdef __LITTLE_ENDIAN
        endianess = 0;
#else
        endianess = 1;
#endif
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_12r(CMIC_CMC_BASE), ((pci_cos << 16) | endianess));

        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_10r(CMIC_CMC_BASE),
                    (ptp_priv->dma_mem & 0xffffffff));
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_11r(CMIC_CMC_BASE),
                    (ptp_priv->dma_mem >> 32) & 0xffffffff);

        ptp_priv->dcb_type = dcb_type;
    }

    if (debug & DBG_LVL_VERB) {
        printk(KERN_EMERG"%s %p:%p\n",__FUNCTION__,
               ptp_priv->base_addr,(void *)ptp_priv->shared_addr);
    }

    return;
}

static void bksync_ptp_cmicx_dma_init(int dcb_type)
{
    int endianess;
    int num_pports = 256;

    /* Initialize the Base address for CMIC and shared Memory access */
    ptp_priv->base_addr = lkbde_get_dev_virt(0);
    ptp_priv->dma_dev = lkbde_get_dma_dev(0);

    ptp_priv->num_pports = num_pports;
    ptp_priv->dcb_type = dcb_type;
    ptp_priv->dma_mem_size = 16384;/*sizeof(bksync_uc_linux_ipc_t);*/

    if (ptp_priv->shared_addr == NULL) {
        ptp_priv->shared_addr = kzalloc(16384, GFP_KERNEL);
        ptp_priv->port_stats = kzalloc((sizeof(bksync_port_stats_t) * num_pports), GFP_KERNEL);
    }

    if (ptp_priv->shared_addr != NULL) {
        /* Reset memory. */
        memset((void *)ptp_priv->shared_addr, 0, ptp_priv->dma_mem_size);

#ifdef __LITTLE_ENDIAN
        endianess = 0;
#else
        endianess = 1;
#endif
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_12r(CMIC_CMC_BASE), ((pci_cos << 16) | endianess));

        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_10r(CMIC_CMC_BASE), 1);
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_11r(CMIC_CMC_BASE), 1);

    }

    if (debug & DBG_LVL_VERB) {
        printk(KERN_EMERG"%s %p:%p\n",__FUNCTION__,
               ptp_priv->base_addr,(void *)ptp_priv->shared_addr);
    }


    hostcmd_regs[0] = CMIC_CMC_SCHAN_MESSAGE_21r(CMIC_CMC_BASE);
    hostcmd_regs[1] = CMIC_CMC_SCHAN_MESSAGE_20r(CMIC_CMC_BASE);
    hostcmd_regs[2] = CMIC_CMC_SCHAN_MESSAGE_19r(CMIC_CMC_BASE);
    hostcmd_regs[3] = CMIC_CMC_SCHAN_MESSAGE_18r(CMIC_CMC_BASE);
    hostcmd_regs[4] = CMIC_CMC_SCHAN_MESSAGE_17r(CMIC_CMC_BASE);

    return;
}


static void bksync_ptp_dma_init(int dcb_type)
{
    switch (dcb_type) {
        case 26:
            bksync_ptp_cmicm_dma_init(dcb_type);
            ptp_priv->dcb_type = dcb_type;
            break;
        case 32:
        case 36:
        case 38:
            bksync_ptp_cmicx_dma_init(dcb_type);
            ptp_priv->dcb_type = dcb_type;
            break;
        default:
            break;
    }

    return;
}



/**
 * bksync_ioctl_cmd_handler
 * @kmsg: kcom message - ptp clock ioctl command.
 * Description: This function will handle ioctl commands
 * from user mode.
 */
static int
bksync_ioctl_cmd_handler(kcom_msg_clock_cmd_t *kmsg, int len, int dcb_type)
{
    kmsg->hdr.type = KCOM_MSG_TYPE_RSP;

    if (!module_initialized && kmsg->clock_info.cmd != KSYNC_M_HW_INIT) {
        kmsg->hdr.status = KCOM_E_NOT_FOUND;
        return sizeof(kcom_msg_hdr_t);
    }

    switch(kmsg->clock_info.cmd) {
        case KSYNC_M_HW_INIT:
            pci_cos = kmsg->clock_info.data[0];
            if (kmsg->clock_info.data[1] == 0 || kmsg->clock_info.data[1] == 1) {
                fw_core = kmsg->clock_info.data[1];
                bksync_ptp_dma_init(dcb_type);
                if (bksync_ptp_init(&(ptp_priv->ptp_caps)) >= 0) {
                    module_initialized = 1;
                }
            }
            break;
        case KSYNC_M_HW_DEINIT:
            bksync_ptp_deinit(&(ptp_priv->ptp_caps));
            module_initialized = 0;
            break;
        case KSYNC_M_HW_TS_DISABLE:
            bksync_ptp_hw_tstamp_disable(0, kmsg->clock_info.data[0], 0);
            break;
        case KSYNC_M_VERSION:
            break;
        default:
            kmsg->hdr.status = KCOM_E_NOT_FOUND;
            return sizeof(kcom_msg_hdr_t);
    }

    return sizeof(*kmsg);
}



/**
 * bksync_ptp_register
 * @priv: driver private structure
 * Description: this function will register the ptp clock driver
 * to kernel. It also does some house keeping work.
 */
static int bksync_ptp_register(void)
{
    int err = -ENODEV;

    /* Support on core-0 or core-1 */
    if (fw_core < 0 || fw_core > 1) {
        goto exit;
    }

    /* default transport is raw, ieee 802.3 */
    switch (network_transport) {
        case 2: /* IEEE 802.3 */
        case 4: /* UDP IPv4   */
        case 6: /* UDP IPv6   */
            break;
        default:
            network_transport = 0;
    }

    ptp_priv = kzalloc(sizeof(*ptp_priv), GFP_KERNEL);
    if (!ptp_priv) {
        err = -ENOMEM;
        goto exit;
    }

    /* Reset memory */
    memset(ptp_priv, 0, sizeof(*ptp_priv));

    err = -ENODEV;

    ptp_priv->ptp_caps = bksync_ptp_caps;

    mutex_init(&(ptp_priv->ptp_lock));
    mutex_init(&(ptp_priv->ptp_pair_lock));

    /* Register ptp clock driver with bksync_ptp_caps */
    ptp_priv->ptp_clock = ptp_clock_register(&ptp_priv->ptp_caps, NULL);

    if (IS_ERR(ptp_priv->ptp_clock)) {
        ptp_priv->ptp_clock = NULL;
    } else if (ptp_priv->ptp_clock) {
        err = 0;

        /* Register BCM-KNET HW Timestamp Callback Functions */
        bkn_hw_tstamp_enable_cb_register(bksync_ptp_hw_tstamp_enable);
        bkn_hw_tstamp_disable_cb_register(bksync_ptp_hw_tstamp_disable);
        bkn_hw_tstamp_tx_time_get_cb_register(bksync_ptp_hw_tstamp_tx_time_get);
        bkn_hw_tstamp_tx_meta_get_cb_register(bksync_ptp_hw_tstamp_tx_meta_get);
        bkn_hw_tstamp_rx_time_upscale_cb_register(bksync_ptp_hw_tstamp_rx_time_upscale);
        bkn_hw_tstamp_ptp_clock_index_cb_register(bksync_ptp_hw_tstamp_ptp_clock_index_get);
        bkn_hw_tstamp_ioctl_cmd_cb_register(bksync_ioctl_cmd_handler);

    }

     /* Initialize proc files */
     bksync_proc_root = proc_mkdir("bcm/ksync", NULL);
     bksync_proc_init();
     ptp_priv->shared_addr = NULL;
     ptp_priv->port_stats = NULL;
exit:
    return err;
}

static int bksync_ptp_remove(void)
{
    if (!ptp_priv)
        return 0;

    bksync_ptp_time_keep_deinit();

    bksync_proc_cleanup();
    remove_proc_entry("bcm/ksync", NULL);

    /* UnRegister BCM-KNET HW Timestamp Callback Functions */
    bkn_hw_tstamp_enable_cb_unregister(bksync_ptp_hw_tstamp_enable);
    bkn_hw_tstamp_disable_cb_unregister(bksync_ptp_hw_tstamp_disable);
    bkn_hw_tstamp_tx_time_get_cb_unregister(bksync_ptp_hw_tstamp_tx_time_get);
    bkn_hw_tstamp_tx_meta_get_cb_unregister(bksync_ptp_hw_tstamp_tx_meta_get);
    bkn_hw_tstamp_rx_time_upscale_cb_unregister(bksync_ptp_hw_tstamp_rx_time_upscale);
    bkn_hw_tstamp_ptp_clock_index_cb_unregister(bksync_ptp_hw_tstamp_ptp_clock_index_get);
    bkn_hw_tstamp_ioctl_cmd_cb_unregister(bksync_ioctl_cmd_handler);
 
    if (module_initialized) {
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_10r(CMIC_CMC_BASE), 0);
        DEV_WRITE32(ptp_priv, CMIC_CMC_SCHAN_MESSAGE_11r(CMIC_CMC_BASE), 0);
    }
    /* Deinitialize the PTP */
    bksync_ptp_deinit(&(ptp_priv->ptp_caps));
    module_initialized = 0;

    if (ptp_priv->port_stats != NULL) {
        kfree((void *)ptp_priv->port_stats);
        ptp_priv->port_stats = NULL;
    }
    if (ptp_priv->shared_addr != NULL) {
        if (HOSTCMD_USE_REGS) {
            kfree((void *)ptp_priv->shared_addr);
        } else {
            DMA_FREE_COHERENT(ptp_priv->dma_dev, ptp_priv->dma_mem_size,
                              (void *)ptp_priv->shared_addr, (dma_addr_t)ptp_priv->dma_mem);
        }
        ptp_priv->shared_addr = NULL;
        DBG_ERR(("Free R5 memory\n"));
    }

    /* Unregister the bcm ptp clock driver */
    ptp_clock_unregister(ptp_priv->ptp_clock);

    /* Free Memory */
    kfree(ptp_priv);

    return 0;
}
#endif


/*
 * Generic module functions
 */

/*
 * Function: _pprint
 *
 * Purpose:
 *    Print proc filesystem information.
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
    static int
_pprint(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    /* put some goodies here */
    pprintf("Broadcom BCM PTP Hardware Clock Module\n");
#else
    pprintf("Broadcom BCM PTP Hardware Clock Module not supported\n");
#endif
    return 0;
}

/*
 * Function: _init
 *
 * Purpose:
 *    Module initialization.
 *    Attached SOC all devices and optionally initializes these.
 * Parameters:
 *    None
 * Returns:
 *    0 on success, otherwise -1
 */
    static int
_init(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    bksync_ptp_register();
    return 0;
#else
    return -1;
#endif
}

/*
 * Function: _cleanup
 *
 * Purpose:
 *    Module cleanup function
 * Parameters:
 *    None
 * Returns:
 *    Always 0
 */
    static int
_cleanup(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,17,0)
    mutex_destroy(&(ptp_priv->ptp_lock));
    mutex_destroy(&(ptp_priv->ptp_pair_lock));
    bksync_ptp_remove();
    return 0;
#else
    return -1;
#endif
}

static gmodule_t _gmodule = {
name: MODULE_NAME,
      major: MODULE_MAJOR,
      init: _init,
      cleanup: _cleanup,
      pprint: _pprint,
      ioctl: NULL,
      open: NULL,
      close: NULL,
};

    gmodule_t*
gmodule_get(void)
{
    EXPORT_NO_SYMBOLS;
    return &_gmodule;
}
