/*! \file mcs_bs_common.h
 *
 * Broadsync (BS) common definitions shared with
 * the uKernel.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MCS_BS_COMMON_H
#define MCS_BS_COMMON_H

/*! Max timeout for messaging between SDK and UKERNEL */
#define MCS_BS_MAX_UC_MSG_TIMEOUT (5000000) /* 5 secs */

/*! Max size of data that can be exchaned with firmware using DMA */
#define MAX_BS_DMA_MSG_SIZE_BYTES 250

/*! Structure for shared memory used for debug data. */
typedef struct mcs_bs_log_info_s {
    /*! Size of allocated memory. */
    volatile uint32_t size;
    /*! Last written index. */
    volatile uint32_t head;
    /*! Last read index. */
    uint32_t tail;
    /*! Actual data begin. */
    volatile uint8_t buf[4];
} mcs_bs_log_info_t;

#endif /* MCS_BFD_COMMON_H */

