/*! \file sub_devlist.h
 *
 * BCM56990_A0 subordinate device/driver list.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BCM56990_A0_SUB_DEV_ENTRY
/*
 * BCM56990_A0_SUB_DEV_ENTRY macros.
 *
 * Before including this file, define BCM56990_A0_SUB_DEV_ENTRY
 * as a macro to operate on the following parameters:
 *
 *     #define BCM56990_A0_SUB_DEV_ENTRY(_dn,_vn,_pf,_pd,_r0)
 *
 *     _dn: Device Name
 *     _vn: Variant Name
 *     _pf: Product Family
 *     _pd: Product Description
 *     _r0: Reserved
 *
 * Note that this macro will be undefined at the end of this definition.
 */

#if BCMDRD_CONFIG_INCLUDE_BCM56990_Ax == 1 || BCMDRD_CONFIG_INCLUDE_BCM56990 == 1
BCM56990_A0_SUB_DEV_ENTRY(BCM56990_Ax, bcm56990_a0, "Tomahawk4", "BCM56990", 0)
#endif

#if BCMDRD_CONFIG_INCLUDE_BCM56990_Bx == 1 || BCMDRD_CONFIG_INCLUDE_BCM56990 == 1
BCM56990_A0_SUB_DEV_ENTRY(BCM56990_Bx, bcm56990_b0, "Tomahawk4", "BCM56990", 0)
#endif

#if BCMDRD_CONFIG_INCLUDE_BCM56996 == 1
BCM56990_A0_SUB_DEV_ENTRY(BCM56996, bcm56996_a0, "Tomahawk4G", "BCM56996", 0)
#endif

#if (BCMDRD_CONFIG_INCLUDE_BCM56990_Bx == 1 || BCMDRD_CONFIG_INCLUDE_BCM56990 == 1) || \
    BCMDRD_CONFIG_INCLUDE_BCM56996 == 1
BCM56990_A0_SUB_DEV_ENTRY(BCM5699x, bcm5699x, "Tomahawk4", "BCM56990", 0)
#endif

/* End BCM56990_A0_SUB_DEV_ENTRY Macros */

#undef BCM56990_A0_SUB_DEV_ENTRY
#endif /* BCM56990_A0_SUB_DEV_ENTRY */

#ifdef BCM56990_A0_SUB_DRV_ENTRY
/*
 * BCM56990_A0_SUB_DRV_ENTRY macros.
 *
 * Before including this file, define BCM56990_A0_SUB_DRV_ENTRY
 * as a macro to operate on the following parameters:
 *
 *     #define BCM56990_A0_SUB_DRV_ENTRY(_dn,_bd)
 *
 *     _dn: Driver Name
 *     _bd: Base Device
 *
 * Note that this macro will be undefined at the end of this definition.
 */

BCM56990_A0_SUB_DRV_ENTRY(BCM56990_Ax, bcm56990_a0)
BCM56990_A0_SUB_DRV_ENTRY(BCM56990_Bx, bcm56990_b0)
BCM56990_A0_SUB_DRV_ENTRY(BCM56996, bcm56996_a0)
BCM56990_A0_SUB_DRV_ENTRY(BCM5699x, bcm56990_a0)

/* End BCM56990_A0_SUB_DRV_ENTRY Macros */

#undef BCM56990_A0_SUB_DRV_ENTRY
#endif /* BCM56990_A0_SUB_DRV_ENTRY */

#ifdef BCM56990_A0_DRV_ATTACH_ENTRY
/*
 * BCM56990_A0_DRV_ATTACH_ENTRY macros.
 *
 * Before including this file, define BCM56990_A0_DRV_ATTACH_ENTRY
 * as a macro to operate on the following parameters:
 *
 *     #define BCM56990_A0_DRV_ATTACH_ENTRY((_dn,_vn,_pf,_pd,_r0)
 *
 *     _dn: Device Name
 *     _vn: Variant Name
 *     _pf: Product Family
 *     _pd: Product Description
 *     _r0: Reserved
 *
 * Note that this macro will be undefined at the end of this definition.
 */

#if BCMDRD_CONFIG_INCLUDE_BCM56990_Bx == 1 || BCMDRD_CONFIG_INCLUDE_BCM56990 == 1
BCM56990_A0_DRV_ATTACH_ENTRY(BCM56990_Bx, bcm56990_b0, "Tomahawk4", "BCM56990", 0)
#endif

#if BCMDRD_CONFIG_INCLUDE_BCM56996 == 1
BCM56990_A0_DRV_ATTACH_ENTRY(BCM56996, bcm56996_a0, "Tomahawk4G", "BCM56996", 0)
#endif

/* End BCM56990_A0_DRV_ATTACH_ENTRY Macros */

#undef BCM56990_A0_DRV_ATTACH_ENTRY
#endif /* BCM56990_A0_DRV_ATTACH_ENTRY */
