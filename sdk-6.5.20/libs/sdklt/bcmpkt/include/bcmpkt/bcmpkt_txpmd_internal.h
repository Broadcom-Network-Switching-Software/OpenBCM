/*! \file bcmpkt_txpmd_internal.h
 *
 * TX Packet MetaData (TXPMD, called SOBMH in hardware) access interface
 * (Internal use only).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_TXPMD_INTERNAL_H
#define BCMPKT_TXPMD_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmpkt/bcmpkt_txpmd_defs.h>
#include <bcmpkt/bcmpkt_internal.h>

/*!
 * Array of TXPMD field getter functions for a particular device
 * type.
 */
typedef struct bcmpkt_txpmd_fget_s {
    bcmpkt_field_get_f fget[BCMPKT_TXPMD_FID_COUNT];
} bcmpkt_txpmd_fget_t;

/*!
 * Array of TXPMD field setter functions for a particular device
 * type. These functions are used for internally configuring packet
 * filter.
 */
typedef struct bcmpkt_txpmd_fset_s {
    bcmpkt_field_set_f fset[BCMPKT_TXPMD_FID_COUNT];
} bcmpkt_txpmd_fset_t;

/*!
 * Array of TXPMD field address and length getter functions for a multiple
 * words field of a particular device type. *addr is output address and return
 * length.
 */
typedef struct bcmpkt_txpmd_figet_s {
    bcmpkt_ifield_get_f fget[BCMPKT_TXPMD_I_FID_COUNT];
} bcmpkt_txpmd_figet_t;

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern void _bd##_txpmd_view_info_get(bcmpkt_pmd_view_info_t *info);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

#endif  /* BCMPKT_TXPMD_INTERNAL_H */
