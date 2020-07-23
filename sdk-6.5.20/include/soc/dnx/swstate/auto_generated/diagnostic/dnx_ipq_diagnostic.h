
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IPQ_DIAGNOSTIC_H__
#define __DNX_IPQ_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ipq_types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_IPQ_DB_INFO,
    DNX_IPQ_DB_BASE_QUEUES_INFO,
    DNX_IPQ_DB_BASE_QUEUES_NUM_COS_INFO,
    DNX_IPQ_DB_BASE_QUEUES_SYS_PORT_REF_COUNTER_INFO,
    DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
    DNX_IPQ_DB_INFO_NOF_ENTRIES
} sw_state_dnx_ipq_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_ipq_db_info[SOC_MAX_NUM_DEVICES][DNX_IPQ_DB_INFO_NOF_ENTRIES];

extern const char* dnx_ipq_db_layout_str[DNX_IPQ_DB_INFO_NOF_ENTRIES];
int dnx_ipq_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_ipq_db_base_queues_dump(
    int unit, int base_queues_idx_0, int base_queues_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ipq_db_base_queues_num_cos_dump(
    int unit, int base_queues_idx_0, int base_queues_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ipq_db_base_queues_sys_port_ref_counter_dump(
    int unit, int base_queues_idx_0, int base_queues_idx_1, dnx_sw_state_dump_filters_t filters);

int dnx_ipq_db_base_queue_is_asymm_dump(
    int unit, int base_queue_is_asymm_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
