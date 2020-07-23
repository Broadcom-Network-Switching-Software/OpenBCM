/*! \file bcmbd_cmicx_schanfifo.h
 *
 * CMICx SCHAN FIFO data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_SCHANFIFO_H
#define BCMBD_CMICX_SCHANFIFO_H

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_schanfifo_internal.h>

/*! Maximum number of channels. */
#define CMICX_SCHANFIFO_CHAN_MAX        2
/*! Maximum number of commands. */
#define CMICX_SCHANFIFO_CMDS_MAX        176
/*! Maximum word size of command memory. */
#define CMICX_SCHANFIFO_CMDS_WSIZE      352
/*! Response size per command. */
#define CMICX_SCHANFIFO_RESP_SIZE       128
/*! Summary size per command. */
#define CMICX_SCHANFIFO_SUM_SIZE        2
/*! AXI ID. */
#define CMICX_SCHANFIFO_AXI_ID          0x5
/*! Summary update interval. */
#define CMICX_SCHANFIFO_SUM_UI          0x10
/*! Wait time in Sec. */
#define CMICX_SCHANFIFO_WAIT_TIME       SECOND_USEC
/*! Default poll times. */
#define CMICX_SCHANFIFO_POLLS_DFLT      10000

/*!
 * \name Endian control flags.
 * \anchor SCHANFIFO_EF_xxx
 */

/*! \{ */
/*! Big-endianess mode. */
#define SCHANFIFO_EF_BIGENDIAN          (1 << 0)
/*! \} */

/*!
 * \brief CMICx SCHAN FIFO device structure.
 */
typedef struct cmicx_schanfifo_dev_s {
    /*! Device number. */
    int unit;

    /*! Device control. */
    schanfifo_ctrl_t *ctrl;

    /*! Base of command memory. */
    uint64_t cmds_base;

    /*! Response buffer. */
    void *resp_addr[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Response buffer physical address. */
    uint64_t resp_paddr[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Response size. */
    uint32_t resp_size[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Summary buffer. */
    void *sum_addr[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Summary buffer physical address. */
    uint64_t sum_paddr[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Summary size. */
    uint32_t sum_size[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Semaphore for interrupt. */
    sal_sem_t intr[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Cache for control register. */
    CMIC_SCHANFIFO_CTRLr_t ctl_reg[CMICX_SCHANFIFO_CHAN_MAX];

    /*! Polling times. */
    uint32_t polls;

    /*! Control flags. */
    uint32_t flags;

    /*! Endian control. */
    uint32_t endians;

    /*! Device initialization indicator. */
    int inited;
} cmicx_schanfifo_dev_t;

#endif /* BCMBD_CMICX_SCHANFIFO_H */
