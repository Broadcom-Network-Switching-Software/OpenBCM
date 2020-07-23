/*! \file bcmbd_cmicx_fifodma.h
 *
 * CMICx FIFODMA data structures
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_FIFODMA_H
#define BCMBD_CMICX_FIFODMA_H

#include <sal/sal_types.h>
#include <bcmbd/bcmbd_fifodma.h>
#include <bcmbd/bcmbd_fifodma_internal.h>

/*! Maximum number of CMCs in CMICx */
#define CMICX_FIFODMA_CMC_MAX   1
/*! Mask of CMCs */
#define CMICX_FIFODMA_CMC_MASK  0x1
/*! Maximum number of channels per CMC */
#define CMICX_FIFODMA_CMC_CHAN  12
/*! Maximum number of channels in device */
#define CMICX_FIFODMA_CHAN_MAX  (CMICX_FIFODMA_CMC_MAX * CMICX_FIFODMA_CMC_CHAN)
/*! Mask of channels per CMC */
#define CMICX_FIFODMA_CHAN_MASK 0xfff
/*! Maximum number of selectabe host memory entries */
#define CMICX_FIFODMA_SEL_MAX   10

/*! Wait microseconds */
#define CMICX_FIFODMA_WAIT      200
/*! Retry times */
#define CMICX_FIFODMA_RETRY     10000

/*!
 * \name Control flags.
 * \anchor FIFODMA_CF_xxx
 */

/*! \{ */
/*! Big-endianess mode */
#define FIFODMA_CF_BIGENDIAN    (1 << 0)
/*! \} */

/*!
 * \brief CMICx FIFODMA device structure.
 */
typedef struct cmicx_fifodma_dev_s {
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
} cmicx_fifodma_dev_t;

#endif /* BCMBD_CMICX_FIFODMA_H */
