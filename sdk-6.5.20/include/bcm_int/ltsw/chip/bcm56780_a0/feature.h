/*! \file feature.h
 *
 * LTSW feature header file.
 * This file contains the definitions of feature.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_LTSW_FEATURE_H
#define BCM56780_A0_LTSW_FEATURE_H

/*!
 * \brief Initialize features to be supported.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56780_a0_ltsw_feature_init(int unit);

#endif  /* BCM56780_A0_LTSW_FEATURE_H */
