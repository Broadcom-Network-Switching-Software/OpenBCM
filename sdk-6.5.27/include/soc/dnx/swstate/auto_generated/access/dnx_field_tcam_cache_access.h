
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_CACHE_ACCESS_H__
#define __DNX_FIELD_TCAM_CACHE_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_cache_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef int (*dnx_field_tcam_cache_shadow_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_field_tcam_cache_shadow_init_cb)(
    int unit);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_1);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_alloc_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 nof_instances_to_alloc_0);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_set_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 key_data_idx_0, uint8 key_data);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_get_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 key_data_idx_0, uint8 *key_data);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_read_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 from_idx, uint32 nof_elements, uint8 *dest);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_write_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 from_idx, uint32 nof_elements, const uint8 *source);

typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_fill_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 from_idx, uint32 nof_elements, uint8 value);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_1);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_alloc_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 nof_instances_to_alloc_0);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_set_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 payload_data_idx_0, uint8 payload_data);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_get_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 payload_data_idx_0, uint8 *payload_data);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_read_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 from_idx, uint32 nof_elements, uint8 *dest);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_write_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 from_idx, uint32 nof_elements, const uint8 *source);

typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_fill_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 from_idx, uint32 nof_elements, uint8 value);



typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_set_cb set;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_get_cb get;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_read_cb range_read;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_write_cb range_write;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_fill_cb range_fill;
} dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_alloc_cb alloc;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_cbs key_data;
} dnx_field_tcam_cache_shadow_tcam_bank_entry_key_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_alloc_cb alloc;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_cbs entry_key;
} dnx_field_tcam_cache_shadow_tcam_bank_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_set_cb set;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_get_cb get;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_read_cb range_read;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_write_cb range_write;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_fill_cb range_fill;
} dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_alloc_cb alloc;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_cbs payload_data;
} dnx_field_tcam_cache_shadow_payload_table_entry_payload_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_alloc_cb alloc;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_cbs entry_payload;
} dnx_field_tcam_cache_shadow_payload_table_cbs;

typedef struct {
    dnx_field_tcam_cache_shadow_is_init_cb is_init;
    dnx_field_tcam_cache_shadow_init_cb init;
    dnx_field_tcam_cache_shadow_tcam_bank_cbs tcam_bank;
    dnx_field_tcam_cache_shadow_payload_table_cbs payload_table;
} dnx_field_tcam_cache_shadow_cbs;





extern dnx_field_tcam_cache_shadow_cbs dnx_field_tcam_cache_shadow;

#endif 
