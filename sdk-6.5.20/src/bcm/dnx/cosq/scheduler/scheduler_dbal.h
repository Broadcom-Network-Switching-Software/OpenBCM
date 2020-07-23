/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_dbal.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_DBAL_H_INCLUDED_
#define _DNX_SCHEDULER_DBAL_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>

#define DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_if) \
    (((sch_if) < dnx_data_sch.interface.nof_big_calendars_get(unit)) ? DBAL_TABLE_SCH_IF_BIG_CALENDAR : DBAL_TABLE_SCH_IF_REGULAR_CALENDAR)

#define DNX_SCH_IF_SHAPER_DBAL_TABLE_GET(unit, sch_if) \
    (((sch_if) < dnx_data_sch.interface.nof_channelized_calendars_get(unit)) ? DBAL_TABLE_SCH_IF_SHAPER : DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER)

typedef struct
{
    dbal_enum_value_result_type_sch_flow_credit_src_cos_e type;
    uint32 simple_cos;
    uint32 cl_weight;
    uint32 hr_weight_mantissa;
    uint32 hr_weight_exp;
    int hr_sel_dual;
} dnx_sch_flow_credit_source_cos_data_t;

typedef struct
{
    int se_id;
    dnx_sch_flow_credit_source_cos_data_t cos;
} dnx_sch_flow_credit_source_t;

typedef enum
{
    DNX_SCH_PORT_TC_CALENDAR = 0,
    DNX_SCH_PORT_TCG_CALENDAR
} dnx_sch_port_calendar_e;

/*
 * \brief - Configure flow attributes to their default values
 */
shr_error_e dnx_sch_flow_attributes_reset(
    int unit,
    int core_id,
    int flow_id);

/**
 * \brief - 
 * function for managing the flow attribute "is_high_priority" 
 * that is used for RCI throttling  mechanism 
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - Core ID
 * \param [in] flow_id - The connector flow ID or the SE flow ID
 * \param [out] is_high_priority - the flow attribute we get
 *        "is_high_priority" - 1 for high 0 for low
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_flow_is_high_priority_hw_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_high_priority);

/**
 * \brief - 
 * function for managing the flow attribute "is_high_priority" 
 * that is used fpr RCI throttling  mechanism 
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - Core ID
 * \param [in] flow_id - The connector flow ID or the SE flow ID
 * \param [in] is_high_priority - the flow attribute we set
 *        "is_high_priority" - 1 for high 0 for low
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_flow_is_high_priority_hw_set(
    int unit,
    int core_id,
    int flow_id,
    int is_high_priority);

/**
 * \brief - 
 * function for managing the flow attribute "is_remote" that is 
 * used fpr RCI throttling  mechanism 
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - Core ID
 * \param [in] flow_id - The connector flow ID or the SE flow ID
 * \param [out] is_remote - the flow attribute we get
 *        "is_remote" - 1 for remote/fabric 0 for local
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_flow_is_remote_hw_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_remote);

/**
 * \brief - 
 * function for managing the flow attribute "is_remote" that is 
 * used fpr RCI throttling  mechanism 
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core_id - Core ID
 * \param [in] flow_id - The connector flow ID or the SE flow ID
 * \param [in] is_remote - the flow attribute we set
 *        "is_remote" - 1 for remote/fabric 0 for local
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_flow_is_remote_hw_set(
    int unit,
    int core_id,
    int flow_id,
    int is_remote);

/*
 * \brief - Configure flow credit source properties
 */
shr_error_e dnx_sch_flow_credit_src_hw_set(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_t * credit_source);

/*
 * \brief - Obtain flow credit source properties
 */
shr_error_e dnx_sch_flow_credit_src_hw_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_t * credit_source);

/*
 * \brief - Get flow credit source SE ID 
 */
shr_error_e dnx_sch_flow_credit_source_se_id_get(
    int unit,
    int core_id,
    int flow_id,
    int *credit_source_se_id);

/*
 * \brief - Get flow credit source cos properties
 */
shr_error_e dnx_sch_flow_credit_source_cos_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_cos_data_t * credit_source_cos);

/*
 * \brief - Configure is_composite flow attribite
 */
shr_error_e dnx_sch_flow_is_composite_hw_set(
    int unit,
    int core_id,
    int flow_id_pair,
    int is_composite);

/*
 * \brief - Configure is composite property  in FSF table for range of flows
 */
shr_error_e dnx_sch_flow_is_composite_hw_range_set_fsf(
    int unit,
    int core_id,
    int flow_id_pair,
    int nof_pairs,
    int is_composite);

/*
 * \brief - Obtain is_composite flow attribute
 */
shr_error_e dnx_sch_flow_is_composite_hw_get(
    int unit,
    int core_id,
    int flow_id_pair,
    int *is_composite);

/*
 * \brief - Configure is composite property of flow
 */
shr_error_e dnx_sch_connector_is_composite_mapping_hw_set(
    int unit,
    int core_id,
    int flow_quartet,
    int is_composite);

/*
 * \brief - Reset attributes of Shaper Descriptor table 
 * to their default values (aka reset values)
 */
shr_error_e dnx_sch_flow_shaper_attributes_reset(
    int unit,
    int core_id,
    int flow_id);

/*
 * \brief - Configure shaper max burst
 */
shr_error_e dnx_sch_flow_shaper_max_burst_set(
    int unit,
    int core_id,
    int flow_id,
    uint32 max_burst);

/*
 * \brief - Get shaper max burst
 */
shr_error_e dnx_sch_flow_shaper_max_burst_get(
    int unit,
    int core_id,
    int flow_id,
    uint32 *max_burst);

/*
 * \brief - Update dynamic number of tokens for the flow
 * Called after updating max_burst,
 */
shr_error_e dnx_sch_flow_shaper_token_count_set(
    int unit,
    int core_id,
    int flow_id[],
    int nof_remote_cores,
    int token_count);

/*
 * \brief - Configure shaper bandwidth
 */
shr_error_e dnx_sch_flow_shaper_bandwidth_set(
    int unit,
    int core_id,
    int flow_id,
    uint32 peak_rate_man,
    uint32 peak_rate_exp);

/*
 * \brief - Get shaper bandwidth
 */
shr_error_e dnx_sch_flow_shaper_bandwidth_get(
    int unit,
    int core_id,
    int flow_id,
    uint32 *peak_rate_man,
    uint32 *peak_rate_exp);

/*
 * \brief - Configure is slow enable flow property
 */
shr_error_e dnx_sch_flow_is_slow_enable_set(
    int unit,
    int core_id,
    int flow_id,
    int is_slow_enable);

/*
 * \brief - Get is slow enable flow property
 */
shr_error_e dnx_sch_flow_is_slow_enable_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_slow_enable);

/*
 * \brief - Configure slow rate profile
 */
shr_error_e dnx_sch_flow_shaper_slow_profile_set(
    int unit,
    int core_id,
    int flow_id,
    int slow_profile);

/*
 * \brief - Get slow rate profile
 */
shr_error_e dnx_sch_flow_shaper_slow_profile_get(
    int unit,
    int core_id,
    int flow_id,
    int *slow_profile);

/*
 * \brief - Configure slow rate of slow profile
 */
shr_error_e dnx_sch_slow_profile_rate_hw_set(
    int unit,
    int core_id,
    int slow_profile,
    int slow_level,
    uint32 slow_rate_man,
    uint32 slow_rate_exp,
    uint32 max_bucket);

/*
 * \brief - Get slow rate of slow profile
 */
shr_error_e dnx_sch_slow_profile_rate_hw_get(
    int unit,
    int core_id,
    int slow_profile,
    int slow_level,
    uint32 *slow_rate_man,
    uint32 *slow_rate_exp,
    uint32 *max_bucket);

/**
 * \brief - Configure remote FAP id for this flow
 */
shr_error_e dnx_sch_flow_fap_mapping_range_set(
    int unit,
    int core_id,
    int octet_id,
    int nof_octets,
    uint32 remote_fap_id);

/**
 * \brief -
 * Get  remote FAP id for this flow
 */
shr_error_e dnx_sch_flow_fap_mapping_get(
    int unit,
    int core_id,
    int octet_id,
    uint32 *remote_fap_id);

/**
 * \brief - Configure base remote queue for this flow
 */
shr_error_e dnx_sch_flow_queue_mapping_range_set(
    int unit,
    int core_id,
    int flow_quartet_id,
    int nof_quartets,
    uint32 queue_quad);

/**
 * \brief -
 * Get base remote queue for this flow
 */
shr_error_e dnx_sch_flow_queue_mapping_get(
    int unit,
    int core_id,
    int flow_quartet_id,
    uint32 *queue_quad);

/**
 * \brief - 
 * is Scheduler Element behaves as a scheduler
 */
shr_error_e dnx_sch_se_is_scheduler_get(
    int unit,
    int core_id,
    int se_id,
    int *is_scheduler);

/**
 * \brief - 
 * configure all Scheduler Elements in the range to be / not to be scheduler
 */
shr_error_e dnx_sch_se_is_scheduler_set_range(
    int unit,
    int core_id,
    int se_id_start,
    int se_id_end,
    int is_scheduler);

/**
 * \brief - 
 * Configure cl class mapping
 */
shr_error_e dnx_sch_cl_mapping_set(
    int unit,
    int core_id,
    int cl_id,
    int cl_profile);

/**
 * \brief - 
 * Obtain cl class mapping
 */
shr_error_e dnx_sch_cl_mapping_get(
    int unit,
    int core_id,
    int cl_id,
    int *cl_profile);

/**
 * \brief - 
 * Configure cl parameters
 */
shr_error_e dnx_sch_cl_config_set(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_config_t * cl_config);

/**
 * \brief - 
 * Obtain cl parameters
 */
shr_error_e dnx_sch_cl_config_get(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_config_t * cl_config);

/**
 * \brief - 
 * Configure cl descrete weights
 */
shr_error_e dnx_sch_cl_weight_set(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_weight_t * cl_weight);

/**
 * \brief - 
 * Obtain cl descrete weights
 */
shr_error_e dnx_sch_cl_weight_get(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_weight_t * cl_weight);

/**
 * \brief - 
 * Configure Color group of the scheduling element
 */
shr_error_e dnx_sch_se_color_group_set(
    int unit,
    int core_id,
    int se_id,
    int group);

/**
 * \brief - 
 * Get Color group of the scheduling element
 */
shr_error_e dnx_sch_se_color_group_get(
    int unit,
    int core_id,
    int se_id,
    int *group);

/**
 * \brief - Configure TC to TCG mapping
 */
shr_error_e dnx_sch_port_tc_to_tcg_map_set(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int tcg);

/**
 * \brief - Get TCG for specific TC
 */
shr_error_e dnx_sch_port_tc_to_tcg_map_get(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int *tcg);

/**
 * \brief - Configure is HR used as port HR
 */
shr_error_e dnx_sch_port_hr_enable_range_set(
    int unit,
    int core_id,
    int base_hr_id,
    int nof_hr,
    int is_port);

/**
 * \brief - Get is HR used as port HR
 */
shr_error_e dnx_sch_hr_is_port_get(
    int unit,
    int core_id,
    int hr_id,
    int *is_port);

/**
 * \brief - Configure force FC for given port
 */
shr_error_e dnx_sch_port_hr_force_fc_range_set(
    int unit,
    int core_id,
    int base_hr_id,
    int nof_hr,
    int force_fc);

/**
 * \brief - Configure number of port priorities
 */
shr_error_e dnx_sch_port_priorities_nof_set(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_nof_ps_priorities_e ps_priorities);

/**
 * \brief - Get number of port priorities
 */
shr_error_e dnx_sch_port_priorities_nof_get(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_nof_ps_priorities_e * ps_priorities);

/**
 * \brief - Clear number of port priorities
 */
shr_error_e dnx_sch_port_priorities_nof_clear(
    int unit,
    int core_id,
    int hr_id);

/**
 * \brief - Enable scheduler interface by disabling pause
 */
shr_error_e dnx_sch_interface_enable_set(
    int unit,
    int core_id,
    int interface_id,
    int enable);

/**
 * \brief - Map EGQ interface to scheduler interface
 */
shr_error_e dnx_sch_egq_if_to_sch_if_map_set(
    int unit,
    int core_id,
    int egq_if,
    int sch_if);

/**
 * \brief - UnMap EGQ interface to scheduler interface
 */
shr_error_e dnx_sch_egq_if_to_sch_if_map_clear(
    int unit,
    int core_id,
    int egq_if);

/**
 * \brief - Get SCH interface mapped to given EGQ interface
 */
shr_error_e dnx_sch_egq_if_to_sch_if_map_get(
    int unit,
    int core_id,
    int egq_if,
    int *sch_if);

/**
 * \brief - Configure TCG EIR weight 
 *  
 * @param unit [in] - unit
 * @param core [in] - core
 * @param base_hr [in] - port base_hr 
 * @param tcg [in] - tcg 
 * @param is_valid [in] - is_valid 
 * @param weight [in] - EIR weight to set
 *  
 * \note When setting TCG weight FC must be forced on the port 
 *        in order for the change to take effect. Check
 *        dnx_scheduler_port_tcg_weight_set
 *  
 * * @return shr_error_e 
 */
shr_error_e dnx_sch_port_tcg_weight_set(
    int unit,
    int core,
    int base_hr,
    int tcg,
    int is_valid,
    int weight);

/**
 * \brief - Get TCG EIR weight 
 *  
 * @param unit [in] - unit
 * @param core [in] - core
 * @param base_hr [in] - port base_hr 
 * @param tcg [in] - tcg 
 * @param is_valid [out] - is_valid 
 * @param weight [out] - EIR weight 
 * 
 * @return shr_error_e 
 */
shr_error_e dnx_sch_port_tcg_weight_get(
    int unit,
    int core,
    int base_hr,
    int tcg,
    int *is_valid,
    int *weight);

/**
 * \brief - Configure Port TC bandwidth
 */
shr_error_e dnx_sch_port_tc_bandwidth_set(
    int unit,
    int core_id,
    int hr_id,
    uint32 quanta_to_add);

/**
 * \brief - Get Port TC bandwidth
 */
shr_error_e dnx_sch_port_tc_bandwidth_get(
    int unit,
    int core_id,
    int hr_id,
    uint32 *quanta_to_add);

/**
 * \brief - Configure Port TC max burst
 */
shr_error_e dnx_sch_port_tc_max_burst_set(
    int unit,
    int core_id,
    int hr_id,
    uint32 max_burst);

/**
 * \brief - Get Port TC max burst
 */
shr_error_e dnx_sch_port_tc_max_burst_get(
    int unit,
    int core_id,
    int hr_id,
    uint32 *max_burst);

/**
 * \brief - Reset Port TC max burst to default value
 */
shr_error_e dnx_sch_port_tc_max_burst_reset(
    int unit,
    int core_id,
    int hr_id);

/**
 * \brief - Configure Port TCG bandwidth
 * \note - global_tcg is global TCG id in range 0 - total number of TCGs per core
 */
shr_error_e dnx_sch_port_tcg_bandwidth_set(
    int unit,
    int core_id,
    int global_tcg,
    uint32 quanta_to_add);

/**
 * \brief - Get Port TCG bandwidth
 */
shr_error_e dnx_sch_port_tcg_bandwidth_get(
    int unit,
    int core_id,
    int global_tcg,
    uint32 *quanta_to_add);

/**
 * \brief - Reset Port TCG bandwidth
 */
shr_error_e dnx_sch_port_tcg_bandwidth_reset(
    int unit,
    int core_id,
    int global_tcg);

/**
 * \brief - Configure Port TCG max burst
 */
shr_error_e dnx_sch_port_tcg_max_burst_set(
    int unit,
    int core_id,
    int global_tcg,
    uint32 max_burst);

/**
 * \brief - Get Port TCG max burst
 */
shr_error_e dnx_sch_port_tcg_max_burst_get(
    int unit,
    int core_id,
    int global_tcg,
    uint32 *max_burst);

/**
 * \brief - reset Port TCG max burst to default value
 */
shr_error_e dnx_sch_port_tcg_max_burst_reset(
    int unit,
    int core_id,
    int global_tcg);

/**
 * \brief - Initialize port shapers configuration
 */
shr_error_e dnx_sch_port_shaper_config_init(
    int unit);

/**
 * \brief - Get Port shaper calendar parameters
 */
shr_error_e dnx_sch_port_shaper_calendar_params_get(
    int unit,
    int core_id,
    dnx_sch_port_calendar_e port_calendar,
    uint32 *access_period,
    uint32 *calendar_length);

/**
 * \brief - port shaper credit divider set
 */
shr_error_e dnx_sch_port_shaper_credit_divider_set(
    int unit,
    uint32 credit_worth);

/**
 * \brief - port shaper credit divider get
 */
shr_error_e dnx_sch_port_shaper_credit_divider_get(
    int unit,
    uint32 *credit_worth);

/**
 * \brief - Configure HR mode
 */
shr_error_e dnx_sch_hr_mode_set(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_hr_mode_e hr_mode);

/**
 * \brief - Get HR mode
 */
shr_error_e dnx_sch_hr_mode_get(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_hr_mode_e * hr_mode);

/**
 * \brief - scheduler interface calendar speed set
 */
shr_error_e dnx_sch_interface_calendar_speed_set(
    int unit,
    int core_id,
    int sch_if,
    int nof_interfaces,
    uint32 cal_speed);

/**
 * \brief - scheduler interface calendar speed get
 */
shr_error_e dnx_sch_interface_calendar_speed_get(
    int unit,
    int core_id,
    int sch_if,
    uint32 *cal_speed);

/**
 * \brief - scheduler interface calendar set
 */
shr_error_e dnx_sch_interface_calendar_set(
    int unit,
    int core_id,
    int sch_if,
    int interface_index,
    int select,
    int nof_slots,
    uint32 *slots);

/**
 * \brief - scheduler interface calendar get
 */
shr_error_e dnx_sch_interface_calendar_get(
    int unit,
    int core_id,
    int sch_if,
    int select,
    int nof_slots,
    uint32 *slots);

/**
 * \brief - scheduler interface calendar parameters set
 */
shr_error_e dnx_sch_interface_calendar_params_set(
    int unit,
    int core_id,
    int sch_if,
    int select,
    int nof_slots);

/**
 * \brief - scheduler interface calendar parameters get
 */
shr_error_e dnx_sch_interface_calendar_params_get(
    int unit,
    int core_id,
    int sch_if,
    int *select,
    int *nof_slots);

/**
 * \brief - scheduler interface shaper bandwidth set
 */
shr_error_e dnx_sch_interface_shaper_bandwidth_hw_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 rate);

/**
 * \brief - scheduler interface shaper bandwidth get
 */
shr_error_e dnx_sch_interface_shaper_bandwidth_hw_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 *rate);

/**
 * \brief - map port to interface
 */
shr_error_e dnx_sch_dbal_port_to_interface_map_set(
    int unit,
    int core_id,
    int sch_if,
    int base_hr);

/**
 * \brief - unmap port to interface
 */
shr_error_e dnx_sch_dbal_port_to_interface_map_clear(
    int unit,
    int core_id,
    int sch_if);

/**
 * \brief - set erp enable
 */
shr_error_e dnx_sch_erp_enable_set(
    int unit,
    int core_id,
    uint32 enable);

/**
 * \brief - get erp enable
 */
shr_error_e dnx_sch_erp_enable_get(
    int unit,
    int core_id,
    uint32 *enable);

/**
 * \brief - Configure Global DRM entry
 */
shr_error_e dnx_sch_global_drm_set(
    int unit,
    int rci_level,
    int active_links,
    uint32 rate);

/**
 * \brief - Get Global DRM entry
 */
shr_error_e dnx_sch_global_drm_get(
    int unit,
    int rci_level,
    int active_links,
    uint32 *rate);

/**
 * \brief - Configure Core DRM entry
 */
shr_error_e dnx_sch_core_drm_set(
    int unit,
    int core,
    int rci_level,
    int active_links,
    uint32 rate);

/**
 * \brief - Get Core DRM entry
 */
shr_error_e dnx_sch_core_drm_get(
    int unit,
    int core,
    int rci_level,
    int active_links,
    uint32 *rate);

/**
 * \brief - Configure overwrite slow rate global configuration
 */
shr_error_e dnx_sch_overwrite_slow_rate_set(
    int unit,
    int overwrite_slow_rate);

/**
 * \brief - Reset delete mechanism (dlm)
 */
shr_error_e dnx_sch_dlm_reset_trigger_set(
    int unit);

/**
 * \brief - Configure delete mechanism (dlm) enable for all groups 
 */
shr_error_e dnx_sch_dlm_enable_all_set(
    int unit,
    int enable);

/**
 * \brief - Configure SMP backup message enable global configuration
 */
shr_error_e dnx_sch_backup_msg_enable_set(
    int unit,
    int backup_msg_enable);

/**
 * \brief - Configure shared shaper mode
 */
shr_error_e dnx_sch_shared_shaper_mode_set(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_shared_shaper_mode_e mode);

/**
 * \brief -
 * Map port to base HR
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [in] tm_port -  tm port
 * \param [in] base_hr_id -  mapped base_hr
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_port_to_base_hr_mapping_set(
    int unit,
    int core_id,
    uint32 tm_port,
    int base_hr_id);
/**
 * \brief -
 * Clear DSP to Base HR mapping by setting to default values
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [in] tm_port -  tm port
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_port_to_base_hr_mapping_clear(
    int unit,
    int core_id,
    uint32 tm_port);

/**
 * \brief -
 * Get the mapped base HR to the provided port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [in] tm_port -  tm port
 * \param [out] base_hr_id -  mapped base_hr
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_port_to_base_hr_mapping_get(
    int unit,
    int core_id,
    uint32 tm_port,
    int *base_hr_id);

/**
 * \brief configure LAG port core flip
 */
shr_error_e dnx_sch_dbal_lag_port_member_set(
    int unit,
    int core_id,
    int tm_port,
    int member);

/**
 * \brief configure LAG port core flip
 */
shr_error_e dnx_sch_dbal_lag_port_core_flip_set(
    int unit,
    int core_id,
    int tm_port,
    int core_flip);

/**
 * \brief configure LAG port member
 */
shr_error_e dnx_sch_dbal_dummy_tm_port_set(
    int unit,
    int tm_port);

/**
 * \brief -
 * Set the credit counter configuration
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [in] counter_type - configure what types of credits to count. see dbal_enum_value_field_sch_credit_counter_type_e
 * \param [in] is_group_match - count only credits with group matching to 'group' arg
 * \param [in] is_flow_match - count only credits with flow matching to 'flow' arg
 * \param [in] group - group to count credits for (if is_group_match is set)
 * \param [in] flow - flow to count credits for (if is_flow_match is set)
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_credit_counter_config_set(
    int unit,
    int core_id,
    dbal_enum_value_field_sch_credit_counter_type_e counter_type,
    uint32 is_group_match,
    uint32 is_flow_match,
    uint32 group,
    uint32 flow);

/**
 * \brief -
 * Get the credit counter configuration
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [out] counter_type - configure what types of credits to count. see dbal_enum_value_field_sch_credit_counter_type_e
 * \param [out] is_group_match - count only credits with group matching to 'group' arg
 * \param [out] is_flow_match - count only credits with flow matching to 'flow' arg
 * \param [out] group - group to count credits for (if is_group_match is set)
 * \param [out] flow - flow to count credits for (if is_flow_match is set)
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_credit_counter_config_get(
    int unit,
    int core_id,
    dbal_enum_value_field_sch_credit_counter_type_e * counter_type,
    uint32 *is_group_match,
    uint32 *is_flow_match,
    uint32 *group,
    uint32 *flow);

/**
 * \brief -
 * Get the credit counter value
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core_id -  relevant core
 * \param [out] counter_val - credit counter value
 * \param [out] is_ovf - indication if counter overflowed
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 */
shr_error_e dnx_sch_dbal_credit_counter_get(
    int unit,
    int core_id,
    uint32 *counter_val,
    uint32 *is_ovf);

/**
 *
 * \brief -Function to set the credit delete mechanism configurations to a special values
 * Please make sure this fix is needed for future devices.
 *
 * \param [in] unit - relevant unit
 *
 *
 * \return shr_error_e
 */
shr_error_e dnx_sch_dbal_dlm_init(
    int unit);

/**
 * \brief configure q-pair to HR mapping
 */
shr_error_e dnx_sch_dbal_qpair_to_hr_mapping_set(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair,
    int hr_id,
    int valid);

/**
 * \brief clear q-pair to HR mapping
 */
shr_error_e dnx_sch_dbal_qpair_to_hr_mapping_clear(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair);

/**
 * \brief get q-pair to HR mapping
 */
shr_error_e dnx_sch_dbal_qpair_to_hr_mapping_get(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair,
    int *hr_id,
    int *valid);

/**
 * \brief - Configure qpm override global configuration
 */
shr_error_e dnx_sch_dbal_qpm_override_set(
    int unit,
    int qpm_override);

/**
 * \brief configure low rate connectors range
 */
shr_error_e dnx_sch_dbal_low_rate_range_set(
    int unit,
    int core_id,
    int flow_min,
    int flow_max,
    int enable);

/**
 * \brief get low rate connectors range
 */
shr_error_e dnx_sch_dbal_low_rate_range_get(
    int unit,
    int core_id,
    int *flow_min,
    int *flow_max,
    uint32 *enable);

/**
 * \brief configure double low rate
 *
 * \param [in] unit - Unit ID
 * \param [in] factor - low rate factor
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_dbal_low_rate_factor_set(
    int unit,
    dbal_enum_value_field_low_rate_factor_e factor);

/**
 * \brief - get  low rate factor
 *
 * \param [in] unit - Unit ID
 * \param [out] factor - factor
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_scheduler_dbal_low_rate_factor_get(
    int unit,
    dbal_enum_value_field_low_rate_factor_e * factor);

/**
 * \brief - Configure priority propagation enable
 */
shr_error_e dnx_sch_dbal_priority_propagation_enable_set(
    int unit,
    int priority_propagation_enable);

/**
 * \brief - Configure TCG strict priority mode
 */
shr_error_e dnx_sch_dbal_tcg_strict_priority_set(
    int unit,
    int tcg_strict_priority);

/**
 * \brief - Get TCG strict priority mode
 */
shr_error_e dnx_sch_dbal_tcg_strict_priority_get(
    int unit,
    int *tcg_strict_priority);

/**
 * \brief -
 * configure device scheduler
 */
shr_error_e dnx_sch_dbal_device_scheduler_config_set(
    int unit,
    int mode,
    int weight);

/**
 * \brief - Configure interface DVS priority
 */
shr_error_e dnx_sch_dbal_interface_priority_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 priority);

/**
 * \brief - get interface DVS priority
 */
shr_error_e dnx_sch_dbal_interface_priority_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 *priority);

/**
 * \brief - Clear interface DVS priority
 */
shr_error_e dnx_sch_dbal_interface_priority_clear(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if);

/**
 * \brief - Map scheduler interface to EGQ interface (in reverse mode)
 */
shr_error_e dnx_sch_dbal_sch_if_to_egq_if_map_set(
    int unit,
    int core_id,
    int sch_if,
    int egq_if,
    int is_high_priority);

/**
 * \brief - clear scheduler interface to EGQ interface mapping (in reverse mode)
 */
shr_error_e dnx_sch_dbal_sch_if_to_egq_if_map_clear(
    int unit,
    int core_id,
    int sch_if);

/**
 * \brief - Get scheduler interface to EGQ interface mapping (in reverse mode)
 */
shr_error_e dnx_sch_dbal_sch_if_to_egq_if_map_get(
    int unit,
    int core_id,
    int sch_if,
    int *egq_if,
    int *is_high_priority);

/**
 * \brief - Configure number of priorities for port priority propagation
 */
shr_error_e dnx_sch_dbal_priority_propagation_priorities_nof_set(
    int unit,
    int core_id,
    int sch_if,
    dbal_enum_value_field_nof_ps_priorities_e priority_propagation_priorities);

/**
 * \brief - Get number of priorities for port priority propagation
 */
shr_error_e dnx_sch_dbal_priority_propagation_priorities_nof_get(
    int unit,
    int core_id,
    int sch_if,
    dbal_enum_value_field_nof_ps_priorities_e * priority_propagation_priorities);

/**
 * \brief - Clear number of priorities for port priority propagation
 */
shr_error_e dnx_sch_dbal_priority_propagation_priorities_nof_clear(
    int unit,
    int core_id,
    int sch_if);

/**
 * \brief - Configure port priority propagation enable
 */
shr_error_e dnx_sch_dbal_port_priority_propagation_enable_set(
    int unit,
    int core_id,
    int hr_id,
    int port_priority_propagation_enable);

/**
 * \brief - Get port priority propagation enable
 */
shr_error_e dnx_sch_dbal_port_priority_propagation_enable_get(
    int unit,
    int core_id,
    int hr_id,
    int *port_priority_propagation_enable);

/**
 * \brief - Enable SCH CALRX and RXI blocks
 */
shr_error_e dnx_sch_calrx_rxi_enable_set(
    int unit,
    int enable);

#endif /** _DNX_SCHEDULER_DBAL_H_INCLUDED_ */
