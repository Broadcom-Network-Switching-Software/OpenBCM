/*
 * $Id: compat_655.h,v 1.1 2016/10/20
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.5 routines
 */

#ifndef _COMPAT_655_H_
#define _COMPAT_655_H_

#ifdef	BCM_RPC_SUPPORT

#include <bcm/types.h>
#include <bcm/trunk.h>
#include <bcm/field.h>
#include <bcm_int/compat/compat_6516.h>

typedef struct bcm_compat655_trunk_member_s {
    uint32 flags;                   /* BCM_TRUNK_MEMBER_xxx */
    bcm_gport_t gport;              /* Trunk member GPORT ID. */
    int dynamic_scaling_factor;     /* Dynamic load balancing threshold scaling
                                       factor. */
    int dynamic_load_weight;        /* Relative weight of historical load in
                                       determining member quality. */
} bcm_compat655_trunk_member_t;

extern int bcm_compat655_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *trunk_info,
    int member_count,
    bcm_compat655_trunk_member_t *member_array);

extern int bcm_compat655_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *t_data,
    int member_max,
    bcm_compat655_trunk_member_t *member_array,
    int *member_count);

#define BCM_COMPAT655_FIELD_QUALIFY_CNT     (609)
#define BCM_COMPAT655_FIELD_USER_NUM_UDFS   (93)

typedef struct bcm_compat655_field_qset_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCM_COMPAT655_FIELD_QUALIFY_CNT +
                                 BCM_COMPAT655_FIELD_USER_NUM_UDFS)];
    SHR_BITDCL udf_map[_SHR_BITDCLSIZE(BCM_COMPAT655_FIELD_USER_NUM_UDFS)];
} bcm_compat655_field_qset_t;

extern int bcm_compat655_field_group_create(
    int unit,
    bcm_compat655_field_qset_t qset,
    int pri,
    bcm_field_group_t *group);

extern int bcm_compat655_field_group_create_id(
    int unit,
    bcm_compat655_field_qset_t qset,
    int pri,
    bcm_field_group_t group);

extern int bcm_compat655_field_qset_data_qualifier_add(
    int unit,
    bcm_compat655_field_qset_t *qset,
    int qual_id);

#endif	/* BCM_RPC_SUPPORT */

#endif	/* !_COMPAT_655_H */
