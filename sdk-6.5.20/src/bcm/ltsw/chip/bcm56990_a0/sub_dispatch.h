/*! \file sub_dispatch.h
 *
 * BCM56990_A0 subordinate dispatch definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_SUB_DISPATCH_H
#define BCM56990_A0_SUB_DISPATCH_H

#include "sub_devlist.h"

/*!
 * \brief Subordinate drivers supported.
 */
#define BCM56990_A0_SUB_DRV_ENTRY(_dn,_bd) \
    BCM56990_A0_SUB_DRV_##_dn,
typedef enum bcm56990_a0_sub_drv_e {
    BCM56990_A0_SUB_DRV_NONE = 0,
    #include "sub_devlist.h"
    BCM56990_A0_SUB_DRV_CNT
} bcm56990_a0_sub_drv_t;

/*!
 * \brief Subordinate modules needed.
 */
typedef enum bcm56990_a0_sub_mod_e {
    /*! Invalid module. */
    BCM56990_A0_SUB_MOD_INVAL = 0,

    /*! Device module. */
    BCM56990_A0_SUB_MOD_DEV = 1,

    /*! UFT module. */
    BCM56990_A0_SUB_MOD_UFT = 2,

    /*! Port module. */
    BCM56990_A0_SUB_MOD_PORT = 3,

    /*! L3 egress module. */
    BCM56990_A0_SUB_MOD_L3_EGRESS = 4,

    /*! L2 module. */
    BCM56990_A0_SUB_MOD_L2 = 5,

    /*! MPLS module. */
    BCM56990_A0_SUB_MOD_MPLS = 6,

    /*! L3 interface module. */
    BCM56990_A0_SUB_MOD_L3_INTF = 7,

    /*! LB_HASH module. */
    BCM56990_A0_SUB_MOD_LB_HASH = 8,

    /*! L3 FIB module. */
    BCM56990_A0_SUB_MOD_L3_FIB = 9,

    /*! L3 module. */
    BCM56990_A0_SUB_MOD_L3 = 10,

    /*! VLAN module. */
    BCM56990_A0_SUB_MOD_VLAN = 11,

    /*! Mirror module. */
    BCM56990_A0_SUB_MOD_MIRROR = 12,

    /*! Flexctr module. */
    BCM56990_A0_SUB_MOD_FLEXCTR = 13,

    /*! Stat module. */
    BCM56990_A0_SUB_MOD_STAT = 14,

    /*! Virtual module. */
    BCM56990_A0_SUB_MOD_VIRTUAL = 15,

    /*! Flex Flow module. */
    BCM56990_A0_SUB_MOD_FLEXFLOW = 16,

    /*! Tunnel module. */
    BCM56990_A0_SUB_MOD_TUNNEL = 17,

    /*! Field module. */
    BCM56990_A0_SUB_MOD_FIELD = 18,

    /*! INT module. */
    BCM56990_A0_SUB_MOD_INT = 19,

    /*! L3 AACL module. */
    BCM56990_A0_SUB_MOD_L3_AACL = 20,

    /*! STG module. */
    BCM56990_A0_SUB_MOD_STG = 21,

    /*! Count of submodules. */
    BCM56990_A0_SUB_MOD_CNT
} bcm56990_a0_sub_mod_t;

/*! Generic definition */
#define BCM56990_A0_SUB_DRV NULL

/*!
 * \brief Attach subordinate driver.
 *
 * \param [in] unit Unit number.
 * \param [in] hdl Handle pointer.
 * \param [in] drv Subordinate driver.
 * \param [in] mod Subordinate module.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56990_a0_sub_drv_attach(int unit, void *hdl,
                           bcm56990_a0_sub_drv_t drv,
                           bcm56990_a0_sub_mod_t mod);

#endif /* BCM56990_A0_SUB_DISPATCH_H */
