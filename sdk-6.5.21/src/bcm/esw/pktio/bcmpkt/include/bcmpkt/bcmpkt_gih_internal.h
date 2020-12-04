/*! \file bcmpkt_gih_internal.h
 *
 * Generic Internal header(GIH) access interface
 * (Internal use only).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_GIH_INTERNAL_H
#define BCMPKT_GIH_INTERNAL_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <shr/shr_types.h>
#endif
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmpkt/bcmpkt_generic_loopback_t_defs.h>

/*!
 * Array of GIH field get functions for a particular device
 * type.
 */
typedef struct bcmpkt_gih_fget_s {
    /*! Array of field get functions. */
    bcmpkt_field_get_f fget[BCMPKT_GIH_FID_COUNT];
} bcmpkt_gih_fget_t;

/*!
 * Array of GIH field set functions for a particular device
 * type. These functions are used for internally configuring packet
 * filter.
 */
typedef struct bcmpkt_gih_fset_s {
    /*! Array of field set functions. */
    bcmpkt_field_set_f fset[BCMPKT_GIH_FID_COUNT];
} bcmpkt_gih_fset_t;

/*!
 * Array of GIH field address and length getter functions for a multiple
 * words field of a particular device type.
 */
typedef struct bcmpkt_gih_figet_s {
    /*! Array of field get functions */
    bcmpkt_ifield_get_f fget[BCMPKT_GIH_I_FID_COUNT];
} bcmpkt_gih_figet_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    extern void _bd##_vu##_va##_gih_view_info_get(bcmpkt_pmd_view_info_t *info);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMPKT_GIH_INTERNAL_H */
