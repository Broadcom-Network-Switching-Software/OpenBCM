/*! \file udf.h
 *
 * UDF headfiles to declare internal APIs for BCM56880_A0 device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCM56880_A0_LTSW_UDF_H
#define BCM56880_A0_LTSW_UDF_H

/*!
 * \brief Initialize UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHE_E_NONE No errors.
 */
extern int
bcm56880_a0_ltsw_udf_init(int unit);


#endif /* BCM56880_A0_LTSW_UDF_H */
