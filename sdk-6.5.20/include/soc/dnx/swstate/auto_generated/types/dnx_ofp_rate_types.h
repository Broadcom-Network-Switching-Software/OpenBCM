
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_OFP_RATE_TYPES_H__
#define __DNX_OFP_RATE_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>



typedef struct {
    
    uint32 rate;
    
    uint32 valid;
} dnx_ofp_rate_db_tcg_shaping_t;


typedef struct {
    
    uint32 rate;
    
    uint32 valid;
} dnx_ofp_rate_db_qpair_shaping_t;


typedef struct {
    
    uint32 rate;
    
    uint32 burst;
    
    uint32 valid;
} dnx_ofp_rate_db_otm_shaping_t;


typedef struct {
    
    uint32 cal_instance;
    
    dnx_ofp_rate_db_otm_shaping_t shaping;
} dnx_ofp_rate_db_otm_t;


typedef struct {
    
    uint32 rate;
    
    uint32 modified;
    
    uint32 nof_calcal_instances;
} dnx_ofp_rate_db_cal_t;


typedef struct {
    
    dnx_ofp_rate_db_otm_t* otm[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    
    dnx_ofp_rate_db_qpair_shaping_t qpair[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS];
    
    dnx_ofp_rate_db_tcg_shaping_t tcg[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG];
    
    dnx_ofp_rate_db_cal_t otm_cal[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS];
    
    uint32 calcal_is_modified[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    
    uint32 calcal_len[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} dnx_ofp_rate_db_t;


#endif 
