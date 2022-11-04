
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_ACCESS_H__
#define __DNX_SCHEDULER_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/dnx_scheduler_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef int (*dnx_scheduler_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_scheduler_db_init_cb)(
    int unit);

typedef int (*dnx_scheduler_db_port_shaper_alloc_cb)(
    int unit);

typedef int (*dnx_scheduler_db_port_shaper_rate_set_cb)(
    int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int rate);

typedef int (*dnx_scheduler_db_port_shaper_rate_get_cb)(
    int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int *rate);

typedef int (*dnx_scheduler_db_port_shaper_valid_set_cb)(
    int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int valid);

typedef int (*dnx_scheduler_db_port_shaper_valid_get_cb)(
    int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int *valid);

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

typedef int (*dnx_scheduler_db_virtual_flows_is_enabled_set_cb)(
    int unit, sch_virtual_flows_sw_state_type_e is_enabled);

typedef int (*dnx_scheduler_db_virtual_flows_is_enabled_get_cb)(
    int unit, sch_virtual_flows_sw_state_type_e *is_enabled);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_get_cb)(
    int unit, uint32 is_flow_virtual_idx_0, CONST SHR_BITDCL **is_flow_virtual);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_bitmap_cb)(
    int unit, uint32 is_flow_virtual_idx_0);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_cb)(
    int unit);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_set_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_clear_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_get_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num, uint8* result);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_read_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_write_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_and_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_or_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_xor_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_remove_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_negate_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_clear_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_set_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_null_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_test_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_eq_cb)(
    int unit, uint32 is_flow_virtual_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result);

typedef int (*dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_count_cb)(
    int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _range, int *result);

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

typedef int (*dnx_scheduler_db_general_non_default_flow_order_set_cb)(
    int unit, uint32 general_idx_0, uint8 non_default_flow_order);

typedef int (*dnx_scheduler_db_general_non_default_flow_order_get_cb)(
    int unit, uint32 general_idx_0, uint8 *non_default_flow_order);

typedef int (*dnx_scheduler_db_general_is_composite_fq_supported_set_cb)(
    int unit, uint32 general_idx_0, uint8 is_composite_fq_supported);

typedef int (*dnx_scheduler_db_general_is_composite_fq_supported_get_cb)(
    int unit, uint32 general_idx_0, uint8 *is_composite_fq_supported);

typedef int (*dnx_scheduler_db_low_rate_ranges_alloc_cb)(
    int unit);

typedef int (*dnx_scheduler_db_low_rate_ranges_pattern_set_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 pattern);

typedef int (*dnx_scheduler_db_low_rate_ranges_pattern_get_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *pattern);

typedef int (*dnx_scheduler_db_low_rate_ranges_range_start_set_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 range_start);

typedef int (*dnx_scheduler_db_low_rate_ranges_range_start_get_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *range_start);

typedef int (*dnx_scheduler_db_low_rate_ranges_range_end_set_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 range_end);

typedef int (*dnx_scheduler_db_low_rate_ranges_range_end_get_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *range_end);

typedef int (*dnx_scheduler_db_low_rate_ranges_is_enabled_set_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 is_enabled);

typedef int (*dnx_scheduler_db_low_rate_ranges_is_enabled_get_cb)(
    int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *is_enabled);



typedef struct {
    dnx_scheduler_db_port_shaper_rate_set_cb set;
    dnx_scheduler_db_port_shaper_rate_get_cb get;
} dnx_scheduler_db_port_shaper_rate_cbs;

typedef struct {
    dnx_scheduler_db_port_shaper_valid_set_cb set;
    dnx_scheduler_db_port_shaper_valid_get_cb get;
} dnx_scheduler_db_port_shaper_valid_cbs;

typedef struct {
    dnx_scheduler_db_port_shaper_alloc_cb alloc;
    dnx_scheduler_db_port_shaper_rate_cbs rate;
    dnx_scheduler_db_port_shaper_valid_cbs valid;
} dnx_scheduler_db_port_shaper_cbs;

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
    dnx_scheduler_db_virtual_flows_is_enabled_set_cb set;
    dnx_scheduler_db_virtual_flows_is_enabled_get_cb get;
} dnx_scheduler_db_virtual_flows_is_enabled_cbs;

typedef struct {
    dnx_scheduler_db_virtual_flows_is_flow_virtual_get_cb get;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_bitmap_cb alloc_bitmap;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_cb alloc;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_set_cb bit_set;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_clear_cb bit_clear;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_get_cb bit_get;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_read_cb bit_range_read;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_write_cb bit_range_write;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_and_cb bit_range_and;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_or_cb bit_range_or;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_xor_cb bit_range_xor;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_remove_cb bit_range_remove;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_negate_cb bit_range_negate;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_clear_cb bit_range_clear;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_set_cb bit_range_set;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_null_cb bit_range_null;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_test_cb bit_range_test;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_eq_cb bit_range_eq;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_count_cb bit_range_count;
} dnx_scheduler_db_virtual_flows_is_flow_virtual_cbs;

typedef struct {
    dnx_scheduler_db_virtual_flows_is_enabled_cbs is_enabled;
    dnx_scheduler_db_virtual_flows_is_flow_virtual_cbs is_flow_virtual;
} dnx_scheduler_db_virtual_flows_cbs;

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
    dnx_scheduler_db_general_non_default_flow_order_set_cb set;
    dnx_scheduler_db_general_non_default_flow_order_get_cb get;
} dnx_scheduler_db_general_non_default_flow_order_cbs;

typedef struct {
    dnx_scheduler_db_general_is_composite_fq_supported_set_cb set;
    dnx_scheduler_db_general_is_composite_fq_supported_get_cb get;
} dnx_scheduler_db_general_is_composite_fq_supported_cbs;

typedef struct {
    dnx_scheduler_db_general_alloc_cb alloc;
    dnx_scheduler_db_general_reserved_erp_hr_flow_id_cbs reserved_erp_hr_flow_id;
    dnx_scheduler_db_general_nof_reserved_erp_hr_cbs nof_reserved_erp_hr;
    dnx_scheduler_db_general_non_default_flow_order_cbs non_default_flow_order;
    dnx_scheduler_db_general_is_composite_fq_supported_cbs is_composite_fq_supported;
} dnx_scheduler_db_general_cbs;

typedef struct {
    dnx_scheduler_db_low_rate_ranges_pattern_set_cb set;
    dnx_scheduler_db_low_rate_ranges_pattern_get_cb get;
} dnx_scheduler_db_low_rate_ranges_pattern_cbs;

typedef struct {
    dnx_scheduler_db_low_rate_ranges_range_start_set_cb set;
    dnx_scheduler_db_low_rate_ranges_range_start_get_cb get;
} dnx_scheduler_db_low_rate_ranges_range_start_cbs;

typedef struct {
    dnx_scheduler_db_low_rate_ranges_range_end_set_cb set;
    dnx_scheduler_db_low_rate_ranges_range_end_get_cb get;
} dnx_scheduler_db_low_rate_ranges_range_end_cbs;

typedef struct {
    dnx_scheduler_db_low_rate_ranges_is_enabled_set_cb set;
    dnx_scheduler_db_low_rate_ranges_is_enabled_get_cb get;
} dnx_scheduler_db_low_rate_ranges_is_enabled_cbs;

typedef struct {
    dnx_scheduler_db_low_rate_ranges_alloc_cb alloc;
    dnx_scheduler_db_low_rate_ranges_pattern_cbs pattern;
    dnx_scheduler_db_low_rate_ranges_range_start_cbs range_start;
    dnx_scheduler_db_low_rate_ranges_range_end_cbs range_end;
    dnx_scheduler_db_low_rate_ranges_is_enabled_cbs is_enabled;
} dnx_scheduler_db_low_rate_ranges_cbs;

typedef struct {
    dnx_scheduler_db_is_init_cb is_init;
    dnx_scheduler_db_init_cb init;
    dnx_scheduler_db_port_shaper_cbs port_shaper;
    dnx_scheduler_db_hr_cbs hr;
    dnx_scheduler_db_interface_cbs interface;
    dnx_scheduler_db_fmq_cbs fmq;
    dnx_scheduler_db_virtual_flows_cbs virtual_flows;
    dnx_scheduler_db_general_cbs general;
    dnx_scheduler_db_low_rate_ranges_cbs low_rate_ranges;
} dnx_scheduler_db_cbs;






const char *
sch_virtual_flows_sw_state_type_e_get_name(sch_virtual_flows_sw_state_type_e value);


extern dnx_scheduler_db_cbs dnx_scheduler_db;

#endif 
