/** \file algo/swstate/auto_generated/types/dnx_ingress_reassembly_types.h
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

#ifndef __DNX_INGRESS_REASSEMBLY_TYPES_H__
#define __DNX_INGRESS_REASSEMBLY_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
/*
 * STRUCTs
 */

typedef struct {
    /**
     * reassembly context of RCY interface
     */
    uint32 reassembly_context[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_RCY_INTERFACES][DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_RCY_PRIORITIES_NOF];
} dnx_ingress_reassembly_rcy_t;

typedef struct {
    /**
     * reassembly context
     */
    uint32 reassembly_context;
    /**
     * how many channels use this reassembly context
     */
    uint32 ref_count;
} dnx_ingress_reassembly_info_t;

typedef struct {
    /**
     * reassembly info of Mirror ports
     */
    dnx_ingress_reassembly_info_t info[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES][DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_MIRROR_PRIORITIES_NOF];
} dnx_ingress_reassembly_mirror_t;

/**
 * DB for ingress reassembly
 */
typedef struct {
    /**
     * PORT_INGR_REASSEMBLY_CONTEXT
     */
    dnx_algo_res_t* alloc;
    /**
     * reassembly contexts allocated for RCY interface
     */
    dnx_ingress_reassembly_rcy_t rcy;
    /**
     * reassembly contexts allocated for Mirror ports
     */
    dnx_ingress_reassembly_mirror_t mirror;
    /**
     * saved reassembly context for disabled ports
     */
    uint32 disabled_context[SOC_MAX_NUM_PORTS];
} dnx_ingress_reassembly_db_t;


#endif /* __DNX_INGRESS_REASSEMBLY_TYPES_H__ */
