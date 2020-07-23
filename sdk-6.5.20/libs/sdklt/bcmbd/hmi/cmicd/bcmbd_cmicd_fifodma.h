/*! \file bcmbd_cmicd_fifodma.h
 *
 * CMICd FIFODMA data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_FIFODMA_H
#define BCMBD_CMICD_FIFODMA_H

#include <sal/sal_types.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_fifodma_internal.h>

/*! Maximum number of CMCs in CMICd */
#define CMICD_FIFODMA_CMC_MAX   3
/*! Mask of CMCs */
#define CMICD_FIFODMA_CMC_MASK  0x7
/*! Maximum number of channels per CMC */
#define CMICD_FIFODMA_CMC_CHAN  4
/*! Maximum number of channels in device */
#define CMICD_FIFODMA_CHAN_MAX  (CMICD_FIFODMA_CMC_MAX * CMICD_FIFODMA_CMC_CHAN)
/*! Mask of channels per CMC */
#define CMICD_FIFODMA_CHAN_MASK 0xf
/*! Maximum number of selectabe host memory entries */
#define CMICD_FIFODMA_SEL_MAX   8

/*! Wait microseconds */
#define CMICD_FIFODMA_WAIT      200
/*! Retry times */
#define CMICD_FIFODMA_RETRY     10000

/*!
 * \name Control flags.
 * \anchor FIFODMA_CF_xxx
 */

/*! \{ */
/*! Big-endianess mode */
#define FIFODMA_CF_BIGENDIAN    (1 << 0)
/*! \} */

/*!
 * \brief CMICd FIFODMA device structure.
 */
typedef struct cmicd_fifodma_dev_s {
    /*! Device number */
    int unit;

    /*! FIFODMA control */
    fifodma_ctrl_t *ctrl;

    /*! Bitmap of CMCs enabled */
    uint32_t bm_cmc;

    /*! Bitmap of channels enabled */
    uint32_t bm_chan;

    /*! Control flags (\ref FIFODMA_CF_xxx). */
    uint32_t flags;

    /*! Device initialization indicator */
    int inited;
} cmicd_fifodma_dev_t;

#endif /* BCMBD_CMICD_FIFODMA_H */
