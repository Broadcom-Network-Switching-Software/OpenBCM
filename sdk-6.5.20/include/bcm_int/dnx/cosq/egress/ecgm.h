/** \file include/bcm_int/dnx/cosq/egress/ecgm.h
* 
*
* Egress congestion (ECGM)
* internal API implementation
* functions and types.
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_COSQ_EGRESS_CONGESTION_H_INCLUDED_
/** { */
#define _DNX_COSQ_EGRESS_CONGESTION_H_INCLUDED_

#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <sal/types.h>
#include <bcm/cosq.h>   /* (legacy) for bcm_cosq_egress_multicast_config_t */
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

typedef enum
{
    ECGM_LONG_PACKETS_FIFO = 0,
    ECGM_LCD_FIFO = 1,
    ECGM_TDM_AND_SHORT_PACKETS = 2
} dnx_ecgm_delete_fifo_type_e;

/**
 * ECGM resources Typedefs
 *
 * {
 * note that these types are used in two APIs:
 * for ecgm templates - used as port profile and interface profile fields.
 * for several dbal accessing functions - used as table parameters.
 * any change to these types should consider these internal APIs.
 *
 * \see     algo_ecgm.h
 *          egress_congestion_dbal.h
 */

/* values for FADT (fair adaptive dynamic thresholds) scheme. */
typedef struct
{
    uint32 min;
    uint32 max;
    int alpha;
} dnx_ecgm_fadt_threshold_t;

/* unicast flow control thresholds for packet descriptors and data buffers */
typedef struct
{
    dnx_ecgm_fadt_threshold_t db_fc;
    dnx_ecgm_fadt_threshold_t pd_fc;
} dnx_ecgm_uc_fc_thresholds_t;

/* unicast flow control and drop thresholds */
typedef struct
{
    dnx_ecgm_uc_fc_thresholds_t flow_control;
    uint32 pd_drop;
    uint32 db_drop;
} dnx_ecgm_uc_thresholds_t;

/* multicast drop thresholds for packet descriptors and data buffers */
typedef struct
{
    dnx_ecgm_fadt_threshold_t pd_drop;
    uint32 db_drop;
} dnx_ecgm_mc_drop_thresholds_t;

/* multicast queue resources */
typedef struct
{
    /*
     * amount of reserved packet descriptors
     */
    uint32 reserved_pds;

    /*
     * mapped PD service pool
     */
    uint32 pd_sp;

    /*
     * drop thresholds per DP
     */
    dnx_ecgm_mc_drop_thresholds_t drop_thresholds[DNX_COSQ_NOF_DP];
} dnx_ecgm_mc_queue_resources_t;

typedef struct
{
    uint32 total_pd;
    uint32 uc_pd;
    uint32 mc_pd;
    uint32 total_db;
    uint32 uc_db;
    uint32 mc_db;
} ecgm_core_global_resources_statistics_t;

typedef struct
{
    uint32 pd;
    uint32 db;
    uint32 reserved_pd;
    uint32 reserved_db;
} ecgm_service_pool_resources_statistics_t;

/**
 * }
 */

/**
 * ECGM internal API
 */

/* ECGM_MULTICAST_QUEUE_MAP_TO_SP */

/**
 * \brief - enable dynamic memory access to all tables under ecgm block
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ecgm_dbal_dynamic_memory_access_set(
    int unit,
    int enable);

/**
 * \brief - Gets enable dynamic memory access for ecgm block
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ecgm_dbal_dynamic_memory_access_get(
    int unit,
    uint32 *enable);

/**
 * \brief
 *  Init function for ECGM resources.
 *  initializes port profile and interface profile templates
 *  and set HW registers to requiered default values.
 *
 * \param [in] unit - device unit number
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 *
 * \see
 *   * None
 */
shr_error_e dnx_ecgm_init(
    int unit);

/**
* \brief
*   Deinit function for ECGM resources.
*   releases allocated resources.
*
* \param [in] unit - device unit number
*
* \return
*   shr_error_e
*
* \remarks
*   * None
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_deinit(
    int unit);

/**
* \brief - callback function for port add sequence.
*          allocates empty resource-profiles for the new port.
*
* \param [in] unit - device unit number
*
* \return
*   shr_error_e
*
* \see
*  port_dyn.c
*/
shr_error_e dnx_ecgm_port_add(
    int unit);

/**
* \brief - callback function for port add sequence.
*          frees ecgm resources consumed by the port.
*
* \param [in] unit - device unit number
*
* \return
*   shr_error_e
*
* \see
*   port_dyn.c
*/
shr_error_e dnx_ecgm_port_remove(
    int unit);

/**
* \brief - Map a multicast queue to a PD service pool.
*
* \param [in] unit - device unit number
* \param [in] gport - multicast queue gport
* \param [in] queue_offset - offset from base Q-pair (priority).
* \param [in] pd_service_pool - packet descriptors service pool id (0/1) to map the queue to.
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_map_mc_queue_to_pd_sp_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t queue_offset,
    int pd_service_pool);

/**
* \brief - Map a multicast queue to a PD service pool.
*
* \param [in] unit - device unit number
* \param [in] gport - multicast queue gport
* \param [in] queue_offset - offset from base Q-pair (priority).
* \param [out] pd_service_pool - mapped packet descriptors service pool id (0/1).
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_map_mc_queue_to_pd_sp_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t queue_offset,
    int *pd_service_pool);

/**
* \brief - Mapping multicast COS params on device level.
*          per system TC and system DP:
*          1. Multicast TC (config->priority).
*          2. Data buffers service pool (config->unscheduled_sp).
*          3. Eligibility to use resources (config->unscheduled_se).
*
* \param [in] unit - device unit number.
* \param [in] system_tc - traffic class (0-7).
* \param [in] system_dp - drop precedence (0-3).
* \param [in] config - bcm_cosq_egress_multicast_config_t
*                      relevant fields in the struct are:
*                      priority, unscheduled_sp and unscheduled_se.
* \return
*   shr_error_e
*
* \remarks
*   generally, db sp #0 (bcmCosqEgressMulticastServicePool0)
*   is considered high priority, and #1 (bcmCosqEgressMulticastServicePool1)
*   is considered low priority. this can be configured either way,
*   but should be consistent through all sp configurations.
*
*   mapping an MC-TC to a new SP can only be done if reserved DB for the chosen MC-TC is 0 (for both cores).
*
*   mapping all multicast COS params cannot be done when the device traffic is enabled, 
*   unless the SOC property "allow modifications during traffic" is enabled.
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_core_multicast_config_set(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config);

/**
* \brief - Get multicast COS params mapped to system TC and system DP.
*          see more info about mapping in the set function above.
*
* \param [in] unit - device unit number.
* \param [in] system_tc - traffic class (0-7).
* \param [in] system_dp - drop precedence (0-3).
* \param [in] config - bcm_cosq_egress_multicast_config_t.
*
* \return
*   shr_error_e
*/
shr_error_e dnx_ecgm_core_multicast_config_get(
    int unit,
    bcm_cos_t system_tc,
    bcm_color_t system_dp,
    bcm_cosq_egress_multicast_config_t * config);

/**
* \brief - set ecgm thresholds.
*
* \param [in] unit - device unit number
* \param [in] gport - may be of type: CORE, INTERFACE, MCAST_EGRESS_QUEUE, UCAST_EGRESS_QUEUE or 0.
*                     if gport == 0 - will indicate CORE, and threshold will be set for all cores.
* \param [in] cosq - valid values are: -1 or 0-7 (when represents traffic class)
*                    must be consistent with flags and types indicated by threshold struct.
* \param [in] threshold - a struct containing information about the requierd threshold.
*                         the ONLY relevant fields from the struct are:
*                         flags - indicate the type of threshold action.
*                         dp - drop precedence (0-3), only for relevant some multicast queue thresholds.
*                               when used - flag must be set with BCM_COSQ_THRESHOLD_PER_DP.
*                         type - the threshold resource type.
*                          priority - BCM_COSQ_HIGH_PRIORITY / BCM_COSQ_LOW_PRIORITY
*                                     relevant only for interface thresholds.
*                                     MUST be indicated if gport is interface.
*                         value - indicates the threshold value to be set.
* \return
*   shr_error_e
*
* \remarks
*       NOTE that threshold struct contains a 'tc' field - but this will NOT be used.
*       instead - the given cosq is used to indicate the chosen queue/tc.
* \see
 */
shr_error_e dnx_ecgm_gport_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
* \brief - get ecgm thresholds.
*
* \param [in] unit - device unit number
* \param [in] gport - may be of type: CORE, INTERFACE, MCAST_EGRESS_QUEUE, UCAST_EGRESS_QUEUE or 0.
*                     if gport == 0 - will indicate CORE, and threshold will be given from core id = 0.
* \param [in] cosq - valid values are: -1 or 0-7 (when represents traffic class)
*                    must be consistent with flags and types indicated by threshold struct.
* \param [out] threshold - a struct containing information about the requierd threshold.
*                          the ONLY relevant fields from the struct are:
*                          flags - indicate the type of threshold action.
*                          dp - drop precedence (0-3), only for relevant some multicast queue thresholds.
*                               when used - flag must be set with BCM_COSQ_THRESHOLD_PER_DP.
*                          type - the threshold resource type.
*                          priority - BCM_COSQ_HIGH_PRIORITY / BCM_COSQ_LOW_PRIORITY
*                                     relevant only for interface thresholds.
*                                     MUST be indicated if gport is interface.
*                          value - will be set with the corresponding threshold value.
* \return
*   shr_error_e
*
* \remarks
*       NOTE that threshold struct contains a 'tc' field - but this will NOT be used.
*       instead - the given cosq is used to indicate the chosen queue/tc.
* \see
 */
shr_error_e dnx_ecgm_gport_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_threshold_t * threshold);

/**
* \brief -  Get the current values of the ECGM resources per core.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] core_res_cur - Current values of the following resources:
*                               Total PDs, UC PDs, MC PDs, Total DBs, UC DBs, MC DBs.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_global_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    ecgm_core_global_resources_statistics_t * core_res_cur);

/**
* \brief -  Get the current values of the ECGM resources per InterFace.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] ecgm_priority - Interface priority
* \param [in] egq_if - Interface number
* \param [out] pd - Current value of allocated Packet Descriptors.
* \param [out] db - Current value of allocated Data Buffers.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    dbal_enum_value_field_ecgm_priority_e ecgm_priority,
    uint32 egq_if,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the current values of the ECGM resources per Port.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] otm_port - Port number.
* \param [out] pd - Current value of allocated Packet Descriptors.
* \param [out] db - Current value of allocated Data Buffers.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_port_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 otm_port,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the current values of the ECGM resources per Queue.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] queue_pair - Queue Pair number
* \param [out] pd - Current value of allocated Packet Descriptors.
* \param [out] db - Current value of allocated Data Buffers.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_queue_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 queue_pair,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the current values of the ECGM resources per Service Pool.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sp_id - Service Pool number
* \param [out] sp_resource - Current values of the following resources:
*                               PDs, DBs, reserved PDs, resreved DBs.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_service_pool_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    ecgm_service_pool_resources_statistics_t * sp_resource);

/**
* \brief -  Get the current values of the ECGM resources per Service Pool per Traffic Class.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sp_id - Service Pool number
* \param [in] tc - Traffic Class
* \param [out] pd - Current value of allocated Packet Descriptors.
* \param [out] db - Current value of allocated Data Buffers.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_service_pool_tc_resources_statistics_current_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    uint32 tc,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per core since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [out] core_resource - Maximum values of the following resources:
*                               Total PDs, UC PDs, MC PDs, Total DBs, UC DBs, MC DBs.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_core_global_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    ecgm_core_global_resources_statistics_t * core_resource);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per InterFace since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] ecgm_priority - Interface priority
* \param [in] egq_if - Interface number
* \param [out] pd - Maximum number of allocated Packet Descriptors that have been reached since the last CPU read.
* \param [out] db - Maximum number of allocated Data Buffers that have been reached since the last CPU read.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_interface_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    dbal_enum_value_field_ecgm_priority_e ecgm_priority,
    uint32 egq_if,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per Port since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] otm_port - Port number
* \param [in] pd - Maximum number of allocated Packet Descriptors that have been reached since the last CPU read.
* \param [in] db - Maximum number of allocated Data Buffers that have been reached since the last CPU read.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_port_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 otm_port,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per Queue since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_mc - UC = 0, MC = 1
* \param [in] queue_pair - Queue Pair number
* \param [out] pd - Maximum number of allocated Packet Descriptors that have been reached since the last CPU read.
* \param [out] db - Maximum number of allocated Data Buffers that have been reached since the last CPU read.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_queue_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 is_mc,
    uint32 queue_pair,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per Service Pool since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sp_id - Service Pool number
* \param [out] sp_resource - Maximum number of allocated resources that have been reached since the last CPU read:
*                               PDs, DBs. (Note that the members "reserved PDs" and "reserved DBs" do not change).
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_service_pool_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    ecgm_service_pool_resources_statistics_t * sp_resource);

/**
* \brief -  Get the maximum number of allocated ECGM resources that have been reached per Service Pool per Traffic Class since the last CPU read.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] sp_id - Service Pool number
* \param [in] tc - Traffic Class
* \param [out] pd - Maximum number of allocated Packet Descriptors that have been reached since the last CPU read.
* \param [out] db - Maximum number of allocated Data Buffers that have been reached since the last CPU read.
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_service_pool_tc_resources_statistics_max_get(
    uint32 unit,
    uint32 core_id,
    uint32 sp_id,
    uint32 tc,
    uint32 *pd,
    uint32 *db);

/**
* \brief -  Clears the maximum number of allocated ECGM resources that have been reached since the last CPU read.
*
* \param [in] unit - chip unit id
*
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_resources_statistics_max_clear(
    int unit);

/**
* \brief -  Set the ECGM tacking control bit.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] is_tracking - set the tacking: 1 = on, 0 = off.
* \return
*   shr_error_e
*
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_disable_max_statistics_tracking_set(
    uint32 unit,
    uint32 core_id,
    uint32 is_tracking);

/**
* \brief - Get a disable indication bit for a queue pair number.
*
* \param [in] unit - chip unit id
* \param [in] core_id - core id
* \param [in] queue_pair - queue pair id 0-511
* \param [out] is_queue_disabled - the queue-pair is disabled when the bit is on
*
* \return
*   shr_error_e
*
* \remarks
*   * None
* \see
*   * None
*/
shr_error_e dnx_ecgm_dbal_queue_disable_get(
    uint32 unit,
    uint32 core_id,
    uint32 queue_pair,
    uint32 *is_queue_disabled);

/** } */

#endif  /** _DNX_COSQ_EGRESS_CONGESTION_H_INCLUDED_ */
