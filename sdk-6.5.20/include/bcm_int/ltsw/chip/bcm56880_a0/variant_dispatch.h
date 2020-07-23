/*! \file variant_dispatch.h
 *
 * BCM56880_A0 variant dispatch functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_LTSW_VARIANT_DISPATCH_H
#define BCM56880_A0_LTSW_VARIANT_DISPATCH_H

/*!
 * \brief Variant module identity.
 */
typedef enum bcm56880_a0_ltsw_variant_mod_e {
    /*! Invalid module. */
    BCM56880_A0_LTSW_VARIANT_INVALID = 0,

    /*! Feature. */
    BCM56880_A0_LTSW_VARIANT_FEATURE = 1,

    /*! SBR. */
    BCM56880_A0_LTSW_VARIANT_SBR = 2,

    /*! DEV_MISC. */
    BCM56880_A0_LTSW_VARIANT_DEV_MISC = 3,

    /*! L3 FIB. */
    BCM56880_A0_LTSW_VARIANT_L3_FIB = 4,

    /*! L2. */
    BCM56880_A0_LTSW_VARIANT_L2 = 5,

    /*! Stat. */
    BCM56880_A0_LTSW_VARIANT_STAT = 6,

    /*! PKTIO. */
    BCM56880_A0_LTSW_VARIANT_PKTIO = 7,

    /*! COSQ. */
    BCM56880_A0_LTSW_VARIANT_COSQ = 8,

    /*! FLEXCTR. */
    BCM56880_A0_LTSW_VARIANT_FLEXCTR = 9,

    /*! FLEXSTATE. */
    BCM56880_A0_LTSW_VARIANT_FLEXSTATE = 10,

    /*! SWITCH. */
    BCM56880_A0_LTSW_VARIANT_SWITCH = 11,

    /*! MPLS. */
    BCM56880_A0_LTSW_VARIANT_MPLS = 12,

    /*! FIELD. */
    BCM56880_A0_LTSW_VARIANT_FIELD = 13,

    /*! VLAN. */
    BCM56880_A0_LTSW_VARIANT_VLAN = 14,

    /*! L3 INTF. */
    BCM56880_A0_LTSW_VARIANT_L3_INTF = 15,

    /*! UFT. */
    BCM56880_A0_LTSW_VARIANT_UFT = 16,

    /*! QOS. */
    BCM56880_A0_LTSW_VARIANT_QOS = 17,

    /*! FLOW. */
    BCM56880_A0_LTSW_VARIANT_FLOW = 18,

    /*! PORT. */
    BCM56880_A0_LTSW_VARIANT_PORT = 19,

    /*! RX. */
    BCM56880_A0_LTSW_VARIANT_RX = 20,

    /*! IFA. */
    BCM56880_A0_LTSW_VARIANT_IFA = 21,

    /*! L3. */
    BCM56880_A0_LTSW_VARIANT_L3 = 22,

    /*! RANGE. */
    BCM56880_A0_LTSW_VARIANT_RANGE = 23,

    /*! Always the last. */
    BCM56880_A0_LTSW_VARIANT_MAX
} bcm56880_a0_ltsw_variant_mod_t;

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
bcm56880_a0_variant_get(int unit, int *variant);

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
bcm56880_a0_ltsw_variant_drv_attach(int unit,
                                    bcm56880_a0_ltsw_variant_mod_t module);

#endif  /* BCM56880_A0_LTSW_VARIANT_DISPATCH_H */
