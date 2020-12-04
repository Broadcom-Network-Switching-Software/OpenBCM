/*! \file bcm56880_a0_ptm_ser.h
 *
 * APIs of bcm56880_a0_ptm_ser.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_PTM_SER_H
#define BCM56880_A0_PTM_SER_H

#include "bcmptm_common_ser.h"

/* Get data driver from bcm56880_a0 */
extern const bcmptm_ser_data_driver_t *
bcm56880_a0_ptm_ser_data_driver_register(int unit);

#endif /* BCM56880_A0_PTM_SER_H */

