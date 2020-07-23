/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/cosq/scheduler.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_H_INCLUDED_
/*
 * { 
 */
#define _DNX_SCHEDULER_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(flow_id) ((flow_id)/dnx_data_sch.flow.region_size_get(unit))

#define DNX_SCH_FLOW_ID_FROM_REMOTE_CORE(base_flow_id, nof_remote_cores, core) ((base_flow_id) + (dnx_data_sch.flow.region_size_get(unit)/(nof_remote_cores))*(core))

#define DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id) (se_id - dnx_data_sch.flow.hr_se_id_min_get(unit))

#define DNX_SCHEDULER_HR_SE_ID_GET(unit, hr_id) (hr_id + dnx_data_sch.flow.hr_se_id_min_get(unit))

#define DNX_SCHEDULER_QUARTET_SIZE 4

#define DNX_SCHEDULER_QUARTET_TO_FLOW_ID(quartet) ((quartet) * DNX_SCHEDULER_QUARTET_SIZE)
#define DNX_SCHEDULER_FLOW_ID_TO_QUARTET(flow_id) ((flow_id) / DNX_SCHEDULER_QUARTET_SIZE)

/* ENUMS
 * {
 */

typedef enum
{
    /*
     *  undefined value
     */
    DNX_SCH_ELEMENT_SE_TYPE_NONE = 0,
    /*
     *  HR scheduler element
     */
    DNX_SCH_ELEMENT_SE_TYPE_HR = 1,
    /*
     *  CL scheduler element
     */
    DNX_SCH_ELEMENT_SE_TYPE_CL = 2,
    /*
     *  FQ scheduler element
     */
    DNX_SCH_ELEMENT_SE_TYPE_FQ = 3,
    /*
     *  Must be the last value
     */
    DNX_SCH_ELEMENT_SE_TYPE_LAST
} dnx_sch_element_se_type_e;

/**
 * \brief
 *   All flow allocations types.
 *
 * \remark
 */
typedef enum
{
    DNX_SCH_ALLOC_FLOW_TYPE_INVALID = -1,
    /*
     * Normal connector
     */
    DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR = 0,
    /*
     * Composite connector 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR = 1,
    /*
     * Normal FQ 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_FQ,
    /*
     * Composite FQ 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ,
    /*
     * Normal CL 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_CL,
    /*
     * Composite CL 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL,
    /*
     * Enhanced CL
     */
    DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL,
    /*
     * Normal HR 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_HR,
    /*
     * Composite HR 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR,
    /*
     * Dual shaper (2p) CL-FQ
     */
    DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ,
    /*
     * 4P Shared shaper 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4,
    /*
     * 8P Shared shaper 
     */
    DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8,

    DNX_SCH_ALLOC_FLOW_TYPE_NOF
} dnx_sch_alloc_flow_type_e;

typedef enum
{
    DNX_SCHEDULER_REGION_EVEN = 0,
    DNX_SCHEDULER_REGION_ODD,
    DNX_SCHEDULER_REGION_ODD_EVEN_MODE_NOF
} dnx_scheduler_region_odd_even_mode_t;

/* ENUMS
 * }
 */
#define DNX_SCH_CL_WEIGHTS_NOF 4

typedef struct
{
    dbal_enum_value_field_cl_mode_e cl_mode;
    dbal_enum_value_field_wfq_weight_mode_e wfq_weight_mode;
    dbal_enum_value_field_cl_enhanced_mode_e enhanced_mode;
} dnx_sch_cl_config_t;

typedef struct
{
    int weight[DNX_SCH_CL_WEIGHTS_NOF];
} dnx_sch_cl_weight_t;

typedef struct
{
    dnx_sch_cl_config_t config;
    dnx_sch_cl_weight_t weight;
} dnx_sch_cl_info_t;

/*
 * API:
 * {
 */

/**
 * \brief - 
 * E2E scheduler port add handling
 * Allocate HR, E2E interface allocate
 *  
 * \param [in] unit -  Unit-ID 
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * bcm_fabric_rci_biasing_probability_info_t
 */
shr_error_e dnx_sch_port_add_handle(
    int unit);

/**
 * \brief - 
 * E2E scheduler port remove handling, 
 * deallocate HR and E2E interface deallocate
 *  
 * \param [in] unit -  Unit-ID 
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_port_remove_handle(
    int unit);

/**
 * \brief - 
 * add scheduler only part to a port
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] port - port ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_port_add_sch(
    int unit,
    bcm_port_t port);

/**
 * \brief - 
 * remove scheduler only part from a port
 * deallocate HR and E2E interface deallocate
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] port - port ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_port_remove_sch(
    int unit,
    bcm_port_t port);

/**
 * \brief - 
 * init E2E scheduler module
 *  
 * \param [in] unit -  Unit-ID 
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_init(
    int unit);

/**
 * \brief - 
 * deinit E2E scheduler module
 *  
 * \param [in] unit -  Unit-ID 
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_deinit(
    int unit);

/**
 * \brief
 *   Print CL class template data
 * \par DIRECT INPUT
 *   \param [in] unit - unit ID
 *   \param [in] data - template data
 * \par INDIRECT INPUT
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 * \see
 *   * None
 */
void dnx_sch_cl_class_print_cb(
    int unit,
    const void *data);

/**
 * \brief - 
 * colors port's HRs according to default coloring algorith
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] port - Port ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_port_interface_ports_color(
    int unit,
    bcm_port_t port);

/**
 * \brief - Get a bitmap of allocated HRs in a specified PS
 */
shr_error_e dnx_sch_allocated_hrs_get(
    int unit,
    bcm_core_t core,
    int ps,
    uint32 *allocated_hrs,
    int *priority,
    int *priority_propagation_enable);

/**
 * \brief - allocate port HR
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] core - core ID
 * \param [in] port_info - basic port info
 * \param [in] nof_priorities - number of scheduler priorities
 * \param [in] priority_propagation_enable - priority propagation enable
 * \param [in] with_id - should allocate WITH ID
 * \param [out] hr_id - allocated HR ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_scheduler_port_hr_allocate(
    int unit,
    bcm_core_t core,
    dnx_algo_port_info_s port_info,
    int nof_priorities,
    int priority_propagation_enable,
    int with_id,
    int *hr_id);

/**
 * \brief - Get a nof_remote_cores from provided voq_connector 
 *        gport
 */
shr_error_e dnx_sch_voq_connector_nof_remote_cores_get(
    int unit,
    bcm_core_t core,
    int voq_connector_id,
    int *nof_remote_cores);

/**
 * \brief Get scheduler element type from flow id.
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - core ID
 * \param [in] flow_id - flow ID
 * \param [out] se_type - element type
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_flow_se_type_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_element_se_type_e * se_type);

/**
 * \brief Get flow id and core from gport
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] gport - input gport
 * \param [in] cosq - the index inside the bundle for VOQ connectors
 * \param [out] core - core ID
 * \param [out] flow_id - flow ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
/**
 * \brief - obtain flow id from gport and cosq
 */
shr_error_e dnx_scheduler_gport_core_flow_id_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *core,
    int *flow_id);

/**
 * \brief - is the flow allocated
 */
shr_error_e dnx_scheduler_flow_is_allocated(
    int unit,
    int core,
    int flow_id,
    int *is_allocated);

/**
 * \brief - convert  SE ID to port+TC
 */
int dnx_sch_convert_se_id_to_port_tc(
    int unit,
    int core,
    int se_id,
    bcm_port_t * logical_port,
    int *tc);

/**
 * \brief - Function to set flow to be tracked when counting 
 *        credit count
 */
shr_error_e dnx_sch_flow_credit_counter_configuration_set(
    int unit,
    int core,
    int flow_id);

/**
 *
 * \brief - Function to disable the filters applied on the credit counters per flow
 *
 * \author db889754 (2/20/2018)
 *
 * \param unit [in]- unit
 * \param core [in]- core
 *
 * \return shr_error_e
 */
shr_error_e dnx_sch_flow_credit_counter_configuration_reset(
    int unit,
    int core);

/**
 * \brief - Function to return the credit count of the flow 
 *        configured by this function -
 *        dnx_sch_flow_credit_counter_configuration_set
 */
shr_error_e dnx_sch_flow_credit_counter_info_get(
    int unit,
    int core_id,
    uint32 *credit_count);

/**
 * \brief - Function to set port to be tracked when counting 
 *        credit count
 */
shr_error_e dnx_sch_port_credit_counter_configuration_set(
    int unit,
    int core,
    int port);

/**
 *
 * \brief - Function to disable the filters applied on the credit counters per port
 *
 * \param unit [in]- unit
 * \param core [in]- core
 *
 * \return shr_error_e
 */
shr_error_e dnx_sch_port_credit_counter_configuration_reset(
    int unit,
    int core);

/**
 * \brief - Function to return the credit count of the port 
 *        configured by this function -
 *        dnx_sch_port_credit_counter_configuration_set
 */
shr_error_e dnx_sch_port_credit_counter_info_get(
    int unit,
    int core_id,
    uint32 *credit_count);

/**
 * \brief - Function to set port to be tracked when counting 
 *        flow control indications count
 */
shr_error_e dnx_sch_port_fc_counter_configuration_set(
    int unit,
    int core,
    int port);

/**
 * \brief - Function to return the fc indications count of the 
 *        port configured by this function -
 *        dnx_sch_port_fc_counter_configuration_set
 */
shr_error_e dnx_sch_port_fc_counter_info_get(
    int unit,
    int core_id,
    uint32 *fc_count);

/**
 *  
 * \brief Function to retrieve the slow status of a given flow 
 *  
 * @param unit [in] - unit 
 * @param core_id [in] - core_id
 * @param flow_id [in] - flow_id
 * @param slow_status [out] - 2 bit value: 
 * 0 - OFF 
 * 1 - ON-SLOW 
 * 2 - ON-NORMAL 
 *  MAX returned value is restricted to 2!
 * @return shr_error_e 
 */
shr_error_e dnx_sch_flow_slow_status_get(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_flow_slow_status_e * slow_status);

/**
 * \brief - convert flow ID to SE ID
 */
shr_error_e dnx_sch_convert_flow_to_se_id(
    int unit,
    int flow_id,
    int *se_id);

/*
 * \brief - convert SE ID to flow ID
 */
shr_error_e dnx_sch_convert_se_to_flow_id(
    int unit,
    int se_id,
    int *flow_id);

/**
 * \brief - 
 * allocate HRs for advanced FMQ mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core -  core-ID 
 * \param [in] hr_nof - number of required HRs 
 * \param [out] hr_id - array of allocated HR IDs
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
int dnx_scheduler_fmq_hr_allocate(
    int unit,
    bcm_core_t core,
    int hr_nof,
    uint32 *hr_id);

/**
 * \brief - 
 * deallocate advanced FMQ mode HRs 
 * 
 * called when switching to simple FMQ mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core -  core-ID 
 * \param [in] hr_nof - number of required HRs 
 * \param [in] hr_id - array of HR IDs
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
int dnx_scheduler_fmq_hr_deallocate(
    int unit,
    bcm_core_t core,
    int hr_nof,
    uint32 *hr_id);
/**
 * \brief -
 * configure rci_biasing_mode to HW
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] rci_biasing_mode - rci biasing mode
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_rci_throttling_mode_hw_set(
    int unit,
    dbal_enum_value_field_rci_biasing_mode_e rci_biasing_mode);

/**
 * \brief -
 * read rci_biasing_mode from HW
 *
 * \param [in] unit -  Unit-ID 
 * \param [out] rci_biasing_mode - rci biasing mode
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_rci_throttling_mode_hw_get(
    int unit,
    dbal_enum_value_field_rci_biasing_mode_e * rci_biasing_mode);

/**
 * \brief - Get FMQ Class from FMQ HR gport
 *
 * \param [in] unit - Unit ID
 * \param [in] fmq_hr_gport - fmq_hr_gport
 * \param [out] fmq_class - fmq_class
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_fmq_hr_gport_fmq_class_get(
    int unit,
    bcm_gport_t fmq_hr_gport,
    int *fmq_class);

/**
 * \brief - returns flow_id of connector num. cos
 * in case of composite connector, returns flow_id of the second subflow as well
 */
shr_error_e dnx_scheduler_connector_flow_id_calc(
    int unit,
    int core,
    int base_flow_id,
    int cos,
    int is_non_contiguous,
    int is_composite,
    int called_by_voq_remove,
    int flow[2],
    int *nof_subflows);

/**
 * \brief - Set mode and weight of attaching a flow to its parent
 */
shr_error_e dnx_scheduler_gport_e2e_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/**
 * \brief - Get mode and weight of attaching a flow to its parent
 */
shr_error_e dnx_scheduler_gport_e2e_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/**
 * \brief - cosq control set on flow gport
 */
int dnx_scheduler_cosq_control_flow_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief - cosq control get on flow gport
 */
int dnx_scheduler_cosq_control_flow_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - get bandwidth of a flow
 */
shr_error_e dnx_scheduler_gport_flow_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - configure bandwidth of a flow
 */
shr_error_e dnx_scheduler_gport_flow_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/* 
 *  \brief -
 * Attach given port priority (TC) to Port TCG
 */
int dnx_scheduler_gport_e2e_port_tc_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/** 
 * \brief -
 * Get Port TCG of a given port priority (TC).
 */
int dnx_scheduler_gport_e2e_port_tc_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/** 
 * \brief -
 * Set EIR Weight for E2E Port TCG
 */
int dnx_scheduler_gport_e2e_port_tcg_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight);

/** 
 * \brief -
 * Get EIR Weight for E2E Port TCG
 */
int dnx_scheduler_gport_e2e_port_tcg_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight);

/** 
 * \brief -
 * Configure Port-TC (HR) shaping rate, set single port rate.
 */
int dnx_scheduler_gport_e2e_port_tc_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/** 
 * \brief -
 * Get Port-TC (HR) shaping rate
 */
int dnx_scheduler_gport_e2e_port_tc_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/** 
 * \brief -
 * Configure TCG shaping rate - E2E
 */
int dnx_scheduler_gport_e2e_port_tcg_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/** 
 * \brief -
 * Get E2E Port-TCG shapring rate
 */
int dnx_scheduler_gport_e2e_port_tcg_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port tc scheduler 
 */
int dnx_scheduler_control_e2e_port_tc_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e port tc
 */
int dnx_scheduler_control_e2e_port_tc_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port tcg scheduler 
 */
int dnx_scheduler_control_e2e_port_tcg_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/** 
 * \brief -
 * Get various feature dependent control type
 * for the e2e port tcg
 */
int dnx_scheduler_control_e2e_port_tcg_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port
 */
int dnx_scheduler_cosq_control_e2e_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e port
 */
int dnx_scheduler_cosq_control_e2e_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/** 
 * \brief -
 * Configure E2E port bandwidth
 */
int dnx_scheduler_gport_e2e_port_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/** 
 * \brief -
 * Get E2E port bandwidth
 */
int dnx_scheduler_gport_e2e_port_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/** 
 * \brief -
 * Configure E2E interface bandwidth
 */
int dnx_scheduler_gport_e2e_interface_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/** 
 * \brief -
 * Get E2E interface bandwidth
 */
int dnx_scheduler_gport_e2e_interface_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Has the region connectors
 */
shr_error_e dnx_scheduler_region_has_connector(
    int unit,
    int core,
    int region_index,
    int *has_connector);

/**
 * \brief - Is the region interdigitated
 */
shr_error_e dnx_scheduler_region_is_interdigitated_get(
    int unit,
    int core,
    int region_index,
    int *is_interdigitated);

/**
 * \brief - Get shared shaper mode
 */
shr_error_e dnx_sch_shared_shaper_mode_get(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_shared_shaper_mode_e * mode);

/**
 * \brief returns true iff flow is SE
 */
shr_error_e dnx_sch_flow_is_se_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_se);

/**
 * \brief -
 * returns TRUE iff flow_id is reserved element
 */
shr_error_e dnx_scheduler_flow_is_reserved_element(
    int unit,
    int core,
    int flow_id,
    int *is_reserved);

/**
 * \brief Obtain is_composite property of flow
 */
shr_error_e dnx_sch_flow_is_composite_get(
    int unit,
    int core_id,
    int base_flow_id,
    int *is_composite);

/**
 * \brief - Get region odd/even mode 
 */
shr_error_e dnx_scheduler_region_odd_even_mode_get(
    int unit,
    int core,
    int region_index,
    dnx_scheduler_region_odd_even_mode_t * odd_even_mode);

/**
 * \brief - get number of shared priorities for provided flow
 */
shr_error_e dnx_scheduler_element_shared_shaper_nof_priorities_get(
    int unit,
    int core,
    int flow_id,
    int *nof_priorities);

/**
 * \brief - get real scheduler gport
 * mainly used to replace FMQ gport by actual sched gport
 */
shr_error_e dnx_scheduler_gport_actual_sched_gport_get(
    int unit,
    bcm_gport_t sched_gport,
    int allow_core_all,
    bcm_gport_t * actual_sched_gport);

/**
 * \brief
 *   connect VOQ connector to remote VOQ (egress side)
 */
int dnx_scheduler_connection_set(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect);

/**
 * \brief
 *   obtain remote VOQ connected to provided VOQ connector (egress side)
 */
int dnx_scheduler_connection_get(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect);

/**
 * \brief -
 * Allocate and configure new scheduling element
 * legacy API implementation
 */
int dnx_cosq_sched_gport_add(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport);

/**
 * \brief - main function implementing bcm_cosq_voq_connector_gport_add API
 */
shr_error_e dnx_scheduler_connector_gport_add(
    int unit,
    int core,
    int src_modid,
    uint32 nof_remote_cores,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport);

/**
 * \brief - main function implementing bcm_cosq_gport_delete for VOQ_CONNECTOR gport
 */
shr_error_e dnx_scheduler_voq_connector_gport_delete(
    int unit,
    bcm_gport_t gport);

/**
 * \brief - delete scheduling element
 */
int dnx_scheduler_element_gport_delete(
    int unit,
    bcm_gport_t gport);

/**
 * \brief - get information about VOQ connector gport
 */
int dnx_scheduler_connector_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_voq_connector_gport_t * config);

/**
 * \brief - get information about scheduler element gport
 * in terms of legacy bcm_cosq_gport_add API
 */
int dnx_sched_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags);

/**
 * \brief - configure mapping of qpair to HR
 */
shr_error_e dnx_scheduler_internal_fc_map_set(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target);

/**
 * \brief - get mapping of qpair to HR
 */
shr_error_e dnx_scheduler_internal_fc_map_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count);

shr_error_e dnx_scheduler_low_rate_factor_set(
    int unit,
    int factor);

shr_error_e dnx_scheduler_low_rate_factor_get(
    int unit,
    int *factor);

shr_error_e dnx_scheduler_low_rate_connector_range_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range);

shr_error_e dnx_scheduler_low_rate_connector_range_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range);

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e interface
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  E2E interface gport
 * \param [in] cosq -  TC
 * \param [in] type -  control type
 * \param [in] arg  -  control value
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
int dnx_scheduler_cosq_control_e2e_interface_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg);

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e interface
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  E2E interface gport
 * \param [in] cosq -  TC
 * \param [in] type -  control type
 * \param [in] arg  -  control value
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
int dnx_scheduler_cosq_control_e2e_interface_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg);

/**
 * \brief - get scheduler port for a given port
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] src_port - egress logical port
 * \param [out] target_port - scheduler port attached to src_port
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
shr_error_e dnx_scheduler_port_scheduler_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * target_port);

/**
 * \brief - update number of tokens on reserved SE
 *          to make sure no credits are issued to unattached flows
 *          following soft reset
 */
shr_error_e dnx_sch_reserve_se_update(
    int unit);

/**
 * \ brief configure SMP message counter fabric type, id  and filter
 */

shr_error_e dnx_sch_fsm_counter_configuration_fabric_type_set(
    int unit,
    int core,
    uint32 flow__id,
    uint32 fabric_type,
    uint32 filter_by_type,
    uint32 filter_by_flow_id);

/**
 * \ brief get SMP message counter fabric type, id  and filter
 */
shr_error_e dnx_sch_fsm_counter_configuration_fabric_type_get(
    int unit,
    int core,
    uint32 *flow_id,
    uint32 *fabric_type,
    uint32 *filter_by_type,
    uint32 *filter_by_flow_id);

/**
 * \ brief read SMP message counter
 */
shr_error_e dnx_sch_fsm_counter_info_get(
    int unit,
    int core,
    uint32 *count,
    uint32 *overflow);

#endif /*_DNX_SCHEDULER_H_INCLUDED_*/
