
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DBAL_ACCESS_H__
#define __DBAL_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dbal_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>



typedef int (*dbal_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dbal_db_init_cb)(
    int unit);


typedef int (*dbal_db_sw_tables_table_type_set_cb)(
    int unit, uint32 sw_tables_idx_0, dbal_sw_state_table_type_e table_type);


typedef int (*dbal_db_sw_tables_table_type_get_cb)(
    int unit, uint32 sw_tables_idx_0, dbal_sw_state_table_type_e *table_type);


typedef int (*dbal_db_sw_tables_entries_alloc_bitmap_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _nof_bits_to_alloc);


typedef int (*dbal_db_sw_tables_entries_bit_set_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _bit_num);


typedef int (*dbal_db_sw_tables_entries_bit_clear_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _bit_num);


typedef int (*dbal_db_sw_tables_entries_bit_get_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _bit_num, uint8* result);


typedef int (*dbal_db_sw_tables_entries_bit_range_read_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dbal_db_sw_tables_entries_bit_range_write_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dbal_db_sw_tables_entries_bit_range_and_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dbal_db_sw_tables_entries_bit_range_or_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dbal_db_sw_tables_entries_bit_range_xor_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dbal_db_sw_tables_entries_bit_range_remove_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dbal_db_sw_tables_entries_bit_range_negate_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count);


typedef int (*dbal_db_sw_tables_entries_bit_range_clear_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count);


typedef int (*dbal_db_sw_tables_entries_bit_range_set_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count);


typedef int (*dbal_db_sw_tables_entries_bit_range_null_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dbal_db_sw_tables_entries_bit_range_test_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dbal_db_sw_tables_entries_bit_range_eq_cb)(
    int unit, uint32 sw_tables_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dbal_db_sw_tables_entries_bit_range_count_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _range, int *result);


typedef int (*dbal_db_sw_tables_hash_table_create_cb)(
    int unit, uint32 sw_tables_idx_0, sw_state_htbl_init_info_t *init_info);


typedef int (*dbal_db_sw_tables_hash_table_find_cb)(
    int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, uint32 *data_index, uint8 *found);


typedef int (*dbal_db_sw_tables_hash_table_insert_cb)(
    int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, const uint32 *data_index, uint8 *success);


typedef int (*dbal_db_sw_tables_hash_table_get_next_cb)(
    int unit, uint32 sw_tables_idx_0, SW_STATE_HASH_TABLE_ITER *iter, const dbal_sw_state_hash_key *key, const uint32 *data_index);


typedef int (*dbal_db_sw_tables_hash_table_clear_cb)(
    int unit, uint32 sw_tables_idx_0);


typedef int (*dbal_db_sw_tables_hash_table_delete_cb)(
    int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key);


typedef int (*dbal_db_sw_tables_hash_table_insert_at_index_cb)(
    int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, uint32 data_idx, uint8 *success);


typedef int (*dbal_db_sw_tables_hash_table_delete_by_index_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 data_index);


typedef int (*dbal_db_sw_tables_hash_table_get_by_index_cb)(
    int unit, uint32 sw_tables_idx_0, uint32 data_index, dbal_sw_state_hash_key *key, uint8 *found);


typedef int (*dbal_db_mdb_access_skip_read_from_shadow_set_cb)(
    int unit, uint8 skip_read_from_shadow);


typedef int (*dbal_db_mdb_access_skip_read_from_shadow_get_cb)(
    int unit, uint8 *skip_read_from_shadow);


typedef int (*dbal_db_mdb_phy_tables_nof_entries_set_cb)(
    int unit, uint32 mdb_phy_tables_idx_0, int nof_entries);


typedef int (*dbal_db_mdb_phy_tables_nof_entries_get_cb)(
    int unit, uint32 mdb_phy_tables_idx_0, int *nof_entries);


typedef int (*dbal_db_mdb_phy_tables_nof_entries_inc_cb)(
    int unit, uint32 mdb_phy_tables_idx_0, uint32 inc_value);


typedef int (*dbal_db_mdb_phy_tables_nof_entries_dec_cb)(
    int unit, uint32 mdb_phy_tables_idx_0, uint32 dec_value);


typedef int (*dbal_db_tables_properties_table_status_set_cb)(
    int unit, uint32 tables_properties_idx_0, dbal_table_status_e table_status);


typedef int (*dbal_db_tables_properties_table_status_get_cb)(
    int unit, uint32 tables_properties_idx_0, dbal_table_status_e *table_status);


typedef int (*dbal_db_tables_properties_nof_set_operations_set_cb)(
    int unit, uint32 tables_properties_idx_0, int nof_set_operations);


typedef int (*dbal_db_tables_properties_nof_set_operations_get_cb)(
    int unit, uint32 tables_properties_idx_0, int *nof_set_operations);


typedef int (*dbal_db_tables_properties_nof_set_operations_inc_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 inc_value);


typedef int (*dbal_db_tables_properties_nof_set_operations_dec_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 dec_value);


typedef int (*dbal_db_tables_properties_nof_get_operations_set_cb)(
    int unit, uint32 tables_properties_idx_0, int nof_get_operations);


typedef int (*dbal_db_tables_properties_nof_get_operations_get_cb)(
    int unit, uint32 tables_properties_idx_0, int *nof_get_operations);


typedef int (*dbal_db_tables_properties_nof_get_operations_inc_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 inc_value);


typedef int (*dbal_db_tables_properties_nof_get_operations_dec_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 dec_value);


typedef int (*dbal_db_tables_properties_nof_entries_set_cb)(
    int unit, uint32 tables_properties_idx_0, int nof_entries);


typedef int (*dbal_db_tables_properties_nof_entries_get_cb)(
    int unit, uint32 tables_properties_idx_0, int *nof_entries);


typedef int (*dbal_db_tables_properties_nof_entries_inc_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 inc_value);


typedef int (*dbal_db_tables_properties_nof_entries_dec_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 dec_value);


typedef int (*dbal_db_tables_properties_tcam_handler_id_set_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 tcam_handler_id);


typedef int (*dbal_db_tables_properties_tcam_handler_id_get_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 *tcam_handler_id);


typedef int (*dbal_db_tables_properties_indications_bm_set_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 indications_bm);


typedef int (*dbal_db_tables_properties_indications_bm_get_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 *indications_bm);


typedef int (*dbal_db_tables_properties_iterator_optimized_alloc_cb)(
    int unit, uint32 tables_properties_idx_0);


typedef int (*dbal_db_tables_properties_iterator_optimized_bitmap_alloc_bitmap_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _nof_bits_to_alloc);


typedef int (*dbal_db_tables_properties_iterator_optimized_bitmap_bit_set_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num);


typedef int (*dbal_db_tables_properties_iterator_optimized_bitmap_bit_clear_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num);


typedef int (*dbal_db_tables_properties_iterator_optimized_bitmap_bit_get_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num, uint8* result);


typedef int (*dbal_db_tables_properties_iterator_optimized_bitmap_bit_range_read_cb)(
    int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dbal_db_dbal_dynamic_tables_table_id_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, dbal_tables_e table_id);


typedef int (*dbal_db_dbal_dynamic_tables_table_id_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, dbal_tables_e *table_id);


typedef int (*dbal_db_dbal_dynamic_tables_table_name_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 table_name_idx_0, CONST char *table_name);


typedef int (*dbal_db_dbal_dynamic_tables_table_name_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 table_name_idx_0, CONST char **table_name);


typedef int (*dbal_db_dbal_dynamic_tables_labels_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 labels_idx_0, dbal_labels_e labels);


typedef int (*dbal_db_dbal_dynamic_tables_labels_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 labels_idx_0, dbal_labels_e *labels);


typedef int (*dbal_db_dbal_dynamic_tables_labels_alloc_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_tables_data_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, CONST dbal_logical_table_t *data);


typedef int (*dbal_db_dbal_dynamic_tables_data_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, CONST dbal_logical_table_t **data);


typedef int (*dbal_db_dbal_dynamic_tables_key_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t *key_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_key_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t **key_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_key_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_tables_multi_res_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t *multi_res_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_multi_res_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t **multi_res_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_multi_res_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_tables_res_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t *res_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_res_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t **res_info_data);


typedef int (*dbal_db_dbal_dynamic_tables_res_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_set_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF access_info);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_get_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF *access_info);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_alloc_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 _nof_bytes_to_alloc);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_memread_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_memwrite_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_memcmp_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*dbal_db_dbal_dynamic_tables_access_info_memset_cb)(
    int unit, uint32 dbal_dynamic_tables_idx_0, uint32 _offset, size_t _len, int _value);


typedef int (*dbal_db_dbal_dynamic_xml_tables_table_id_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, dbal_tables_e table_id);


typedef int (*dbal_db_dbal_dynamic_xml_tables_table_id_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, dbal_tables_e *table_id);


typedef int (*dbal_db_dbal_dynamic_xml_tables_table_name_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 table_name_idx_0, CONST char *table_name);


typedef int (*dbal_db_dbal_dynamic_xml_tables_table_name_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 table_name_idx_0, CONST char **table_name);


typedef int (*dbal_db_dbal_dynamic_xml_tables_labels_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 labels_idx_0, dbal_labels_e labels);


typedef int (*dbal_db_dbal_dynamic_xml_tables_labels_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 labels_idx_0, dbal_labels_e *labels);


typedef int (*dbal_db_dbal_dynamic_xml_tables_labels_alloc_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_xml_tables_data_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, CONST dbal_logical_table_t *data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_data_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, CONST dbal_logical_table_t **data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_key_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t *key_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_key_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t **key_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_key_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t *multi_res_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t **multi_res_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_xml_tables_res_info_data_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t *res_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_res_info_data_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t **res_info_data);


typedef int (*dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_set_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF access_info);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_get_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF *access_info);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_alloc_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 _nof_bytes_to_alloc);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_memread_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint8 *_dst, uint32 _offset, size_t _len);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_memwrite_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, const uint8 *_src, uint32 _offset, size_t _len);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_memcmp_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result);


typedef int (*dbal_db_dbal_dynamic_xml_tables_access_info_memset_cb)(
    int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 _offset, size_t _len, int _value);


typedef int (*dbal_db_dbal_fields_field_id_set_cb)(
    int unit, uint32 dbal_fields_idx_0, dbal_fields_e field_id);


typedef int (*dbal_db_dbal_fields_field_id_get_cb)(
    int unit, uint32 dbal_fields_idx_0, dbal_fields_e *field_id);


typedef int (*dbal_db_dbal_fields_field_type_set_cb)(
    int unit, uint32 dbal_fields_idx_0, dbal_field_types_defs_e field_type);


typedef int (*dbal_db_dbal_fields_field_type_get_cb)(
    int unit, uint32 dbal_fields_idx_0, dbal_field_types_defs_e *field_type);


typedef int (*dbal_db_dbal_fields_field_name_set_cb)(
    int unit, uint32 dbal_fields_idx_0, uint32 field_name_idx_0, CONST char *field_name);


typedef int (*dbal_db_dbal_fields_field_name_get_cb)(
    int unit, uint32 dbal_fields_idx_0, uint32 field_name_idx_0, CONST char **field_name);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_set_cb)(
    int unit, uint32 multi_res_info_status_idx_0, dbal_multi_res_info_status_e multi_res_info_status);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_get_cb)(
    int unit, uint32 multi_res_info_status_idx_0, dbal_multi_res_info_status_e *multi_res_info_status);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_alloc_cb)(
    int unit);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_alloc_cb)(
    int unit);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_set_cb)(
    int unit, uint32 multi_res_info_data_sep_idx_0, CONST multi_res_info_t *multi_res_info_data);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_get_cb)(
    int unit, uint32 multi_res_info_data_sep_idx_0, CONST multi_res_info_t **multi_res_info_data);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_set_cb)(
    int unit, uint32 multi_res_info_data_sep_idx_0, uint32 res_info_data_idx_0, dbal_table_field_info_t res_info_data);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_get_cb)(
    int unit, uint32 multi_res_info_data_sep_idx_0, uint32 res_info_data_idx_0, dbal_table_field_info_t *res_info_data);


typedef int (*dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_alloc_cb)(
    int unit, uint32 multi_res_info_data_sep_idx_0);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_find_cb)(
    int unit, const hw_ent_direct_map_hash_key *key, uint32 *data_index, uint8 *found);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_insert_cb)(
    int unit, const hw_ent_direct_map_hash_key *key, const uint32 *data_index, uint8 *success);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const hw_ent_direct_map_hash_key *key, const uint32 *data_index);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_clear_cb)(
    int unit);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_delete_cb)(
    int unit, const hw_ent_direct_map_hash_key *key);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_insert_at_index_cb)(
    int unit, const hw_ent_direct_map_hash_key *key, uint32 data_idx, uint8 *success);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_delete_by_index_cb)(
    int unit, uint32 data_index);


typedef int (*dbal_db_hw_ent_direct_map_hash_tbl_get_by_index_cb)(
    int unit, uint32 data_index, hw_ent_direct_map_hash_key *key, uint8 *found);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_create_cb)(
    int unit, sw_state_htbl_init_info_t *init_info);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_find_cb)(
    int unit, const hw_ent_groups_map_hash_key *key, uint32 *data_index, uint8 *found);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_insert_cb)(
    int unit, const hw_ent_groups_map_hash_key *key, const uint32 *data_index, uint8 *success);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_get_next_cb)(
    int unit, SW_STATE_HASH_TABLE_ITER *iter, const hw_ent_groups_map_hash_key *key, const uint32 *data_index);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_clear_cb)(
    int unit);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_delete_cb)(
    int unit, const hw_ent_groups_map_hash_key *key);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_insert_at_index_cb)(
    int unit, const hw_ent_groups_map_hash_key *key, uint32 data_idx, uint8 *success);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_delete_by_index_cb)(
    int unit, uint32 data_index);


typedef int (*dbal_db_hw_ent_groups_map_hash_tbl_get_by_index_cb)(
    int unit, uint32 data_index, hw_ent_groups_map_hash_key *key, uint8 *found);




typedef struct {
    dbal_db_sw_tables_table_type_set_cb set;
    dbal_db_sw_tables_table_type_get_cb get;
} dbal_db_sw_tables_table_type_cbs;


typedef struct {
    dbal_db_sw_tables_entries_alloc_bitmap_cb alloc_bitmap;
    dbal_db_sw_tables_entries_bit_set_cb bit_set;
    dbal_db_sw_tables_entries_bit_clear_cb bit_clear;
    dbal_db_sw_tables_entries_bit_get_cb bit_get;
    dbal_db_sw_tables_entries_bit_range_read_cb bit_range_read;
    dbal_db_sw_tables_entries_bit_range_write_cb bit_range_write;
    dbal_db_sw_tables_entries_bit_range_and_cb bit_range_and;
    dbal_db_sw_tables_entries_bit_range_or_cb bit_range_or;
    dbal_db_sw_tables_entries_bit_range_xor_cb bit_range_xor;
    dbal_db_sw_tables_entries_bit_range_remove_cb bit_range_remove;
    dbal_db_sw_tables_entries_bit_range_negate_cb bit_range_negate;
    dbal_db_sw_tables_entries_bit_range_clear_cb bit_range_clear;
    dbal_db_sw_tables_entries_bit_range_set_cb bit_range_set;
    dbal_db_sw_tables_entries_bit_range_null_cb bit_range_null;
    dbal_db_sw_tables_entries_bit_range_test_cb bit_range_test;
    dbal_db_sw_tables_entries_bit_range_eq_cb bit_range_eq;
    dbal_db_sw_tables_entries_bit_range_count_cb bit_range_count;
} dbal_db_sw_tables_entries_cbs;


typedef struct {
    dbal_db_sw_tables_hash_table_create_cb create;
    dbal_db_sw_tables_hash_table_find_cb find;
    dbal_db_sw_tables_hash_table_insert_cb insert;
    dbal_db_sw_tables_hash_table_get_next_cb get_next;
    dbal_db_sw_tables_hash_table_clear_cb clear;
    dbal_db_sw_tables_hash_table_delete_cb delete;
    dbal_db_sw_tables_hash_table_insert_at_index_cb insert_at_index;
    dbal_db_sw_tables_hash_table_delete_by_index_cb delete_by_index;
    dbal_db_sw_tables_hash_table_get_by_index_cb get_by_index;
} dbal_db_sw_tables_hash_table_cbs;


typedef struct {
    
    dbal_db_sw_tables_table_type_cbs table_type;
    
    dbal_db_sw_tables_entries_cbs entries;
    
    dbal_db_sw_tables_hash_table_cbs hash_table;
} dbal_db_sw_tables_cbs;


typedef struct {
    dbal_db_mdb_access_skip_read_from_shadow_set_cb set;
    dbal_db_mdb_access_skip_read_from_shadow_get_cb get;
} dbal_db_mdb_access_skip_read_from_shadow_cbs;


typedef struct {
    
    dbal_db_mdb_access_skip_read_from_shadow_cbs skip_read_from_shadow;
} dbal_db_mdb_access_cbs;


typedef struct {
    dbal_db_mdb_phy_tables_nof_entries_set_cb set;
    dbal_db_mdb_phy_tables_nof_entries_get_cb get;
    dbal_db_mdb_phy_tables_nof_entries_inc_cb inc;
    dbal_db_mdb_phy_tables_nof_entries_dec_cb dec;
} dbal_db_mdb_phy_tables_nof_entries_cbs;


typedef struct {
    
    dbal_db_mdb_phy_tables_nof_entries_cbs nof_entries;
} dbal_db_mdb_phy_tables_cbs;


typedef struct {
    dbal_db_tables_properties_table_status_set_cb set;
    dbal_db_tables_properties_table_status_get_cb get;
} dbal_db_tables_properties_table_status_cbs;


typedef struct {
    dbal_db_tables_properties_nof_set_operations_set_cb set;
    dbal_db_tables_properties_nof_set_operations_get_cb get;
    dbal_db_tables_properties_nof_set_operations_inc_cb inc;
    dbal_db_tables_properties_nof_set_operations_dec_cb dec;
} dbal_db_tables_properties_nof_set_operations_cbs;


typedef struct {
    dbal_db_tables_properties_nof_get_operations_set_cb set;
    dbal_db_tables_properties_nof_get_operations_get_cb get;
    dbal_db_tables_properties_nof_get_operations_inc_cb inc;
    dbal_db_tables_properties_nof_get_operations_dec_cb dec;
} dbal_db_tables_properties_nof_get_operations_cbs;


typedef struct {
    dbal_db_tables_properties_nof_entries_set_cb set;
    dbal_db_tables_properties_nof_entries_get_cb get;
    dbal_db_tables_properties_nof_entries_inc_cb inc;
    dbal_db_tables_properties_nof_entries_dec_cb dec;
} dbal_db_tables_properties_nof_entries_cbs;


typedef struct {
    dbal_db_tables_properties_tcam_handler_id_set_cb set;
    dbal_db_tables_properties_tcam_handler_id_get_cb get;
} dbal_db_tables_properties_tcam_handler_id_cbs;


typedef struct {
    dbal_db_tables_properties_indications_bm_set_cb set;
    dbal_db_tables_properties_indications_bm_get_cb get;
} dbal_db_tables_properties_indications_bm_cbs;


typedef struct {
    dbal_db_tables_properties_iterator_optimized_bitmap_alloc_bitmap_cb alloc_bitmap;
    dbal_db_tables_properties_iterator_optimized_bitmap_bit_set_cb bit_set;
    dbal_db_tables_properties_iterator_optimized_bitmap_bit_clear_cb bit_clear;
    dbal_db_tables_properties_iterator_optimized_bitmap_bit_get_cb bit_get;
    dbal_db_tables_properties_iterator_optimized_bitmap_bit_range_read_cb bit_range_read;
} dbal_db_tables_properties_iterator_optimized_bitmap_cbs;


typedef struct {
    dbal_db_tables_properties_iterator_optimized_alloc_cb alloc;
    
    dbal_db_tables_properties_iterator_optimized_bitmap_cbs bitmap;
} dbal_db_tables_properties_iterator_optimized_cbs;


typedef struct {
    
    dbal_db_tables_properties_table_status_cbs table_status;
    
    dbal_db_tables_properties_nof_set_operations_cbs nof_set_operations;
    
    dbal_db_tables_properties_nof_get_operations_cbs nof_get_operations;
    
    dbal_db_tables_properties_nof_entries_cbs nof_entries;
    
    dbal_db_tables_properties_tcam_handler_id_cbs tcam_handler_id;
    
    dbal_db_tables_properties_indications_bm_cbs indications_bm;
    
    dbal_db_tables_properties_iterator_optimized_cbs iterator_optimized;
} dbal_db_tables_properties_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_table_id_set_cb set;
    dbal_db_dbal_dynamic_tables_table_id_get_cb get;
} dbal_db_dbal_dynamic_tables_table_id_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_table_name_set_cb set;
    dbal_db_dbal_dynamic_tables_table_name_get_cb get;
} dbal_db_dbal_dynamic_tables_table_name_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_labels_set_cb set;
    dbal_db_dbal_dynamic_tables_labels_get_cb get;
    dbal_db_dbal_dynamic_tables_labels_alloc_cb alloc;
} dbal_db_dbal_dynamic_tables_labels_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_data_set_cb set;
    dbal_db_dbal_dynamic_tables_data_get_cb get;
} dbal_db_dbal_dynamic_tables_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_key_info_data_set_cb set;
    dbal_db_dbal_dynamic_tables_key_info_data_get_cb get;
    dbal_db_dbal_dynamic_tables_key_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_tables_key_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_multi_res_info_data_set_cb set;
    dbal_db_dbal_dynamic_tables_multi_res_info_data_get_cb get;
    dbal_db_dbal_dynamic_tables_multi_res_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_tables_multi_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_res_info_data_set_cb set;
    dbal_db_dbal_dynamic_tables_res_info_data_get_cb get;
    dbal_db_dbal_dynamic_tables_res_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_tables_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_tables_access_info_set_cb set;
    dbal_db_dbal_dynamic_tables_access_info_get_cb get;
    dbal_db_dbal_dynamic_tables_access_info_alloc_cb alloc;
    dbal_db_dbal_dynamic_tables_access_info_memread_cb memread;
    dbal_db_dbal_dynamic_tables_access_info_memwrite_cb memwrite;
    dbal_db_dbal_dynamic_tables_access_info_memcmp_cb memcmp;
    dbal_db_dbal_dynamic_tables_access_info_memset_cb memset;
} dbal_db_dbal_dynamic_tables_access_info_cbs;


typedef struct {
    
    dbal_db_dbal_dynamic_tables_table_id_cbs table_id;
    
    dbal_db_dbal_dynamic_tables_table_name_cbs table_name;
    
    dbal_db_dbal_dynamic_tables_labels_cbs labels;
    
    dbal_db_dbal_dynamic_tables_data_cbs data;
    
    dbal_db_dbal_dynamic_tables_key_info_data_cbs key_info_data;
    
    dbal_db_dbal_dynamic_tables_multi_res_info_data_cbs multi_res_info_data;
    
    dbal_db_dbal_dynamic_tables_res_info_data_cbs res_info_data;
    
    dbal_db_dbal_dynamic_tables_access_info_cbs access_info;
} dbal_db_dbal_dynamic_tables_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_table_id_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_table_id_get_cb get;
} dbal_db_dbal_dynamic_xml_tables_table_id_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_table_name_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_table_name_get_cb get;
} dbal_db_dbal_dynamic_xml_tables_table_name_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_labels_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_labels_get_cb get;
    dbal_db_dbal_dynamic_xml_tables_labels_alloc_cb alloc;
} dbal_db_dbal_dynamic_xml_tables_labels_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_data_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_data_get_cb get;
} dbal_db_dbal_dynamic_xml_tables_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_key_info_data_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_key_info_data_get_cb get;
    dbal_db_dbal_dynamic_xml_tables_key_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_xml_tables_key_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_get_cb get;
    dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_res_info_data_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_res_info_data_get_cb get;
    dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc_cb alloc;
} dbal_db_dbal_dynamic_xml_tables_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_dynamic_xml_tables_access_info_set_cb set;
    dbal_db_dbal_dynamic_xml_tables_access_info_get_cb get;
    dbal_db_dbal_dynamic_xml_tables_access_info_alloc_cb alloc;
    dbal_db_dbal_dynamic_xml_tables_access_info_memread_cb memread;
    dbal_db_dbal_dynamic_xml_tables_access_info_memwrite_cb memwrite;
    dbal_db_dbal_dynamic_xml_tables_access_info_memcmp_cb memcmp;
    dbal_db_dbal_dynamic_xml_tables_access_info_memset_cb memset;
} dbal_db_dbal_dynamic_xml_tables_access_info_cbs;


typedef struct {
    
    dbal_db_dbal_dynamic_xml_tables_table_id_cbs table_id;
    
    dbal_db_dbal_dynamic_xml_tables_table_name_cbs table_name;
    
    dbal_db_dbal_dynamic_xml_tables_labels_cbs labels;
    
    dbal_db_dbal_dynamic_xml_tables_data_cbs data;
    
    dbal_db_dbal_dynamic_xml_tables_key_info_data_cbs key_info_data;
    
    dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_cbs multi_res_info_data;
    
    dbal_db_dbal_dynamic_xml_tables_res_info_data_cbs res_info_data;
    
    dbal_db_dbal_dynamic_xml_tables_access_info_cbs access_info;
} dbal_db_dbal_dynamic_xml_tables_cbs;


typedef struct {
    dbal_db_dbal_fields_field_id_set_cb set;
    dbal_db_dbal_fields_field_id_get_cb get;
} dbal_db_dbal_fields_field_id_cbs;


typedef struct {
    dbal_db_dbal_fields_field_type_set_cb set;
    dbal_db_dbal_fields_field_type_get_cb get;
} dbal_db_dbal_fields_field_type_cbs;


typedef struct {
    dbal_db_dbal_fields_field_name_set_cb set;
    dbal_db_dbal_fields_field_name_get_cb get;
} dbal_db_dbal_fields_field_name_cbs;


typedef struct {
    
    dbal_db_dbal_fields_field_id_cbs field_id;
    
    dbal_db_dbal_fields_field_type_cbs field_type;
    
    dbal_db_dbal_fields_field_name_cbs field_name;
} dbal_db_dbal_fields_cbs;


typedef struct {
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_set_cb set;
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_get_cb get;
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_alloc_cb alloc;
} dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_cbs;


typedef struct {
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_set_cb set;
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_get_cb get;
} dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_set_cb set;
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_get_cb get;
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_alloc_cb alloc;
} dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_cbs;


typedef struct {
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_alloc_cb alloc;
    
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_cbs multi_res_info_data;
    
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_cbs res_info_data;
} dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_cbs;


typedef struct {
    
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_cbs multi_res_info_status;
    
    dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_cbs multi_res_info_data_sep;
} dbal_db_dbal_ppmc_table_res_types_cbs;


typedef struct {
    dbal_db_hw_ent_direct_map_hash_tbl_create_cb create;
    dbal_db_hw_ent_direct_map_hash_tbl_find_cb find;
    dbal_db_hw_ent_direct_map_hash_tbl_insert_cb insert;
    dbal_db_hw_ent_direct_map_hash_tbl_get_next_cb get_next;
    dbal_db_hw_ent_direct_map_hash_tbl_clear_cb clear;
    dbal_db_hw_ent_direct_map_hash_tbl_delete_cb delete;
    dbal_db_hw_ent_direct_map_hash_tbl_insert_at_index_cb insert_at_index;
    dbal_db_hw_ent_direct_map_hash_tbl_delete_by_index_cb delete_by_index;
    dbal_db_hw_ent_direct_map_hash_tbl_get_by_index_cb get_by_index;
} dbal_db_hw_ent_direct_map_hash_tbl_cbs;


typedef struct {
    dbal_db_hw_ent_groups_map_hash_tbl_create_cb create;
    dbal_db_hw_ent_groups_map_hash_tbl_find_cb find;
    dbal_db_hw_ent_groups_map_hash_tbl_insert_cb insert;
    dbal_db_hw_ent_groups_map_hash_tbl_get_next_cb get_next;
    dbal_db_hw_ent_groups_map_hash_tbl_clear_cb clear;
    dbal_db_hw_ent_groups_map_hash_tbl_delete_cb delete;
    dbal_db_hw_ent_groups_map_hash_tbl_insert_at_index_cb insert_at_index;
    dbal_db_hw_ent_groups_map_hash_tbl_delete_by_index_cb delete_by_index;
    dbal_db_hw_ent_groups_map_hash_tbl_get_by_index_cb get_by_index;
} dbal_db_hw_ent_groups_map_hash_tbl_cbs;


typedef struct {
    dbal_db_is_init_cb is_init;
    dbal_db_init_cb init;
    
    dbal_db_sw_tables_cbs sw_tables;
    
    dbal_db_mdb_access_cbs mdb_access;
    
    dbal_db_mdb_phy_tables_cbs mdb_phy_tables;
    
    dbal_db_tables_properties_cbs tables_properties;
    
    dbal_db_dbal_dynamic_tables_cbs dbal_dynamic_tables;
    
    dbal_db_dbal_dynamic_xml_tables_cbs dbal_dynamic_xml_tables;
    
    dbal_db_dbal_fields_cbs dbal_fields;
    
    dbal_db_dbal_ppmc_table_res_types_cbs dbal_ppmc_table_res_types;
    
    dbal_db_hw_ent_direct_map_hash_tbl_cbs hw_ent_direct_map_hash_tbl;
    
    dbal_db_hw_ent_groups_map_hash_tbl_cbs hw_ent_groups_map_hash_tbl;
} dbal_db_cbs;






const char *
dbal_sw_state_table_type_e_get_name(dbal_sw_state_table_type_e value);



const char *
dbal_table_status_e_get_name(dbal_table_status_e value);



const char *
dbal_multi_res_info_status_e_get_name(dbal_multi_res_info_status_e value);




extern dbal_db_cbs dbal_db;

#endif 
