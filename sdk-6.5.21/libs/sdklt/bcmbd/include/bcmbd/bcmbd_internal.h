/*! \file bcmbd_internal.h
 *
 * Internal Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_INTERNAL_H
#define BCMBD_INTERNAL_H

#include <bcmbd/bcmbd_sbusdma_internal.h>
#include <bcmbd/bcmbd_fifodma_internal.h>
#include <bcmbd/bcmbd_ccmdma_internal.h>
#include <bcmbd/bcmbd_schanfifo_internal.h>
#include <bcmbd/bcmbd_config.h>

/*!
 * \brief Get pointer to SBUSDMA device control structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device control structure, or NULL if not found.
 */
extern sbusdma_ctrl_t *
bcmbd_sbusdma_ctrl_get(int unit);

/*!
 * \brief Get pointer to FIFODMA device control structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device control structure, or NULL if not found.
 */
extern fifodma_ctrl_t *
bcmbd_fifodma_ctrl_get(int unit);

/*!
 * \brief Get pointer to CCMDMA device control structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device control structure, or NULL if not found.
 */
extern ccmdma_ctrl_t *
bcmbd_ccmdma_ctrl_get(int unit);

/*!
 * \brief Get pointer to SCHAN FIFO device control structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device control structure, or NULL if not found.
 */
extern schanfifo_ctrl_t *
bcmbd_schanfifo_ctrl_get(int unit);

#endif /* BCMBD_INTERNAL_H */
