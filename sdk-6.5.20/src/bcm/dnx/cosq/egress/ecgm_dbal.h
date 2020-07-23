/** \file src/bcm/dnx/cosq/egress/ecgm_dbal.h
* 
*
* internal API for Egress Congestion dbal functions
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _EGRESS_CONGESTION_DBAL_H_INCLUDED_
/** { */
#define _EGRESS_CONGESTION_DBAL_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/*
* Typedefs
* These enums & types are used in DBAL tables get/set functions.
* Other types used are found in bcm_int/dnx/cosq/ecgm.h
* \see files: src/bcm/dnx/cosq/ecgm_dbal.c
*              tools/autocoder/dbal/dnx/field_types/field_types_definition.xml
*              tools/autocoder/dbal/dnx/tables/tables_hard_logic/TM/hl_tm_ecgm_thresholds.xml
*              tools/autocoder/dbal/dnx/tables/tables_hard_logic/TM/hl_tm_ecgm_mapping.xml
*/

typedef enum
{
    ECGM_TC_SP_THRESHOLD_MODE_STRICT_PRIORITY = 0,
    ECGM_TC_SP_THRESHOLD_MODE_DISCRETE_PARTITIONING = 1
} ecgm_tc_sp_threshold_mode_e;

/** used for DBAL_TABLE_ECGM_MAP_SYS_TC_DP_TO_MC_COS_PARAMS */
typedef struct
{
    uint32 mc_tc;               /* mc tc mapped to system tc */
    uint32 se_dp;               /* shared resources eligibilty according to system dp */
    uint32 mc_sp;               /* DB service pool 0-1 */
} ecgm_cos_params_t;

/** used for DBAL_TABLE_ECGM_CORE_TC_MULTICAST_THRESHOLDS */
typedef struct
{
    uint32 pd;
    uint32 db;
} ecgm_fc_thresholds_t;

/** used for    DBAL_TABLE_ECGM_CORE_GLOBAL_THRESHOLDS
*               DBAL_TABLE_ECGM_CORE_UNICAST_THRESHOLDS
*               DBAL_TABLE_ECGM_CORE_MULTICAST_THRESHOLDS
*/
typedef struct
{
    ecgm_fc_thresholds_t flow_control;
    uint32 pd_drop;
    uint32 db_drop;
} ecgm_core_thresholds_t;

/** used for DBAL_TABLE_ECGM_CORE_MULTICAST_SP_THRESHOLDS */
typedef struct
{
    ecgm_core_thresholds_t sp;
    uint32 reserved_pd;
    uint32 reserved_db;
} ecgm_mc_sp_thresholds_t;

/** used for DBAL_TABLE_ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS */
typedef struct
{
    dnx_ecgm_fadt_threshold_t db_drop;
    uint32 pd_drop;
    uint32 db_drop_alpha_1;
    uint32 reserved_db;
} ecgm_mc_sp_tc_thresholds_t;

typedef struct
{
    uint32 fqp_high_priority;
    uint32 stop_mc_low;
    uint32 stop_mc_high;
    uint32 stop_all;
} ecgm_delete_fifo_thresholds_t;

/** Delete FIFO modes:
* there are 2 delete FIFOs per core,
* first mode sets them so that one delete fifo
* is dedicaed for TDM traffic only,
* and one delete fifo for all other traffic.
* second mode is setting one delete fifo
* for both TDM and short packets (<=512B),
* and one delete fifo for long packets.
*/

typedef enum
{
    ECGM_TDM_AND_NON_TDM = 0,
    ECGM_SHORT_AND_LONG_PACKETS = 1
} ecgm_delete_fifo_mode_e;

/*
* Internal dbal functions
*/

/**
* \brief -  Set service pool ID for a multicast queue
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id
* \param [in] queue_offset - queue offset (traffic class) 0-7
* \param [in] mc_pd_sp - service pool id
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_to_sp_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 mc_pd_sp);

/**
* \brief -  Get service pool ID for a multicast queue
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id
* \param [in] queue_offset - queue offset (traffic class) 0-7
* \param [out] mc_pd_sp - service pool id
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_to_sp_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 *mc_pd_sp);

/**
* \brief - Set enable bit for multicast queue to service pool mapping.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] map_enable - configure the mapping as enabled(1) / disabled(0).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_to_sp_enable_set(
    uint32 unit,
    uint32 core_id,
    uint32 map_enable);

/**
* \brief - Get enable bit for multicast queue to service pool mapping
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] map_enable - get the mapping status: enabled(1) / disabled(0).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_to_sp_enable_get(
    uint32 unit,
    uint32 core_id,
    uint32 *map_enable);

/**
* \brief -  Set eligibility to use shared resources (data buffers) per service pool and system DP.
*           hw table: ECGM_MAP_MC_SE
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] system_dp - system drop precedence 0-3
* \param [in] service_pool - service pool id 0/1
* \param [in] shared_resource_eligibility - eligibility to use data buffers shared resources.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_mc_se_set(
    uint32 unit,
    uint32 core_id,
    uint32 system_dp,
    uint32 service_pool,
    uint32 shared_resource_eligibility);

/**
* \brief -  Get eligibility to use shared resources (data buffers) per service pool and system DP.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] system_dp - system drop precedence 0-3
* \param [in] service_pool - service pool id 0/1
* \param [out] shared_resource_eligibility - eligibility to use data buffers shared resources indication.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_mc_se_get(
    uint32 unit,
    uint32 core_id,
    uint32 system_dp,
    uint32 service_pool,
    uint32 *shared_resource_eligibility);

/**
* \brief -  Set a data buffers service pool id to a multicast TC.
*
* \param [in] unit - chip unit id
* \param [in] mc_tc - CGM multicast traffic class 0-7
* \param [in] db_service_pool - service pool id 0/1
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_mc_tc_to_db_sp_set(
    uint32 unit,
    uint32 mc_tc,
    uint32 db_service_pool);

/**
* \brief -  Get a data buffers service pool id for a multicast TC.
*
* \param [in] unit - chip unit id
* \param [in] mc_tc - CGM multicast traffic class 0-7
* \param [out] db_service_pool - service pool id 0/1
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_mc_tc_to_db_sp_get(
    uint32 unit,
    uint32 mc_tc,
    uint32 *db_service_pool);

/**
* \brief -  Set an interface profile id to an EGQ interface id
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_interface - egq interface id 0-63
* \param [in] interface_profile - interface profile id 0-7
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_egq_if_to_interface_profile_map_set(
    uint32 unit,
    uint32 core_id,
    uint32 egq_interface,
    uint32 interface_profile);

/**
* \brief -  Get the interface profile id of an EGQ interface id
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] egq_interface - egq interface id 0-63
* \param [out] interface_profile - interface profile id 0-7
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_egq_if_to_interface_profile_map_get(
    uint32 unit,
    uint32 core_id,
    uint32 egq_interface,
    uint32 *interface_profile);

/**
* \brief - Set a disable indication bit for a queue pair number.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] queue_pair - queue pair id 0-511
* \param [in] is_queue_disabled - the queue-pair is disabled when the bit is on
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_queue_disable_set(
    uint32 unit,
    uint32 core_id,
    uint32 queue_pair,
    uint32 is_queue_disabled);

/**
* \brief - Set a packet descriptors drop threshold for an interface profile and priority.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] priority - HIGH / LOW
* \param [in] interface_profile - interface profile id 0-7
* \param [in] pd_drop - packet descriptors drop threshold
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_multicast_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    uint32 pd_drop);

/**
* \brief - Get the packet descriptors drop threshold for an interface profile and priority.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] priority - HIGH / LOW
* \param [in] interface_profile - interface profile id 0-7
* \param [out] pd_drop - packet descriptors drop threshold
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_multicast_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    uint32 *pd_drop);

/**
* \brief - Set a port profile id to an OTM port.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tm_port - otm port number 0-255
* \param [in] base_q_pair - base queue pair number 0-511
* \param [in] port_profile - port profile id 0-15
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_tm_port_to_port_profile_set(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 base_q_pair,
    uint32 port_profile);

/**
* \brief - Get the port profile id for an OTM port.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tm_port - otm port number 0-255
* \param [out] port_profile - port profile id 0-15
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_tm_port_to_port_profile_get(
    uint32 unit,
    uint32 core_id,
    uint32 tm_port,
    uint32 *port_profile);

/**
* \brief - Set a reserved packet descriptors drop threshold to a multicast queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset (traffic class) 0-7
* \param [in] reserved_pd_drop - reserved pd drop threshold
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_tc_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 reserved_pd_drop);

/**
* \brief - Get the reserved packet descriptors drop threshold for a multicast queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset (traffic class) 0-7
* \param [out] reserved_pd_drop - reserved pd drop threshold
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_tc_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 *reserved_pd_drop);

/**
* \brief - Set multicast traffic class, drop precedence eligibility and priority to a system traffic class and drop precedence.
*           setting all the cos_params fields.
*           NOTE: for changing one parameter only - use the get function first to get the current info and modify the specific field.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sys_tc - system traffic class 0-7
* \param [in] sys_dp - system drop precedence 0-3
* \param [in] cos_params - the entry is set with all of ecgm_cos_params_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_set(
    uint32 unit,
    uint32 core_id,
    uint32 sys_tc,
    uint32 sys_dp,
    const ecgm_cos_params_t * cos_params);

/**
* \brief -  Get multicast traffic class, drop precedence eligibility and priority to a system traffic class and drop precedence.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sys_tc - system traffic class 0-7
* \param [in] sys_dp - system drop precedence 0-3
* \param [out] cos_params - ecgm_cos_params_t struct is filled with all the current entry information (mc tc, dp eligibility & priority).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_sys_tc_dp_to_mc_cos_params_get(
    uint32 unit,
    uint32 core_id,
    uint32 sys_tc,
    uint32 sys_dp,
    ecgm_cos_params_t * cos_params);

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds.
*           setting all the thresholds fields.
*           NOTE:   to change one parameter only - use the get function first to
*                   get the current info and modify the specific field.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [in] set_value - value to set the field to.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_global_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value);

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_global_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value);

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds for unicast traffic.
*           setting all the thresholds fields.
*           NOTE:   to change one parameter only - use the get function first to
*                   get the current info and modify the specific field.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [in] set_value - value to set the field to.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_unicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value);

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds for unicast traffic.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_unicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value);

/**
* \brief -  Set the core's packet descriptors and data buffers drop and flow control thresholds for multicast traffic.
*           setting all the thresholds fields.
*           NOTE:   to change one parameter only - use the get function first to
*                   get the current info and modify the specific field.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [in] set_value - value to set the field to.*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_multicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 set_value);

/**
* \brief -  Get the core's packet descriptors and data buffers drop and flow control thresholds for multicast traffic.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_multicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    dbal_fields_e field_name,
    uint32 *value);

/**
* \brief -  Set resources thresholds for a service pool:
*           drop, flow control and reserved thresholds
*           for packet descriptors and data buffers.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] service_pool_id - service pool id 0/1
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
*                           DBAL_FIELD_RESERVED_PD
*                           DBAL_FIELD_RESERVED_DB
* \param [in] value - parameter to set the field value to.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_multicast_sp_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    uint32 value);

/**
* \brief -  Get resources thresholds for a service pool:
*           drop, flow control and reserved thresholds
*           for packet descriptors and data buffers.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] service_pool_id - service pool id 0/1
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_DROP
*                           DBAL_FIELD_DB_FLOW_CONTROL
*                           DBAL_FIELD_RESERVED_PD
*                           DBAL_FIELD_RESERVED_DB
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_dbal_core_multicast_sp_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    uint32 *value);

/**
* \brief -  Set data buffers and packet descriptors flow control thresholds for a multicast traffic class.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tc - traffic class 0-7
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [in] value - parameter to set the field value to.
* 
* \return
*   shr_error_e
 */
shr_error_e dnx_ecgm_dbal_core_tc_multicast_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    dbal_fields_e field_name,
    uint32 value);

/**
* \brief -  Get data buffers and packet descriptors flow control thresholds for a multicast traffic class.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tc - traffic class 0-7
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_FLOW_CONTROL
*                           DBAL_FIELD_DB_FLOW_CONTROL
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
 */
shr_error_e dnx_ecgm_dbal_core_tc_multicast_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    dbal_fields_e field_name,
    uint32 *value);

/* ECGM_CORE_TC_MULTICAST_SP_THRESHOLDS */

/**
* \brief -  Set multicast service pool resource thresholds for a traffic class.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tc - traffic class 0-7
* \param [in] service_pool_id - service pool id 0/1
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_DB_DROP_MAX
*                           DBAL_FIELD_DB_DROP_MIN
*                           DBAL_FIELD_DB_DROP_ALPHA_0
*                           DBAL_FIELD_DB_DROP_ALPHA_1
*                           DBAL_FIELD_RESERVED_DB
* \param [in] value - parameter to set the field value to.
*
* \return
*   shr_error_e
*       Invalid parameter - in case the value given is not within the valid range.
*
* \remarks
*   valid values for alphas: -7 <= alpha <= 7
*   valid values for PD / DB: 0 <= threshold <= device resource (PD/DB) max
*/
shr_error_e dnx_ecgm_dbal_core_tc_multicast_sp_threshold_set(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    int value);

/**
* \brief -  Get multicast service pool resource thresholds for a traffic class.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] tc - traffic class 0-7
* \param [in] service_pool_id - service pool 0/1
* \param [in] field_name - name of field to set. might be only one of the following:
*                           DBAL_FIELD_PD_DROP
*                           DBAL_FIELD_DB_DROP_MAX
*                           DBAL_FIELD_DB_DROP_MIN
*                           DBAL_FIELD_DB_DROP_ALPHA_0
*                           DBAL_FIELD_DB_DROP_ALPHA_1
*                           DBAL_FIELD_RESERVED_DB
* \param [out] value - parameter to get the field value into.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_dbal_core_tc_multicast_sp_threshold_get(
    uint32 unit,
    uint32 core_id,
    uint32 tc,
    uint32 service_pool_id,
    dbal_fields_e field_name,
    int *value);

/**
* \brief -  Set data buffers and packet descriptors flow control thresholds
*           for unicast interface profile and priority.
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] priority - priority HIGH/LOW
* \param [in] interface_profile - interface profile id 0-7
* \param [in] thresholds - the entry is set with all of dnx_ecgm_uc_fc_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_unicast_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    const dnx_ecgm_uc_fc_thresholds_t * thresholds);

/**
* \brief -  Get data buffers and packet descriptors flow control thresholds
*           for unicast interface profile and priority.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] priority - priority HIGH/LOW
* \param [in] interface_profile - interface profile id 0-7
* \param [out] thresholds - dnx_ecgm_uc_fc_thresholds_t struct is filled with all the entry information
*                           (PD & DB fadt thresholds)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_unicast_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dbal_enum_value_field_ecgm_priority_e priority,
    uint32 interface_profile,
    dnx_ecgm_uc_fc_thresholds_t * thresholds);

/**
* \brief -  Set data buffers and packet descriptors thresholds for port profile.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] thresholds - the entry is set with all of dnx_ecgm_uc_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_unicast_port_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    const dnx_ecgm_uc_thresholds_t * thresholds);

/**
* \brief -  Get data buffers and packet descriptors thresholds for port profile.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [out] thresholds - dnx_ecgm_uc_thresholds_t struct is filled with all the entry information
*                           (PD and DB fadt thresholds)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_unicast_port_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    dnx_ecgm_uc_thresholds_t * thresholds);

/**
* \brief -  Set data buffers and packet descriptors thresholds for a unicast queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset 0-7
* \param [in] thresholds - the entry is set with all of dnx_ecgm_uc_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_unicast_queue_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    const dnx_ecgm_uc_thresholds_t * thresholds);

/**
* \brief -  Get data buffers and packet descriptors thresholds for a unicast queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset 0-7
* \param [out] thresholds - dnx_ecgm_uc_thresholds_t struct is filled with all the entry information
*                           (PD and DB fadt thresholds)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_unicast_queue_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    dnx_ecgm_uc_thresholds_t * thresholds);

/**
* \brief -  Set data buffers and packet descriptors drop thresholds for a multicast port.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] thresholds - the entry is set with all of dnx_ecgm_mc_drop_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_port_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    const dnx_ecgm_mc_drop_thresholds_t * thresholds);

/**
* \brief -  Get data buffers and packet descriptors drop thresholds for a multicast port.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [out] thresholds - dnx_ecgm_mc_drop_thresholds_t struct is filled with all the entry information
*                           (PD fadt thresholds and DB drop threshold)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_port_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    dnx_ecgm_mc_drop_thresholds_t * thresholds);

/**
* \brief -  Set data buffers and packet descriptors drop thresholds for a multicast queue and dp.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset 0-7
* \param [in] dp - drop precedence 0-3
* \param [in] thresholds - the entry is set with all of dnx_ecgm_mc_drop_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_tc_dp_thresholds_set(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 dp,
    const dnx_ecgm_mc_drop_thresholds_t * thresholds);

/**
* \brief -  Get data buffers and packet descriptors drop thresholds for a multicast queue and dp.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] port_profile - port profile id 0-15
* \param [in] queue_offset - queue offset 0-7
* \param [in] dp - drop precedence 0-3
* \param [out] thresholds - dnx_ecgm_mc_drop_thresholds_t struct is filled with all the entry information
*                           (PD fadt thresholds and DB drop threshold)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_multicast_queue_tc_dp_thresholds_get(
    uint32 unit,
    uint32 core_id,
    uint32 port_profile,
    uint32 queue_offset,
    uint32 dp,
    dnx_ecgm_mc_drop_thresholds_t * thresholds);

/**
* \brief -  Set delete FIFO mode.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] mode - delete fifo mode of operation configuration (see enum definition above).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_delete_fifo_mode_set(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_mode_e mode);

/**
* \brief -  Get delete FIFO mode.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] mode - delete fifo mode of operation configuration (see enum definition above).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_delete_fifo_mode_get(
    uint32 unit,
    uint32 core_id,
    ecgm_delete_fifo_mode_e * mode);

/**
* \brief -  Set delete FIFO thresholds.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] type - type of fifo to be configured (long packets / lcd fifo / short packets)
* \param [in] thresholds - the entry is set with all of ecgm_delete_fifo_thresholds_t struct fields.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_delete_fifo_thresholds_set(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    const ecgm_delete_fifo_thresholds_t * thresholds);

/**
* \brief -  Get delete FIFO thresholds.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] type - type of fifo to get thresholds to (long packets / lcd fifo / short packets)
* \param [out] thresholds - ecgm_delete_fifo_thresholds_t struct is filled with all the fifo type threshold information
*                           (fqp_high_quality, stop_mc_low, stop_mc_high, stop_all)
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_delete_fifo_thresholds_get(
    uint32 unit,
    uint32 core_id,
    dnx_ecgm_delete_fifo_type_e type,
    ecgm_delete_fifo_thresholds_t * thresholds);

/**
* \brief -  Poll the PD count value for a certain queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] time_out - time out value, maximal time for polling
* \param [in] min_polls - minimum polls
* \param [in] queue_pair - queue pair number
* \param [in] is_multicast - is multicast queue (must be 0/1)
*
* \return
*   shr_error_e
*   returns error if the pd value does not get to zero, within time
*   indicated by time_out.
*   otherwise returns BCM_E_NONE.
*
* \remarks
*   function doesn't validate values.
*   valid values (as defined in brief) must be used.
*/
shr_error_e dnx_ecgm_dbal_queue_pd_count_zero_poll(
    uint32 unit,
    uint32 core_id,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 queue_pair,
    int is_multicast);

/**
* \brief - translate an int value to the corresponding uint32 value
*          according to HW alpha bit size.
*
* \param [in] unit - chip unit id
* \param [in] alpha_int - int value to translate
* \param [out] alpha_uint32 - uint32 value
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_dbal_alpha_int_to_hw_get(
    int unit,
    int alpha_int,
    uint32 *alpha_uint32);

/**
* \brief - translate a uint32 value to the corresponding int value
*          according to HW alpha bit size.
*
* \param [in] unit - chip unit id
* \param [in] alpha_uint32 - uint32 value to translate
* \param [out] alpha_int - int value
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_dbal_alpha_hw_to_int_get(
    int unit,
    uint32 alpha_uint32,
    int *alpha_int);

/**
* \brief - returns max value for alpha. 
*
* \param [in] unit - chip unit id
* \param [out] max_alpha - max alpha value as uint32
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_dbal_alpha_max_val_get(
    int unit,
    uint32 *max_alpha);

/**
* \brief -  Set TC SP threshold mode for both PD and DB.
*           The shared pool consumption is controlled by setting a limit per TC.
*           There are two options to manage these limits:
*                0 - for Strict priority managing
*                1 - for Discrete partitioning managing
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] pd_mode - configured mode for PD threshold.
* \param [in] db_mode - configured mode for DB threshold.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_tc_sp_threshold_mode_set(
    uint32 unit,
    uint32 core_id,
    ecgm_tc_sp_threshold_mode_e pd_mode,
    ecgm_tc_sp_threshold_mode_e db_mode);

/**
* \brief -  Get TC SP threshold mode for both PD and DB.
*           The shared pool consumption is controlled by setting a limit per TC.
*           There are two options to manage these limits:
*                0 - for Strict priority managing
*                1 - for Discrete partitioning managing
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] pd_mode - configured mode for PD threshold.
* \param [out] db_mode - configured mode for DB threshold.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_tc_sp_threshold_mode_get(
    uint32 unit,
    uint32 core_id,
    ecgm_tc_sp_threshold_mode_e * pd_mode,
    ecgm_tc_sp_threshold_mode_e * db_mode);

#endif  /** _EGRESS_CONGESTION_DBAL_H_INCLUDED_ */
