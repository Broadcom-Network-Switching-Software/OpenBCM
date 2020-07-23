/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF FABRIC H
 */
 
#ifndef _SOC_DNXF_FABRIC_H_
#define _SOC_DNXF_FABRIC_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <bcm/types.h>
#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <bcm/fabric.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

typedef enum soc_dnxf_fabric_link_device_mode_e {
    soc_dnxf_fabric_link_device_mode_fe2, 
    soc_dnxf_fabric_link_device_mode_multi_stage_fe1,
    soc_dnxf_fabric_link_device_mode_multi_stage_fe3,
    soc_dnxf_fabric_link_device_mode_repeater
} soc_dnxf_fabric_link_device_mode_t;


typedef struct soc_dnxf_fabric_link_remote_pipe_mapping_s {
    uint32 num_of_remote_pipes;    

    uint32 remote_pipe_mapping[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES]; 
} soc_dnxf_fabric_link_remote_pipe_mapping_t;

#endif 
