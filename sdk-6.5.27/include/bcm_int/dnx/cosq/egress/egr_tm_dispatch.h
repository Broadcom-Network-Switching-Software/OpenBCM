/** \file egr_tm_dispatch.h
 * 
 * 
 * Internal DNX egress tm dispatched APIs
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef COSQ_EGRESS_EGRTMDISPATCH_H_INCLUDED
/** { */
#define COSQ_EGRESS_EGRTMDISPATCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (both DNX2 and others) family only!"
#endif

#include <bcm/types.h>
#include <bcm/cosq.h>

/*************
 * DEFINES   *
 *************/
/* { */
/* } */

/*************
 * TYPEDEFS   *
 *************/
/* { */
/* } */
/****************
 * PROTOTYPES   *
 ****************/
/* { */

/**
 * \brief - port add handle for dynamic port
 *
 * \param [in] unit - unit ID
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_queuing_port_add_handle(
    int unit);

/**
 * \brief - port remove handle for dynamic port
 *
 * \param [in] unit - unit ID
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_queuing_port_remove_handle(
    int unit);

/**
 * \brief - port disable handle for dynamic port
 *
 * \param [in] unit - unit ID
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_queuing_port_disable_handle(
    int unit);

/**
 * \brief - allocate an egress TM interface
 *
 * \param [in] unit - unit ID
 * \param [in] port - logical port.
 * \param [in] master_logical_port - master logical port associated with input port.
 * \param [in] core - core ID.
 * \param [in] interface_info - Interface info, based on which the egress interface ID is determined.
 * \param [in] is_channelized_interface - indication that the interface is channelized
 * \param [in] flexe_channel_id - flexe channel id. Vaild only for flexe
 * \param [out] egr_if_id - resulted allocated egress interface ID.
 * \param [out] ch_egr_if_id - resulted allocated channelized egress interface ID, when interface is channelized both are allocated.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_egr_tm_interface_allocate(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_interface_info_t * interface_info,
    int is_channelized_interface,
    int flexe_channel_id,
    int *egr_if_id,
    int *ch_egr_if_id);

/**
 * \brief - allocate a base queue or base qpair.
 *
 * \param [in] unit - unit ID
 * \param [in] flags - supported flags are BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID and BCM_PORT_ADD_SCH_PRIORITY_PROPAGATION_EN
 * \param [in] port - logical port.
 * \param [in] core - core ID
 * \param [in] egr_interface_id - egress interface ID
 * \param [in] nof_priorities - number of priorities
 * \param [in] is_channelized - indication if interface is channelized
 * \param [in] is_rcy - indication if interface is RCY
 * \param [in,out] base_queue - allocated base_queue when output, base_queue to allocate when input with WITH_ID flag.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_base_queue_allocate(
    int unit,
    uint32 flags,
    bcm_port_t port,
    bcm_core_t core,
    int egr_interface_id,
    int nof_priorities,
    int is_channelized,
    int is_rcy,
    int *base_queue);

/**
 * \brief - set a control attribute per port LOCAL.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_GPORT_LOCAL_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] arg - input arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_local_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - get a control attribute per port LOCAL.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_GPORT_LOCAL_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [out] arg - output arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_local_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - set a control attribute per port LOCAL_INTERFACE.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_GPORT_LOCAL_INTERFACE_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] arg - input arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_local_interface_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - get a control attribute per port LOCAL_INTERFACE.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_GPORT_LOCAL_INTERFACE_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [out] arg - output arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_local_interface_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - set a control attribute per port RCY and RCY_MIRR.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_RCY_SET or BCM_COSQ_GPORT_PORT_RCY_MIRR_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] is_mirror - indicate of its recycle mirror
 * \param [in] arg - input arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_rcy_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int is_mirror,
    int arg);

/**
 * \brief - get a control attribute per port RCY and RCY_MIRR.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_RCY_SET or BCM_COSQ_GPORT_PORT_RCY_MIRR_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] is_mirror - indicate of its recycle mirror
 * \param [out] arg - output arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_rcy_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int is_mirror,
    int *arg);

/**
 * \brief - set a control attribute per port TC.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_TC_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] arg - input arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_tc_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - get a control attribute per port TC.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_TC_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [out] arg - output arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_tc_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - set a control attribute per port TCG.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_TCG_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [in] arg - input arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_tcg_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - get a control attribute per port TCG.
 *
 * \param [in] unit - unit ID
 * \param [in] gport - BCM_COSQ_GPORT_PORT_TCG_SET encoded port
 * \param [in] cosq - The priority (TC) within the port
 * \param [in] type - bcmCosqControl...
 * \param [out] arg - output arg
 *
 * \return
 *   see shr_error_e
 * \remark
 *   description per type is in lower level functions
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_cosq_control_port_tcg_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - set port priority to high/low
 *
 * \param [in] unit - unit ID
 * \param [in] gport - logical port gport.
 * \param [in] cosq - not used
 * \param [in] mode -   BCM_COSQ_SP0 - High priority
 *                      BCM_COSQ_SP1 - Low priority
 * \param [in] weight - not used
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_priority_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/**
 * \brief - get port priority, either high or low
 *
 * \param [in] unit - unit ID
 * \param [in] gport - logical port gport.
 * \param [in] cosq - not used
 * \param [out] mode -   BCM_COSQ_SP0 - High priority
 *                       BCM_COSQ_SP1 - Low priority
 * \param [out] weight - not used
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_priority_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/**
 * \brief - init a port's TXi credit count
 *
 * \param [in] unit - unit ID
 * \param [in] logical_port - logical port.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_credit_init(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - set port configurations based on speed
 *
 * \param [in] unit - unit ID
 * \param [in] logical_port - logical port.
 * \param [in] speed - port speed (Mbps)
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_speed_set_config(
    int unit,
    bcm_port_t logical_port,
    int speed);

/**
 * \brief - Switch between active and stand-by egress TM scheduling calendars
 *
 * \param [in] unit - unit ID
 * \param [in] core - core (BCM_CORE_ALL is also valid).
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_scheduling_calendar_switch(
    int unit,
    bcm_core_t core);

/**
 * \brief - Set stand-by egress TM scheduling calendar
 *
 * \param [in] unit - unit ID
 * \param [in] core - core (BCM_CORE_ALL is also valid).
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_scheduling_stand_by_calendar_set(
    int unit,
    bcm_core_t core);

/**
 * \brief - enable/disable a port and flush traffic out of its queues if requested.
 *
 * \param [in] unit - unit ID
 * \param [in] logical_port
 * \param [in] enable - boolean indication -    1 - enable port
 *                                              0 - disable port
 * \param [in] flush - boolean indication -     1 - flush port if was disabled
 *                                              0 - don't flush port if was disabled
 *
 * \return
 *   see shr_error_e
 * \remark
 *   *  Flushing a port is possible only if it was disabled.
 *      trying to flush an enabled port will not do anything.
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_enable_and_flush_set(
    int unit,
    bcm_port_t logical_port,
    uint32 enable,
    uint32 flush);

/**
 * \brief - Get indication if port is enabled/disabled.
 *
 * \param [in] unit - unit ID
 * \param [in] logical_port
 * \param [out] enable - boolean indication -    1 - enabled port
 *                                              0 - disabled port
 *
 * \return
 *   see shr_error_e
 * \remark
 *   *  none
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_port_enable_get(
    int unit,
    bcm_port_t logical_port,
    int *enable);

/**
 * \brief - set queue's weight in a qpair
 *
 * \param [in] unit - unit ID
 * \param [in] gport - UCAST_EGRESS_QUEUE or MCAST_EGRESS_QUEUE gport.
 * \param [in] cosq - queue's TC
 * \param [in] mode -          BCM_COSQ_SP0 - input queue has SP over the other one in the queue pair.
 *                             BCM_COSQ_SP1 - the other queue in the queue pair has SP over input queue.
 *                             -1 - input queue has a valid weight.
 * \param [in] weight - weight to set for input queue
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_queue_wfq_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/**
 * \brief - get queue's weight in a qpair
 *
 * \param [in] unit - unit ID
 * \param [in] gport - UCAST_EGRESS_QUEUE or MCAST_EGRESS_QUEUE gport.
 * \param [in] cosq - queue's TC
 * \param [out] mode -          BCM_COSQ_SP0 - input queue has SP over the other one in the queue pair.
 *                              BCM_COSQ_SP1 - the other queue in the queue pair has SP over input queue.
 *                              -1 - input queue has a valid weight.
 * \param [out] weight - weight set for input queue
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_queue_wfq_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/**
 * \brief - set an IPC interface bandwidth's weight over the other interfaces
 *
 * \param [in] unit - unit ID
 * \param [in] gport - LOCAL_INTERFACE gport.
 * \param [in] weight - weight to set for given interface
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_ipc_set(
    int unit,
    bcm_gport_t gport,
    int weight);

/**
 * \brief - get an IPC interface bandwidth's weight over the other interfaces
 *
 * \param [in] unit - unit ID
 * \param [in] gport - LOCAL_INTERFACE gport.
 * \param [out] weight - weight set for given interface
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_ipc_get(
    int unit,
    bcm_gport_t gport,
    int *weight);

/**
 * \brief - get recycle or mirror's weight when scheduling between both interfaces
 *
 * \param [in] unit - unit ID
 * \param [in] gport - GPORT_RCY or GPROT_RCY_MIRR.
 * \param [out] weight - weight set for input interface (0 - 255)
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_rcy_eir_weight_get(
    int unit,
    bcm_gport_t gport,
    int *weight);

/**
 * \brief - set recycle or mirror's weight when scheduling between both interfaces
 *
 * \param [in] unit - unit ID
 * \param [in] gport - GPORT_RCY or GPROT_RCY_MIRR.
 * \param [in] weight - weight to set for input interface (0 - 255)
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_egr_tm_rcy_eir_weight_set(
    int unit,
    bcm_gport_t gport,
    int weight);

/*
 * \brief - set egress priority of a port based on ingress TC and DP
 *
 * \param [in] unit - unit ID
 * \param [in] gport - either UCAST_EGRESS_QUEUE, MCAST_EGRESS_QUEUE or TRUNK. gport should represent the object which you want to get the mapping of
 * \param [in] count - nof keys/offsets.
 * \param [in] key_array - keys (TC/DP) array.
 * \param [in] offset_array - offset (from base) array.
 * \return
 *   see shr_error_e
 * \remark
 *   * none
 * \see
 *   * none
 */
shr_error_e dnx_egr_tm_cosq_gport_egress_map_set(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array);

/*
 * \brief - get egress priority of a port based on ingress TC and DP
 *
 * \param [in] unit - unit ID
 * \param [in] gport - either UCAST_EGRESS_QUEUE, MCAST_EGRESS_QUEUE or TRUNK. gport should represnt the object which you want to get the mapping of
 * \param [in] count - nof keys/offsets.
 * \param [in] key_array - keys (TC/DP) array.
 * \param [out] offset_array - offset (from base) array.
 * \return
 *   see shr_error_e
 * \remark
 *   * none
 * \see
 *   * none
 */
shr_error_e dnx_egr_tm_cosq_gport_egress_map_get(
    int unit,
    bcm_gport_t gport,
    int count,
    bcm_cosq_queue_offset_mapping_key_t * key_array,
    bcm_cosq_queue_offset_t * offset_array);

/**
 * \brief - Configure channelized interface shaper's rate.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -Local interface gport.
 * \param [in] cosq - Not used. Must be set to '0'.
 * \param [in] kbits_sec_min - Not used. Must be set to '0'.
 * \param [in] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [in] flags - Not used. Must be set to '0'.
 * \return
 *   shr_error_e
 * \remark
 *   * Input gport must represent an interface of channelized port.
 */
shr_error_e dnx_egr_tm_interface_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Configure OTM shaper's rate.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -Local gport.
 * \param [in] cosq - Not used. Must be set to '0'.
 * \param [in] kbits_sec_min - Not used. Must be set to '0'.
 * \param [in] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [in] flags - Not used. Must be set to '0'.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_egr_tm_port_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Get channelized interface shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -Local interface gport.
 * \param [in] cosq - Not used. Must be set to '0'.
 * \param [out] kbits_sec_min - Not used, must not be NULL
 * \param [out] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [out] flags - Not used, must not be NULL
 * \return
 *   shr_error_e
 * \remark
 *   * Input port must be channelized.
 */
shr_error_e dnx_egr_tm_interface_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Get OTM shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -Local gport.
 * \param [in] cosq - Not used. Must be set to '0'.
 * \param [out] kbits_sec_min - Not used, must not be NULL
 * \param [out] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [out] flags - Not use, must not be NULL
 * \return
 *   shr_error_e
 * \remark
 *   * Input port must be channelized.
 */
shr_error_e dnx_egr_tm_port_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Configure queue shaper's rate.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport - port tc gport.
 * \param [in] cosq - cosq value
 * \param [in] kbits_sec_min - Not used. Must be set to '0'.
 * \param [in] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [in] flags - Not used. Must be set to '0'.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_egr_tm_queue_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Get queue shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -port tc gport.
 * \param [in] cosq - cosq value
 * \param [out] kbits_sec_min - Not used, must not be NULL
 * \param [out] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [out] flags - Not used, must not be NULL
 * \return
 *   shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_egr_tm_queue_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Configure cir shaper's rate.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -port tc sched.
 * \param [in] cosq - cosq value
 * \param [in] kbits_sec_min - Not used. Must be set to '0'.
 * \param [in] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [in] flags - Not used. Must be set to '0'.
 * \return
 *   shr_error_e
 * \remark
 */
shr_error_e dnx_egr_tm_cir_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Get cir shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -port tc sched.
 * \param [in] cosq - cosq value
 * \param [out] kbits_sec_min - Not used, must not be NULL
 * \param [out] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [out] flags - Not used, must not be NULL
 * \return
 *   shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_egr_tm_cir_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Set recycle shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -port recycle or recycle mirror.
 * \param [in] is_mirror - indicate if recycle or mirror
 * \param [in] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [in] flags - indicate if cir shaper or total shaper
 * \return
 *   shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_egr_tm_recycle_shaper_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    uint32 is_mirror,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * \brief - Get recycle shaper's rate configuration.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gport -port recycle or recycle mirror.
 * \param [in] is_mirror - indicate if recycle or mirror
 * \param [out] kbits_sec_max - Maximal bandwidth in kbits/sec
 * \param [out] flags - indicate if cir shaper or total shaper
 * \return
 *   shr_error_e
 * \remark
 * \see
 */
shr_error_e dnx_egr_tm_recycle_shaper_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    uint32 is_mirror,
    uint32 *kbits_sec_max,
    uint32 *flags);

/** } */
#endif /* COSQ_EGRESS_EGRTMDISPATCH_H_INCLUDED */
