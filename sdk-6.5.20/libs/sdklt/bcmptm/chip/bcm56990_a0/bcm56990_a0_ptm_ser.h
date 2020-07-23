/*!\file bcm56990_a0_ptm_ser.h
*
*APIs of bcm56990_a0_ptm_ser.c
 */
/*
*$Copyright:.$
*/

#ifndef BCM56990_A0_PTM_SER_H
#define BCM56990_A0_PTM_SER_H

#include "bcmptm_common_ser.h"

/* Get data driver from bcm56990_a0 */
extern const bcmptm_ser_data_driver_t *
bcm56990_a0_ptm_ser_data_driver_register(int unit);

#endif /* BCM56990_A0_PTM_SER_H */

