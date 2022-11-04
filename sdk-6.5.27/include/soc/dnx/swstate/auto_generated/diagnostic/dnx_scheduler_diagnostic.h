
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_DIAGNOSTIC_H__
#define __DNX_SCHEDULER_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_scheduler_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_SCHEDULER_DB_INFO,
    DNX_SCHEDULER_DB_PORT_SHAPER_INFO,
    DNX_SCHEDULER_DB_PORT_SHAPER_RATE_INFO,
    DNX_SCHEDULER_DB_PORT_SHAPER_VALID_INFO,
    DNX_SCHEDULER_DB_HR_INFO,
    DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
    DNX_SCHEDULER_DB_INTERFACE_INFO,
    DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
    DNX_SCHEDULER_DB_FMQ_INFO,
    DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
    DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_VIRTUAL_FLOWS_INFO,
    DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_ENABLED_INFO,
    DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
    DNX_SCHEDULER_DB_GENERAL_INFO,
    DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
    DNX_SCHEDULER_DB_GENERAL_NON_DEFAULT_FLOW_ORDER_INFO,
    DNX_SCHEDULER_DB_GENERAL_IS_COMPOSITE_FQ_SUPPORTED_INFO,
    DNX_SCHEDULER_DB_LOW_RATE_RANGES_INFO,
    DNX_SCHEDULER_DB_LOW_RATE_RANGES_PATTERN_INFO,
    DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_START_INFO,
    DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_END_INFO,
    DNX_SCHEDULER_DB_LOW_RATE_RANGES_IS_ENABLED_INFO,
    DNX_SCHEDULER_DB_INFO_NOF_ENTRIES
} sw_state_dnx_scheduler_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_scheduler_db_info[SOC_MAX_NUM_DEVICES][DNX_SCHEDULER_DB_INFO_NOF_ENTRIES];

extern const char* dnx_scheduler_db_layout_str[DNX_SCHEDULER_DB_INFO_NOF_ENTRIES];
int dnx_scheduler_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_shaper_dump(
    int unit, int port_shaper_idx_0, int port_shaper_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_shaper_rate_dump(
    int unit, int port_shaper_idx_0, int port_shaper_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_shaper_valid_dump(
    int unit, int port_shaper_idx_0, int port_shaper_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_hr_dump(
    int unit, int hr_idx_0, int hr_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_hr_is_colored_dump(
    int unit, int hr_idx_0, int hr_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_interface_dump(
    int unit, int interface_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_interface_modified_dump(
    int unit, int interface_idx_0, int modified_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_fmq_dump(
    int unit, int fmq_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_fmq_reserved_hr_flow_id_dump(
    int unit, int fmq_idx_0, int reserved_hr_flow_id_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_fmq_nof_reserved_hr_dump(
    int unit, int fmq_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_fmq_fmq_flow_id_dump(
    int unit, int fmq_idx_0, int fmq_flow_id_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_virtual_flows_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_virtual_flows_is_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_virtual_flows_is_flow_virtual_dump(
    int unit, int is_flow_virtual_idx_0, int is_flow_virtual_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_reserved_erp_hr_flow_id_dump(
    int unit, int general_idx_0, int reserved_erp_hr_flow_id_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_nof_reserved_erp_hr_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_non_default_flow_order_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_is_composite_fq_supported_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_low_rate_ranges_dump(
    int unit, int low_rate_ranges_idx_0, int low_rate_ranges_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_low_rate_ranges_pattern_dump(
    int unit, int low_rate_ranges_idx_0, int low_rate_ranges_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_low_rate_ranges_range_start_dump(
    int unit, int low_rate_ranges_idx_0, int low_rate_ranges_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_low_rate_ranges_range_end_dump(
    int unit, int low_rate_ranges_idx_0, int low_rate_ranges_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_low_rate_ranges_is_enabled_dump(
    int unit, int low_rate_ranges_idx_0, int low_rate_ranges_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
