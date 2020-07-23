/*! \file bcm56990_b0_pvt_intr.h
 *
 * PVT interrupt sources for BCM56990_B0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_PVT_INTR_H
#define BCM56990_B0_PVT_INTR_H

/*!
 * \name PVT interrupt sources.
 *
 * The PVT interrupts are organized into a single bit array, the bit order
 * reflects the bit order in the hardware registers. Each of 16 PVT monitors
 * has two interrupt sources: minimal temperature and maximal temperature.
 *
 * Please refer to the hardware documentation for a description of the
 * individual interrupt sources.
 *
 * \{
 */

#define PVT_INTR_BASE               0

#define PVT_INTR_MON_0_MAX          (PVT_INTR_BASE + 0)
#define PVT_INTR_MON_0_MIN          (PVT_INTR_BASE + 1)
#define PVT_INTR_MON_1_MAX          (PVT_INTR_BASE + 2)
#define PVT_INTR_MON_1_MIN          (PVT_INTR_BASE + 3)
#define PVT_INTR_MON_2_MAX          (PVT_INTR_BASE + 4)
#define PVT_INTR_MON_2_MIN          (PVT_INTR_BASE + 5)
#define PVT_INTR_MON_3_MAX          (PVT_INTR_BASE + 6)
#define PVT_INTR_MON_3_MIN          (PVT_INTR_BASE + 7)
#define PVT_INTR_MON_4_MAX          (PVT_INTR_BASE + 8)
#define PVT_INTR_MON_4_MIN          (PVT_INTR_BASE + 9)
#define PVT_INTR_MON_5_MAX          (PVT_INTR_BASE + 10)
#define PVT_INTR_MON_5_MIN          (PVT_INTR_BASE + 11)
#define PVT_INTR_MON_6_MAX          (PVT_INTR_BASE + 12)
#define PVT_INTR_MON_6_MIN          (PVT_INTR_BASE + 13)
#define PVT_INTR_MON_7_MAX          (PVT_INTR_BASE + 14)
#define PVT_INTR_MON_7_MIN          (PVT_INTR_BASE + 15)
#define PVT_INTR_MON_8_MAX          (PVT_INTR_BASE + 16)
#define PVT_INTR_MON_8_MIN          (PVT_INTR_BASE + 17)
#define PVT_INTR_MON_9_MAX          (PVT_INTR_BASE + 18)
#define PVT_INTR_MON_9_MIN          (PVT_INTR_BASE + 19)
#define PVT_INTR_MON_10_MAX         (PVT_INTR_BASE + 20)
#define PVT_INTR_MON_10_MIN         (PVT_INTR_BASE + 21)
#define PVT_INTR_MON_11_MAX         (PVT_INTR_BASE + 22)
#define PVT_INTR_MON_11_MIN         (PVT_INTR_BASE + 23)
#define PVT_INTR_MON_12_MAX         (PVT_INTR_BASE + 24)
#define PVT_INTR_MON_12_MIN         (PVT_INTR_BASE + 25)
#define PVT_INTR_MON_13_MAX         (PVT_INTR_BASE + 26)
#define PVT_INTR_MON_13_MIN         (PVT_INTR_BASE + 27)
#define PVT_INTR_MON_14_MAX         (PVT_INTR_BASE + 28)
#define PVT_INTR_MON_14_MIN         (PVT_INTR_BASE + 29)
#define PVT_INTR_MON_15_MAX         (PVT_INTR_BASE + 30)
#define PVT_INTR_MON_15_MIN         (PVT_INTR_BASE + 31)

#define NUM_PVT_INTR                32

#define MAX_PVT_INTR                (PVT_INTR_BASE + NUM_PVT_INTR)

/*! \} */

#endif /* BCM56990_B0_PVT_INTR_H */
