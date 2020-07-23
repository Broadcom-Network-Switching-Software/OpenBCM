
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_DIAGNOSTIC_H__
#define __DNX_SCHEDULER_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_scheduler_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_SCHEDULER_DB_INFO,
    DNX_SCHEDULER_DB_PORT_INFO,
    DNX_SCHEDULER_DB_PORT_RATE_INFO,
    DNX_SCHEDULER_DB_PORT_VALID_INFO,
    DNX_SCHEDULER_DB_HR_INFO,
    DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
    DNX_SCHEDULER_DB_INTERFACE_INFO,
    DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
    DNX_SCHEDULER_DB_FMQ_INFO,
    DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
    DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_GENERAL_INFO,
    DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
    DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
    DNX_SCHEDULER_DB_INFO_NOF_ENTRIES
} sw_state_dnx_scheduler_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_scheduler_db_info[SOC_MAX_NUM_DEVICES][DNX_SCHEDULER_DB_INFO_NOF_ENTRIES];

extern const char* dnx_scheduler_db_layout_str[DNX_SCHEDULER_DB_INFO_NOF_ENTRIES];
int dnx_scheduler_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_dump(
    int unit, int port_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_rate_dump(
    int unit, int port_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_port_valid_dump(
    int unit, int port_idx_0, dnx_sw_state_dump_filters_t filters);

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

int dnx_scheduler_db_general_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_reserved_erp_hr_flow_id_dump(
    int unit, int general_idx_0, int reserved_erp_hr_flow_id_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_scheduler_db_general_nof_reserved_erp_hr_dump(
    int unit, int general_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
