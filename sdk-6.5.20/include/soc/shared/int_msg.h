/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    shr_int_msg.h
 */

#ifndef SHR_INT_MSG_H_
#define SHR_INT_MSG_H_

#include <soc/shared/int.h>
#include <soc/shared/shr_pkt.h>
#include <shared/pack.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif


typedef struct __COMPILER_ATTR__ shr_int_turnaround_msg_ctrl_init_s {
    /* Local RX DMA channel */
    uint32 rx_channel;

    /* Local TX DMA channel */
    uint32 tx_channel;

} shr_int_turnaround_msg_ctrl_init_t;

/*
 * Create INT Turnaround config information
 */
typedef struct __COMPILER_ATTR__ shr_int_turnaround_msg_ctrl_config_s {
    /* INT Turnaround Source mac */
    bcm_mac_t        src_mac;

    /* Length of the INT Turnaround LB header encapsulation */
    uint16 lb_encap_length;

    /* LoopBack header encapsulation */
    uint8 lb_encap[SHR_LB_HEADER_LENGTH];
} shr_int_turnaround_msg_ctrl_config_t;

/*
 * Create INT Turnaround statistics information
 */
typedef struct __COMPILER_ATTR__ shr_int_turnaround_msg_ctrl_stat_s {
    /* Number of packet received */
    uint32     rx_pkt_cnt_hi;
    uint32     rx_pkt_cnt_lo;

    /* Number of packet transmitted */
    uint32     tx_pkt_cnt_hi;
    uint32     tx_pkt_cnt_lo;

    /* Number of non INT pkt dropped */
    uint32     drop_pkt_cnt_hi;
    uint32     drop_pkt_cnt_lo;

    /* Number of pkt dropped due to missing configuration */
    uint32     int_init_config_drop_hi;
    uint32     int_init_config_drop_lo;

    /* Number of pkt dropped due to Hop count exceeded Hop limit */
    uint32     int_hop_cnt_invalid_drop_hi;
    uint32     int_hop_cnt_invalid_drop_lo;

    /* Number of pkt dropped due to current length exceeded maximum length */
    uint32     int_hdr_len_invalid_drop_hi;
    uint32     int_hdr_len_invalid_drop_lo;

    /* Number of pkt dropped due to invalid packet size */
    uint32     int_pkt_size_invalid_drop_hi;
    uint32     int_pkt_size_invalid_drop_lo;

} shr_int_turnaround_msg_ctrl_stat_t;

/*
 *  INT Turnaround control messages
 */
typedef union shr_int_turnaround_msg_ctrl_s {
    shr_int_turnaround_msg_ctrl_init_t            int_init;
    shr_int_turnaround_msg_ctrl_config_t          int_turnaround_config_info;
    shr_int_turnaround_msg_ctrl_stat_t            int_turnaround_stat_info;
} shr_int_turnaround_msg_ctrl_t;
#endif /* SHR_INT_MSG_H_ */
