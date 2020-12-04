/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        family.h
 * Purpose:     Define the chip families implemented under the BCM API
 */

#ifndef	_BCM_INT_FAMILY_H_
#define	_BCM_INT_FAMILY_H_


/****************************************************************
 *
 * BCM Family type:
 *    Allayer:  Includes 5632, 5633 and variants
 *    XGS:  Includes Hercules and Draco (5670/5671, 5690)
 */
typedef enum bcm_chip_family_e {
    BCM_FAMILY_ALLAYER,
    BCM_FAMILY_HERCULES,
    BCM_FAMILY_DRACO,
    BCM_FAMILY_DRACO15,
    BCM_FAMILY_LYNX,
    BCM_FAMILY_TUCANA,
    BCM_FAMILY_HERCULES15,
    BCM_FAMILY_EASYRIDER,
    BCM_FAMILY_FIREBOLT,
    BCM_FAMILY_HUMV,
    BCM_FAMILY_BRADLEY,
    BCM_FAMILY_TRIUMPH,
    BCM_FAMILY_SCORPION, 
    BCM_FAMILY_CONQUEROR,
    BCM_FAMILY_TRIUMPH2,
    BCM_FAMILY_TRIDENT,
    BCM_FAMILY_KATANA,
    BCM_FAMILY_DPP,
    BCM_FAMILY_DFE,
    BCM_FAMILY_TITAN,
    BCM_FAMILY_DNXF,
    BCM_FAMILY_DNX
} bcm_chip_family_t;

extern int bcm_chip_family_set(int unit, bcm_chip_family_t f); 
extern bcm_chip_family_t bcm_chip_family_get(int unit); 

#define BCM_CHIP_FAMILY(unit)	(bcm_chip_family_get(unit))



#endif	/* !_BCM_INT_FAMILY_H_ */
