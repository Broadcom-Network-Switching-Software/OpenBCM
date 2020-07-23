/*! \file bcmcth_mon_rx_flex_internal.h
 *
 * Packet RX internal access interface to obtain all profiles.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_RX_FLEX_INTERNAL_H
#define BCMCTH_MON_RX_FLEX_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmlrd/bcmlrd_conf.h>

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern void _bd##_vu##_va##_mon_rx_flex_profiles_get(SHR_BITDCL* profiles);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMCTH_MON_RX_FLEX_INTERNAL_H */
