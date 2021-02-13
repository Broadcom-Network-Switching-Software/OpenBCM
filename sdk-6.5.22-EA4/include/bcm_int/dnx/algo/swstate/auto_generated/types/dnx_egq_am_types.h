/** \file algo/swstate/auto_generated/types/dnx_egq_am_types.h
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

#ifndef __DNX_EGQ_AM_TYPES_H__
#define __DNX_EGQ_AM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_cbs.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
/*
 * STRUCTs
 */

/**
 * DB used to egq databases ported from legacy code
 */
typedef struct {
    /**
     * Egr TC DP mapping
     */
    dnx_algo_template_t* egr_tc_dp_mapping;
    /**
     * FQP profile manager
     */
    dnx_algo_template_t* fqp_profile;
} dnx_egq_am_db_t;


#endif /* __DNX_EGQ_AM_TYPES_H__ */
