
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_COSQ_INGRESS_DIAGNOSTIC_H__
#define __DNX_COSQ_INGRESS_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_cosq_ingress_types.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_COSQ_INGRESS_DB_INFO,
    DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_INFO,
    DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_NIF_INFO,
    DNX_COSQ_INGRESS_DB_FIXED_PRIORITY_MIRROR_INFO,
    DNX_COSQ_INGRESS_DB_INFO_NOF_ENTRIES
} sw_state_dnx_cosq_ingress_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_cosq_ingress_db_info[SOC_MAX_NUM_DEVICES][DNX_COSQ_INGRESS_DB_INFO_NOF_ENTRIES];

extern const char* dnx_cosq_ingress_db_layout_str[DNX_COSQ_INGRESS_DB_INFO_NOF_ENTRIES];
int dnx_cosq_ingress_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_cosq_ingress_db_fixed_priority_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_cosq_ingress_db_fixed_priority_nif_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_cosq_ingress_db_fixed_priority_mirror_dump(
    int unit, int mirror_idx_0, int mirror_idx_1, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 
