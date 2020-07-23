/*! \file bcm56880_a0_pvt.h
 *
 * Power Voltage Thermal control header for BCM56880_A0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_PVT_H
#define BCM56880_A0_PVT_H

/*! Maximum unit supported. */
#define MAX_UNITS                  BCMDRD_CONFIG_MAX_UNITS

/*! Default temperature to trigger hardware reset. */
#define HW_RESET_TEMPERATURE 115

/*! Default temperature to trigger software interrupt. */
#define SW_MIN_INTERRUPT_TEMPERATURE 105

/*! Default data to trigger software MAX interrupt. */
#define SW_MAX_INTERRUPT_THRESHOLD 0x7FF

/*! Index number of PVT sensor used for AVS. */
#define AVS_SENSOR     15

/*! Total PVT sensor count. */
#define PVT_SENSOR_CNT 16

/*! Total control register count. */
#define CTRL_REG_CNT   6

/*! Max threshold interrupt mask bit. */
#define INTR_MASK_MAX (1L << 0)

/*! Min threshold interrupt mask bit. */
#define INTR_MASK_MIN (1L << 1)

/*! Max threshold interrupt status bit. */
#define INTR_STATUS_MAX (1L << 0)

/*! Min threshold interrupt status bit. */
#define INTR_STATUS_MIN (1L << 1)

/*!
 * \brief PVT module reset and init.
 *
 * This function reset PVT module and init threshold configuration.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Firmware successfully loaded.
 * \retval SHR_E_FAIL Failed to load firmware.
 */
extern int
bcm56880_a0_pvt_init(int unit);

#endif /* BCM56880_A0_PVT_H */
