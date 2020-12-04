/*
 * $Id: compat_656.h,v 1.1 2016/10/27
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.6 routines
 */

#ifndef _COMPAT_656_H_
#define _COMPAT_656_H_

#ifdef	BCM_RPC_SUPPORT

#include <bcm/types.h>
#include <bcm/field.h>

#define BCM_COMPAT656_FIELD_QUALIFY_CNT     (620)
#define BCM_COMPAT656_FIELD_USER_NUM_UDFS   (93)

typedef struct bcm_compat656_field_qset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_COMPAT656_FIELD_QUALIFY_CNT +
                                 BCM_COMPAT656_FIELD_USER_NUM_UDFS)];
    SHR_BITDCL udf_map[_SHR_BITDCLSIZE(BCM_COMPAT656_FIELD_USER_NUM_UDFS)];
} bcm_compat656_field_qset_t;

extern int bcm_compat656_field_group_create(
    int unit,
    bcm_compat656_field_qset_t qset,
    int pri,
    bcm_field_group_t *group);

extern int bcm_compat656_field_group_create_id(
    int unit,
    bcm_compat656_field_qset_t qset,
    int pri,
    bcm_field_group_t group);

extern int bcm_compat656_field_qset_data_qualifier_add(
    int unit,
    bcm_compat656_field_qset_t *qset,
    int qual_id);

#endif	/* BCM_RPC_SUPPORT */

#endif	/* !_COMPAT_656_H */
