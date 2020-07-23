/*! \file bcmbd_cmicx_ccmdma.h
 *
 * CMICx CCMDMA data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_CCMDMA_H
#define BCMBD_CMICX_CCMDMA_H

#include <sal/sal_types.h>
#include <sal/sal_sem.h>
#include <sal/sal_spinlock.h>
#include <bcmbd/bcmbd_ccmdma.h>
#include <bcmbd/bcmbd_ccmdma_internal.h>

/*! Maximum number of CMCs in CMICx. */
#define CMICX_CCMDMA_CMC_MAX    2
/*! Mask of CMCs. */
#define CMICX_CCMDMA_CMC_MASK   0x3
/*! Maximum number of channels per CMC. */
#define CMICX_CCMDMA_CMC_CHAN   2
/*! Maximum number of channels in device. */
#define CMICX_CCMDMA_CHAN_MAX   (CMICX_CCMDMA_CMC_MAX * CMICX_CCMDMA_CMC_CHAN)
/*! Mask of channels per CMC. */
#define CMICX_CCMDMA_CHAN_MASK  0x3

/*! Wait microseconds. */
#define CMICX_CCMDMA_WAIT       200
/*! Retry times. */
#define CMICX_CCMDMA_RETRY      10000

/*!
 * \name Control flags.
 * \anchor CCMDMA_CF_xxx
 */

/*! \{ */
/*! Big-endianess mode. */
#define CCMDMA_CF_BIGENDIAN     (1 << 0)
/*! \} */

/*!
 * \name Device status.
 * \anchor CCMDMA_DS_xxx
 */

/*! \{ */
/*! Channels usage status. */
#define CCMDMA_DS_CHAN_BUSY     (1 << 0)
/*! \} */

struct cmicx_ccmdma_dev_s;

/*! CCMDMA operation. */
typedef int (*ccmdma_op_t)(struct cmicx_ccmdma_dev_s *dev,
                           bcmbd_ccmdma_work_t *work);

/*!
 * \brief CMICx CCMDMA device structure.
 */
typedef struct cmicx_ccmdma_dev_s {
    /*! Device number. */
    int unit;

    /*! Device control. */
    ccmdma_ctrl_t *ctrl;

    /*! Bitmap of CMCs enabled. */
    uint32_t bm_cmc;

    /*! Bitmap of channels enabled. */
    uint32_t bm_chan;

    /*! Interrupt source. */
    uint32_t (*int_src)[CMICX_CCMDMA_CMC_CHAN];

    /*! Semaphore for channel management. */
    sal_sem_t sem;

    /*! Spinlock for channel management. */
    sal_spinlock_t lock;

    /*! Operation vector. */
    ccmdma_op_t ccmdma_op;

    /*! Control flags (\ref CCMDMA_CF_xxx). */
    uint32_t flags;

    /*! Device status (\ref CCMDMA_DS_xxx). */
    uint32_t status;

    /*! Device initialization indicator. */
    int inited;
} cmicx_ccmdma_dev_t;

/*!
 * \brief Get CCMDMA channel.
 *
 * \param [in] dev Device structure point.
 * \param [in] cmc CMC number.
 * \param [in] ch Channel number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
cmicx_ccmdma_chan_get(cmicx_ccmdma_dev_t *dev, int *cmc, int *chan);

/*!
 * \brief Put CCMDMA channel.
 *
 * \param [in] dev Device structure point.
 * \param [in] cmc CMC number.
 * \param [in] ch Channel number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
cmicx_ccmdma_chan_put(cmicx_ccmdma_dev_t *dev, int cmc, int chan);

/*!
 * \brief CCMDMA transfer.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_ccmdma_xfer(int unit, bcmbd_ccmdma_work_t *work);

#endif /* BCMBD_CMICX_CCMDMA_H */
