/*! \file bcmbd_cmicd_ccmdma.h
 *
 * CMICd CCMDMA data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_CCMDMA_H
#define BCMBD_CMICD_CCMDMA_H

#include <sal/sal_types.h>
#include <sal/sal_spinlock.h>
#include <bcmbd/bcmbd_ccmdma.h>
#include <bcmbd/bcmbd_ccmdma_internal.h>

/*! Maximum number of CMCs in CMICd. */
#define CMICD_CCMDMA_CMC_MAX    3
/*! Mask of CMCs */
#define CMICD_CCMDMA_CMC_MASK   0x7
/*! Maximum number of channels per CMC. */
#define CMICD_CCMDMA_CMC_CHAN   1
/*! Maximum number of channels in device. */
#define CMICD_CCMDMA_CHAN_MAX   (CMICD_CCMDMA_CMC_MAX * CMICD_CCMDMA_CMC_CHAN)
/*! Mask of channels per CMC. */
#define CMICD_CCMDMA_CHAN_MASK  0x1

/*! Wait microseconds. */
#define CMICD_CCMDMA_WAIT       200
/*! Retry times. */
#define CMICD_CCMDMA_RETRY      10000

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

/*!
 * \brief CMICd CCMDMA device structure.
 */
typedef struct cmicd_ccmdma_dev_s {
    /*! Device number. */
    int unit;

    /*! Device control. */
    ccmdma_ctrl_t *ctrl;

    /*! Bitmap of CMCs enabled. */
    uint32_t bm_cmc;

    /*! Bitmap of channels enabled. */
    uint32_t bm_chan;

    /*! Semaphore for channel management. */
    sal_sem_t sem;

    /*! Spinlock for channel management. */
    sal_spinlock_t lock;

    /*! Control flags (\ref CCMDMA_CF_xxx). */
    uint32_t flags;

    /*! Device status (\ref CCMDMA_DS_xxx). */
    uint32_t status;

    /*! Device initialization indicator. */
    int inited;
} cmicd_ccmdma_dev_t;

/*!
 * \brief CCMDMA transfer.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_ccmdma_xfer(int unit, bcmbd_ccmdma_work_t *work);

#endif /* BCMBD_CMICD_CCMDMA_H */
