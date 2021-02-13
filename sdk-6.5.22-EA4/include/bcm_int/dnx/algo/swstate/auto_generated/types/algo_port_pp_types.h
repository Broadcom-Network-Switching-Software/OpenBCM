/** \file algo/swstate/auto_generated/types/algo_port_pp_types.h
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

#ifndef __ALGO_PORT_PP_TYPES_H__
#define __ALGO_PORT_PP_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/port_tpid_types.h>
/*
 * STRUCTs
 */

/**
 * General data about algo port
 */
typedef struct {
    /**
     * template mngr for traps destinations
     */
    dnx_algo_template_t* mngr;
    /**
     * trap id - if exist per trap index (otherwise -1)
     */
    uint32** trap_id;
} dnx_algo_port_db_force_forward_t;

/**
 * Algo esem resources sw state
 */
typedef struct {
    /**
     * Template for ESEM access command
     */
    dnx_algo_template_t access_cmd;
} algo_port_esem_t;

/**
 * recycle app info
 */
typedef struct {
    /**
     * recycle app type
     */
    uint32 type;
} dnx_algo_recycle_app_info_t;

/**
 * Algo port PP resources sw state
 */
typedef struct {
    /**
     * Egress SEM default result profile IDs resource
     */
    dnx_algo_res_t esem_default_result_profile;
    /**
     * Ingress LLVP Profile Template
     */
    dnx_algo_template_t port_pp_tpid_profile_ingress;
    /**
     * Egress LLVP Profile Template
     */
    dnx_algo_template_t port_pp_tpid_profile_egress;
    /**
     * Egress Acceptable Frame Type Profile Template
     */
    dnx_algo_template_t egress_acceptable_frame_type_profile;
    /**
     * EGRESS PORT SIT PROFILE
     */
    dnx_algo_template_t egress_pp_port_sit_profile;
    /**
     * pp port lb profile enablers vector table
     */
    dnx_algo_template_t pp_port_lb_profile;
    /**
     * DB used by force forward mechanism
     */
    dnx_algo_port_db_force_forward_t force_forward;
    /**
     * ESEM command ID allocation manager with resource bitmap
     */
    algo_port_esem_t esem;
    /**
     * Recycle app port information
     */
    dnx_algo_recycle_app_info_t* recycle_app[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} algo_port_pp_sw_state_t;


#endif /* __ALGO_PORT_PP_TYPES_H__ */
