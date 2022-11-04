
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_HIT_INDICATION_ACCESS_H__
#define __DNX_FIELD_TCAM_HIT_INDICATION_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_hit_indication_types.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef int (*dnx_field_tcam_hit_indication_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_field_tcam_hit_indication_init_cb)(
    int unit);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc_cb)(
    int unit);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_alloc_bitmap_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _nof_bits_to_alloc);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_set_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_clear_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_get_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num, uint8* result);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_read_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_write_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_and_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_or_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_xor_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_remove_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_negate_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_clear_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_set_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_null_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_test_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_eq_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_count_cb)(
    int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _range, int *result);



typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_alloc_bitmap_cb alloc_bitmap;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_set_cb bit_set;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_clear_cb bit_clear;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_get_cb bit_get;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_read_cb bit_range_read;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_write_cb bit_range_write;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_and_cb bit_range_and;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_or_cb bit_range_or;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_xor_cb bit_range_xor;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_remove_cb bit_range_remove;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_negate_cb bit_range_negate;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_clear_cb bit_range_clear;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_set_cb bit_range_set;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_null_cb bit_range_null;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_test_cb bit_range_test;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_eq_cb bit_range_eq;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_count_cb bit_range_count;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_cbs;

typedef struct {
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc_cb alloc;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_cbs bmp;
} dnx_field_tcam_hit_indication_tcam_action_hit_indication_cbs;

typedef struct {
    dnx_field_tcam_hit_indication_is_init_cb is_init;
    dnx_field_tcam_hit_indication_init_cb init;
    dnx_field_tcam_hit_indication_tcam_action_hit_indication_cbs tcam_action_hit_indication;
} dnx_field_tcam_hit_indication_cbs;





extern dnx_field_tcam_hit_indication_cbs dnx_field_tcam_hit_indication;

#endif 
