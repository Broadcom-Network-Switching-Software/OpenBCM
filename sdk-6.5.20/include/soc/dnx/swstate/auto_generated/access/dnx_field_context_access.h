
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_CONTEXT_ACCESS_H__
#define __DNX_FIELD_CONTEXT_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>
#include <include/soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>



typedef int (*dnx_field_context_sw_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_field_context_sw_init_cb)(
    int unit);


typedef int (*dnx_field_context_sw_context_info_alloc_cb)(
    int unit);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_ipmf1_sw_info_t *context_ipmf1_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_context_compare_info_t *compare_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_compare_info_t *compare_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, CONST dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_compare_mode_e *mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint16 *key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, CONST dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint8 *bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_info_idx_0, uint32 attach_info_idx_0, int *offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_context_hashing_info_t *hashing_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_hashing_info_t *hashing_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_hash_mode_e *mode);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *is_set);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, dnx_field_qual_t *qual_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint16 *lsb);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 key_qual_map_idx_0, uint8 *size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set_cb)(
    int unit, uint32 context_info_idx_0, uint16 key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_get_cb)(
    int unit, uint32 context_info_idx_0, uint16 *key_size_in_bits);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 id_idx_0, dbal_enum_value_field_field_key_e *id);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *full_key_allocated);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_bit_range_t *bit_range);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *bit_range_valid);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *bit_range_offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *bit_range_size);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, dnx_field_input_type_e *input_type);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *input_arg);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_get_cb)(
    int unit, uint32 context_info_idx_0, uint32 attach_info_idx_0, int *offset);


typedef int (*dnx_field_context_sw_context_info_context_ipmf2_info_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf2_info_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_ipmf2_sw_info_t *context_ipmf2_info);


typedef int (*dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_t cascaded_from);


typedef int (*dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_t *cascaded_from);


typedef int (*dnx_field_context_sw_context_info_state_table_info_set_cb)(
    int unit, uint32 context_info_idx_0, CONST dnx_field_context_state_table_info_t *state_table_info);


typedef int (*dnx_field_context_sw_context_info_state_table_info_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_state_table_info_t *state_table_info);


typedef int (*dnx_field_context_sw_context_info_state_table_info_mode_set_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e mode);


typedef int (*dnx_field_context_sw_context_info_state_table_info_mode_get_cb)(
    int unit, uint32 context_info_idx_0, dnx_field_context_state_table_mode_e *mode);


typedef int (*dnx_field_context_sw_context_info_state_table_info_is_set_set_cb)(
    int unit, uint32 context_info_idx_0, uint8 is_set);


typedef int (*dnx_field_context_sw_context_info_state_table_info_is_set_get_cb)(
    int unit, uint32 context_info_idx_0, uint8 *is_set);


typedef int (*dnx_field_context_sw_context_info_name_value_stringncat_cb)(
    int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src);


typedef int (*dnx_field_context_sw_context_info_name_value_stringncmp_cb)(
    int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *cmp_string, int *result);


typedef int (*dnx_field_context_sw_context_info_name_value_stringncpy_cb)(
    int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *src);


typedef int (*dnx_field_context_sw_context_info_name_value_stringlen_cb)(
    int unit, uint32 context_info_idx_0, uint32 name_idx_0, uint32 *size);


typedef int (*dnx_field_context_sw_context_info_name_value_stringget_cb)(
    int unit, uint32 context_info_idx_0, uint32 name_idx_0, char *o_string);




typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_cbs qual_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_cbs lsb;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_cbs size;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_cbs key_qual_map;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_cbs key_size_in_bits;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_cbs bit_range_valid;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_cbs bit_range_offset;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_cbs bit_range_size;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_cbs id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_cbs full_key_allocated;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_cbs bit_range;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_cbs input_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_cbs input_arg;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_cbs offset;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_cbs key_template;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_cbs key_id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_cbs attach_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_cbs mode;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_cbs is_set;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_cbs key_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_cbs qual_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_cbs lsb;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_cbs size;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_cbs key_qual_map;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_cbs key_size_in_bits;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_cbs bit_range_valid;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_cbs bit_range_offset;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_cbs bit_range_size;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_cbs id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_cbs full_key_allocated;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_cbs bit_range;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_cbs input_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_cbs input_arg;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_cbs offset;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_cbs key_template;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_cbs key_id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_cbs attach_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_cbs mode;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_cbs is_set;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_cbs key_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_cbs pair_1;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_cbs pair_2;
} dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_cbs qual_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_cbs lsb;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_cbs size;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_cbs key_qual_map;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_cbs key_size_in_bits;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_cbs bit_range_valid;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_cbs bit_range_offset;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_cbs bit_range_size;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_cbs id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_cbs full_key_allocated;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_cbs bit_range;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_cbs input_type;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_cbs input_arg;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_cbs offset;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_cbs key_template;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_cbs key_id;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_cbs attach_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_cbs mode;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_cbs is_set;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_cbs key_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf1_info_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf1_info_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_cbs compare_info;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_cbs hashing_info;
} dnx_field_context_sw_context_info_context_ipmf1_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_get_cb get;
} dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_cbs;


typedef struct {
    dnx_field_context_sw_context_info_context_ipmf2_info_set_cb set;
    dnx_field_context_sw_context_info_context_ipmf2_info_get_cb get;
    
    dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_cbs cascaded_from;
} dnx_field_context_sw_context_info_context_ipmf2_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_state_table_info_mode_set_cb set;
    dnx_field_context_sw_context_info_state_table_info_mode_get_cb get;
} dnx_field_context_sw_context_info_state_table_info_mode_cbs;


typedef struct {
    dnx_field_context_sw_context_info_state_table_info_is_set_set_cb set;
    dnx_field_context_sw_context_info_state_table_info_is_set_get_cb get;
} dnx_field_context_sw_context_info_state_table_info_is_set_cbs;


typedef struct {
    dnx_field_context_sw_context_info_state_table_info_set_cb set;
    dnx_field_context_sw_context_info_state_table_info_get_cb get;
    
    dnx_field_context_sw_context_info_state_table_info_mode_cbs mode;
    
    dnx_field_context_sw_context_info_state_table_info_is_set_cbs is_set;
} dnx_field_context_sw_context_info_state_table_info_cbs;


typedef struct {
    dnx_field_context_sw_context_info_name_value_stringncat_cb stringncat;
    dnx_field_context_sw_context_info_name_value_stringncmp_cb stringncmp;
    dnx_field_context_sw_context_info_name_value_stringncpy_cb stringncpy;
    dnx_field_context_sw_context_info_name_value_stringlen_cb stringlen;
    dnx_field_context_sw_context_info_name_value_stringget_cb stringget;
} dnx_field_context_sw_context_info_name_value_cbs;


typedef struct {
    
    dnx_field_context_sw_context_info_name_value_cbs value;
} dnx_field_context_sw_context_info_name_cbs;


typedef struct {
    dnx_field_context_sw_context_info_alloc_cb alloc;
    
    dnx_field_context_sw_context_info_context_ipmf1_info_cbs context_ipmf1_info;
    
    dnx_field_context_sw_context_info_context_ipmf2_info_cbs context_ipmf2_info;
    
    dnx_field_context_sw_context_info_state_table_info_cbs state_table_info;
    
    dnx_field_context_sw_context_info_name_cbs name;
} dnx_field_context_sw_context_info_cbs;


typedef struct {
    dnx_field_context_sw_is_init_cb is_init;
    dnx_field_context_sw_init_cb init;
    
    dnx_field_context_sw_context_info_cbs context_info;
} dnx_field_context_sw_cbs;






const char *
dnx_field_context_compare_mode_e_get_name(dnx_field_context_compare_mode_e value);



const char *
dnx_field_context_hash_mode_e_get_name(dnx_field_context_hash_mode_e value);



const char *
dnx_field_context_state_table_mode_e_get_name(dnx_field_context_state_table_mode_e value);




extern dnx_field_context_sw_cbs dnx_field_context_sw;

#endif 
