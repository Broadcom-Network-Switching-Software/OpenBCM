
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IPQ_ACCESS_H__
#define __DNX_IPQ_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ipq_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>



typedef int (*dnx_ipq_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_ipq_db_init_cb)(
    int unit);


typedef int (*dnx_ipq_db_base_queues_alloc_cb)(
    int unit);


typedef int (*dnx_ipq_db_base_queues_num_cos_set_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint8 num_cos);


typedef int (*dnx_ipq_db_base_queues_num_cos_get_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint8 *num_cos);


typedef int (*dnx_ipq_db_base_queues_sys_port_ref_counter_set_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, int sys_port_ref_counter);


typedef int (*dnx_ipq_db_base_queues_sys_port_ref_counter_get_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, int *sys_port_ref_counter);


typedef int (*dnx_ipq_db_base_queues_sys_port_ref_counter_inc_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint32 inc_value);


typedef int (*dnx_ipq_db_base_queues_sys_port_ref_counter_dec_cb)(
    int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint32 dec_value);


typedef int (*dnx_ipq_db_base_queue_is_asymm_alloc_bitmap_cb)(
    int unit);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_set_cb)(
    int unit, uint32 _bit_num);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_clear_cb)(
    int unit, uint32 _bit_num);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_get_cb)(
    int unit, uint32 _bit_num, uint8* result);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_read_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_write_cb)(
    int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_and_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_or_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_xor_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_remove_cb)(
    int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_negate_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_clear_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_set_cb)(
    int unit, uint32 _first, uint32 _count);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_null_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_test_cb)(
    int unit, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_eq_cb)(
    int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_ipq_db_base_queue_is_asymm_bit_range_count_cb)(
    int unit, uint32 _first, uint32 _range, int *result);




typedef struct {
    dnx_ipq_db_base_queues_num_cos_set_cb set;
    dnx_ipq_db_base_queues_num_cos_get_cb get;
} dnx_ipq_db_base_queues_num_cos_cbs;


typedef struct {
    dnx_ipq_db_base_queues_sys_port_ref_counter_set_cb set;
    dnx_ipq_db_base_queues_sys_port_ref_counter_get_cb get;
    dnx_ipq_db_base_queues_sys_port_ref_counter_inc_cb inc;
    dnx_ipq_db_base_queues_sys_port_ref_counter_dec_cb dec;
} dnx_ipq_db_base_queues_sys_port_ref_counter_cbs;


typedef struct {
    dnx_ipq_db_base_queues_alloc_cb alloc;
    
    dnx_ipq_db_base_queues_num_cos_cbs num_cos;
    
    dnx_ipq_db_base_queues_sys_port_ref_counter_cbs sys_port_ref_counter;
} dnx_ipq_db_base_queues_cbs;


typedef struct {
    dnx_ipq_db_base_queue_is_asymm_alloc_bitmap_cb alloc_bitmap;
    dnx_ipq_db_base_queue_is_asymm_bit_set_cb bit_set;
    dnx_ipq_db_base_queue_is_asymm_bit_clear_cb bit_clear;
    dnx_ipq_db_base_queue_is_asymm_bit_get_cb bit_get;
    dnx_ipq_db_base_queue_is_asymm_bit_range_read_cb bit_range_read;
    dnx_ipq_db_base_queue_is_asymm_bit_range_write_cb bit_range_write;
    dnx_ipq_db_base_queue_is_asymm_bit_range_and_cb bit_range_and;
    dnx_ipq_db_base_queue_is_asymm_bit_range_or_cb bit_range_or;
    dnx_ipq_db_base_queue_is_asymm_bit_range_xor_cb bit_range_xor;
    dnx_ipq_db_base_queue_is_asymm_bit_range_remove_cb bit_range_remove;
    dnx_ipq_db_base_queue_is_asymm_bit_range_negate_cb bit_range_negate;
    dnx_ipq_db_base_queue_is_asymm_bit_range_clear_cb bit_range_clear;
    dnx_ipq_db_base_queue_is_asymm_bit_range_set_cb bit_range_set;
    dnx_ipq_db_base_queue_is_asymm_bit_range_null_cb bit_range_null;
    dnx_ipq_db_base_queue_is_asymm_bit_range_test_cb bit_range_test;
    dnx_ipq_db_base_queue_is_asymm_bit_range_eq_cb bit_range_eq;
    dnx_ipq_db_base_queue_is_asymm_bit_range_count_cb bit_range_count;
} dnx_ipq_db_base_queue_is_asymm_cbs;


typedef struct {
    dnx_ipq_db_is_init_cb is_init;
    dnx_ipq_db_init_cb init;
    
    dnx_ipq_db_base_queues_cbs base_queues;
    
    dnx_ipq_db_base_queue_is_asymm_cbs base_queue_is_asymm;
} dnx_ipq_db_cbs;





extern dnx_ipq_db_cbs dnx_ipq_db;

#endif 
