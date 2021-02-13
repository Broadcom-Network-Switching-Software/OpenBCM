/** \file algo/swstate/auto_generated/types/cosq_latency_types.h
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

#ifndef __COSQ_LATENCY_TYPES_H__
#define __COSQ_LATENCY_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_internal.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/cosq/latency/cosq_latency.h>
/*
 * STRUCTs
 */

/**
 * strucuture holds the queues quartet profile configuration. use also for template manager!!!
 */
typedef struct {
    /**
     * mapping to latency profile per queue as follow: lattency_profile = {latency_map(3),VoQ[1:0]}
     */
    int latency_map;
    /**
     * currently not in used
     */
    int tc_map;
} dnx_cosq_latency_queue_template_t;

/**
 * strucuture holds given exponent and coefficient used to manipulate the CQ probability
 */
typedef struct {
    /**
     * formula F(CQ) = a*(CQ)^b
     */
    uint32 exponent_b;
    /**
     * formula F(CQ) = a*(CQ)^b
     */
    uint32 coefficient_a;
} dnx_cosq_latency_ecn_probability_convert_t;

/**
 * cosq latency database per unit
 */
typedef struct {
    /**
     * holds ingress latency allocation manager
     */
    dnx_algo_res_t ingress_latency_res;
    /**
     * holds end to end latency allocation manager
     */
    dnx_algo_res_t end_to_end_latency_res;
    /**
     * holds end to end AQM latency allocation manager
     */
    dnx_algo_res_t end_to_end_aqm_latency_res;
    /**
     * holds queues to latency profiles mapping templates
     */
    dnx_algo_template_t queue_template;
    /**
     * Fof end to end AQM latency profile - holds given exponent and coefficient used to manipulate the CQ probability
     */
    dnx_cosq_latency_ecn_probability_convert_t probability_convert;
    /**
     * holds AQM flow id allocation manager
     */
    dnx_algo_res_t aqm_flow_id_res;
} dnx_cosq_latency_db_t;


#endif /* __COSQ_LATENCY_TYPES_H__ */
