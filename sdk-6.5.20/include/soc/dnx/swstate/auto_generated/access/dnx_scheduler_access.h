
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_ACCESS_H__
#define __DNX_SCHEDULER_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_scheduler_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>



typedef int (*dnx_scheduler_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_scheduler_db_init_cb)(
    int unit);


typedef int (*dnx_scheduler_db_port_alloc_cb)(
    int unit);


typedef int (*dnx_scheduler_db_port_rate_set_cb)(
    int unit, uint32 port_idx_0, int rate);


typedef int (*dnx_scheduler_db_port_rate_get_cb)(
    int unit, uint32 port_idx_0, int *rate);


typedef int (*dnx_scheduler_db_port_valid_set_cb)(
    int unit, uint32 port_idx_0, int valid);


typedef int (*dnx_scheduler_db_port_valid_get_cb)(
    int unit, uint32 port_idx_0, int *valid);


typedef int (*dnx_scheduler_db_hr_alloc_cb)(
    int unit);


typedef int (*dnx_scheduler_db_hr_is_colored_set_cb)(
    int unit, uint32 hr_idx_0, uint32 hr_idx_1, int is_colored);


typedef int (*dnx_scheduler_db_hr_is_colored_get_cb)(
    int unit, uint32 hr_idx_0, uint32 hr_idx_1, int *is_colored);


typedef int (*dnx_scheduler_db_interface_alloc_cb)(
    int unit);


typedef int (*dnx_scheduler_db_interface_modified_alloc_bitmap_cb)(
    int unit, uint32 interface_idx_0);


typedef int (*dnx_scheduler_db_interface_modified_bit_set_cb)(
    int unit, uint32 interface_idx_0, uint32 _bit_num);


typedef int (*dnx_scheduler_db_interface_modified_bit_clear_cb)(
    int unit, uint32 interface_idx_0, uint32 _bit_num);


typedef int (*dnx_scheduler_db_interface_modified_bit_get_cb)(
    int unit, uint32 interface_idx_0, uint32 _bit_num, uint8* result);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_read_cb)(
    int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_write_cb)(
    int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_and_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_or_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_xor_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_remove_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_negate_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_clear_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_set_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_null_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_test_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_eq_cb)(
    int unit, uint32 interface_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);


typedef int (*dnx_scheduler_db_interface_modified_bit_range_count_cb)(
    int unit, uint32 interface_idx_0, uint32 _first, uint32 _range, int *result);


typedef int (*dnx_scheduler_db_fmq_alloc_cb)(
    int unit);


typedef int (*dnx_scheduler_db_fmq_reserved_hr_flow_id_set_cb)(
    int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int reserved_hr_flow_id);


typedef int (*dnx_scheduler_db_fmq_reserved_hr_flow_id_get_cb)(
    int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int *reserved_hr_flow_id);


typedef int (*dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc_cb)(
    int unit, uint32 fmq_idx_0);


typedef int (*dnx_scheduler_db_fmq_nof_reserved_hr_set_cb)(
    int unit, uint32 fmq_idx_0, int nof_reserved_hr);


typedef int (*dnx_scheduler_db_fmq_nof_reserved_hr_get_cb)(
    int unit, uint32 fmq_idx_0, int *nof_reserved_hr);


typedef int (*dnx_scheduler_db_fmq_nof_reserved_hr_inc_cb)(
    int unit, uint32 fmq_idx_0, uint32 inc_value);


typedef int (*dnx_scheduler_db_fmq_nof_reserved_hr_dec_cb)(
    int unit, uint32 fmq_idx_0, uint32 dec_value);


typedef int (*dnx_scheduler_db_fmq_fmq_flow_id_set_cb)(
    int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int fmq_flow_id);


typedef int (*dnx_scheduler_db_fmq_fmq_flow_id_get_cb)(
    int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int *fmq_flow_id);


typedef int (*dnx_scheduler_db_fmq_fmq_flow_id_alloc_cb)(
    int unit, uint32 fmq_idx_0);


typedef int (*dnx_scheduler_db_general_alloc_cb)(
    int unit);


typedef int (*dnx_scheduler_db_general_reserved_erp_hr_flow_id_set_cb)(
    int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int reserved_erp_hr_flow_id);


typedef int (*dnx_scheduler_db_general_reserved_erp_hr_flow_id_get_cb)(
    int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int *reserved_erp_hr_flow_id);


typedef int (*dnx_scheduler_db_general_nof_reserved_erp_hr_set_cb)(
    int unit, uint32 general_idx_0, int nof_reserved_erp_hr);


typedef int (*dnx_scheduler_db_general_nof_reserved_erp_hr_get_cb)(
    int unit, uint32 general_idx_0, int *nof_reserved_erp_hr);


typedef int (*dnx_scheduler_db_general_nof_reserved_erp_hr_inc_cb)(
    int unit, uint32 general_idx_0, uint32 inc_value);


typedef int (*dnx_scheduler_db_general_nof_reserved_erp_hr_dec_cb)(
    int unit, uint32 general_idx_0, uint32 dec_value);




typedef struct {
    dnx_scheduler_db_port_rate_set_cb set;
    dnx_scheduler_db_port_rate_get_cb get;
} dnx_scheduler_db_port_rate_cbs;


typedef struct {
    dnx_scheduler_db_port_valid_set_cb set;
    dnx_scheduler_db_port_valid_get_cb get;
} dnx_scheduler_db_port_valid_cbs;


typedef struct {
    dnx_scheduler_db_port_alloc_cb alloc;
    
    dnx_scheduler_db_port_rate_cbs rate;
    
    dnx_scheduler_db_port_valid_cbs valid;
} dnx_scheduler_db_port_cbs;


typedef struct {
    dnx_scheduler_db_hr_is_colored_set_cb set;
    dnx_scheduler_db_hr_is_colored_get_cb get;
} dnx_scheduler_db_hr_is_colored_cbs;


typedef struct {
    dnx_scheduler_db_hr_alloc_cb alloc;
    
    dnx_scheduler_db_hr_is_colored_cbs is_colored;
} dnx_scheduler_db_hr_cbs;


typedef struct {
    dnx_scheduler_db_interface_modified_alloc_bitmap_cb alloc_bitmap;
    dnx_scheduler_db_interface_modified_bit_set_cb bit_set;
    dnx_scheduler_db_interface_modified_bit_clear_cb bit_clear;
    dnx_scheduler_db_interface_modified_bit_get_cb bit_get;
    dnx_scheduler_db_interface_modified_bit_range_read_cb bit_range_read;
    dnx_scheduler_db_interface_modified_bit_range_write_cb bit_range_write;
    dnx_scheduler_db_interface_modified_bit_range_and_cb bit_range_and;
    dnx_scheduler_db_interface_modified_bit_range_or_cb bit_range_or;
    dnx_scheduler_db_interface_modified_bit_range_xor_cb bit_range_xor;
    dnx_scheduler_db_interface_modified_bit_range_remove_cb bit_range_remove;
    dnx_scheduler_db_interface_modified_bit_range_negate_cb bit_range_negate;
    dnx_scheduler_db_interface_modified_bit_range_clear_cb bit_range_clear;
    dnx_scheduler_db_interface_modified_bit_range_set_cb bit_range_set;
    dnx_scheduler_db_interface_modified_bit_range_null_cb bit_range_null;
    dnx_scheduler_db_interface_modified_bit_range_test_cb bit_range_test;
    dnx_scheduler_db_interface_modified_bit_range_eq_cb bit_range_eq;
    dnx_scheduler_db_interface_modified_bit_range_count_cb bit_range_count;
} dnx_scheduler_db_interface_modified_cbs;


typedef struct {
    dnx_scheduler_db_interface_alloc_cb alloc;
    
    dnx_scheduler_db_interface_modified_cbs modified;
} dnx_scheduler_db_interface_cbs;


typedef struct {
    dnx_scheduler_db_fmq_reserved_hr_flow_id_set_cb set;
    dnx_scheduler_db_fmq_reserved_hr_flow_id_get_cb get;
    dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc_cb alloc;
} dnx_scheduler_db_fmq_reserved_hr_flow_id_cbs;


typedef struct {
    dnx_scheduler_db_fmq_nof_reserved_hr_set_cb set;
    dnx_scheduler_db_fmq_nof_reserved_hr_get_cb get;
    dnx_scheduler_db_fmq_nof_reserved_hr_inc_cb inc;
    dnx_scheduler_db_fmq_nof_reserved_hr_dec_cb dec;
} dnx_scheduler_db_fmq_nof_reserved_hr_cbs;


typedef struct {
    dnx_scheduler_db_fmq_fmq_flow_id_set_cb set;
    dnx_scheduler_db_fmq_fmq_flow_id_get_cb get;
    dnx_scheduler_db_fmq_fmq_flow_id_alloc_cb alloc;
} dnx_scheduler_db_fmq_fmq_flow_id_cbs;


typedef struct {
    dnx_scheduler_db_fmq_alloc_cb alloc;
    
    dnx_scheduler_db_fmq_reserved_hr_flow_id_cbs reserved_hr_flow_id;
    
    dnx_scheduler_db_fmq_nof_reserved_hr_cbs nof_reserved_hr;
    
    dnx_scheduler_db_fmq_fmq_flow_id_cbs fmq_flow_id;
} dnx_scheduler_db_fmq_cbs;


typedef struct {
    dnx_scheduler_db_general_reserved_erp_hr_flow_id_set_cb set;
    dnx_scheduler_db_general_reserved_erp_hr_flow_id_get_cb get;
} dnx_scheduler_db_general_reserved_erp_hr_flow_id_cbs;


typedef struct {
    dnx_scheduler_db_general_nof_reserved_erp_hr_set_cb set;
    dnx_scheduler_db_general_nof_reserved_erp_hr_get_cb get;
    dnx_scheduler_db_general_nof_reserved_erp_hr_inc_cb inc;
    dnx_scheduler_db_general_nof_reserved_erp_hr_dec_cb dec;
} dnx_scheduler_db_general_nof_reserved_erp_hr_cbs;


typedef struct {
    dnx_scheduler_db_general_alloc_cb alloc;
    
    dnx_scheduler_db_general_reserved_erp_hr_flow_id_cbs reserved_erp_hr_flow_id;
    
    dnx_scheduler_db_general_nof_reserved_erp_hr_cbs nof_reserved_erp_hr;
} dnx_scheduler_db_general_cbs;


typedef struct {
    dnx_scheduler_db_is_init_cb is_init;
    dnx_scheduler_db_init_cb init;
    
    dnx_scheduler_db_port_cbs port;
    
    dnx_scheduler_db_hr_cbs hr;
    
    dnx_scheduler_db_interface_cbs interface;
    
    dnx_scheduler_db_fmq_cbs fmq;
    
    dnx_scheduler_db_general_cbs general;
} dnx_scheduler_db_cbs;





extern dnx_scheduler_db_cbs dnx_scheduler_db;

#endif 
