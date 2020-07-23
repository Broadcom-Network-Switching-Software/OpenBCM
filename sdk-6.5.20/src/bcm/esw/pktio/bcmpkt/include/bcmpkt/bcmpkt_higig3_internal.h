/*! \file bcmpkt_higig3_internal.h
 *
 * Higig3 access interface
 * (Internal use only).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_HIGIG3_INTERNAL_H
#define BCMPKT_HIGIG3_INTERNAL_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_types.h>
#endif
#include <bcmpkt/bcmpkt_higig3_defs.h>
#include <bcmpkt/bcmpkt_internal.h>

/*!
 * Array of HIGIG3 field get functions for a particular device
 * type.
 */
typedef struct bcmpkt_higig3_fget_s {
    /*! Array of field get functions. */
    bcmpkt_field_get_f fget[BCMPKT_HIGIG3_FID_COUNT];
} bcmpkt_higig3_fget_t;

/*!
 * Array of HIGIG3 field set functions for a particular device
 * type. These functions are used for internally configuring packet
 * filter.
 */
typedef struct bcmpkt_higig3_fset_s {
    /*! Array of field set functions. */
    bcmpkt_field_set_f fset[BCMPKT_HIGIG3_FID_COUNT];
} bcmpkt_higig3_fset_t;

/*!
 * Array of HIGIG3 field address and length get functions for a multiple
 * words field of a particular device type. *addr is output address and return
 * length.
 */
typedef struct bcmpkt_higig3_figet_s {
    /*! Array of field get functions */
    bcmpkt_ifield_get_f fget[BCMPKT_HIGIG3_I_FID_COUNT];
} bcmpkt_higig3_figet_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern void _bd##_vu##_va##_higig3_view_info_get(bcmpkt_pmd_view_info_t *info);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif  /* BCMPKT_HIGIG3_INTERNAL_H */
