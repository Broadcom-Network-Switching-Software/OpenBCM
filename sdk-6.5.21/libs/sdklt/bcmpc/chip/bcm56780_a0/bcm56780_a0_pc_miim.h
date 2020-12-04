/*! \file bcm56780_a0_pc_miim.h
 *
 * We should discard this file and get the iProc MDIO address format from a
 * shared header file which will be used in the iProc MIIM driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCM56780_A0_PC_MIIM_H
#define BCM56780_A0_PC_MIIM_H

/*!
 * \addtogroup BCM56780_A0_PHY_ADDR_FORMAT
 *
 * \brief BCM56780_A0 PHY Address Format.
 *
 * PHY address definition:
 * <b> internal_bus (1b) | bus_number (4b) | clause_45 (1b) | phy_id (5b) </b>
 */

/*! PHY address in MII bus. */
#define BCM56780_A0_PC_MIIM_PHY_ID        0x0000001f

/*! Select clause 45 access method. */
#define BCM56780_A0_PC_MIIM_CLAUSE45      0x00000020

/*! Set internal bus number. */
#define BCM56780_A0_PC_MIIM_IBUS(_b)      (((_b) << 6) | 0x400)

/*! Set external bus number. */
#define BCM56780_A0_PC_MIIM_EBUS(_b)      ((_b) << 6)

/*! Get internal bus number. */
#define BCM56780_A0_PC_MIIM_IBUS_NUM(_a)  ((_a & ~0x400) >> 6)


#endif /* BCM56780_A0_PC_MIIM_H */

