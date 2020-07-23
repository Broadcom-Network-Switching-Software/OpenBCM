/** \file src/bcm/dnx/cosq/ingress/ingress_congestion.h
 * 
 * 
 * Internal API of ingress congestion inside cosq module
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _INGRESS_CONGESTION_H_INCLUDED_
/** { */
#define _INGRESS_CONGESTION_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

/*
 * Typedefs
 */

typedef enum
{
    DNX_INGRESS_CONGESTION_GLOBAL_SRAM_RESOURCE_INVALID = -1,
    DNX_INGRESS_CONGESTION_GLOBAL_SRAM_PACKET_DESCRIPTORS_BUFFERS = 0,
    DNX_INGRESS_CONGESTION_GLOBAL_SRAM_BUFFERS
} dnx_ingress_congestion_global_sram_resource_t;

typedef enum
{
    /** enum values match HW values */
    DNX_INGRESS_CONGESTION_CMD_FIFO_MCH = 0,
    DNX_INGRESS_CONGESTION_CMD_FIFO_MCL = 1,
    DNX_INGRESS_CONGESTION_CMD_FIFO_UC = 2
} dnx_ingress_congestion_cmd_fifo_type_t;

/*
 * Specifies what keys should be configured under the rate class.
 * Possible keys:
 * - dp - for VOQ and VSQ
 * - pool - for source port VSQ only
 */
typedef struct
{
    int nof_dp;
    int dp[DNX_COSQ_NOF_DP];
    int nof_pool;
    int pool[DNX_DATA_MAX_INGR_CONGESTION_VSQ_POOL_NOF];
} dnx_ingress_congestion_rate_class_threshold_key_t;

/*
 * Internal API Functions 
 */

/**
 * \brief -  Extract list of keys from discard flags
 */
int dnx_ingress_congestion_discard_key_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_gport_discard_t * discard,
    dnx_ingress_congestion_rate_class_threshold_key_t * key_info);

/**
 * \brief -
 * verify that the threshold is below maximum of the resource
 */
int dnx_ingress_congestion_threshold_verify(
    int unit,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    char *threshold_name,
    uint32 threshold);

/*
 * brief - get resource type from flags.
 * used for color_size_set/get
 */
shr_error_e dnx_ingress_congestion_color_size_res_type_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e * res_type);

/**
 * \brief - Initialize dnx ingress_congestion module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_init(
    int unit);

/**
 * \brief - Deinitialize  dnx ingress_congestion module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_deinit(
    int unit);

shr_error_e dnx_ingress_congestion_port_remove(
    int unit,
    bcm_port_t port);

/**
 * \brief - Verify the queues can be configured to the rate class given by the user. the purpose is to check we have enough resources for the guaranteed.
 *
 * \param [in] unit -The unit number.
 * \param [in] config- user configuration from API. this function only uses config->numq and config->queue_attributes.rate_class.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_bundle_gport_config_rate_class_verify(
    int unit,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config);

/**
 *
 * \brief - Verify guaranteed queue values for the specified resource type (bytes, PDs, buffers).
 *
 * \param unit
 * \param core - device core id
 * \param res_type - resource type (bytes, SRAM PDs, SRAM buffers)
 * \param reservation_change - change in guaranteed size for the resource.
 *
 * \return int
 */
shr_error_e dnx_ingress_congestion_res_type_guaranteed_verify(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int reservation_change);

/**
 *
 * \brief - update the shared resources thresholds for reject status (set and clear),
 * according to the new guaranteed level.
 * Notes:
 * 1. guaranteed_change is in HW granularity resolution (from DNX data). caller must convert to this granularity..
 *    see DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET
 * 2. guaranteed_change can be < 0, in case guaranteed is reduced
 * 3. this function does not verify! it is expected that caller will call verify function
 *    to check there is enough resources to make the change.
 *    see dnx_ingress_congestion_res_type_guaranteed_verify.
 *
 * \param unit
 * \param core - device core id
 * \param res_type - resource type (bytes, SRAM PDs, SRAM buffers)
 * \param guaranteed_change - change in guaranteed size for the resource.
 *
 * \return int
 */
shr_error_e dnx_ingress_congestion_shared_res_reject_status_thr_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e res_type,
    int guaranteed_change);

/**
 * \brief - update the shared resource thresholds according to the new guaranteed.
 * this is called when adding new VOQs using API cosq_ingress_queue_bundle_gport_add.
 * the new queues are assigned to a rate class profile, which sets a certain guarantee to each new VOQ.
 * it means the shared resources should be decreased by that amount.
 *
 * \param [in] unit - The unit number.
 * \param [in] is_add - context : are we adding queues or removing queues.
 * \param [in] config - user configuration from bundle_gport_add API.
 *             this function only uses config->numq and config->queue_attributes.rate_class
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ingress_congestion_voq_guaranteed_set(
    int unit,
    int is_add,
    bcm_cosq_ingress_queue_bundle_gport_config_t * config);

/**
 * \brief - set DRAM recovery threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [in] threshold - threshold info. relevant fields:
 *              - flags => should be set to BCM_COSQ_THRESHOLD_QSIZE_RECOVERY
 *              - type => should be set to bcmCosqThresholdBytes.
 *              - value => threshold value to set.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_ingress_congestion_dram_recovery_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
 * \brief - Get DRAM recovery threshold
 *
 * \param [in] unit - The unit number.
 * \param [in] gport - rate class profile gport
 * \param [in] cosq - irrelevant here. set to 0
 * \param [in,out] threshold - threshold info.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *    dnx_ingress_congestion_dram_recovery_threshold_set
 */
shr_error_e dnx_ingress_congestion_dram_recovery_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/** 
 * \brief - Set WRED configuration
 */
int dnx_ingress_congestion_voq_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

/** 
 * \brief - Retrieve WRED configuration
 */
int dnx_ingress_congestion_voq_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

/*
 * \brief - init configuration for the programmable counters. (including VOQ, VSQ and common configuration)
 */
shr_error_e dnx_ingress_congestion_programmable_counter_init_all_ones_config_set(
    int unit);

/* ----------------------------------------------------------------------------------------------
 * VSQ functions
 * ----------------------------------------------------------------------------------------------
 */

/**
 * \brief - 
 * set VSQ WRED configuration
 */
int dnx_ingress_congestion_vsq_discard_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

/**
 * \brief - 
 * retrieve VSQ WRED configuration
 */
int dnx_ingress_congestion_vsq_discard_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_gport_discard_t * discard);

/**
 * \brief - 
 * set VSQ tail drop and guaranteed configuration
 */
int dnx_ingress_congestion_vsq_color_size_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - 
 * retrieve VSQ tail drop and guaranteed configuration
 */
int dnx_ingress_congestion_vsq_color_size_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size);

/**
 * \brief - 
 * retrieve information from VSQ GPORT:
 * VSQ type - A-F or Global,
 * and VSQ ID - for VSQ A-F
 */
int dnx_ingress_congestion_gport_vsq_get(
    int unit,
    bcm_gport_t vsq_gport,
    bcm_cos_queue_t cosq,
    int *core_id,
    dnx_ingress_congestion_vsq_group_e * vsq_type,
    int *vsq_id);

/**
 * \brief -
 * create VSQ gport according to vsq info
 */
int dnx_ingress_congestion_vsq_gport_set(
    int unit,
    bcm_cosq_vsq_info_t * vsq_info,
    bcm_gport_t * vsq_gport);

/** 
 * \brief - get TC->PG mapping for the provided source port
 */
int dnx_ingress_congestion_vsq_tc_pg_mapping_get(
    int unit,
    bcm_port_t src_port,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping);

/** 
 * \brief - configure new TC->PG mapping for the provided source port, including
 *  - update of template manager
 *  - HW update
 */
int dnx_ingress_congestion_vsq_pg_tc_profile_exchange_and_set(
    int unit,
    bcm_port_t src_port,
    dnx_ingress_congestion_vsq_tc_pg_mapping_t * tc_pg_mapping);

/**
 * \brief
 *  Get VSQ-E index and PG-Base corresponding to PP-port.
 *
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] src_port -
 *      logical port.
 *  \param [out] src_port_vsq_index -
 *      VSQ-E index.
 *  \param [out] pg_base -
 *      PG-Base index.
 */
int dnx_ingress_congestion_src_vsq_mapping_get(
    int unit,
    int src_port,
    int *src_port_vsq_index,
    int *pg_base);

/** 
 * \brief - set src port to vsq e/f
 */
int dnx_ingress_congestion_src_vsq_mapping_set(
    int unit,
    bcm_port_t src_port,
    int src_port_vsq_index,
    int pg_base,
    int numq);

/** 
 * \brief - update VSQ SW state with created vsq E and F parameters
 */
int dnx_ingress_congestion_port_based_vsq_sw_state_update(
    int unit,
    int core_id,
    bcm_port_t src_port,
    int src_port_vsq_index,
    int pg_base);

/**
 * \brief
 *  returns whether the mapping of src port to vsq-e is enabled
 *
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] src_port -
 *      logical port.
 *  \param [out] enable -
 *      Whether the mapping is enabled/disabled.
 */
int dnx_ingress_congestion_src_vsq_mapping_enable_get(
    int unit,
    int src_port,
    int *enable);

/**
 * \brief -
 * Destroy source VSQ gports
 */
int dnx_ingress_congestion_src_vsqs_gport_destroy(
    int unit,
    bcm_gport_t vsq_gport);

/**
 * \brief
 * Set resource allocation for source VSQs - VSQ-E and VSQ-F
 */
shr_error_e dnx_ingress_congestion_src_vsq_resource_allocation_set(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts);

/**
 * \brief
 * Get resource allocation for source VSQs - VSQ-E and VSQ-F
 */
shr_error_e dnx_ingress_congestion_src_vsq_resource_allocation_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts);

/**
 * \brief
 * Set resource allocation for global VSQ
 */
shr_error_e dnx_ingress_congestion_global_vsq_resource_allocation_set(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts);

/**
 * \brief
 * Get resource allocation for global VSQ
 */
shr_error_e dnx_ingress_congestion_global_vsq_resource_allocation_get(
    int unit,
    uint32 flags,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    bcm_cosq_allocation_entity_t * target,
    bcm_cosq_resource_amounts_t * amounts);

/**
 * \brief - Get next congested element from congestion fifo
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] flags - flags
 *   \param [in] key -  key attributes that represent the congestion element (type, core)
 *   \param [out] data - returned congested element info (gport id, type of resource)
 *
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 *
 * \see
 *   * None
 */
int dnx_cosq_inst_congestion_info_get(
    int unit,
    uint32 flags,
    bcm_cosq_inst_congestion_info_key_t * key,
    bcm_cosq_inst_congestion_info_data_t * data);

/* ----------------------------------------------------------------------------------------------
 * Other functions
 * ----------------------------------------------------------------------------------------------
 */
/*
 * brief - set override pp port with reassembly ctxt for all reassembly ctxts
 */
shr_error_e dnx_ingress_congestion_override_pp_port_with_reassembly_context_enable_set(
    int unit,
    int enable);

#endif /** _INGRESS_CONGESTION_H_INCLUDED_ */
