/*! \file bcmbd_ccmdma.h
 *
 * CCMDMA data structures and APIs
 *
 * Cross Couple Memory (CCM) DMA engines provides the ability to transfer a
 * block of data from one host memory space to another.
 * This is particularly useful when two hosts do not share any memory regions,
 * but it can also be used to move data within the the memory space of a single
 * host.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CCMDMA_H
#define BCMBD_CCMDMA_H

#include <sal/sal_types.h>
#include <bcmdrd_config.h>

/*! Maximum number of CCMDMA devices supported. */
#define CCMDMA_DEV_NUM_MAX      BCMDRD_CONFIG_MAX_UNITS

/*!
 * \name Work flags.
 * \anchor CCMDMA_WF_xxx
 */

/*! \{ */
/*! Use interrupt mode. */
#define CCMDMA_WF_INT_MODE      (1 << 0)
/*! Source in inner/device memory. */
#define CCMDMA_WF_SRC_INNER     (1 << 1)
/*! Destination in outer/host memory. */
#define CCMDMA_WF_DST_OUTER     (1 << 2)
/*! \} */

/*!
 * \brief CCMDMA work.
 */
typedef struct bcmbd_ccmdma_work_s {
    /*! Source memory physical address. */
    uint64_t src_paddr;

    /*! Destination memory physical address. */
    uint64_t dst_paddr;

    /*! Transfer count in 32-bit words. */
    uint32_t xfer_count;

    /*! Work flags (\ref CCMDMA_WF_xxx). */
    uint32_t flags;
} bcmbd_ccmdma_work_t;

/*!
 * \brief Attach CCMDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_ccmdma_attach(int unit);

/*!
 * \brief Detach CCMDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_ccmdma_detach(int unit);

/*!
 * \brief CCMDMA transfer.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_ccmdma_xfer(int unit, bcmbd_ccmdma_work_t *work);

#endif /* BCMBD_CCMDMA_H */
