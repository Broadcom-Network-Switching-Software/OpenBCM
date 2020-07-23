
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IQS_DIAGNOSTIC_H__
#define __DNX_IQS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_iqs_types.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_IQS_DB_INFO,
    DNX_IQS_DB_CREDIT_REQUEST_PROFILE_INFO,
    DNX_IQS_DB_CREDIT_REQUEST_PROFILE_VALID_INFO,
    DNX_IQS_DB_CREDIT_REQUEST_PROFILE_THRESHOLDS_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_QUEUE_MIN_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_QUEUE_MAX_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_ENABLE_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_RETRANSMIT_TH_INFO,
    DNX_IQS_DB_CREDIT_WATCHDOG_DELETE_TH_INFO,
    DNX_IQS_DB_INFO_NOF_ENTRIES
} sw_state_dnx_iqs_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_iqs_db_info[SOC_MAX_NUM_DEVICES][DNX_IQS_DB_INFO_NOF_ENTRIES];

extern const char* dnx_iqs_db_layout_str[DNX_IQS_DB_INFO_NOF_ENTRIES];
int dnx_iqs_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_request_profile_dump(
    int unit, int credit_request_profile_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_request_profile_valid_dump(
    int unit, int credit_request_profile_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_request_profile_thresholds_dump(
    int unit, int credit_request_profile_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_queue_min_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_queue_max_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_enable_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_retransmit_th_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_iqs_db_credit_watchdog_delete_th_dump(
    int unit, int delete_th_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
