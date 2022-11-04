
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_MAPPER_ACCESS_H__
#define __DNX_FIELD_TCAM_ACCESS_MAPPER_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_access_mapper_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef int (*dnx_field_tcam_access_mapper_sw_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_field_tcam_access_mapper_sw_init_cb)(
    int unit);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc_cb)(
    int unit);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_set_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 head_idx_0, uint32 head);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_get_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 head_idx_0, uint32 *head);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_alloc_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 nof_instances_to_alloc_0);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_set_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 next_idx_0, uint32 next);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_get_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 next_idx_0, uint32 *next);

typedef int (*dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_alloc_cb)(
    int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 nof_instances_to_alloc_0);



typedef struct {
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_set_cb set;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_get_cb get;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_alloc_cb alloc;
} dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_cbs;

typedef struct {
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_set_cb set;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_get_cb get;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_alloc_cb alloc;
} dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_cbs;

typedef struct {
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc_cb alloc;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_cbs head;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_cbs next;
} dnx_field_tcam_access_mapper_sw_key_2_location_hash_cbs;

typedef struct {
    dnx_field_tcam_access_mapper_sw_is_init_cb is_init;
    dnx_field_tcam_access_mapper_sw_init_cb init;
    dnx_field_tcam_access_mapper_sw_key_2_location_hash_cbs key_2_location_hash;
} dnx_field_tcam_access_mapper_sw_cbs;





extern dnx_field_tcam_access_mapper_sw_cbs dnx_field_tcam_access_mapper_sw;

#endif 
