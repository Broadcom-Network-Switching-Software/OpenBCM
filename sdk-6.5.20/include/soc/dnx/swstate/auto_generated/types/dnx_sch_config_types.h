
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCH_CONFIG_TYPES_H__
#define __DNX_SCH_CONFIG_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/types.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>



typedef enum {
    
    DNX_SCH_IPF_CONFIG_MODE_INVERSE = 0,
    
    DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL,
    
    DNX_SCH_IPF_CONFIG_MODE_NONE
} dnx_sch_ipf_config_mode_e;



typedef struct {
    uint8 num_cos;
    uint8 connection_valid;
    uint8 src_modid;
} dnx_sch_connector_swstate_info_t;

typedef struct {
    int weight;
    int mode;
} dnx_sch_credit_src_info_t;

typedef struct {
    uint32 child_count;
} dnx_sch_se_swstate_info_t;

typedef struct {
    dnx_sch_credit_src_info_t credit_src;
} dnx_sch_flow_swstate_info_t;


typedef struct {
    int* hr_group_bw[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int groups_bw[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][3];
    dnx_sch_ipf_config_mode_e ipf_config_mode;
    dnx_sch_flow_swstate_info_t* flow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    dnx_sch_connector_swstate_info_t* connector[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    dnx_sch_se_swstate_info_t* se[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} dnx_sch_config_t;


#endif 
