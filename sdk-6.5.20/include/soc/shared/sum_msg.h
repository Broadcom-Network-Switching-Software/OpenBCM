/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    shr_sum_msg.h
 */

#ifndef SHR_SUM_MSG_H_
#define SHR_SUM_MSG_H_

#include <soc/shared/sum.h>
#include <soc/shared/shr_pkt.h>
#include <shared/pack.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif

/*
 * SUM config information
 */
typedef struct __COMPILER_ATTR__ shr_sum_msg_ctrl_config_s {
    /* Host buffer address to update stats information */
    uint32 host_buf_addr;

    /* sampling interval */
    uint32 usec_sample_interval;

    /* Maximum samples */
    uint32 max_history;

    /* Type of counter to be monitor packet/byte/both */
    uint32 sum_type;

    /* Number of ports being monitored */
    uint32 num_ports;

    /* List of physical ports */
    uint32 pports[SHR_SUM_MAX_PORTS];

    /* List of logical ports */
    uint32 lports[SHR_SUM_MAX_PORTS];

    /* MIB Table Base Address */
    uint32 mib_tbl_addr;

    /* MIB Table Acc Type */
    uint8 mib_tbl_acc_type;

    /* SUM WARM BOOT INDICATOR */
    uint8 sum_wb_indicate;
} shr_sum_msg_ctrl_config_t;

/* SUM usage information */
typedef struct __COMPILER_ATTR__ shr_sum_msg_buf_info_s {
    /* Host buffer current offset */
    uint32 host_buf_cur_offset;

    /* Host buffer status */
    uint32 host_buf_status;

    /* sampling interval */
    uint32 sample_interval;

} shr_sum_msg_buf_info_t;
/*
 *  SUM control messages
 */
typedef union shr_sum_msg_ctrl_s {
    shr_sum_msg_ctrl_config_t           config_info;
    shr_sum_msg_buf_info_t              buf_info;
} shr_sum_msg_ctrl_t;
#endif /* SHR_SUM_MSG_H_ */
