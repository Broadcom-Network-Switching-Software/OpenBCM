/*! \file bcmbd_cmicx2.h
 *
 * Definitions for CMICv v2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX2_H
#define BCMBD_CMICX2_H

#include <bcmbd/bcmbd_cmicx.h>

/*! Number of 32-bit words in S-channel message buffer */
#define BCMBD_CMICX2_SCHAN_WORDS        32

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx2_feature_init(int unit);

/*!
 * \brief Reset M0SSQ block.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE Successed.
 * \retval SHR_E_FAIL Failed.
 */
extern int
bcmbd_cmicx2_m0ssq_reset(int unit);

#endif /* BCMBD_CMICX2_H */
