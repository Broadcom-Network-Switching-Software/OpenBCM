/** \file dnx/swstate/auto_generated/types/dnx_scheduler_types.h
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

#ifndef __DNX_SCHEDULER_TYPES_H__
#define __DNX_SCHEDULER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
/*
 * STRUCTs
 */

/**
 * info about shaper parameters
 */
typedef struct {
    /**
     * rate value
     */
    int rate;
    /**
     * Boolean, is valid value
     */
    int valid;
} dnx_scheduler_db_shaper_t;

/**
 * info about HRs
 */
typedef struct {
    /**
     * is HR colored
     */
    int is_colored;
} dnx_scheduler_db_hr_info_t;

/**
 * info about SCH interfaces
 */
typedef struct {
    /**
     * is interface modified and required calendar recalculation
     */
    SHR_BITDCL* modified;
} dnx_scheduler_db_interface_info_t;

/**
 * info about FMQ
 */
typedef struct {
    /**
     * User reserved HR flow ids
     */
    int* reserved_hr_flow_id;
    int nof_reserved_hr;
    /**
     * Allocated FMQ flow IDs
     */
    int* fmq_flow_id;
} dnx_scheduler_db_fmq_info;

/**
 * general info
 */
typedef struct {
    /**
     * unused reserved ERP HR flow ids
     */
    int reserved_erp_hr_flow_id[8];
    int nof_reserved_erp_hr;
    /**
     * [Boolean] indicate if core has non-default flow quartet order configuration
     */
    uint8 non_default_flow_order;
    /**
     * [Boolean] indicate if composite FQ scheduling elements are supported per core
     */
    uint8 is_composite_fq_supported;
} dnx_scheduler_db_general_info;

/**
 * DB for use by Scheduler
 */
typedef struct {
    /**
     * Port shaper per base HR
     */
    dnx_scheduler_db_shaper_t** port_shaper;
    /**
     * Info per HR
     */
    dnx_scheduler_db_hr_info_t** hr;
    /**
     * Info per sch interface
     */
    dnx_scheduler_db_interface_info_t* interface;
    /**
     * Info per FMQ
     */
    dnx_scheduler_db_fmq_info* fmq;
    /**
     * General info
     */
    dnx_scheduler_db_general_info* general;
} dnx_scheduler_db_t;


#endif /* __DNX_SCHEDULER_TYPES_H__ */
