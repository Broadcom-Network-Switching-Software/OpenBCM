/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file flow_lif_mgmt.c
 *
 *  The LIF Management sub-module provides a general mechanism
 *  to address the complexity of In-LIF & Out-LIF allocations
 *  with the appropriate result-type format as well performing
 *  the necessary HW access and Replace operations as part of
 *  the FLOW module.
 *  The sub-module is aimed to service APIs that influence the
 *  table result-type, mainly the creation API, both in create
 *  mode and in replace mode.
 *  The module's main APIs
 *  (dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw
 *  & dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw) for
 *  In-LIFs & Out-LIFs respectively) receive from the FLOW
 *  module a DBAL handler for an already allocated LIF, filled
 *  with the table key and the relevant fields to commit.
 *  One of the main features of the sub-module is the
 *  result-type selection. The sub-module traverses the
 *  result-type formats from short to long and finds the first
 *  format that matches the supplied fields in the DBAL handle.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include "src/bcm/dnx/flow/flow_def.h"
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
#define MAX_NUM_OF_FIELDS_IN_SUPERSET       (32)
#define FIELD_NOT_EXISTS_IN_RT              (-1)
#define MAX_NUM_OF_LOOKUP_TABLES_PER_MODULE (20)

/*
 * Enum defining which type of replace is done on the entry
 */
typedef enum
{
    FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE,
    /** Following enum refers to the case where we have the same size and ll indication is valid: either both lifs need ll, don't need ll, or the old lif required ll and the new one doesn't*/
    FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE,
    FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF,
    FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF
} flow_lif_mgmt_replace_mode_e;

/*
 * Global SW DB for mapping field index from superset to all participating result-types
 */
typedef struct
{
    int8 field_idx_in_result_type[DBAL_MAX_NUMBER_OF_RESULT_TYPES][MAX_NUM_OF_FIELDS_IN_SUPERSET];
}
flow_lif_mgmt_dbal_field_mapping_info_t;

static flow_lif_mgmt_dbal_field_mapping_info_t *flow_dbal_field_mapping[BCM_MAX_NUM_UNITS] = { 0 };

/*
 * Temporary conuters for FLOW performance
 */
#define NOF_FLOW_TIMERS     (20)
char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
uint32 timers_group;
uint32 timer_idx[NOF_FLOW_TIMERS];
#define FLOW_TIMERS_MODE    (0)

#define FLOW_TIMER_SET(timer_desc, flow_timer_idx)  \
if (FLOW_TIMERS_MODE)                               \
{                                                   \
    utilex_ll_timer_set(timer_desc, timers_group, timer_idx[flow_timer_idx]);   \
}

#define FLOW_TIMER_STOP(flow_timer_idx)             \
if (FLOW_TIMERS_MODE)                               \
{                                                   \
    utilex_ll_timer_stop(timers_group, timer_idx[flow_timer_idx]);              \
}

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * Declaration of extern functions
 * {
 */
extern shr_error_e dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id);

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

extern shr_error_e dnx_vlan_port_match_inlif_update(
    int unit,
    dbal_tables_e inlif_dbal_table_id,
    uint32 old_local_in_lif_id,
    uint32 new_local_in_lif_id);

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

static shr_error_e dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 table_specific_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint32 *selected_result_type,
    uint8 is_test);

/**
 * \brief - Print debug info in case result type wasn't found.
 * Print options for fields that can be omitted to get valid result
 *
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle
 *   \param [in] dbal_table_id - table
 *   \param [in] table_specific_flags - specific flags requested by the user
 *   \param [in] logical_phase - Out-LIF logical phase
 *   \param [in] entry_handle_id - original handle id containing the fields to set
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_no_result_type_found_debug(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dbal_tables_e dbal_table_id,
    uint32 table_specific_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id)
{
    int field_idx, field_idx_to_ignore;
    shr_error_e rv;
    CONST dbal_logical_table_t *table;
    uint8 hint_found = FALSE;
    char str_tmp[256];
    char msg[10000] = { 0 };
    size_t strn_size = 256;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(str_tmp, 0, sizeof(str_tmp));
    sal_strncat_s(msg, "\n****************** LIF TABLE MANAGER DEBUG INFO ******************\n", sizeof(msg));
    sal_snprintf(str_tmp, strn_size, "COULD NOT FIND RESULT TYPE IN TABLE %s CONTAINING THE FOLLOWING FIELDS: \r\n",
                 dbal_logical_table_to_string(unit, dbal_table_id));
    sal_strncat_s(msg, str_tmp, sizeof(msg));

    /** Get table information */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all required fields */
    for (field_idx = 0; field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id = table->multi_res_info[table->nof_result_types].results_info[field_idx].field_id;

        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            /** No need to print result type field */
            continue;
        }

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            uint32 field_val_nof_bits;

            /** This field is required */
            SHR_IF_ERR_EXIT(utilex_bitstream_size_of_bitstream_in_bits
                            (field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS, (uint32 *) &field_val_nof_bits));

            sal_memset(str_tmp, 0, sizeof(str_tmp));
            sal_snprintf(str_tmp, strn_size, "%s: size %d \r\n", dbal_field_to_string(unit, field_id),
                         field_val_nof_bits);
            sal_strncat_s(msg, str_tmp, sizeof(msg));
        }

    }

    sal_memset(str_tmp, 0, sizeof(str_tmp));
    sal_snprintf(str_tmp, strn_size, "\nTable specific flags value: %d \r\n\n", table_specific_flags);
    sal_strncat_s(msg, str_tmp, sizeof(msg));

    /*
     * Trying to omit one field to see if a result can be found
     */
    for (field_idx_to_ignore = 0;
         field_idx_to_ignore < table->multi_res_info[table->nof_result_types].nof_result_fields; field_idx_to_ignore++)
    {
        dbal_fields_e field_id_to_ignore;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id_to_ignore = table->multi_res_info[table->nof_result_types].results_info[field_idx_to_ignore].field_id;

        if (field_id_to_ignore == DBAL_FIELD_RESULT_TYPE)
        {
            /** No need to print result type field */
            continue;
        }

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id_to_ignore, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /*
             * Try omitting this field and find a result type
             */
            uint32 result_type_decision;
            uint32 test_entry_handle_id;

            /*
             * Copy all fields beside the chosen one to a new handle
             */
            DBAL_HANDLE_COPY(unit, entry_handle_id, &test_entry_handle_id);
            dbal_entry_value_field_unset(unit, test_entry_handle_id, field_id_to_ignore, INST_SINGLE);

            /*
             * Try choosing result type
             */
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                            (unit, flow_handle_info, table_specific_flags, logical_phase,
                             test_entry_handle_id, &result_type_decision, TRUE));
            if (result_type_decision != FLOW_LIF_MGMT_INVALID_RESULT_TYPE)
            {
                if (hint_found == FALSE)
                {
                    sal_strncat_s(msg, "HINT: One of these fields can be omitted to receive a successful result: \r\n",
                                  sizeof(msg));
                    hint_found = TRUE;
                }
                sal_strncat_s(msg, dbal_field_to_string(unit, field_id_to_ignore), sizeof(msg));
                sal_strncat_s(msg, "\r\n", sizeof(msg));
            }
            DBAL_HANDLE_FREE(unit, test_entry_handle_id);
        }
    }
    if (hint_found == FALSE)
    {
        sal_strncat_s(msg,
                      "HINT: Couldn't find a single field that can be omitted to receive a successful result. \r\n",
                      sizeof(msg));
    }
    sal_strncat_s(msg, "HINT: Make sure all the mandatory fields are configured. \r\n", sizeof(msg));
    sal_strncat_s(msg, "\n****************** END OF LIF TABLE MANAGER DEBUG INFO ******************\n", sizeof(msg));

    LOG_WARN_EX(BSL_LOG_MODULE, "%s %s%s%s\r\n", msg, EMPTY, EMPTY, EMPTY);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Lookup a field_id in the superset, assuming it's a sub-field and the superset has the Parent 
 */
static shr_error_e
dnx_flow_lif_mgmt_parent_in_superset_get(
    int unit,
    dbal_fields_e sub_field_id,
    multi_res_info_t * superset_info,
    int *superset_parent_idx)
{
    int superset_field_idx;
    uint8 is_sub_field_of_parent;
    dbal_fields_e superset_parent_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse the Parent superset fields in order to find the sub-field from the result type.
     * The Parent fields are always at the start of the superset, after the result-type field
     */
    for (superset_field_idx = 1; superset_field_idx < superset_info->nof_result_fields; superset_field_idx++)
    {
        if (SHR_BITGET
            (superset_info->results_info[superset_field_idx].field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD))
        {
            superset_parent_field_id = superset_info->results_info[superset_field_idx].field_id;

            /*
             * If Parent and sub-field relations were found - exit with the parent field index in the superset
             */
            SHR_IF_ERR_EXIT(dbal_fields_sub_field_match
                            (unit, superset_parent_field_id, sub_field_id, &is_sub_field_of_parent));
            if (is_sub_field_of_parent)
            {
                *superset_parent_idx = superset_field_idx;
                break;
            }
        }
        else
        {
            /*
             * The Parents at the start of the superset have all been traversed - No match found
             */
            return _SHR_E_NOT_FOUND;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Allocate and Initialize the FLOW-LIF-Mgmt field mapping 
 * See .h file  
 */
shr_error_e
dnx_flow_lif_mgmt_init(
    int unit)
{
    int flow_number_of_apps, mapping_db_size;
    int field_idx, superset_field_idx, result_type_idx, flow_app_idx;
    multi_res_info_t *superset_info;
    dbal_tables_e flow_dbal_table;
    dbal_fields_e field_id, superset_field_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flow_number_of_apps = dnx_flow_number_of_apps();

    mapping_db_size = sizeof(flow_lif_mgmt_dbal_field_mapping_info_t) * flow_number_of_apps;
    SHR_ALLOC(flow_dbal_field_mapping[unit], mapping_db_size, "flow_dbal_field_mapping_info_t", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);
    sal_memset(flow_dbal_field_mapping[unit], FIELD_NOT_EXISTS_IN_RT, mapping_db_size);

    /*
     * Perform mapping for all the FLOW Apps that have LIFs (Not of Match type)
     */
    for (flow_app_idx = FLOW_APP_FIRST_IDX; flow_app_idx < flow_number_of_apps; flow_app_idx++)
    {
        dbal_logical_table_t *table;
        flow_lif_mgmt_dbal_field_mapping_info_t *flow_app_field_mapping;

        /*
         * Skip the handling of FLOW match applications
         */
        const dnx_flow_app_config_t *flow_app_info = dnx_flow_app_info_get(unit, flow_app_idx);
        if (flow_app_info->flow_app_type == FLOW_APP_TYPE_MATCH)
        {
            continue;
        }

        /*
         * Prepare the information per FLOW 
         * Decrement the FLOW index by one as index 0 isn't used
         */
        flow_dbal_table = flow_app_info->flow_table;
        flow_app_field_mapping = &(flow_dbal_field_mapping[unit][flow_app_idx - 1]);
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, flow_dbal_table, &table));
        superset_info = &(table->multi_res_info[table->nof_result_types]);

        /*
         * Traverse the result-types to find the field mapping for each of them 
         * Start after the result-type field that is always the first field (0) 
         */
        for (result_type_idx = 0; result_type_idx < table->nof_result_types; result_type_idx++)
        {
            multi_res_info_t *result_type_info = &(table->multi_res_info[result_type_idx]);

            /*
             * Verify that the defined maximum number of fields per result type isn't reached
             */
            if (result_type_info->nof_result_fields > MAX_NUM_OF_FIELDS_IN_SUPERSET)
            {
                SHR_ERR_EXIT(_SHR_E_INIT,
                             "Error: Num of fields in a result type %s of table %s exceeds the defined max of %d",
                             dbal_result_type_to_string(unit, flow_dbal_table, result_type_idx),
                             dbal_logical_table_to_string(unit, flow_dbal_table), MAX_NUM_OF_FIELDS_IN_SUPERSET);
            }

            /*
             * Traverse the fields in each result-type and attemp to find them in the superset 
             * Start after the result-type field that is always the first field (0)
             */
            for (field_idx = 1; field_idx < result_type_info->nof_result_fields; field_idx++)
            {
                field_id = result_type_info->results_info[field_idx].field_id;

                /*
                 * Traverse the superset fields in order to find the field from the result type
                 * Start after the result-type field that is always the first field (0)
                 */
                for (superset_field_idx = 1; superset_field_idx < superset_info->nof_result_fields;
                     superset_field_idx++)
                {
                    superset_field_id = superset_info->results_info[superset_field_idx].field_id;

                    /*
                     * Found the field's location in the superset - Don't look any further
                     */
                    if (superset_field_id == field_id)
                    {
                        break;
                    }
                }

                /*
                 * If the field wasn't found in the superset, either it's a subfield and we need to look 
                 * for the parent in the superset, or something is wrong with the superset definition 
                 */
                if (superset_field_idx >= superset_info->nof_result_fields)
                {
                    /*
                     * Lookup the field_id by checking the sub-fields of the parent fields in the superset
                     */
                    rv = dnx_flow_lif_mgmt_parent_in_superset_get(unit, field_id, superset_info, &superset_field_idx);
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Error: field %s couldn't be found in superset of table %s",
                                     dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit,
                                                                                                        flow_dbal_table));
                    }
                }

                /*
                 * Found the field's location in the superset - Store it in the mapping DB
                 */
                flow_app_field_mapping->field_idx_in_result_type[result_type_idx][superset_field_idx] = field_idx;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Deallocate the FLOW-LIF-Mgmt field mapping 
 * See .h file  
 */
shr_error_e
dnx_flow_lif_mgmt_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_dbal_field_mapping[unit] != NULL)
    {
        SHR_FREE(flow_dbal_field_mapping[unit]);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the field index in a result type based on
 *        its index in a superset entry by retrieving the value
 *        for the mapping DB from superset to result-type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] flow_handle - FLOW handle
 *   \param [in] result_type - Result type index
 *   \param [in] superset_field_idx - the field's index in the
 *          array of field IDs for the superset
 * \par DIRECT OUTPUT:
 *   \param [out] field_idx - Returned field's index in the
 *          array of field IDs of the givwn result type
 *   \param [out] is_field_exists - Returned indication whether
 *          the field ID is valid in the specified result type
 * \see
 *   dnx_flow_init
 */
static void
dnx_flow_lif_mgmt_is_field_in_result_type(
    int unit,
    uint32 flow_handle,
    uint32 result_type,
    int superset_field_idx,
    int *field_idx,
    uint8 *is_field_exists)
{
    /*
     * Find the field index in the result type and check whether it's valid. 
     * Decrement the FLOW index by one as index 0 isn't used
     */
    *field_idx =
        flow_dbal_field_mapping[unit][flow_handle - 1].field_idx_in_result_type[result_type][superset_field_idx];
    if (*field_idx == FIELD_NOT_EXISTS_IN_RT)
    {
        *is_field_exists = FALSE;
    }
    else
    {
        *is_field_exists = TRUE;

    }
}

/**
 * \brief - Find best result type a for given set of fields part
 *        of the FLOW sequence.
 * Create a bitmap of available result types according to
 *   table_specific_flags
 * For each result type X (starting from smaller and increasing)
 * For each input field Y check if field Y exists in result type
 * X.
 *   If no - continue to next result type
 *   If yes - check the size
 *     of the field If the size is big enough continue to the
 *     next field Else continue to next result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle
 *   \param [in] table_specific_flags - lif table specific information
 *   \param [in] logical_phase - Out-LIF logical phase
 *   \param [in] entry_handle_id - DBAL handle id containing all the fields
 *   \param [out] selected_result_type - selected result type
 *          for the current lif
 *   \param [in] is_test - Indication whether to run in test
 *          mode
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 table_specific_flags,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint32 *selected_result_type,
    uint8 is_test)
{
    int superset_field_idx, field_idx, result_type_iterator;
    lif_table_mngr_table_specific_rules_t table_specific_rules;
    shr_error_e rv;
    uint8 valid_result_type = FALSE;
    CONST dbal_logical_table_t *table;
    uint32 result_type_idx;
    dbal_tables_e dbal_table_id;
    lif_table_mngr_table_info_t sw_state_table_info;
    uint8 found;
    dbal_entry_handle_t *in_entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &in_entry_handle));

    dbal_table_id = in_entry_handle->table_id;
    table = in_entry_handle->table;

    /** Use table specific filtering of result types for tables where additional rules should be applied */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_result_type_filtering
                    (unit, entry_handle_id, table_specific_flags, dbal_table_id, &table_specific_rules));

    /*
     * Retrieve the sorted result types array for the DBAL table
     */
    SHR_IF_ERR_EXIT(lif_table_mngr_db.
                    lif_table_info_htb.find(unit, (int *) &dbal_table_id, &sw_state_table_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error: Failed to retrieve LIF Table manager sorted result types array for DBAL table %s",
                     dbal_logical_table_to_string(unit, dbal_table_id));
    }

    /*
     * Iterate over all possible result types
     */
    for (result_type_iterator = 0; result_type_iterator < table->nof_result_types; result_type_iterator++)
    {
        /*
         * The entry number result_type_iterator in the array will give us the next in line result type,
         * beginning with the smallest and increasing.
         */
        result_type_idx = sw_state_table_info.result_type[result_type_iterator];

        /**If this result type is disabled - move to the next one*/
        if (table->multi_res_info[result_type_idx].is_disabled)
        {
            continue;
        }

        /** If the result type is allowed for this lif, go on */
        if (SHR_BITGET(&(table_specific_rules.allowed_result_types_bmp), result_type_idx))
        {
            valid_result_type = TRUE;
            superset_field_idx = 1;

            /*
             * Loop on all the fields by checking all possible fields in  the superset result type. 
             * Start after the result-type field that is always the first field (0) 
             */
            while ((valid_result_type == TRUE)
                   && (superset_field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields))
            {
                dbal_table_field_info_t *table_field_info;
                dbal_fields_e superset_field_id;
                uint8 is_field_exists;
                dbal_fields_e field_id;

                /** Get the next superset field */
                superset_field_id =
                    table->multi_res_info[table->nof_result_types].results_info[superset_field_idx].field_id;

                /*
                 * Check if the field (or it's child) isn't on the handle. 
                 * In which case, perform Next-outlif-pointer validation and continue 
                 * to evaluate the next field. 
                 */
                if (in_entry_handle->value_field_ids[superset_field_idx] == DBAL_FIELD_EMPTY)
                {
                    /**If field is outlif next pointer and user didn't set it - check if it's not mandatory on this result type*/
                    if (superset_field_id == DBAL_FIELD_NEXT_OUTLIF_POINTER)
                    {
                        dbal_to_phase_info_t dbal_to_phase_info = { 0 };
                        dnx_local_outlif_ll_indication_e ll_indication = DNX_LOCAL_OUTLIF_LL_OPTIONAL;
                        uint8 ll_indication_found = FALSE;

                        dbal_to_phase_info.dbal_result_type = result_type_idx;
                        dbal_to_phase_info.dbal_table = dbal_table_id;

                        SHR_IF_ERR_EXIT(local_outlif_info.logical_phase_info.
                                        dbal_valid_combinations.find(unit, logical_phase, &dbal_to_phase_info,
                                                                     &ll_indication, &ll_indication_found));

                        if ((ll_indication_found) && (ll_indication_found == DNX_LOCAL_OUTLIF_LL_ALWAYS))
                        {
                            /**link list is mandatory on this result type, and user didn't set it --> can't use this RT*/
                            valid_result_type = FALSE;
                            break;
                        }
                    }

                    /** This field is not on the handle, ignore it */
                    superset_field_idx++;
                    continue;
                }

                /*
                 * Verify that the field or a child is defined in the currently validated result-type. 
                 * The retrieval is based only on the field_id, so a found child may later on prove to be of an
                 * unwanted child based on the encoded value - This is validated in the following steps.
                 * If defined, retrieve the field_id as defined in the result type and the parent's field's info.
                 */
                dnx_flow_lif_mgmt_is_field_in_result_type(unit, flow_handle_info->flow_handle, result_type_idx,
                                                          superset_field_idx, &field_idx, &is_field_exists);

                /*
                 * If the field was supplied to the handle and the field was found in the validated result type, 
                 * continue checking whether the field value is encoded to suit the field attributes in this 
                 * result type. 
                 */
                if (is_field_exists)
                {
                    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                    /*
                     * Retrieve the field value according to the superset result-type attributes 
                     * A Failure may happen in case the field on the validated result type is a sub-field and 
                     * the field value has a different sub-field encoding
                     */
                    field_id = table->multi_res_info[result_type_idx].results_info[field_idx].field_id;
                    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id,
                                                                 INST_SINGLE, field_val);
                    if (SHR_SUCCESS(rv))
                    {
                        /*
                         * Validate the field value with the specific field attributes in the validated result type 
                         * A failure may happen in case the value has too large size, wrong enum encoding, etc  
                         */
                        table_field_info = &(table->multi_res_info[result_type_idx].results_info[field_idx]);
                        rv = dbal_value_field32_validate(unit, field_val, table_field_info, &valid_result_type);
                        if (SHR_SUCCESS(rv) && valid_result_type)
                        {
                            superset_field_idx++;
                            continue;
                        }
                    }

                    valid_result_type = FALSE;
                }
                /*
                 * If the field wasn't found in this result type, this isn't a valid result type.
                 */
                else
                {
                    valid_result_type = FALSE;
                }
            }
            /*
             * If we got here with a valid result type it means that all the fields on the handle exist in current result type.
             * No need to continue
             */
            if (valid_result_type == TRUE)
            {
                *selected_result_type = result_type_idx;
                break;
            }
        }
    }

    if (valid_result_type == FALSE)
    {
        /*
         * Result type not found - if not test return debug info,
         * else set selected_result_type to invalid
         */
        if (is_test == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_result_type_found_debug
                            (unit, flow_handle_info, dbal_table_id, table_specific_flags, logical_phase,
                             entry_handle_id));
            SHR_ERR_EXIT(_SHR_E_PARAM, "No result type found \r\n");
        }
        else
        {
            *selected_result_type = FLOW_LIF_MGMT_INVALID_RESULT_TYPE;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - the id of the core
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] local_lif_id - Allocated local In-LIF
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw
 */
static shr_error_e
dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw_verify(
    int unit,
    int core_id,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && ((lif_info->global_lif == 0) || (lif_info->global_lif == DNX_ALGO_GPM_LIF_INVALID)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lif must be allocated when calling dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set\n");
    }

    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        lif_table_mngr_inlif_info_t lif_info_existing;
        uint32 entry_handle_id_existing;
        dbal_tables_e dbal_table_id, existing_dbal_table_id;
        dbal_physical_tables_e physical_table_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

        /*
         * Retrieve the DBAL table ID from local In-LIF info and verify that it's the
         * same as the DBAL table that is on the entry handle.
         */
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, *local_lif_id, core_id, physical_table_id, &existing_dbal_table_id, NULL, NULL));
        if (dbal_table_id != existing_dbal_table_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: handler DBAL table (%s) doesn't match the Replaced LIF's (local LIF - 0x%x) DBAL table (%s)\n",
                         dbal_logical_table_to_string(unit, dbal_table_id), *local_lif_id,
                         dbal_logical_table_to_string(unit, existing_dbal_table_id));
        }

        /*
         * Get existing handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_existing));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_inlif_info
                        (unit, core_id, *local_lif_id, entry_handle_id_existing, &lif_info_existing));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] local_lif_id - Allocated local Out-LIF
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
 */
static shr_error_e
dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw_verify(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && ((lif_info->global_lif == 0) || (lif_info->global_lif == DNX_ALGO_GPM_LIF_INVALID)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lif must be allocated when calling dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set");
    }

    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        lif_table_mngr_outlif_info_t lif_info_existing;
        uint32 entry_handle_id_existing;
        dbal_tables_e dbal_table_id, existing_dbal_table_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

        /*
         * Retrieve the DBAL table ID from local Out-LIF info and verify that it's the
         * same as the DBAL table that is on the entry handle.
         */
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, *local_lif_id,
                                                        &existing_dbal_table_id, NULL, NULL, NULL, NULL, NULL));
        if (dbal_table_id != existing_dbal_table_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: handler DBAL table (%s) doesn't match the Replaced LIF's (local LIF - 0x%x) DBAL table (%s)\n",
                         dbal_logical_table_to_string(unit, dbal_table_id), *local_lif_id,
                         dbal_logical_table_to_string(unit, existing_dbal_table_id));
        }

        /*
         * Get existing handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_existing));
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_outlif_info
                        (unit, *local_lif_id, entry_handle_id_existing, &lif_info_existing));
        /*
         * Verify out phase didn't change
         */
        if (lif_info_existing.logical_phase != lif_info->logical_phase)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "logical phase cannot be replaced. Old value: %d, new value: %d \n",
                         lif_info_existing.logical_phase, lif_info->logical_phase);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace existing entry in lif table with new result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] existing_result_type - result type of the current entry
 *   \param [in] new_result_type - new entry result type
 *   \param [in] local_lif_id - new entry local lif
 *   \param [in] entry_reuse - reuse existing entry for new result type

 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_set_hw_replace_result_type(
    int unit,
    int core_id,
    uint8 is_ingress,
    void *lif_info,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 existing_result_type,
    uint32 new_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    uint32 lif_table_entry_handle_id_existing, lif_table_entry_handle_id_new;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;
    uint8 next_pointer_required_by_user;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** We need two handles - one to read existing values and one to fill new entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_existing));
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_new));
    /** Set keys for the two handles */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, existing_result_type, local_lif_id,
                     lif_table_entry_handle_id_existing));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, new_result_type, local_lif_id,
                     lif_table_entry_handle_id_new));

    /*
     * Check if user is asking for next pointer, this info is required by lif allocation manager
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &next_pointer_required_by_user));

    /*
     * In case we replace result type of an entry we must fill all the fields.
     * We will copy all fields that are mutual for the two result types and fill with defaults all the other values.
     */
    /** Get all fields from existing entry */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id_existing, DBAL_GET_ALL_FIELDS));
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_inlif
                        (unit, core_id, dbal_table_id, (lif_table_mngr_inlif_info_t *) lif_info, new_result_type,
                         existing_result_type, local_lif_id, entry_reuse));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_outlif
                        (unit, dbal_table_id, (lif_table_mngr_outlif_info_t *) lif_info,
                         new_result_type, existing_result_type, next_pointer_required_by_user, local_lif_id,
                         entry_reuse));
    }

    /*
     * For each field in the new result type:
     *    1. If input entry_handle_id should set it: write new value according to user input
     *    2. else if should be rewritten by default: write default value
     *    3. If neither done:
     *       3.1 If present in the old result type: fill it from there
     *       3.2 else fill default
     * Start after the result-type field that is always the first field (0) 
     */
    for (field_idx = 1; field_idx < table->multi_res_info[new_result_type].nof_result_fields; field_idx++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[new_result_type].results_info[field_idx].field_id;

        /** Check if field exists on entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE, field_val);
        }
        else
        /** Field that were not set must get value - either from old table or default */
        {
            int is_field_copied = FALSE;

            /** Check if field exists on old hw entry */
            rv = dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id_existing, field_id,
                                                         INST_SINGLE, field_val);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                /*
                 * Even if the field was found in the previous result type, it may not be valid
                 * in the new result type due to different value ranges, encoding etc.
                 */
                dbal_table_field_info_t table_field_info;
                uint8 is_valid_result_type;
                SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, dbal_table_id, field_id, FALSE,
                                                           new_result_type, 0, &table_field_info));
                rv = dbal_value_field32_validate(unit, field_val, &table_field_info, &is_valid_result_type);
                if (SHR_SUCCESS(rv) && is_valid_result_type)
                {
                    /** Copy old HW value to new entry */
                    dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                     field_val);
                    is_field_copied = TRUE;
                }
            }
            /*
             * If no valid value exists in the existing entry, set the field to its default value.
             */
            if (!is_field_copied)
            {
                /** Set value from default */
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
            }
        }
    }
    /** Now we have all the values in the new entry set and we can write new entry to HW */
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit
                                               (unit, lif_table_entry_handle_id_new,
                                                DBAL_COMMIT | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill SW state table that contains information
 *   on the valid fields for each lif.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] is_replace - indication to replace existing entry
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_flow_lif_mgmt_store_valid_fields(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    dbal_physical_tables_e physical_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    int local_lif_id,
    uint8 is_replace)
{
    int field_idx, superset_field_idx;
    CONST dbal_logical_table_t *table;
    local_lif_info_t local_lif_info;
    uint32 field_ids_bitmap[2];
    dbal_fields_e superset_field_id;
    uint8 is_field_exists;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Calculate the DBAL table information
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    /*
     * Calculate the valid fields bitmap 
     * Start after the result-type field that is always the first field (0) 
     */
    sal_memset(field_ids_bitmap, 0, sizeof(field_ids_bitmap));
    for (superset_field_idx = 1; superset_field_idx < table->multi_res_info[table->nof_result_types].nof_result_fields;
         superset_field_idx++)
    {
        superset_field_id = entry_handle->value_field_ids[superset_field_idx];
        if (superset_field_id != DBAL_FIELD_EMPTY)
        {
            dnx_flow_lif_mgmt_is_field_in_result_type(unit, flow_handle_info->flow_handle, result_type,
                                                      superset_field_idx, &field_idx, &is_field_exists);
            if (is_field_exists)
            {
                /** This field was Set - Add it to the SW state */
                SHR_BITSET(field_ids_bitmap, field_idx);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error: Field %s on handler but not in the selected result-type: %s",
                             dbal_field_to_string(unit, superset_field_id), dbal_result_type_to_string(unit,
                                                                                                       dbal_table_id,
                                                                                                       result_type));
            }
        }
    }

    /*
     * Store the valid fields bitmap entry for the local LIF
     */
    local_lif_info.valid_fields = field_ids_bitmap[0];
    if (is_ingress == FALSE)
    {
        uint8 found;

        /*
         * In case of REPLACE, first delete the relevant valid fields entry
         */
        if (is_replace)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_egress_lif_htb.delete(unit, &local_lif_id));
        }

        SHR_IF_ERR_EXIT(lif_table_mngr_db.
                        valid_fields_per_egress_lif_htb.insert(unit, &local_lif_id, &local_lif_info, &found));
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: Failed to insert LIF Table manager valid fields for local LIF 0x%08X, found - %d",
                         local_lif_id, found);
        }
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_ingress_lif1.set(unit, local_lif_id, &local_lif_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_ingress_lif2.set(unit, local_lif_id, &local_lif_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new entry into lif table, according to given handle id fields
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] current_result_type - existing entry result type, relevant if entry_reuse is 1
 *   \param [in] local_lif_id - new entry local lif
 *   \param [in] entry_reuse - reuse existing entry for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_set_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    void *lif_info,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    uint32 current_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    uint32 lif_table_entry_handle_id;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;
    uint8 next_pointer_required_by_user;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, entry_reuse ? current_result_type : result_type,
                     local_lif_id, lif_table_entry_handle_id));

    if (entry_reuse)
    {
        /*
         * If replacing result type on same entry get the entry first
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));
        /*
         * In case of a different result type - need to change the current one on the dbal handle
         */
        if (result_type != current_result_type)
        {
            SHR_IF_ERR_EXIT(dbal_entry_result_type_update
                            (unit, lif_table_entry_handle_id, result_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE));
        }
        if (is_ingress)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_inlif
                            (unit, core_id, dbal_table_id, (lif_table_mngr_inlif_info_t *) lif_info, result_type,
                             current_result_type, local_lif_id, entry_reuse));
        }
        else
        {
            /*
             * Check if user is asking for next pointer, this info is required by lif allocation manager
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &next_pointer_required_by_user));

            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_outlif
                            (unit, dbal_table_id, (lif_table_mngr_outlif_info_t *) lif_info,
                             result_type, current_result_type, next_pointer_required_by_user, local_lif_id,
                             entry_reuse));
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /*
     * Loop on all the fields and fill the table. 
     * Start after the result-type field that is always the first field (0) 
     */
    for (field_idx = 1; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        /** Check if field exists on entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE, field_val);
        }
        else
        {
            dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_DEFAULT_VALUE);
        }
    }

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw(
    int unit,
    int core_id,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    uint32 selected_result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0;
    dbal_physical_tables_e physical_table_id;
    flow_lif_mgmt_replace_mode_e replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE;
    int validation_enable;
    dbal_entry_handle_t *entry_handle;
    int nof_result_types;

    SHR_FUNC_INIT_VARS(unit);

    validation_enable = TRUE;   /* TBD - Get validation state from flow handle */
    if (validation_enable)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw_verify
                              (unit, core_id, entry_handle_id, local_lif_id, lif_info));

    }
    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * Decide on smallest result type: 
     * If there's only one result-type - Use it 
     * If there's a user supplied selection function - Call it 
     * Otherwise, perform the full algorithm 
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_types = entry_handle->table->nof_result_types;
    if (nof_result_types == 1)
    {
        selected_result_type = 0;
    }
    /*
     * TBD - If a user supplied a selection function - call it else if () { selected_result_type = flow_selection_func(unit,
     * entry_handle); } 
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type(unit,
                                                             flow_handle_info, lif_info->table_specific_flags, 0,
                                                             entry_handle_id, &selected_result_type, FALSE));
    }

    /*
     * If user was requesting to allocate fixed size,
     */
    if (lif_info->entry_size_to_allocate != 0)
    {
        CONST dbal_logical_table_t *table;
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (lif_info->entry_size_to_allocate < table->multi_res_info[selected_result_type].entry_payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not sufficient for smallest result type %d\n",
                         lif_info->entry_size_to_allocate, selected_result_type);
        }
    }

    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit,
                                                                                *local_lif_id, core_id,
                                                                                physical_table_id, NULL,
                                                                                &existing_result_type, NULL));
    }

    /*
     * Lif allocation
     */
    if ((!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
         || (existing_result_type != selected_result_type)))
    {
        int entry_reuse;
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_inlif
                        (unit, core_id, dbal_table_id, lif_info, selected_result_type, existing_result_type,
                         local_lif_id, &entry_reuse));
        if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
        {
            replace_mode = entry_reuse ? FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF : FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF;
        }
    }
    else
    {
        /** Get here is replacing entry with same result type, entry can be reused */
        replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
    }

    /*
     * indications for new local lif that was allocated and new local lif with replace.
     * These indications will be useful later on
     */
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
                        || (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, TRUE, dbal_table_id, physical_table_id, entry_handle_id,
                     selected_result_type, *local_lif_id, !is_new_local_lif));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update
                    (unit, *local_lif_id, 1, core_id, physical_table_id, is_new_local_lif));

    /*
     * set HW
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, core_id, TRUE, dbal_table_id, existing_result_type, old_local_lif_id, *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                        (unit, core_id, TRUE, (void *) lif_info, dbal_table_id, entry_handle_id, selected_result_type,
                         existing_result_type, *local_lif_id, (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF)));
    }

    /*
     * In case of replacing with new local lif: Change all ISEM, TCAM, PP-Port entries to point on the new lif
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_ingress_lookup_pointers
                        (unit, core_id, dbal_table_id, (uint32) old_local_lif_id, (uint32) *local_lif_id));
    }

    /*
     * Create SW global->local mapping
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;
        local_lif_mapping_info.phy_table = physical_table_id;

        if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
        {
            /*
             * In case of replacing with new local lif: Remove mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif));
        }

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif, &local_lif_mapping_info, TRUE));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        lif_mngr_local_inlif_info_t old_local_lif_info;
        sal_memset(&old_local_lif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_replace_result_type
                        (unit, core_id, TRUE, (void *) lif_info, dbal_table_id,
                         entry_handle_id, existing_result_type, selected_result_type, *local_lif_id, FALSE));
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, core_id, TRUE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local old Lif clear
         */
        old_local_lif_info.dbal_table_id = dbal_table_id;
        old_local_lif_info.dbal_result_type = existing_result_type;
        old_local_lif_info.core_id = core_id;
        old_local_lif_info.local_inlif = old_local_lif_id;

        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &old_local_lif_info));

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_termination_match_information
                        (unit, core_id, physical_table_id, old_local_lif_id, *local_lif_id));
        /*
         * old LIF valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                        (unit, physical_table_id, core_id, old_local_lif_id));
    }
    /*
     * Print debug info
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, _SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE),
                     selected_result_type, existing_result_type));

exit:
    SHR_FUNC_EXIT;
}

/* 
 * See .h file for details
 */
shr_error_e
dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    uint32 selected_result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0;
    flow_lif_mgmt_replace_mode_e replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE;
    int entry_reuse;
    uint8 next_pointer_required_by_user;
    uint8 existing_result_type_has_ll;
    int validation_enable;
    dbal_entry_handle_t *entry_handle;
    int nof_result_types;
    int flow_timer_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_TIMER_SET("FULL FLOW LIF-Table-Mngr Initiator API", flow_timer_idx++);
    FLOW_TIMER_SET("Verify func", flow_timer_idx);
    validation_enable = TRUE;   /* TBD - Get validation state from flow handle */
    if (validation_enable)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw_verify
                              (unit, entry_handle_id, local_lif_id, lif_info));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Take DBAL table ID from handle
     */
    FLOW_TIMER_SET("dbal_entry_handle_related_table_get", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Decide on smallest result type: 
     * If there's only one result-type - Use it 
     * If there's a user supplied selection function - Call it 
     * Otherwise, perform the full algorithm 
     */
    FLOW_TIMER_SET("select_result_type", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_types = entry_handle->table->nof_result_types;
    if (nof_result_types == 1)
    {
        selected_result_type = 0;
    }
    /*
     * TBD - If a user supplied a selection function - call it else if () { selected_result_type = flow_selection_func(unit,
     * entry_handle); } 
     */
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type(unit,
                                                             flow_handle_info, lif_info->table_specific_flags,
                                                             lif_info->logical_phase, entry_handle_id,
                                                             &selected_result_type, FALSE));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * If user was requesting to allocate fixed size,
     */

    if (lif_info->entry_size_to_allocate != 0)
    {
        CONST dbal_logical_table_t *table;
        FLOW_TIMER_SET("entry_size_to_allocate", flow_timer_idx);

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        if (lif_info->entry_size_to_allocate < table->multi_res_info[selected_result_type].entry_payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not sufficient for smallest result type %d\n",
                         lif_info->entry_size_to_allocate, selected_result_type);
        }
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        FLOW_TIMER_SET("Retrieve SW Info for Replace", flow_timer_idx);
        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, *local_lif_id,
                                                        NULL, &existing_result_type, NULL, NULL,
                                                        &existing_result_type_has_ll, NULL));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /*
     * Check if user is asking for next pointer, this info is required by lif allocation manager
     */
    FLOW_TIMER_SET("dnx_lif_table_mngr_is_field_exist_on_dbal_handle", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &next_pointer_required_by_user));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Lif allocation
     */
    FLOW_TIMER_SET("dnx_lif_table_mngr_allocate_outlif", flow_timer_idx);
    if ((!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
         || (existing_result_type != selected_result_type)
         || (existing_result_type_has_ll != next_pointer_required_by_user)))
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_outlif
                        (unit, dbal_table_id, lif_info, selected_result_type, existing_result_type,
                         next_pointer_required_by_user, local_lif_id, &entry_reuse));

        if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
        {
            if (entry_reuse)
            {
                if (existing_result_type != selected_result_type)
                {
                    replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF;
                }
                /**Can re-use lif, no need to update the lif mngr sw tables*/
                else
                {
                    replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
                }
            }
            /**entry_reuse == False, need to allocate a new lif*/
            else
            {
                replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF;
            }
        }
    }
    else
    {
        /** Get here if replacing an entry with the same size and ll indication, entry can be reused*/
        replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * indications for new local lif that was allocated and new local lif with replace.
     * These indications will be useful later on
     */
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
                        || (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_store_valid_fields", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields(unit,
                                                         flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id,
                                                         selected_result_type, *local_lif_id, !is_new_local_lif));
    FLOW_TIMER_STOP(flow_timer_idx++);
    FLOW_TIMER_SET("dnx_lif_table_mngr_journal_update", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update(unit, *local_lif_id, 0, 0, 0, is_new_local_lif));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * set HW
     */
    FLOW_TIMER_SET("Set/Copy HW", flow_timer_idx);
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, existing_result_type, old_local_lif_id,
                         *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                        (unit, _SHR_CORE_ALL, FALSE, (void *) lif_info, dbal_table_id, entry_handle_id,
                         selected_result_type, existing_result_type, *local_lif_id,
                         (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF)));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * In case of replacing with new local lif: Change all EEDB pointers to point on the new lif
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        FLOW_TIMER_SET("dnx_lif_table_mngr_update_eedb_next_pointer", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_eedb_next_pointer
                        (unit, (uint32) old_local_lif_id, (uint32) *local_lif_id));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /*
     * Write to GLEM if required
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;

        FLOW_TIMER_SET("GLEM add/replace", flow_timer_idx);
        if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
        {
            /*
             * In case of replacing with new local lif: Update GLM and remove all mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, LIF_LIB_GLEM_KEEP_OLD_VALUE,
                             validation_enable));
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif));
        }
        else
        {
            /*
             * New lif: add new GLEM entry
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, validation_enable));
        }
        FLOW_TIMER_STOP(flow_timer_idx++);

        /** Add new global to local lif mapping */
        FLOW_TIMER_SET("Update GLEM mapping", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif, &local_lif_mapping_info, validation_enable));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        FLOW_TIMER_SET("Update GLEM mapping", flow_timer_idx);

        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_replace_result_type
                        (unit, _SHR_CORE_ALL, FALSE, (void *) lif_info, dbal_table_id,
                         entry_handle_id, existing_result_type, selected_result_type, *local_lif_id, FALSE));
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, _SHR_CORE_ALL, FALSE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local old Lif clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, old_local_lif_id));

        /*
         * LIF table valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, old_local_lif_id));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }
    /*
     * Print debug info
     */
    FLOW_TIMER_SET("dnx_lif_table_mngr_print_debug_info", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, (replace_mode != FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE),
                     selected_result_type, existing_result_type));
    FLOW_TIMER_STOP(flow_timer_idx++);
    FLOW_TIMER_STOP(0);

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
