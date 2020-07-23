
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_COSQ_INGRESS_TYPES_H__
#define __DNX_COSQ_INGRESS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>


typedef struct {
    
    bcm_pbmp_t nif;
    
    SHR_BITDCL mirror[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][((DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES)/SHR_BITWID)+1];
} dnx_cosq_ingress_fixed_priority_t;


typedef struct {
    
    dnx_cosq_ingress_fixed_priority_t fixed_priority;
} dnx_cosq_ingress_db_t;


#endif 
