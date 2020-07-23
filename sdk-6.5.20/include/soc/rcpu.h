/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This header file defines RCPU packet formats and soc layer API.
 */

#ifndef _SOC_RCPU_H
#define _SOC_RCPU_H

#include <soc/types.h>
#include <soc/macipadr.h>
#include <soc/schanmsg.h>
#include <sal/core/sync.h>

#ifdef INCLUDE_RCPU

#define DEFAULT_RCPU_DST_MAC  "00:00:11:22:33:00"
#define DEFAULT_RCPU_SRC_MAC  "00:AA:BB:22:33:00"

#define DEFAULT_RCPU_TPID          0x8100
#define DEFAULT_RCPU_VLAN               1
#define DEFAULT_RCPU_ETHER_TYPE    0x8874
#define DEFAULT_RCPU_SIGNATURE     0x5600

#define DEFAULT_RCPU_MASTER_UNIT        0 /* Default local unit used for in-band RCPU */
#define DEFAULT_RCPU_ETH_UNIT           0 /* Default eth unit used for out-of-band RCPU */

/* RCPU CFG flags encodings */
typedef struct soc_rcpu_cfg_s {
    sal_mac_addr_t   dst_mac;    /* MAC address of the remote unit   */
    sal_mac_addr_t   src_mac;    /* MAC address of local CMIC/NIC    */
    uint32      ether_type;      /* Remote CMIC Ethernet Type        */
    uint32      signature;       /* Rremote CMIC signature           */
    uint16      vlan;            /* Vlan id                          */
    uint16      tpid;            /* TPID                             */
    soc_port_t  port;            /* Port used for RCPU communication */
} soc_rcpu_cfg_t;

/* RX packet callback function. */
typedef int (*rcpu_rx_cb_f)(
    int unit, 
    uint8 *pkt,
    int len,
    void *cookie);

/* RX packet register function. */
typedef int (*rcpu_rx_register_f)(
    int unit, 
    rcpu_rx_cb_f rx,
    void *cookie);

/* RX packet register function. */
typedef int (*rcpu_rx_unregister_f)(
    int unit, 
    rcpu_rx_cb_f rx);

/* TX packet callback function. */
typedef int (*rcpu_tx_f)(
    int unit, 
    uint8 *pkt_buf,
    int len,
    void *cookie);

/* Packet buffer allocation function. */
typedef int (*rcpu_data_alloc_f)(
    int unit, 
    int size, 
    void **pkt_buf);

/* Packet buffer deallocation function. */
typedef int (*rcpu_data_free_f)(
    int unit, 
    void *pkt_buf);

/* Structure of transport related function pointers. */
typedef struct rcpu_trans_ptr_s {
    rcpu_rx_register_f   rcpu_tp_rx_register;
    rcpu_rx_unregister_f rcpu_tp_rx_unregister;
    rcpu_tx_f            rcpu_tp_tx;
    rcpu_data_alloc_f    rcpu_tp_data_alloc;
    rcpu_data_free_f     rcpu_tp_data_free;
    int tp_unit; /* device unit for in-band or ethernet unit for out-of-band */
    int ref_count;
}rcpu_trans_ptr_t;

/* Standard in-band transport pointer structure. */
extern rcpu_trans_ptr_t rcpu_trans_ptr;
#if defined(BCM_OOB_RCPU_SUPPORT)
/* Standard out-of-band transport pointer structure. */
extern rcpu_trans_ptr_t rcpu_oob_trans_ptr;
#endif

/* Operation encodings. */
#define SOC_RCPU_OP_SCHAN_REQ       0x01
#define SOC_RCPU_OP_SCHAN_REPLY     0x02
#define SOC_RCPU_OP_CMIC_REQ        0x03
#define SOC_RCPU_OP_CMIC_REPLY      0x04
#define SOC_RCPU_OP_TOCPU_PKT       0x10
#define SOC_RCPU_OP_FROMCPU_PKT     0x20
#define SOC_RCPU_OP_INTR_PKT        0x30

/* Flags encodings. */
#define SOC_RCPU_FLAG_REPLY         0x01
#define SOC_RCPU_FLAG_MINIPKT       0x02
#define SOC_RCPU_FLAG_MODHDR        0x04
#define SOC_RCPU_FLAG_FAIL          0x08
#define SOC_RCPU_FLAG_BUSY          0x20
#define SOC_RCPU_FLAG_TRUNCATED     0x40
#define SOC_RCPU_FLAG_JUMBO         0x80

/* CMIC packet header format (32 bytes) */
typedef struct rcpu1_cmic_pkt_hdr_s {
    sal_mac_addr_t      dst_mac;    /* Destination MAC address  */
    sal_mac_addr_t      src_mac;    /* Source MAC address       */
    uint16              tpid;       /* vlan TPID                */
    uint16              vlan;       /* vlan tag                 */
    uint16              ethertype;  /* Ether type               */
    uint16              signature;  /* RCPU packet signature    */
    uint8               operation;  /* op-code                  */
    uint8               flags;      /* flags                    */
    uint16              transid;    /* transaction number       */
    uint16              datalen;    /* length of data (in bytes)*/
    uint16              replen;     /* expected data length     */
    uint32              reserved;   /* reserved must be 0       */
} rcpu1_cmic_pkt_hdr_t;

typedef struct rcpu_encap_info_s {
    uint8               cmic_hdr_size;/* Received CMIC header size */
    uint8               dcb_size;     /* Received DCB size         */
    uint8               operation;    /* op-code                   */
    uint8               flags;        /* flags                     */
    uint16              transid;      /* transaction number        */
    uint16              datalen;      /* length of data (in bytes) */
    uint16              replen;       /* expected data length      */
} rcpu_encap_info_t;

/* ToCPU packet handler */
typedef int (*rcpu_tocpu_cb_f)(
    int unit, 
    uint8 *pkt,
    int len,
    rcpu_encap_info_t *info);

typedef struct soc_rcpu_control_s {
    sal_mutex_t         lock;
    sal_sem_t           schan_reply;
    uint32              flags;
    uint8               reply_data[CMIC_SCHAN_WORDS_ALLOC*4];
    rcpu_tocpu_cb_f     tocpu_cb;
    uint16              transid;
    /* some stats */
    uint32              tx_pkt;
    uint32              tx_fail;
    uint32              rx_pkt;
} soc_rcpu_control_t;

/* RCPU interrupt packet data format */
typedef struct soc_rcpu_intr_packet_s {
    uint32              cmc0_irq_stat;
    uint32              cmc0_rcpu_irq_mask;
    uint32              cmc1_irq_stat;
    uint32              cmc1_rcpu_irq_mask;
    uint32              cmc2_irq_stat;
    uint32              cmc2_rcpu_irq_mask;
    uint32              rcpu_irq0_stat;
    uint32              rcpu_irq0_mask;
    uint32              rcpu_irq1_stat;
    uint32              rcpu_irq1_mask;
    uint32              rcpu_irq2_stat;
    uint32              rcpu_irq2_mask;
    uint32              rcpu_irq3_stat;
    uint32              rcpu_irq3_mask;
    uint32              rcpu_irq4_stat;
    uint32              rcpu_irq4_mask;
    uint32              cmic_link_staus_0;
    uint32              cmic_link_staus_1;
    uint32              cmic_link_staus_2;
    uint32              cmic_link_staus_3;
} soc_rcpu_intr_packet_t;

int soc_rcpu_init(int unit);
int soc_rcpu_deinit(int unit);
int soc_rcpu_schan_op(int dev, schan_msg_t *msg, int dwc_write, int dwc_read);
uint32 soc_rcpu_cmic_read(int unit, uint32 addr);
int soc_rcpu_cmic_write(int unit, uint32 addr, uint32 data);

int soc_rcpu_encap(int unit, uint8 *buf, rcpu_encap_info_t *info);
int soc_rcpu_decap(uint8 *buf, int *runit, rcpu_encap_info_t *info);

int soc_rcpu_tocpu_cb_register(int unit, rcpu_tocpu_cb_f f);
int soc_rcpu_tocpu_cb_unregister(int unit);

#endif /* defined(INCLUDE_RCPU) */

#endif	/* _SOC_RCPU_H */
