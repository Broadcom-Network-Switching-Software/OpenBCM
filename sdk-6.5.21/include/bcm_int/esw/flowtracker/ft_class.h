/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_class.h
 * Purpose:     Function declarations for flowtracker class.
 */

#ifndef _BCM_INT_FT_CLASS_H_
#define _BCM_INT_FT_CLASS_H_

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm_int/esw/flowtracker/ft_group.h>

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
extern int
bcmi_ftv2_egr_class_source_set(
    int unit,
    uint32 options,
    bcm_flowtracker_class_source_type_t source_type);

extern int
bcmi_ftv2_egr_class_source_get(
    int unit,
    uint32 options,
    bcm_flowtracker_class_source_type_t *source_type);

extern void
bcmi_ftv2_class_show(int unit);

#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)

typedef struct bcmi_ft_agg_class_source_type_map_s {
    uint32 amft_hw_val;

    uint32 aeft_hw_val;
} bcmi_ft_agg_class_source_type_map_t;

typedef struct bcmi_ft_aggregate_class_state_s {
    /* Number of entries */
    int num_entries;

    /* Bitmap for class ids */
    SHR_BITDCL *bitmap[BCMI_FT_GROUP_TYPE_COUNT];

    /* Reference count per class Id */
    int *ref_count_array;

    /* Class Source type mapping */
    bcmi_ft_agg_class_source_type_map_t *map;

} bcmi_ft_aggregate_class_state_t;


extern bcmi_ft_aggregate_class_state_t
    bcmi_ft_aggregate_class_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_AGG_CLASS_MAX_IDX(_unit_)                                       \
    bcmi_ft_aggregate_class_state[(_unit_)].num_entries

#define BCMI_FT_AGG_CLASS_BITMAP(_unit_, _group_type_)                          \
    bcmi_ft_aggregate_class_state[(_unit_)].bitmap[(_group_type_)]

#define BCMI_FT_AGG_CLASS_REF(_unit_)                                           \
    bcmi_ft_aggregate_class_state[(_unit_)].ref_count_array

#define BCMI_FT_AGG_CLASS_REF_COUNT(_unit_, _idx_)                              \
    bcmi_ft_aggregate_class_state[(_unit_)].ref_count_array[(_idx_)]

#define BCMI_FT_AGG_CLASS_ST_MAP(_unit_)                                        \
    bcmi_ft_aggregate_class_state[(_unit_)].map

#define BCMI_FT_AGG_CLASS_ST_MAP_HW_VAL(_unit_, _source_type_, aft)             \
    (bcmi_ft_aggregate_class_state[(_unit_)].map)[(_source_type_)].aft ##_hw_val

#define BCMI_FT_AGG_CLASS_ID_MIN 1
#define BCMI_FT_AGG_CLASS_ID_MAX 63
#define BCMI_FT_AGG_CLASS_ST_INVALID (-1)

extern int
bcmi_ftv3_aggregate_class_init(
    int unit);

extern int
bcmi_ftv3_aggregate_class_cleanup(
    int unit);

extern int
bcmi_ftv3_aggregate_class_create(
    int unit,
    uint32 options,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info,
    uint32 *aggregate_class);

extern int
bcmi_ftv3_aggregate_class_get(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type,
    bcm_flowtracker_aggregate_class_info_t *aggregate_class_info);

extern int
bcmi_ftv3_aggregate_class_get_all(
    int unit,
    bcm_flowtracker_group_type_t group_type,
    int size,
    uint32 *aggregate_class_array,
    int *actual_size);

extern int
bcmi_ftv3_aggregate_class_destroy(
    int unit,
    uint32 aggregate_class,
    bcm_flowtracker_group_type_t group_type);

extern int
bcmi_ftv3_aggregate_class_entry_add_check(
    int unit,
    uint32 aggregate_class);

extern int
bcmi_ftv3_aggregate_class_ref_update(int unit,
    uint32 aggregate_class,
    int count);

extern void
bcmi_ftv3_aggregate_class_show(int unit,
    uint32 aggregate_class);

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */
#endif /* _BCM_INT_FT_CLASS_H_ */
