/** \file dnx/swstate/auto_generated/types/dnx_ecgm_types.h
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

#ifndef __DNX_ECGM_TYPES_H__
#define __DNX_ECGM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>
/*
 * STRUCTs
 */

/**
 * ECGM related sw state for interface
 */
typedef struct {
    /**
     * logical port
     */
    bcm_port_t port;
    /**
     * profile data
     */
    dnx_ecgm_interface_profile_info_t info;
    /**
     * profile id
     */
    int profile_id;
    /**
     * is data valid
     */
    int valid;
} dnx_ecgm_interface_data_t;

/**
 * ECGM related sw state for port
 */
typedef struct {
    /**
     * logical port
     */
    bcm_port_t port;
    /**
     * profile data
     */
    dnx_ecgm_port_profile_info_t info;
    /**
     * profile id
     */
    int profile_id;
    /**
     * is data valid
     */
    int valid;
} dnx_ecgm_port_data_t;

/**
 * ECGM related sw state
 */
typedef struct {
    /**
     * interface profile info
     */
    dnx_ecgm_interface_data_t interface_caching;
    /**
     * port profile info
     */
    dnx_ecgm_port_data_t port_caching;
} dnx_ecgm_sw_state_t;


#endif /* __DNX_ECGM_TYPES_H__ */
