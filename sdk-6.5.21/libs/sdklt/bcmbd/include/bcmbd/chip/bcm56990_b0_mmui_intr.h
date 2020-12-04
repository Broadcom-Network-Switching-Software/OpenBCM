/*! \file bcm56990_b0_mmui_intr.h
 *
 * MMU instrument interrupt sources for BCM56990_B0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_B0_MMUI_INTR_H
#define BCM56990_B0_MMUI_INTR_H

/*!
 *\name MMU instrument interrupt sources.
 *
 * Please refer to the hardware documentation for a description of the
 * individual interrupt sources.
 *
 *\{
 */

/* MMU_THDR_QE_CPU_INT_STAT_INST */
#define MMUI_INTR_THDR_BST               0

/* MMU_CFAP_INT2_STAT */
#define MMUI_INTR_CFAP_BST               1

/* MMU_THDI_CPU_INT_STAT_INST */
#define MMUI_INTR_THDI_BST               2

/* MMU_THDO_BST_CPU_INT_STAT */
#define MMUI_INTR_THDO_BST               3
#define MMUI_INTR_THDO_EBST_FIFO_FULL    4

#define MMUI_INTR_MAX                    5

/*! \} */

#endif /* BCM56990_B0_MMUI_INTR_H */
