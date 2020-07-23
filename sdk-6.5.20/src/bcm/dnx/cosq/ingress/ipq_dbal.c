/*
 * 
 * ipq_dbal.c
 *
 *  Created on: May 17, 2018
 *      Author: si888124
 *
 * DBAL access functions for Ingress Packet Queuing module.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include "ipq_dbal.h"
/*
 * }
 */

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_fmq_range_get(
    int unit,
    dnx_cosq_ipq_dbal_fmq_range_t * fmq_range)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_FMQ_ENHANCED_RANGE, &entry_handle_id));

    /*
     * Get first queue in FMQ range
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIRST_QUEUE_IN_RANGE, INST_SINGLE,
                               &fmq_range->first_queue);

    /*
     * Get last queue in FMQ range
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_IN_RANGE, INST_SINGLE,
                               &fmq_range->last_queue);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_fmq_range_set(
    int unit,
    dnx_cosq_ipq_dbal_fmq_range_t * fmq_range)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_FMQ_ENHANCED_RANGE, &entry_handle_id));

    /*
     * Set first queue in FMQ range
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_QUEUE_IN_RANGE, INST_SINGLE,
                                 fmq_range->first_queue);

    /*
     * Set last queue in FMQ range
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_IN_RANGE, INST_SINGLE,
                                 fmq_range->last_queue);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_vsq_categories_get(
    int unit,
    dnx_cosq_ipq_dbal_vsq_categories_t * vsq_categories)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_VSQ_CATEGORY_RANGES, &entry_handle_id));

    /*
     * Get last queue in VSQ category 0
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_0, INST_SINGLE,
                               &vsq_categories->last_queue_vsq_category_0);

    /*
     * Get last queue in VSQ category 1
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_1, INST_SINGLE,
                               &vsq_categories->last_queue_vsq_category_1);

    /*
     * Get last queue in VSQ category 2
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_2, INST_SINGLE,
                               &vsq_categories->last_queue_vsq_category_2);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_vsq_categories_set(
    int unit,
    dnx_cosq_ipq_dbal_vsq_categories_t * vsq_categories)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_VSQ_CATEGORY_RANGES, &entry_handle_id));

    /*
     * Set last queue in VSQ category 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_0, INST_SINGLE,
                                 vsq_categories->last_queue_vsq_category_0);

    /*
     * Set last queue in VSQ category 1
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_1, INST_SINGLE,
                                 vsq_categories->last_queue_vsq_category_1);

    /*
     * Set last queue in VSQ category 2
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_QUEUE_VSQ_CATEGORY_2, INST_SINGLE,
                                 vsq_categories->last_queue_vsq_category_2);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_region_is_interdigitated_get(
    int unit,
    uint32 queue_region,
    uint32 *is_interdigitated)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_REGION_INTERDIGITATED_MODE, &entry_handle_id));

    /*
     * Set Queue Region
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REGION_INDEX, queue_region);

    /*
     * Get is interdigitated
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_INTERDIGITATED_MODE, INST_SINGLE,
                               is_interdigitated);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_region_is_interdigitated_set(
    int unit,
    uint32 queue_region,
    uint32 is_interdigitated)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_REGION_INTERDIGITATED_MODE, &entry_handle_id));

    /*
     * Set Queue Region
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REGION_INDEX, queue_region);

    /*
     * Set is interdigitated
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_INTERDIGITATED_MODE, INST_SINGLE,
                                 is_interdigitated);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_range_set(
    int unit,
    int core_id,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 system_port)
{
    uint32 entry_handle_id;
    uint32 last_queue_quartet;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_SYSTEM_PORT_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /*
     * Set Queue quartet
     */
    last_queue_quartet = first_queue_quartet + nof_quartets_to_set - 1;
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, first_queue_quartet,
                                     last_queue_quartet);

    /*
     * Set system port
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, INST_SINGLE, system_port);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_SYSTEM_PORT_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set Queue quartet
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, queue_quartet);

    /*
     * Get system port
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, INST_SINGLE, system_port);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_set(
    int unit,
    uint32 queue_quartet,
    uint32 system_port)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_range_set
                    (unit, BCM_CORE_ALL, queue_quartet, 1, system_port));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - clear function for the DBAL table IPQ_QUARTET_TO_SYSTEM_PORT_MAP.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_clear(
    int unit,
    int core,
    uint32 queue_quartet)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_SYSTEM_PORT_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set Queue quartet range
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, queue_quartet);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - generic set function for the DBAL table IPQ_QUARTET_TO_FLOW_ID_MAP.
 * this allows for multiple set of a specific field
 */
static shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_set(
    int unit,
    int core,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    dbal_fields_e dbal_field,
    uint32 value)
{
    uint32 entry_handle_id;
    uint32 last_queue_quartet;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_FLOW_ID_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set Queue quartet
     */
    last_queue_quartet = first_queue_quartet + nof_quartets_to_set - 1;
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, first_queue_quartet,
                                     last_queue_quartet);

    /*
     * Set dbal field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field, INST_SINGLE, value);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - generic get function for the DBAL table IPQ_QUARTET_TO_FLOW_ID_MAP.
 */
static shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_get(
    int unit,
    int core,
    uint32 queue_quartet,
    dbal_fields_e dbal_field,
    uint32 *value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_FLOW_ID_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set Queue quartet
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, queue_quartet);

    /*
     * Get dbal field
     */
    dbal_value_field32_request(unit, entry_handle_id, dbal_field, INST_SINGLE, value);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_range_set(
    int unit,
    int core,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 flow_quartet_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_set
                    (unit, core, first_queue_quartet, nof_quartets_to_set, DBAL_FIELD_FLOW_QUARTET,
                     flow_quartet_index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_range_set(
    int unit,
    int core,
    uint32 first_queue_quartet,
    uint32 nof_quartets_to_set,
    uint32 is_composite)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_set
                    (unit, core, first_queue_quartet, nof_quartets_to_set, DBAL_FIELD_IS_COMPOSITE, is_composite));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *flow_quartet_index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_get
                    (unit, core, queue_quartet, DBAL_FIELD_FLOW_QUARTET, flow_quartet_index));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_flow_id_set(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 flow_quartet_index)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_set
                    (unit, core, queue_quartet, 1, DBAL_FIELD_FLOW_QUARTET, flow_quartet_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_get(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 *is_composite)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_get
                    (unit, core, queue_quartet, DBAL_FIELD_IS_COMPOSITE, is_composite));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_is_composite_set(
    int unit,
    int core,
    uint32 queue_quartet,
    uint32 flow_quartet_index)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_generic_set
                    (unit, core, queue_quartet, 1, DBAL_FIELD_IS_COMPOSITE, flow_quartet_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - clear function for the DBAL table IPQ_QUARTET_TO_FLOW_ID_MAP.
 */
shr_error_e
dnx_cosq_ipq_dbal_queue_quartet_to_flow_map_clear(
    int unit,
    int core,
    uint32 queue_quartet)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_QUARTET_TO_FLOW_ID_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set Queue quartet range
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUARTET, queue_quartet);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Get system port to VOQ base info
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_get(
    int unit,
    int core,
    uint32 system_port,
    uint32 *base_voq,
    uint32 *is_valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /*
     * Get dbal fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VOQ_BASE, INST_SINGLE, base_voq);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_VALID, INST_SINGLE, is_valid);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Set system port to VOQ base info
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_set(
    int unit,
    int core,
    uint32 system_port,
    uint32 base_voq,
    uint32 is_valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /*
     * Set dbal fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_BASE, INST_SINGLE, base_voq);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_VALID, INST_SINGLE, is_valid);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Enable/Disable system port to VOQ base
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_valid_set(
    int unit,
    int core,
    uint32 system_port,
    uint32 is_valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /*
     * Set dbal fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_VALID, INST_SINGLE, is_valid);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Get system port to VOQ base info
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_get(
    int unit,
    uint32 system_port,
    uint32 *profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /*
     * Get dbal fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, INST_SINGLE, profile);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Set system port to VOQ base info
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_tc_profile_set(
    int unit,
    uint32 system_port,
    uint32 profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);

    /*
     * Set dbal fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, INST_SINGLE, profile);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * brief - clear function for the DBAL table IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP.
 */
shr_error_e
dnx_cosq_ipq_dbal_system_port_to_voq_base_map_clear(
    int unit,
    int core,
    uint32 system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_SYSTEM_PORT_TO_QUEUE_BASE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set system port
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT, system_port);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_ipq_dbal_tc_to_voq_offset_map_set(
    int unit,
    int core,
    uint32 tc,
    uint32 dp,
    uint32 profile,
    uint32 is_flow,
    uint32 is_fmq,
    uint32 voq_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_TRAFFIC_CLASS_TO_VOQ_OFFSET_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    if (!is_flow)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    }
    /*
     * Set TC
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    /*
     * Set TC profile
     */
    if (is_flow)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_PROFILE, profile);
    }
    else if (is_fmq)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_MC, 0);
    }
    else
    {
        /** system port*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSPORT_PROFILE, profile);
    }
    
    if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_tc_profile_from_tm_command))
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE_FROM_TM_COMMAND, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
    }
    if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    }

    /*
     * Set VOQ offset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_OFFSET, INST_SINGLE, voq_offset);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_ipq_dbal_tc_to_voq_offset_map_get(
    int unit,
    int core,
    uint32 tc,
    uint32 dp,
    uint32 profile,
    uint32 is_flow,
    uint32 is_fmq,
    uint32 *voq_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_TRAFFIC_CLASS_TO_VOQ_OFFSET_MAP, &entry_handle_id));
    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set TC
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    /*
     * Set TC profile
     */
    if (is_flow)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_PROFILE, profile);
    }
    else if (is_fmq)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_MC, 0);
    }
    else
    {
        /** system port*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSPORT_PROFILE, profile);
    }
    
    if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_tc_profile_from_tm_command))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE_FROM_TM_COMMAND, 0);
    }
    if (dnx_data_ipq.queues.feature_get(unit, dnx_data_ipq_queues_voq_resolution_per_dp))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    }
    /*
     * Get VOQ offset
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VOQ_OFFSET, INST_SINGLE, voq_offset);
    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_set(
    int unit,
    int core,
    uint32 flow_quartet,
    uint32 flow_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_FLOW_QUARTET_TO_FLOW_PROFILE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set flow
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_FLOW_QUARTET, flow_quartet);

    /*
     * Set dbal fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_PROFILE, INST_SINGLE, flow_profile);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_voq_flow_quartet_to_flow_profile_map_get(
    int unit,
    int core,
    uint32 flow_quartet,
    uint32 *flow_profile)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_FLOW_QUARTET_TO_FLOW_PROFILE_MAP, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set flow
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_FLOW_QUARTET, flow_quartet);

    /*
     * Set dbal fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLOW_PROFILE, INST_SINGLE, flow_profile);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_set(
    int unit,
    uint32 tc,
    dbal_enum_value_field_multicast_priority_e mc_prio)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_TC_MULTICAST_PRIORITY_MAP, &entry_handle_id));

    /*
     * Set TC
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

    /*
     * Set dbal fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_PRIO, INST_SINGLE, mc_prio);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_tc_to_multicast_prio_map_get(
    int unit,
    uint32 tc,
    dbal_enum_value_field_multicast_priority_e * mc_prio)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_TC_MULTICAST_PRIORITY_MAP, &entry_handle_id));

    /*
     * Set flow
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

    /*
     * Set dbal fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MC_PRIO, INST_SINGLE, mc_prio);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_invalid_destination_queue_set(
    int unit,
    int core,
    uint32 invalid_dest_queue)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_INVALID_QUEUE, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set invalid queue
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_DEST_QUEUE, INST_SINGLE, invalid_dest_queue);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_cosq_ipq_dbal_invalid_destination_queue_get(
    int unit,
    int core,
    uint32 *invalid_dest_queue)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IPQ_INVALID_QUEUE, &entry_handle_id));

    /*
     * Set the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Get requested info
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INVALID_DEST_QUEUE, INST_SINGLE, invalid_dest_queue);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
