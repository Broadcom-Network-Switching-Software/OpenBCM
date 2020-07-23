/*! \file bcmdrd_feature.h
 *
 * BCMDRD Feature Interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_FEATURE_H
#define BCMDRD_FEATURE_H

#include <sal/sal_types.h>

/*!
 * \brief Device features which are shared across all device families.
 *
 * \c BCMDRD_FT_ALL can be passed to \ref bcmdrd_feature_disable to
 * clear all features.
 */
typedef enum bcmdrd_feature_e {
    BCMDRD_FT_ALL = 0,
    BCMDRD_FT_CMICM,
    BCMDRD_FT_CMICD,
    BCMDRD_FT_CMICX,
    BCMDRD_FT_CMICX2,
    BCMDRD_FT_IPROC,
    BCMDRD_FT_PASSIVE_SIM,
    BCMDRD_FT_ACTIVE_SIM,
    BCMDRD_FT_EMUL,
    BCMDRD_FT_KNET,
    BCMDRD_FT_FLEXCODE,
    BCMDRD_FT_PHYMOD_SIM,
    BCMDRD_FT_COUNT
} bcmdrd_feature_t;

/*!
 * \brief Set a device feature as present.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Feature to enable for this device.
 */
extern void
bcmdrd_feature_enable(int unit, bcmdrd_feature_t feature);

/*!
 * \brief Set a device feature as not present.
 *
 * Passing feature \c BCMDRD_FT_ALL will clear all features.
 *
 * \param [in] unit Unit number.
 * \param [in] feature Feature to disable for this device.
 */
extern void
bcmdrd_feature_disable(int unit, bcmdrd_feature_t feature);

/*!
 * \brief Check if a feature is present for a device.
 *
 * \param [in] unit Unit number
 * \param [in] feature Feature to check for.
 *
 * \return true if feature is present, otherwise false.
 */
extern bool
bcmdrd_feature_enabled(int unit, bcmdrd_feature_t feature);

/*!
 * \brief Test if running on real hardware.
 *
 * Convenience function to test that we are not running on a simulated
 * or emulated device.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Running on real hardware.
 * \retval false Running on a simulated or emulated device.
 */
bool
bcmdrd_feature_is_real_hw(int unit);

/*!
 * \brief Test if running on a simulated device.
 *
 * Convenience function to test if we are running on a simulated
 * device.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Running on a simulated device.
 * \retval false Running on real hardware or an emulated device.
 */
bool
bcmdrd_feature_is_sim(int unit);

#endif /* BCMDRD_FEATURE_H */

