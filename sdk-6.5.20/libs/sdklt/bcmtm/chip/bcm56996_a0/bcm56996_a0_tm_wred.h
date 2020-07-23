/*! \file bcm56996_a0_tm_wred.h
 *
 * File containing wred related defines and internal function for
 * bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TM_WRED_H
#define BCM56996_A0_TM_WRED_H

#include <bcmpc/bcmpc_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/wred/bcmtm_wred_internal.h>

/*! Queue average offset for congestion profile. */
#define TH4G_WRED_CNG_Q_AVG_OFFSET 0
/*! Queue minimum offset for congestion profile. */
#define TH4G_WRED_CNG_Q_MIN_OFFSET 1
/*! Service pool offset for congestion profile. */
#define TH4G_WRED_CNG_SERVICE_POOL_OFFSET 2
/*!
 * Number of entries per congestion profile.
 *  | SP | Q_MIN | Q_AVG
 *   3  2|   1   |   0
 * SP : 00 Low congestion
 *      01 Medium congestion
 *      11 High congestion
 * Q_MIN and Q_AVG: 0 (no congestion) and 1 (congestion).
 */
#define TH4G_WRED_CNG_ENTRY_PER_PROFILE 12
/*! Maximum number of congestion profile. */
#define TH4G_WRED_CNG_MAX_PROFILE 4

/*!
 * \brief  device specific wred initialization.
 *
 * \param [in] unit         unit number.
 *
 * \retval shr_e_none       no error.
 * \retval !shr_e_none      error.
 */

extern int
bcm56996_a0_tm_wred_enable(int unit);

/*!
 * \brief  Device specific wred initialization.
 *
 * \param [in] unit         Unit number.
 *
 * \retval SHR_E_NONE       No error.
 * \retval SHR_E_UNIT       Invalid Unit number.
 */
extern int
bcm56996_a0_tm_wred_chip_init(int unit, bool warm);

/*!
 * \brief TH4G related function pointers.
 *
 * \param [in] unit Unit Number.
 * \param [out] wred_drv WRED driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Unit information not found.
 */
extern int
bcm56996_a0_tm_wred_drv_get(int unit, void *wred_drv);

#endif /* BCM56996_A0_TM_WRED_H */
