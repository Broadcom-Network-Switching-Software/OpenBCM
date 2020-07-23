/*
 * $Id: compat_658.h,v 2.0 2017/03/23
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.8 routines
 */

#ifndef _COMPAT_658_H_
#define _COMPAT_658_H_

#ifdef	BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/field.h>

#if defined(INCLUDE_L3)
#include <bcm/ecn.h>

/* MPLS ECN Map Structure. */
typedef struct bcm_compat658_ecn_map_s {
    uint32 action_flags;        /* action flags for ECN map. */
    int int_cn;                 /* Internal congestion. */
    uint8 inner_ecn;            /* IP ECN value in payload. */
    uint8 ecn;                  /* IP ECN value. */
    uint8 exp;                  /* Mpls EXP value. */
    uint8 new_ecn;              /* New ECN value. */
    uint8 new_exp;              /* New EXP value. */
} bcm_compat658_ecn_map_t;

/* Get ecn map entry */
extern int bcm_compat658_ecn_map_get(int unit, int ecn_map_id,
                                     bcm_compat658_ecn_map_t *ecn_map);

/* Set ecn map entry */
extern int bcm_compat658_ecn_map_set(int unit, uint32 options, int ecn_map_id,
                                     bcm_compat658_ecn_map_t *ecn_map);
#endif
#endif
#endif	/* !_COMPAT_658_H */
