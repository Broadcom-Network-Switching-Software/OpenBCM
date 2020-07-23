/*! \file variant_dispatch.h
 *
 * BCM56780_A0 variant dispatch functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_LTSW_VARIANT_DISPATCH_H
#define BCM56780_A0_LTSW_VARIANT_DISPATCH_H

/*!
 * \brief Variant module identity.
 */
typedef enum bcm56780_a0_ltsw_variant_mod_e {
    /*! Invalid module. */
    BCM56780_A0_LTSW_VARIANT_INVALID = 0,

    /*! Feature. */
    BCM56780_A0_LTSW_VARIANT_FEATURE = 1,

    /*! SBR. */
    BCM56780_A0_LTSW_VARIANT_SBR = 2,

    /*! DEV_MISC. */
    BCM56780_A0_LTSW_VARIANT_DEV_MISC = 3,

    /*! L3 FIB. */
    BCM56780_A0_LTSW_VARIANT_L3_FIB = 4,

    /*! L2. */
    BCM56780_A0_LTSW_VARIANT_L2 = 5,

    /*! Stat. */
    BCM56780_A0_LTSW_VARIANT_STAT = 6,

    /*! PKTIO. */
    BCM56780_A0_LTSW_VARIANT_PKTIO = 7,

    /*! COSQ. */
    BCM56780_A0_LTSW_VARIANT_COSQ = 8,

    /*! FLEXCTR. */
    BCM56780_A0_LTSW_VARIANT_FLEXCTR = 9,

    /*! FLEXSTATE. */
    BCM56780_A0_LTSW_VARIANT_FLEXSTATE = 10,

    /*! SWITCH. */
    BCM56780_A0_LTSW_VARIANT_SWITCH = 11,

    /*! MPLS. */
    BCM56780_A0_LTSW_VARIANT_MPLS = 12,

    /*! L3 INTF. */
    BCM56780_A0_LTSW_VARIANT_L3_INTF = 13,

    /*! PORT. */
    BCM56780_A0_LTSW_VARIANT_PORT = 14,

    /*! UFT. */
    BCM56780_A0_LTSW_VARIANT_UFT = 15,

    /*! VLAN. */
    BCM56780_A0_LTSW_VARIANT_VLAN = 16,

    /*! RX. */
    BCM56780_A0_LTSW_VARIANT_RX = 17,

    /*! FIELD. */
    BCM56780_A0_LTSW_VARIANT_FIELD = 18,

    /*! QOS. */
    BCM56780_A0_LTSW_VARIANT_QOS = 19,

    /*! FLOW. */
    BCM56780_A0_LTSW_VARIANT_FLOW = 20,

    /*! IFA. */
    BCM56780_A0_LTSW_VARIANT_IFA = 21,

    /*! RANGE. */
    BCM56780_A0_LTSW_VARIANT_RANGE = 22,

    /*! MIRROR. */
    BCM56780_A0_LTSW_VARIANT_MIRROR = 23,

    /*! Always the last. */
    BCM56780_A0_LTSW_VARIANT_MAX
} bcm56780_a0_ltsw_variant_mod_t;

/*!
* \brief Attach driver objects per variant.
*
* \param [in] unit Unit number.
* \param [out] variant Variant index in array.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
bcm56780_a0_variant_get(int unit, int *variant);

/*!
* \brief Attach driver objects per variant.
*
* \param [in] unit Unit number.
* \param [in] module Variant module.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
bcm56780_a0_ltsw_variant_drv_attach(int unit,
                                    bcm56780_a0_ltsw_variant_mod_t module);

#endif  /* BCM56780_A0_LTSW_VARIANT_DISPATCH_H */
