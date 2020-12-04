/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __LINUX_BCM_KNET_H__
#define __LINUX_BCM_KNET_H__

#ifndef __KERNEL__
#include <stdint.h>
#endif

typedef struct  {
    int rc;
    int len;
    int bufsz;
    int reserved;
    uint64_t buf;
} bkn_ioctl_t;

#ifdef __KERNEL__

/*
 * Call-back interfaces for other Linux kernel drivers.
 */
#include <linux/skbuff.h>

typedef struct {
    uint32 netif_user_data;
    uint32 filter_user_data;
    uint16 dcb_type;
    int port;
    uint64_t ts;
    uint32 hwts;
} knet_skb_cb_t;

#define KNET_SKB_CB(_skb) ((knet_skb_cb_t *)_skb->cb)

typedef struct sk_buff *
(*knet_skb_cb_f)(struct sk_buff *skb, int dev_no, void *meta);

typedef int
(*knet_filter_cb_f)(uint8_t *pkt, int size, int dev_no, void *meta,
                    int chan, kcom_filter_t *filter);

typedef int
(*knet_hw_tstamp_enable_cb_f)(int dev_no, int phys_port, int tx_type);

typedef int
(*knet_hw_tstamp_tx_time_get_cb_f)(int dev_no, int phys_port, uint8_t *pkt, uint64_t *ts);

typedef int
(*knet_hw_tstamp_tx_meta_get_cb_f)(int dev_no, int hwts, int hdrlen, struct sk_buff *skb, uint64_t *ts, uint32_t **md);

typedef int
(*knet_hw_tstamp_ptp_clock_index_cb_f)(int dev_no);

typedef int
(*knet_hw_tstamp_rx_time_upscale_cb_f)(int dev_no, int phys_port, struct sk_buff *skb, uint32_t *meta, uint64_t *ts);

typedef int
(*knet_hw_tstamp_ioctl_cmd_cb_f)(kcom_msg_clock_cmd_t *kmsg, int len, int dcb_type);

extern int
bkn_rx_skb_cb_register(knet_skb_cb_f rx_cb);

extern int
bkn_rx_skb_cb_unregister(knet_skb_cb_f rx_cb);

extern int
bkn_tx_skb_cb_register(knet_skb_cb_f tx_cb);

extern int
bkn_tx_skb_cb_unregister(knet_skb_cb_f tx_cb);

extern int
bkn_filter_cb_register(knet_filter_cb_f filter_cb);

extern int
bkn_filter_cb_unregister(knet_filter_cb_f filter_cb);

extern int
bkn_hw_tstamp_enable_cb_register(knet_hw_tstamp_enable_cb_f hw_tstamp_enable_cb);

extern int
bkn_hw_tstamp_enable_cb_unregister(knet_hw_tstamp_enable_cb_f hw_tstamp_enable_cb);

extern int
bkn_hw_tstamp_disable_cb_register(knet_hw_tstamp_enable_cb_f hw_tstamp_disable_cb);

extern int
bkn_hw_tstamp_disable_cb_unregister(knet_hw_tstamp_enable_cb_f hw_tstamp_disable_cb);

extern int
bkn_hw_tstamp_tx_time_get_cb_register(knet_hw_tstamp_tx_time_get_cb_f hw_tstamp_tx_time_get_cb);

extern int
bkn_hw_tstamp_tx_time_get_cb_unregister(knet_hw_tstamp_tx_time_get_cb_f hw_tstamp_tx_time_get_cb);

extern int
bkn_hw_tstamp_tx_meta_get_cb_register(knet_hw_tstamp_tx_meta_get_cb_f hw_tstamp_tx_meta_get_cb);

extern int
bkn_hw_tstamp_tx_meta_get_cb_unregister(knet_hw_tstamp_tx_meta_get_cb_f hw_tstamp_tx_meta_get_cb);

extern int
bkn_hw_tstamp_ptp_clock_index_cb_register(knet_hw_tstamp_ptp_clock_index_cb_f hw_tstamp_ptp_clock_index_cb);

extern int
bkn_hw_tstamp_ptp_clock_index_cb_unregister(knet_hw_tstamp_ptp_clock_index_cb_f hw_tstamp_ptp_clock_index_cb);

extern int
bkn_hw_tstamp_rx_time_upscale_cb_register(knet_hw_tstamp_rx_time_upscale_cb_f hw_tstamp_rx_time_upscale_cb);

extern int
bkn_hw_tstamp_rx_time_upscale_cb_unregister(knet_hw_tstamp_rx_time_upscale_cb_f hw_tstamp_rx_time_upscale_cb);

extern int
bkn_hw_tstamp_ioctl_cmd_cb_register(knet_hw_tstamp_ioctl_cmd_cb_f hw_tstamp_ioctl_cmd_cb);

extern int
bkn_hw_tstamp_ioctl_cmd_cb_unregister(knet_hw_tstamp_ioctl_cmd_cb_f hw_tstamp_ioctl_cmd_cb);

#endif

#endif /* __LINUX_BCM_KNET_H__ */
