/*! \file bcm56990_a0_tm_device_rxq.h
 *
 * File having TM device RX queue chip specific defines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_TM_DEVICE_RXQ_H
#define BCM56990_A0_TM_DEVICE_RXQ_H


/*!
 * \brief bcm56990 TM device rx queue driver get.
 *
 * \param [in] unit Logical unit.
 * \param [out] rxq_drv RX queue driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_INTERNAL rxq_drv is NULL.
 */
extern int
bcm56990_a0_tm_rxq_drv_get(int unit, void *rxq_drv);

#endif /* BCM56990_A0_TM_DEVICE_RXQ_H */
