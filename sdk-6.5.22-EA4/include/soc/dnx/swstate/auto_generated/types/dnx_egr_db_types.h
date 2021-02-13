/** \file dnx/swstate/auto_generated/types/dnx_egr_db_types.h
 *
 * sw state types (structs/enums/typedefs)
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EGR_DB_TYPES_H__
#define __DNX_EGR_DB_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
/*
 * STRUCTs
 */

/**
 * info about OTM port
 */
typedef struct {
    /**
     * ps allocation bitmap
     */
    uint32 allocation_bmap[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
    /**
     * priority mode 1/2/4/8
     */
    uint32 prio_mode[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
    /**
     * interface index 0-63 for channelized. -1 for non channelized
     */
    int if_idx[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS];
} dnx_ps_db_t;

/**
 * info about EGR module
 */
typedef struct {
    /**
     * ps information
     */
    dnx_ps_db_t ps;
    /**
     * Boolean. egress interface occupation
     */
    int interface_occ[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES];
    /**
     * Total number of credits in all egress interfaces per core
     */
    int total_egr_if_credits[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
     * Total number of ESB queues allocated per core
     */
    int total_esb_queues_allocated[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} dnx_egr_db_t;


#endif /* __DNX_EGR_DB_TYPES_H__ */
