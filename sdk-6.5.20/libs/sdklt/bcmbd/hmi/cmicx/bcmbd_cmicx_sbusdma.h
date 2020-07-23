/*! \file bcmbd_cmicx_sbusdma.h
 *
 * CMICx SBUSDMA data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_SBUSDMA_H
#define BCMBD_CMICX_SBUSDMA_H

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_spinlock.h>
#include <bcmbd/bcmbd_sbusdma.h>
#include <bcmbd/bcmbd_sbusdma_internal.h>

/*! Maximum number of CMCs in CMICx. */
#define CMICX_SBUSDMA_CMC_MAX       2
/*! Mask of CMCs. */
#define CMICX_SBUSDMA_CMC_MASK      0x3
/*! Maximum number of channels per CMC. */
#define CMICX_SBUSDMA_CMC_CHAN      8
/*! Maximum number of channels in device. */
#define CMICX_SBUSDMA_CHAN_MAX      (CMICX_SBUSDMA_CMC_MAX * CMICX_SBUSDMA_CMC_CHAN)

/*! Wait microseconds. */
#define CMICX_SBUSDMA_WAIT          200
/*! Retry times. */
#define CMICX_SBUSDMA_RETRY         10000

/*!
 * \brief CMICx SBUSDMA descriptor (8 Dwords).
 */
typedef struct cmicx_sbusdma_desc_s {
    /*! Control word. */
    uint32_t control;

    /*! Request word. */
    uint32_t request;

    /*! Count word. */
    uint32_t count;

    /*! Opcode word. */
    uint32_t opcode;

    /*! Address word. */
    uint32_t address;

    /*! Hostaddr LO word. */
    uint32_t hostaddr_lo;

    /*! Hostaddr HI word. */
    uint32_t hostaddr_hi;

    /*! Request_1 word. */
    uint32_t request_1;
} cmicx_sbusdma_desc_t;

/*!
 * \name Control word definitions.
 */

/*! \{ */
#define CMICX_SBUSDMA_DESC_CTRL_LAST    (1 << 31)
#define CMICX_SBUSDMA_DESC_CTRL_SKIP    (1 << 30)
#define CMICX_SBUSDMA_DESC_CTRL_JUMP    (1 << 29)
#define CMICX_SBUSDMA_DESC_CTRL_APND    (1 << 28)
#define CMICX_SBUSDMA_DESC_CTRL_REMN(x) (((x) & 0xf) << 24)
#define CMICX_SBUSDMA_DESC_BURST_MAX    (128 / sizeof(cmicx_sbusdma_desc_t))
/*! \} */

/*!
 * \brief Descriptors address container.
 */
typedef struct cmicx_sbusdma_desc_addr_s {
    /*! DMA memory physical address. */
    uint64_t paddr;

    /*! DMA memory address. */
    void *addr;

    /*! DMA memory size. */
    uint32_t size;
} cmicx_sbusdma_desc_addr_t;

/*!
 * \name Control flags.
 * \anchor SBUSDMA_CF_xxx
 */

/*! \{ */
/*! Descriptor in big-endianess mode. */
#define SBUSDMA_CF_DESC_BIGENDIAN       (1 << 0)

/*! Memory read in big-endianess mode. */
#define SBUSDMA_CF_WR_BIGENDIAN         (1 << 1)

/*! Memory write in big-endianess mode. */
#define SBUSDMA_CF_RD_BIGENDIAN         (1 << 2)
/*! \} */

/*!
 * \name Device status.
 * \anchor SBUSDMA_DS_xxx
 */

/*! \{ */
/*! Channels usage status. */
#define SBUSDMA_DS_CHAN_BUSY            (1 << 0)
/*! \} */

/*!
 * \brief CMICx SBUSDMA device structure.
 */
typedef struct cmicx_sbusdma_dev_s {
    /*! Device number. */
    int unit;

    /*! SBUSDMA control. */
    sbusdma_ctrl_t *ctrl;

    /*! Bitmap of CMCs enabled. */
    uint32_t bm_cmc;

    /*! Bitmap of channels enabled. */
    uint32_t bm_chan;

    /*! Semaphore for channel management. */
    sal_sem_t sem;

    /*! Spinlock for channel management. */
    sal_spinlock_t lock;

    /*! Control flags (\ref SBUSDMA_CF_xxx). */
    uint32_t flags;

    /*! Device status (\ref SBUSDMA_DS_xxx). */
    uint32_t status;

    /*! Device initialization indicator. */
    int inited;
} cmicx_sbusdma_dev_t;

/*!
 * \brief Execute light work in register mode.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_sbusdma_reg_op(int unit, bcmbd_sbusdma_work_t *work);

#endif /* BCMBD_CMICX_SBUSDMA_H */
