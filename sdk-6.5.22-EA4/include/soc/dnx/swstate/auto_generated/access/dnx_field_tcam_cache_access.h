/** \file dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.h
 *
 * sw state functions declarations
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_CACHE_ACCESS_H__
#define __DNX_FIELD_TCAM_CACHE_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_cache_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/*
 * TYPEDEFs
 */

/**
 * implemented by: dnx_field_tcam_cache_shadow_is_init
 */
typedef int (*dnx_field_tcam_cache_shadow_is_init_cb)(
    int unit, uint8 *is_init);

/**
 * implemented by: dnx_field_tcam_cache_shadow_init
 */
typedef int (*dnx_field_tcam_cache_shadow_init_cb)(
    int unit);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_alloc
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_1);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_alloc
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_alloc_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 nof_instances_to_alloc_0);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_set
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_set_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 key_data_idx_0, uint8 key_data);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_get
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_get_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 key_data_idx_0, uint8 *key_data);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_read
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_read_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 nof_elements, uint8 *dest);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_write
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_write_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 nof_elements, const uint8 *source);

/**
 * implemented by: dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_fill
 */
typedef int (*dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_fill_cb)(
    int unit, uint32 tcam_bank_idx_0, uint32 tcam_bank_idx_1, uint32 entry_key_idx_0, uint32 from_idx, uint32 nof_elements, uint8 value);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_alloc
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_1);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_alloc
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_alloc_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 nof_instances_to_alloc_0);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_set
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_set_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 payload_data_idx_0, uint8 payload_data);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_get
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_get_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 payload_data_idx_0, uint8 *payload_data);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_read
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_read_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 nof_elements, uint8 *dest);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_write
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_write_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 nof_elements, const uint8 *source);

/**
 * implemented by: dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_fill
 */
typedef int (*dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_fill_cb)(
    int unit, uint32 payload_table_idx_0, uint32 payload_table_idx_1, uint32 entry_payload_idx_0, uint32 from_idx, uint32 nof_elements, uint8 value);

/*
 * STRUCTs
 */

/**
 * This structure holds the access functions for the variable key_data
 */
typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_set_cb set;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_get_cb get;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_read_cb range_read;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_write_cb range_write;
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_range_fill_cb range_fill;
} dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_tcam_cache_shadow_entry_t
 */
typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_alloc_cb alloc;
    /**
     * Access struct for key_data
     */
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_key_data_cbs key_data;
} dnx_field_tcam_cache_shadow_tcam_bank_entry_key_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_tcam_cache_shadow_bank_t
 */
typedef struct {
    dnx_field_tcam_cache_shadow_tcam_bank_alloc_cb alloc;
    /**
     * Access struct for entry_key
     */
    dnx_field_tcam_cache_shadow_tcam_bank_entry_key_cbs entry_key;
} dnx_field_tcam_cache_shadow_tcam_bank_cbs;

/**
 * This structure holds the access functions for the variable payload_data
 */
typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_set_cb set;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_get_cb get;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_read_cb range_read;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_write_cb range_write;
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_range_fill_cb range_fill;
} dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_tcam_cache_shadow_payload_entry_t
 */
typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_alloc_cb alloc;
    /**
     * Access struct for payload_data
     */
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_payload_data_cbs payload_data;
} dnx_field_tcam_cache_shadow_payload_table_entry_payload_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_tcam_cache_shadow_payload_table_t
 */
typedef struct {
    dnx_field_tcam_cache_shadow_payload_table_alloc_cb alloc;
    /**
     * Access struct for entry_payload
     */
    dnx_field_tcam_cache_shadow_payload_table_entry_payload_cbs entry_payload;
} dnx_field_tcam_cache_shadow_payload_table_cbs;

/**
 * This structure holds the access functions for the variable dnx_field_tcam_cache_shadow_t
 */
typedef struct {
    dnx_field_tcam_cache_shadow_is_init_cb is_init;
    dnx_field_tcam_cache_shadow_init_cb init;
    /**
     * Access struct for tcam_bank
     */
    dnx_field_tcam_cache_shadow_tcam_bank_cbs tcam_bank;
    /**
     * Access struct for payload_table
     */
    dnx_field_tcam_cache_shadow_payload_table_cbs payload_table;
} dnx_field_tcam_cache_shadow_cbs;

/*
 * Global Variables
 */

/*
 * Global Variables
 */

extern dnx_field_tcam_cache_shadow_cbs dnx_field_tcam_cache_shadow;

#endif /* __DNX_FIELD_TCAM_CACHE_ACCESS_H__ */
