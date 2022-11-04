/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
 *************/
/*
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <src/bcm/dnx/flow/flow_def.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/flow/flow_virtual_lif_mgmt.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_flow_lif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/esem/algo_esem_mnmgt.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */
#define MAX_NUM_OF_FIELDS_IN_SUPERSET       (32)
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
 * Temporary conuters for FLOW performance - remove const from the global variables
 */
#define NOF_FLOW_TIMERS     (20)
#define FLOW_TIMERS_MODE    (0)

const char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
const uint32 timers_group;
const uint32 timer_idx[NOF_FLOW_TIMERS];

#define FLOW_TIMER_SET(timer_desc, flow_timer_idx)  \
if (FLOW_TIMERS_MODE)                               \
{                                                   \
    utilex_ll_timer_set(timer_desc, timers_group, timer_idx[flow_timer_idx]); \
}

#define FLOW_TIMER_STOP(flow_timer_idx)             \
if (FLOW_TIMERS_MODE)                               \
{                                                   \
    utilex_ll_timer_stop(timers_group, timer_idx[flow_timer_idx]); \
}

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
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

static shr_error_e dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    flow_learn_mgmt_l2_learn_data_t * l2_learn_data,
    uint32 selectable_result_types,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint32 *selected_result_type,
    uint8 is_test,
    uint8 is_local_lif);

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/

shr_error_e
flow_lif_mgmt_is_gport_allocated(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint8 *is_allocated)
{
    int decoded_id = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id);
    int is_virtual = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_VIRTUAL)
        || FLOW_GPORT_IS_VIRTUAL(flow_handle_info->flow_id);

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = 0;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        if (!is_virtual)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                            ingress_global_lif.is_allocated(unit, decoded_id, is_allocated));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_flow_lif.flow_ingress_virtual_gport_id.is_allocated(unit, decoded_id, is_allocated));
        }
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        if (!is_virtual)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                            egress_global_lif.is_allocated(unit, decoded_id, is_allocated));
        }
        else
        {
            uint32 gport_subtype = 0;

            if (BCM_GPORT_IS_SET(flow_handle_info->flow_id))
            {
                gport_subtype = BCM_GPORT_SUB_TYPE_GET(flow_handle_info->flow_id);
            }
            else
            {
                dbal_tables_e dbal_table_id = FLOW_LIF_DBAL_TABLE_GET(flow_app_info);
                uint8 is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

                if ((dbal_table_id != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) && (!is_local_lif))
                {
                    gport_subtype = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH;
                }
                else if ((dbal_table_id == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) && (!is_local_lif))
                {
                    gport_subtype = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT;
                }
                else if (is_local_lif)
                {
                    gport_subtype = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong configuration for virtual with no encoding");
                }
            }
            if (gport_subtype == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH)
            {
                SHR_IF_ERR_EXIT(algo_flow_lif.
                                flow_egress_esem_virtual_gport_id.is_allocated(unit, decoded_id, is_allocated));
            }
            else if (gport_subtype == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT)
            {
                SHR_IF_ERR_EXIT(algo_port_pp_db.
                                esem_default_result_profile.is_allocated(unit, decoded_id, is_allocated));
            }
            else if (gport_subtype == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED)
            {
                SHR_IF_ERR_EXIT(algo_flow_lif.
                                flow_egress_virtual_gport_id.is_allocated(unit, decoded_id, is_allocated));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "gport subtype doesn't fit (flow_id=0x%08x)", flow_handle_info->flow_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * {
 */

/**
 * \brief - Print debug info in case result type wasn't found.
 * Print options for fields that can be omitted to get valid result
 *
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle
 *   \param [in] dbal_table_id - table
     \param [in] selectable_result_types - bitmap of selectable
 *          result types following a Flow result-type cb
 *   \param [in] logical_phase - Out-LIF logical phase
 *   \param [in] entry_handle_id - original handle id containing the fields to set
 *   \param [in] is_local_lif - Indication whether it's a local
 *          LIF or a virtual gport
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *
 */
static shr_error_e
dnx_flow_lif_mgmt_no_result_type_found_debug(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dbal_tables_e dbal_table_id,
    uint32 selectable_result_types,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint8 is_local_lif)
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
        else if ((rv != _SHR_E_NOT_FOUND) && (rv != _SHR_E_CONFIG))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving field %s from DBAL entry - %s\n",
                         dbal_field_to_string(unit, field_id), shrextend_errmsg_get(rv));
        }
    }

    sal_memset(str_tmp, 0, sizeof(str_tmp));
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
                            (unit, flow_handle_info, NULL, NULL, selectable_result_types,
                             logical_phase, test_entry_handle_id, &result_type_decision, TRUE, is_local_lif));
            if (result_type_decision != FLOW_LIF_MGMT_INVALID_RESULT_TYPE)
            {
                if (hint_found == FALSE)
                {
                    sal_strncat_s(msg,
                                  "HINT: One of these fields can be omitted to receive a successful result or no matching l2_learn_info: \r\n",
                                  sizeof(msg));
                    hint_found = TRUE;
                }
                sal_strncat_s(msg, dbal_field_to_string(unit, field_id_to_ignore), sizeof(msg));
                sal_strncat_s(msg, "\r\n", sizeof(msg));
            }
            DBAL_HANDLE_FREE(unit, test_entry_handle_id);
        }
        else if ((rv != _SHR_E_NOT_FOUND) && (rv != _SHR_E_CONFIG))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving field %s from DBAL entry - %s\n",
                         dbal_field_to_string(unit, field_id_to_ignore), shrextend_errmsg_get(rv));
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
        if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
        {
            continue;
        }

        if (FLOW_APP_IS_NOT_VALID(flow_app_info))
        {
            continue;
        }

        /*
         * Prepare the information per FLOW 
         * Decrement the FLOW index by one as index 0 isn't used
         */
        flow_dbal_table = FLOW_LIF_DBAL_TABLE_GET(flow_app_info);
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

/** this function narrow down the valid result type according the datapath info. each logical phase as smaller set of
 *  valid result types... */
static shr_error_e
flow_lif_mgmt_result_type_filter_by_encap_access(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    dbal_tables_e dbal_table_id,
    uint32 *allowed_result_types_bmp)
{
    uint8 found;

    flow_app_encap_info_t encap_info = { 0 };
    uint32 res_type_bitmap[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    encap_info.dbal_table = dbal_table_id;
    encap_info.encap_access = logical_phase;

    SHR_IF_ERR_EXIT(flow_db.dbal_table_to_valid_result_types.find(unit, &encap_info, &res_type_bitmap[0], &found));

    if (found)
    {
        (*allowed_result_types_bmp) = (*allowed_result_types_bmp) & (res_type_bitmap[0]);
    }
    /** in case entry not found no option to filter... */

exit:
    SHR_FUNC_EXIT;
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
 *   \param [in] l2_learn_info - flow learn_info user data
 *   \param [in] l2_learn_data - optional internal learn
 *          structure or NULL.
 *   \param [in] selectable_result_types - bitmap of selectable
 *          result types following a Flow result-type cb
 *   \param [in] logical_phase - Out-LIF logical phase
 *   \param [in] entry_handle_id - DBAL handle id containing all the fields
 *   \param [out] selected_result_type - selected result type
 *          for the current lif
 *   \param [in] is_test - Indication whether to run in test
 *          mode
 *   \param [in] is_local_lif - Indication whether it's a local
 *          LIF or a virtual gport
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    flow_learn_mgmt_l2_learn_data_t * l2_learn_data,
    uint32 selectable_result_types,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint32 *selected_result_type,
    uint8 is_test,
    uint8 is_local_lif)
{
    int superset_field_idx, field_idx, result_type_iterator;
    uint32 allowed_result_types_bmp;
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

    /*
     * Use table specific filtering of result types for tables where additional rules should be applied or
     * filtering by the result-type cb 
     */
    if (selectable_result_types)
    {
        allowed_result_types_bmp = selectable_result_types;
    }
    else
    {
        /** in case no result type rule received use all possible result types.. */
        allowed_result_types_bmp = UINT32_MAX;
    }

    if (is_local_lif && (logical_phase != DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID))
    {
        SHR_IF_ERR_EXIT(flow_lif_mgmt_result_type_filter_by_encap_access
                        (unit, logical_phase, dbal_table_id, &allowed_result_types_bmp));
    }

    if (table->nof_result_types == 1)
    {
        /** only one result type, no need to do algorithm, just continue */
        *selected_result_type = 0;

        /** Verify that the learn-data matches the learn-info field in this result-type.
         *  If there's a match, set the learn-info field */
        SHR_EXIT();
    }

    /*
     * Retrieve the sorted result types array for the DBAL table
     */
    SHR_IF_ERR_EXIT(lif_table_mngr_db.lif_table_info_htb.find(unit, (int *) &dbal_table_id, &sw_state_table_info,
                                                              &found));
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
        if (SHR_BITGET(&(allowed_result_types_bmp), result_type_idx))
        {
            valid_result_type = TRUE;
            superset_field_idx = 1;

            /** Verify that the learn-data matches the learn-info filled in this result-type. */
            

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
                    if (is_local_lif && (superset_field_id == DBAL_FIELD_NEXT_OUTLIF_POINTER))
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

    if (valid_result_type == TRUE)
    {
    }
    else
    {
        /*
         * Result type not found - if not test return debug info,
         * else set selected_result_type to invalid
         */
        if (is_test == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_result_type_found_debug
                            (unit, flow_handle_info, dbal_table_id,
                             selectable_result_types, logical_phase, entry_handle_id, is_local_lif));
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

static shr_error_e
dnx_flow_lif_mgmt_key_fields_set(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    int local_lif_id,
    uint32 lif_table_entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Prepare the core index according to the device and working mode */
    if (is_ingress)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif_id);
    }

    if (result_type != -1)
    {
        dbal_entry_value_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_reserve_new_lif(
    int unit,
    int core_id,
    dbal_tables_e dbal_table_id,
    uint32 new_result_type,
    uint32 existing_result_type,
    int local_lif_id,
    uint8 entry_reuse)
{
    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.core_id = core_id;
    inlif_info.local_inlif = local_lif_id;

    /*
     * Replace existing result type with new one
     */
    inlif_info.old_result_type = existing_result_type;
    inlif_info.dbal_result_type = new_result_type;
    if (!entry_reuse)
    {
        /*
         * If can't reuse entry, then a new one was reserved
         */
        inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE;
    }
    inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace the result type of a local outlif
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] logical_phase - LIF phase
 *   \param [in] new_result_type - Chosen result type
 *   \param [in] existing_result_type - Current result type
 *   \param [in] is_next_pointer_valid - Is next pointer field relevant for given lif
 *   \param [in] local_lif_id - Allocated local lif id
 *   \param [in] entry_reuse - Indication in case of reusing the same local lif for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_replace_result_type_outlif(
    int unit,
    dbal_tables_e dbal_table_id,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 new_result_type,
    uint32 existing_result_type,
    uint8 is_next_pointer_valid,
    int local_lif_id,
    uint8 entry_reuse)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = dbal_table_id;
    outlif_info.logical_phase = logical_phase;
    outlif_info.local_outlif = local_lif_id;
    outlif_info.fixed_entry_size = 0;
    /*
     * Replace existing result type with new one
     */
    outlif_info.alternative_result_type = existing_result_type;
    outlif_info.dbal_result_type = new_result_type;

    if (!entry_reuse)
    {
        /*
         * If can't reuse entry, then a new one was reserved
         */
        outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE;
    }
    if (is_next_pointer_valid)
    {
        outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST;
    }
    else
    {
        outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST;
    }
    /*
     * Now replace entry.
     */
    outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace existing entry in lif table with new result 
 *          type. Not applicable for Vitrual gports with no LIF
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] logical_phase - lif phase, applicable only for egress
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
dnx_flow_lif_mgmt_set_hw_fill_updated_lif(
    int unit,
    int core_id,
    uint8 is_ingress,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
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
    uint8 next_pointer_required;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** We need two handles - one to read existing values and one to fill new entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_existing));
    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_new));

    /** Set keys for the two handles */
    if ((core_id == _SHR_CORE_ALL) && (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                        (unit, 0, is_ingress, dbal_table_id, existing_result_type, local_lif_id,
                         lif_table_entry_handle_id_existing));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                        (unit, core_id, is_ingress, dbal_table_id, existing_result_type, local_lif_id,
                         lif_table_entry_handle_id_existing));
    }

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, new_result_type, local_lif_id,
                     lif_table_entry_handle_id_new));

    /*
     * Check if user is asking for next pointer, this info is required by lif allocation manager
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_is_field_on_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0, &next_pointer_required));

    /*
     * In case we replace result type of an entry we must fill all the fields.
     * We will copy all fields that are mutual for the two result types and fill with defaults all the other values.
     */
    /** Get all fields from existing entry */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id_existing, DBAL_GET_ALL_FIELDS));

    if (is_ingress)
    {
        if ((!entry_reuse) && (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_reserve_new_lif
                            (unit, core_id, dbal_table_id, new_result_type, existing_result_type, local_lif_id,
                             entry_reuse));
        }
        else
        {
            /** update the SW state with the new result type*/
            SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set
                            (unit, local_lif_id, core_id, dbal_table_id, new_result_type));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_replace_result_type_outlif
                        (unit, dbal_table_id, logical_phase, new_result_type, existing_result_type,
                         next_pointer_required, local_lif_id, entry_reuse));
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
        /** Fields that were not set must get value - either from old table or default */
        {
            int is_field_copied = FALSE;

            /** Check if field exists on old hw entry */
            rv = dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id_existing, field_id,
                                                         INST_SINGLE, field_val);
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
            else if ((rv != _SHR_E_NOT_FOUND) && (rv != _SHR_E_CONFIG))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving field %s from previous DBAL entry - %s\n",
                             dbal_field_to_string(unit, field_id), shrextend_errmsg_get(rv));
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

static shr_error_e
flow_lif_mgmt_valid_fields_bm_set(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    const dnx_flow_app_config_t * flow_app_info,
    inlif_array_t * local_lifs_array,
    dbal_physical_tables_e physical_table_id,
    local_lif_info_t * local_lif_info,
    uint8 is_replace)
{
    int is_local_lif = TRUE;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        uint8 found;

        /*
         * In case of REPLACE, first delete the relevant valid fields entry
         */
        if (is_replace)
        {
            if (is_local_lif)
            {
                SHR_IF_ERR_EXIT(lif_table_mngr_db.
                                valid_fields_per_egress_lif_htb.delete(unit, &local_lifs_array->local_lifs[0]));
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_flow_lif.
                                valid_fields_per_no_local_lif_egress_htb.delete(unit, &flow_handle_info->flow_id));
            }
        }

        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_lif_htb.insert(unit, &local_lifs_array->local_lifs[0],
                                                                   local_lif_info, &found));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_flow_lif.
                            valid_fields_per_no_local_lif_egress_htb.insert(unit, &flow_handle_info->flow_id,
                                                                            local_lif_info, &found));
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: Failed to insert LIF Table manager valid fields for local LIF 0x%08X, found - %d",
                         local_lifs_array->local_lifs[0], found);
        }
    }
    else if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
    }
    else
    {
        if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_ingress_lif1.set(unit, local_lifs_array->local_lifs[0], local_lif_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_ingress_lif2.set(unit, local_lifs_array->local_lifs[0], local_lif_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SW state table that contains information
 *   on the valid fields for each local Out-LIF.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - Allocated local Out-LIF ID
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
flow_lif_mgmt_valid_fields_clear_by_out_lif(
    int unit,
    int local_lif_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Delete the entry from the SW State */
    SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_egress_lif_htb.delete(unit, &local_lif_id));

exit:
    SHR_FUNC_EXIT;
}

/** Retrieve the valid field bm from the appropriate SW State */
static shr_error_e
flow_lif_mgmt_valid_fields_bm_get(
    int unit,
    int core_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    const dnx_flow_app_config_t * flow_app_info,
    local_lif_info_t * local_lif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        uint8 found = FALSE;

        int is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_lif_htb.find(unit, &gport_hw_resources->local_out_lif,
                                                                 local_lif_info, &found));
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_flow_lif.
                            valid_fields_per_no_local_lif_egress_htb.find(unit, &flow_handle_info->flow_id,
                                                                          local_lif_info, &found));
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Failed to retrieve LIF valid fields bitmap , found - %d", found);
        }
    }
    else
    {
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
        }
        else
        {
            dbal_physical_tables_e physical_table_id;
            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                            (unit, gport_hw_resources->inlif_dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                             &physical_table_id));

            if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
            {
                SHR_IF_ERR_EXIT(lif_table_mngr_db.
                                valid_fields_per_ingress_lif1.get(unit, gport_hw_resources->local_in_lif,
                                                                  local_lif_info));
            }
            else
            {
                SHR_IF_ERR_EXIT(lif_table_mngr_db.
                                valid_fields_per_ingress_lif2.get(unit, gport_hw_resources->local_in_lif,
                                                                  local_lif_info));
            }
        }

        if (local_lif_info->valid_fields == FLOW_LIF_MGMT_VALID_BM_UNINITIALIZED)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Failed to retrieve the valid fields bitmap for gport 0x%x",
                         flow_handle_info->flow_id);
        }
    }

exit:
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
 *   \param [in] local_lifs_array - array of local LIFs per core
    \param [in] is_replace - indication to replace existing entry
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
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
    inlif_array_t * local_lifs_array,
    uint8 is_replace)
{
    int field_idx, superset_field_idx;
    CONST dbal_logical_table_t *table;
    local_lif_info_t local_lif_info;
    uint32 field_ids_bitmap[2];
    dbal_fields_e superset_field_id;
    uint8 is_field_exists;
    dbal_entry_handle_t *entry_handle;
    const dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

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

    local_lif_info.valid_fields = field_ids_bitmap[0];

    SHR_IF_ERR_EXIT(flow_lif_mgmt_valid_fields_bm_set
                    (unit, flow_handle_info, flow_app_info, local_lifs_array, physical_table_id,
                     &local_lif_info, is_replace));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_flow_lif_mgmt_field_is_set_in_hw(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dbal_fields_e field_id,
    uint8 *is_valid)
{
    dbal_tables_e dbal_table_id;
    uint32 result_type;
    int field_idx;
    CONST dbal_logical_table_t *table;
    local_lif_info_t local_lif_info;
    uint32 field_ids_bitmap[2];
    const dnx_flow_app_config_t *flow_app_info;
    int core_id = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    SHR_IF_ERR_EXIT(flow_lif_mgmt_valid_fields_bm_get
                    (unit, core_id, gport_hw_resources, flow_handle_info, flow_app_info, &local_lif_info));

    /** Create a unified field ID bitmap */
    field_ids_bitmap[0] = local_lif_info.valid_fields;
    field_ids_bitmap[1] = 0;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        dbal_table_id = gport_hw_resources->inlif_dbal_table_id;
        result_type = gport_hw_resources->inlif_dbal_result_type;
    }
    else
    {
        dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
        result_type = gport_hw_resources->outlif_dbal_result_type;
    }

    /*
     * Retrieve the table attributes from the DBAL
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /*
     * Retrieve the valid state for the required field
     */
    *is_valid = FALSE;
    /** Loop over the fields and find the relevant field to check */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id_curr = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id == field_id_curr)
        {
            *is_valid = SHR_BITGET(field_ids_bitmap, field_idx) ? TRUE : FALSE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_set_value_fields(
    int unit,
    uint32 entry_handle_id,
    uint32 lif_table_entry_handle_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type)
{
    int field_idx;
    shr_error_e rv;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

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
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE, field_val);
        }
        else if ((rv == _SHR_E_NOT_FOUND) || (rv == _SHR_E_CONFIG))
        {
            dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_DEFAULT_VALUE);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving field %s from DBAL entry - %s\n",
                         dbal_field_to_string(unit, field_id), shrextend_errmsg_get(rv));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** sets the value fields on the HW handle, it is ignoring fields that should be handled outside of flow. currently
 *  the fields are hardcoded, possible improvement is to have a list of fields per application. */
static shr_error_e
dnx_flow_lif_mgmt_shared_db_management_set_value_fields(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 lif_table_entry_handle_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type)
{
    int field_idx;
    shr_error_e rv;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    if (sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR, FLOW_STR_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not supported for applicaiton %s", flow_app_info->app_name);
    }
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
        if (rv == _SHR_E_NONE)
        {
            dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE, field_val);
        }
        else if ((rv == _SHR_E_NOT_FOUND) || (rv == _SHR_E_CONFIG))
        {
            /** This field should be skipped, since it is updated outsude of flow */
            if (field_id != DBAL_FIELD_FWD_DOMAIN_NWK_NAME)
            {
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving field %s from DBAL entry - %s\n",
                         dbal_field_to_string(unit, field_id), shrextend_errmsg_get(rv));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_set_hw_replace_result_type(
    int unit,
    dbal_tables_e dbal_table_id,
    int is_ingress,
    int core_id,
    int is_local_lif,
    uint32 lif_table_entry_handle_id,
    uint32 entry_handle_id,
    uint32 current_result_type,
    uint32 result_type,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int local_lif_id)
{
    int is_core_set_needed;
    dbal_core_mode_e core_mode;

    uint8 next_pointer_required;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));

    is_core_set_needed = (core_id == _SHR_CORE_ALL) && (core_mode == DBAL_CORE_MODE_DPC);

    /** If replacing result type on same entry get the entry first */
    if (is_core_set_needed)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (is_core_set_needed)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** In case of a different result type - need to change the current one on the dbal handle */
    SHR_IF_ERR_EXIT(dbal_entry_result_type_update
                    (unit, lif_table_entry_handle_id, result_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE));

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_reserve_new_lif
                        (unit, core_id, dbal_table_id, result_type, current_result_type, local_lif_id, TRUE));
    }
    else
    {
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_is_field_on_handle
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0, &next_pointer_required));

            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_replace_result_type_outlif
                            (unit, dbal_table_id, logical_phase, result_type, current_result_type,
                             next_pointer_required, local_lif_id, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new entry into lif table, according to given handle id fields
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] local_lif_id - new entry local lif
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_set_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    int local_lif_id)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                    (unit, entry_handle_id, lif_table_entry_handle_id, dbal_table_id, result_type));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace a HW entry that had a result-type change, in
 *          the lif table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] logical_phase - lif related phase
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] current_result_type - existing entry result
 *               type
 *   \param [in] local_lif_id - new entry local lif
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_flow_lif_mgmt_replace_rt_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    uint32 current_result_type,
    int local_lif_id)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, current_result_type, local_lif_id,
                     lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_replace_result_type
                    (unit, dbal_table_id, is_ingress, core_id, TRUE, lif_table_entry_handle_id, entry_handle_id,
                     current_result_type, result_type, logical_phase, local_lif_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                    (unit, entry_handle_id, lif_table_entry_handle_id, dbal_table_id, result_type));

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct
     *  rollback */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_table_fields_get(
    int unit,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    uint32 lif_table_entry_handle_id,
    uint32 *field_ids_bitmap,
    uint32 entry_handle_id)
{
    int field_idx;
    CONST dbal_logical_table_t *table;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(field_ids_bitmap, _SHR_E_INTERNAL, "field_ids_bitmap");

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all the fields and fill lif info */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id != DBAL_FIELD_RESULT_TYPE)
        {
            /** Check in SW state if this field is enabled */
            if (SHR_BITGET(field_ids_bitmap, field_idx))
            {
                /*
                 * Field enabled - copy value from HW to the superset handle
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id, field_id,
                                                                        INST_SINGLE, field_val));
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Log to journal the revert operation of a Terminator 
 *        replace when there's a result-type change with a new
 *        Local In-LIF and working in a symmetric core mode.
 *        This journal log replaces rollback step #2:
 *       1. Add the Old Local In-LIF
 *       2. Modify the New Local In-LIF with old values and
 *       result-type
 *       3. Set match tables to Old Local In-LIF pointers
 *       4. Delete the New Local In-LIF
 *   \param [in] unit - unit id
 *   \param [in] prev_entry_handle_id - DBAL handle of the
 *               retrieved entry before the replace
 *   \param [in] new_local_in_lif - The local In-LIF ID
 *   \param [in] prev_result_type - The result-type before the
 *               modification
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_flow_lif_mgmt_terminator_journal_symmetric_new_lif_update(
    int unit,
    uint32 prev_entry_handle_id,
    int new_local_in_lif,
    uint32 prev_result_type)
{
    dbal_entry_handle_t *get_entry_handle = NULL;
    uint8 is_region_end_required = FALSE;
    uint32 updated_rt_entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** If error recovery is disabled, no need to proceed */
    if (!dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_dbal_access_region_start(unit));
    is_region_end_required = TRUE;

    /** Create a handle with the selected result-type.
        An improvement can be made if we create such a handle and use it not only for journal, but also for hw setting */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, prev_entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &updated_rt_entry_handle_id));

    /** Set the key with core as DBAL_CORE_ALL */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, DBAL_CORE_ALL, TRUE, dbal_table_id, prev_result_type, new_local_in_lif,
                     updated_rt_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                    (unit, prev_entry_handle_id, updated_rt_entry_handle_id, dbal_table_id, prev_result_type));

    /** Retrieve dbal handler is set - A set operation for the previous entry will be added to the journal */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, updated_rt_entry_handle_id, &get_entry_handle));

    /** Call the DBAL journal API */
    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add(unit, get_entry_handle, get_entry_handle, 0));

exit:
    if (is_region_end_required)
    {
        dnx_err_recovery_dbal_access_region_end(unit);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Log to journal the revert operation of a Terminator 
 *        replace when there's a result-type change with a
 *        reused Local In-LIF and working in a symmetric core
 *        mode.
 *       This journal log replaces the suppressed log at
 *       dnx_flow_lif_mgmt_replace_rt_hw().
 *   \param [in] unit - unit id
 *   \param [in] prev_entry_handle_id - DBAL handle of the
 *               retrieved entry before the replace
 *   \param [in] local_in_lif - The local In-LIF ID
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_flow_lif_mgmt_terminator_journal_symmetric_reuse_lif_update(
    int unit,
    uint32 prev_entry_handle_id,
    int local_in_lif)
{
    dbal_entry_handle_t *get_entry_handle = NULL;
    uint8 is_region_end_required = FALSE;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** If error recovery is disabled, no need to proceed */
    if (!dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_dbal_access_region_start(unit));
    is_region_end_required = TRUE;

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, prev_entry_handle_id, &dbal_table_id));

    /** Set the key with core as DBAL_CORE_ALL */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, DBAL_CORE_ALL, TRUE, dbal_table_id, FLOW_LIF_MGMT_INVALID_RESULT_TYPE, local_in_lif,
                     prev_entry_handle_id));

    /** Retrieve dbal handler is set - A set operation for the previous entry will be added to the journal */
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, prev_entry_handle_id, &get_entry_handle));

    /** Call the DBAL journal API */
    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add(unit, get_entry_handle, get_entry_handle, 0));

exit:
    if (is_region_end_required)
    {
        dnx_err_recovery_dbal_access_region_end(unit);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Log to journal the revert operation of an Initiator 
 *          replace when there's a result-type change
 *   \param [in] unit - unit id
 *   \param [in] prev_entry_handle_id - DBAL handle of the
 *               retrieved entry before the replace
 *   \param [in] entry_handle_id - DBAL handle of the replace
 *               entry with the new values
 *   \param [in] local_out_lif - The local Out-LIF ID
 *   \param [in] result_type - The entry selected result-type
 *   \param [in] is_new_local_lif - Indication whether the
 *               replace operation resulted in a new Local-LIF
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_flow_lif_mgmt_initiator_journal_replace_rt_update(
    int unit,
    uint32 prev_entry_handle_id,
    uint32 entry_handle_id,
    int local_out_lif,
    uint32 result_type,
    uint8 is_new_local_lif)
{
    dbal_entry_handle_t *entry_handle = NULL, *get_entry_handle = NULL;
    uint8 is_region_end_required = FALSE;
    uint32 updated_rt_entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** If error recovery is disabled, no need to proceed */
    if (!dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_dbal_access_region_start(unit));
    is_region_end_required = TRUE;

    if (is_new_local_lif)
    {
        /** Create a handle with the selected result-type.
            An improvement can be made if we create such a handle and use it not only for journal, but also for hw setting */
        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &updated_rt_entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                        (unit, DBAL_CORE_DEFAULT, FALSE, dbal_table_id, result_type, local_out_lif,
                         updated_rt_entry_handle_id));

        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                        (unit, entry_handle_id, updated_rt_entry_handle_id, dbal_table_id, result_type));

        /** No retrieve dbal handler is set - A clear operation for the new entry will be added to the journal */
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, updated_rt_entry_handle_id, &entry_handle));
    }
    else
    {
        /** Retrieve dbal handler is set - A set operation for the previous entry will be added to the journal */
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, prev_entry_handle_id, &get_entry_handle));
        entry_handle = get_entry_handle;
    }

    /** Call the DBAL journal API */
    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add(unit, get_entry_handle, entry_handle, 0));

exit:
    if (is_region_end_required)
    {
        dnx_err_recovery_dbal_access_region_end(unit);
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_terminator_info_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id)
{
    uint32 lif_table_entry_handle_id;
    uint32 result_type = gport_hw_resources->inlif_dbal_result_type;
    dbal_tables_e dbal_table_id;
    dbal_physical_tables_e physical_table_id;
    int core_id = _SHR_CORE_ALL;
    int local_lif_id = gport_hw_resources->local_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Take DBAL table ID from handle and get the physical table id from table id
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /*
     * Prepare the core index according to the device and working mode, 
     * before retrieving the entry and the valid fields bitmap
     */

    /** Retrieve the In-LIF entry for futher use */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, TRUE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the values of valid field from lif_table_entry_handle_id and copy them to entry_handle_id */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_info_from_entry_handle_get
                    (unit, flow_app_info, flow_handle_info, gport_hw_resources,
                     lif_table_entry_handle_id, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_mgmt_lif_clear_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_local_inlif_replace(
    int unit,
    int core_id,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 existing_result_type,
    uint32 selected_result_type,
    int local_lif_id,
    int old_local_lif_id,
    dbal_physical_tables_e physical_table_id)
{
    lif_mngr_local_inlif_info_t old_local_lif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Write the new content to the new lif (up to now contained the old lif entry) */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_fill_updated_lif
                    (unit, core_id, TRUE, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, dbal_table_id,
                     entry_handle_id, existing_result_type, selected_result_type, local_lif_id, FALSE));

    /*
     * HW clear
     */
    SHR_IF_ERR_EXIT(dnx_lif_mgmt_lif_clear_hw
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
                    (unit, core_id, physical_table_id, old_local_lif_id, local_lif_id));
    /*
     * old LIF valid fields clear
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                    (unit, physical_table_id, core_id, old_local_lif_id));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_copy_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 src_result_type,
    int src_local_lif_id,
    int dst_local_lif_id)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /** Get source lif entry */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, src_result_type, src_local_lif_id,
                     lif_table_entry_handle_id));

    if (is_ingress && dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)
        && (core_id == _SHR_CORE_ALL))
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Change to new key */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_id, is_ingress, dbal_table_id, -1, dst_local_lif_id, lif_table_entry_handle_id));

    /** Update fields and write back to lif table */
    SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, lif_table_entry_handle_id));

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/************************************* Ingress LIF related prototypes *************************************/

/** for the LIF applications (flow_handle_info), look for related match applications, run over all entries and update
 *  pointers */
static shr_error_e
dnx_flow_lif_mgmt_terminator_match_update(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    int core_id,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    int flow_app_idx, lif_app_idx, flow_number_of_apps;
    const dnx_flow_app_config_t *flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    SHR_FUNC_INIT_VARS(unit);

    flow_number_of_apps = dnx_flow_number_of_apps();

    for (flow_app_idx = FLOW_APP_FIRST_IDX; flow_app_idx < flow_number_of_apps; flow_app_idx++)
    {
        const dnx_flow_app_config_t *match_flow_app_info = dnx_flow_app_info_get(unit, flow_app_idx);
        if (match_flow_app_info->flow_app_type != FLOW_APP_TYPE_TERM_MATCH)
        {
            continue;
        }

        if (FLOW_APP_IS_NOT_VALID(match_flow_app_info))
        {
            continue;
        }

        for (lif_app_idx = 0; lif_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; lif_app_idx++)
        {
            if (!sal_strncasecmp
                (match_flow_app_info->match_payload_apps[lif_app_idx], flow_app_info->app_name, FLOW_STR_MAX))
            {
                int table_idx;
                for (table_idx = 0; table_idx < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_idx++)
                {
                    dbal_tables_e curr_table = match_flow_app_info->flow_table[table_idx];
                    if (curr_table == DBAL_TABLE_EMPTY)
                    {
                        break;
                    }
                    if (curr_table != DBAL_TABLE_EEDB_DUMMY_TABLE_NOT_TO_USE)
                    {
                        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_pointers_single_table
                                        (unit, curr_table, TRUE, (curr_table == DBAL_TABLE_INGRESS_PP_PORT),
                                         old_local_lif_id, new_local_lif_id));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_inlif_allocate(
    int unit,
    uint32 core_bm,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    inlif_array_t * local_lifs_array)
{
    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));
    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.core_id = _SHR_CORE_ALL;
    inlif_info.dbal_result_type = result_type;

    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));
    local_lifs_array->local_lifs[0] = inlif_info.local_inlif;

exit:
    SHR_FUNC_EXIT;

}

/* Lif manager replace, check if entry can be used or not, raise relevant flags and replace */
static shr_error_e
dnx_flow_lif_mgmt_inlif_replace(
    int unit,
    uint32 core_bm,
    dbal_tables_e dbal_table_id,
    uint32 existing_result_type,
    uint32 selected_result_type,
    inlif_array_t * local_lifs_array,
    flow_lif_mgmt_replace_mode_e * replace_mode)
{
    int entry_reuse = 0, smaller_inlif = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        lif_mngr_local_inlif_info_t inlif_info = { 0 };

        inlif_info.dbal_table_id = dbal_table_id;
        inlif_info.old_result_type = existing_result_type;
        inlif_info.dbal_result_type = selected_result_type;
        inlif_info.core_id = core_bm;
        inlif_info.local_inlif = local_lifs_array->local_lifs[0];
        SHR_IF_ERR_EXIT(dnx_algo_local_inlif_can_be_reused(unit, &inlif_info, &entry_reuse, &smaller_inlif));

        if (entry_reuse == FALSE)
        {
            inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE;
            if (smaller_inlif)
            {
                inlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER;
            }
            SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));
            local_lifs_array->local_lifs[0] = inlif_info.local_inlif;
        }
    }

    *replace_mode = entry_reuse ? FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF : FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF;

exit:
    SHR_FUNC_EXIT;
}

/** A legacy journaling function similar to dnx_lif_table_mngr_journal_update */
static shr_error_e
dnx_flow_lif_mgmt_in_lif_phy_db_sbc_journal_update(
    int unit,
    int local_lif_id,
    int is_inlif,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    uint8 is_new_local_lif)
{
    /** in case error recovery enabled, need to perform get to the entry and update the journal. if there is no entry in
     *  HW, need to update the journal with delete entry */
    dbal_entry_handle_t *entry_handle_ptr = NULL;
    dbal_entry_handle_t *get_handle_ptr = NULL;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    uint32 existing_result_type;
    uint32 lif_table_entry_handle_id;
    int rv = 0;
    uint8 is_region_end_required = FALSE;
    int core_idx = core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ALL))
    {
        /*
         * error recovery is disabled, no need to proceed
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_err_recovery_dbal_access_region_start(unit));
    is_region_end_required = TRUE;

    if (!is_inlif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif_id,
                                                        &dbal_table_id, &existing_result_type, NULL, NULL, NULL, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, local_lif_id, core_idx, physical_table_id, &dbal_table_id, &existing_result_type));
    }

    /*
     * check if DBAL entry exists and journal accordingly
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /*
     * set key of handle before getting
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, core_idx, is_inlif, dbal_table_id, existing_result_type, local_lif_id,
                     lif_table_entry_handle_id));

    /*
     * if we don't find any dbal entry we journal CLEAR, otherwise journal ADD
     */
    rv = dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS);
    if ((rv != _SHR_E_NOT_FOUND) && (rv != _SHR_E_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error when trying to get entry");
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, lif_table_entry_handle_id, &entry_handle_ptr));

    if (rv != _SHR_E_NOT_FOUND && !is_new_local_lif)
    {
        get_handle_ptr = entry_handle_ptr;
    }

    SHR_IF_ERR_EXIT(dnx_dbal_journal_log_add(unit, get_handle_ptr, entry_handle_ptr, 0));

exit:
    if (is_region_end_required)
    {
        dnx_err_recovery_dbal_access_region_end(unit);
    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_terminator_info_symmetric_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    flow_learn_mgmt_l2_learn_data_t * l2_learn_data,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 prev_entry_handle_id,
    uint32 selectable_result_types)
{
    uint32 selected_result_type, existing_result_type = gport_hw_resources->inlif_dbal_result_type;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif = 0;
    int old_local_lif_id = 0;
    dbal_physical_tables_e physical_table_id;
    flow_lif_mgmt_replace_mode_e replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE;
    int local_lif_id = gport_hw_resources->local_in_lif;
    inlif_array_t local_lifs_array = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /** Get physical table id */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /** Decide on smallest result type: */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                    (unit, flow_handle_info, l2_learn_info, l2_learn_data, selectable_result_types,
                     DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, entry_handle_id, &selected_result_type, FALSE, TRUE));

    old_local_lif_id = local_lif_id;

    /** Lif allocation */
    if (existing_result_type != selected_result_type)
    {
        local_lifs_array.local_lifs[0] = local_lif_id;
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_inlif_replace
                        (unit, _SHR_CORE_ALL, dbal_table_id, existing_result_type, selected_result_type,
                         &local_lifs_array, &replace_mode));
        local_lif_id = local_lifs_array.local_lifs[0];
    }
    else
    {
        /** Get here is replacing entry with same result type, entry can be reused */
        replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
    }

    is_new_local_lif = (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF);

    /** Save in SW state all the valid fields requested by this action, clear undesired In case of using existing lif */
    local_lifs_array.local_lifs[0] = local_lif_id;
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, TRUE, dbal_table_id, physical_table_id, entry_handle_id,
                     selected_result_type, &local_lifs_array, !is_new_local_lif));

     /** Log into journal */
    if (replace_mode != FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE)
    {
        if (!(dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_in_lif_phy_db_sbc_journal_update
                            (unit, local_lif_id, 1, _SHR_CORE_ALL, physical_table_id, is_new_local_lif));
        }
    }

    /*
     * set HW
     */
    if (is_new_local_lif)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_copy_hw
                        (unit, _SHR_CORE_ALL, TRUE, dbal_table_id, existing_result_type, old_local_lif_id,
                         local_lif_id));

        /** In case of replacing with new local lif ID: Change all related Match entries to point on the new lif */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_match_update
                        (unit, flow_handle_info, _SHR_CORE_ALL, (uint32) old_local_lif_id, (uint32) local_lif_id));

        gport_hw_resources->local_in_lif = local_lif_id;
    }
    else if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF)
    {
        /** Update the Journal before modifying the new entry */
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_journal_symmetric_reuse_lif_update
                            (unit, prev_entry_handle_id, local_lif_id));
        }

        /** Result-Type has changed */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_replace_rt_hw
                        (unit, _SHR_CORE_ALL, TRUE, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, dbal_table_id,
                         entry_handle_id, selected_result_type, existing_result_type, local_lif_id));
    }
    else
    {
        /** replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE, No RT change */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                        (unit, _SHR_CORE_ALL, TRUE, dbal_table_id, entry_handle_id, selected_result_type,
                         local_lif_id));
    }

    /** In case it's a replace with a new local lif */
    if (is_new_local_lif)
    {
        /** Update global->local mapping in case needed */
        if (gport_hw_resources->virtual_id == DNX_ALGO_GPM_LIF_INVALID)
        {
            lif_mapping_local_lif_payload_t local_lif_payload;

            sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
            local_lif_payload.local_lifs_array.local_lifs[0] = local_lif_id;
            local_lif_payload.phy_table = physical_table_id;

            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources->global_in_lif));

            /** Add new global to local lif mapping */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                            (unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources->global_in_lif, &local_lif_payload, TRUE));
        }
        else
        {
            /** Handle virtual mapping */
            SHR_IF_ERR_EXIT(dnx_flow_ingress_virtual_mapping_clear(unit, gport_hw_resources->virtual_id));
            SHR_IF_ERR_EXIT(dnx_flow_ingress_virtual_mapping_add
                            (unit, gport_hw_resources->virtual_id, 1, &local_lif_id));
        }

        /** Update the Journal before modifying the new entry */
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_journal_symmetric_new_lif_update
                            (unit, prev_entry_handle_id, local_lif_id, existing_result_type));
        }

        /** Set the modified new entry and Clear old allocated id and HW old local lif entry */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_local_inlif_replace
                        (unit, _SHR_CORE_ALL, dbal_table_id, entry_handle_id, existing_result_type,
                         selected_result_type, local_lif_id, old_local_lif_id, physical_table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_flow_lif_mgmt_terminator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 prev_entry_handle_id,
    uint32 selectable_result_types)
{
    bcm_flow_l2_learn_info_t *l2_learn_info = NULL;
    flow_learn_mgmt_l2_learn_data_t *l2_learn_data_param = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        && (gport_hw_resources->local_in_lif_per_core.core_id[0] != _SHR_CORE_ALL))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_symmetric_replace
                        (unit, flow_handle_info, l2_learn_info, l2_learn_data_param, gport_hw_resources,
                         entry_handle_id, prev_entry_handle_id, selectable_result_types));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_terminator_info_symmetric_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_l2_learn_info_t * l2_learn_info,
    flow_learn_mgmt_l2_learn_data_t * l2_learn_data,
    uint32 entry_handle_id,
    uint32 selectable_result_types)
{
    int local_lif_id;
    uint32 selected_result_type;
    dbal_tables_e dbal_table_id;
    dbal_physical_tables_e physical_table_id;
    inlif_array_t local_lifs_array = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /** Get physical table id */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /** Decide on smallest result type: */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                    (unit, flow_handle_info, l2_learn_info, l2_learn_data, selectable_result_types,
                     DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, entry_handle_id, &selected_result_type, FALSE, TRUE));

    /** Lif allocation */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_inlif_allocate
                    (unit, _SHR_CORE_ALL, dbal_table_id, selected_result_type, &local_lifs_array));

    local_lif_id = local_lifs_array.local_lifs[0];

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, TRUE, dbal_table_id, physical_table_id, entry_handle_id,
                     selected_result_type, &local_lifs_array, FALSE));

    /** set HW */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                    (unit, _SHR_CORE_ALL, TRUE, dbal_table_id, entry_handle_id, selected_result_type, local_lif_id));

    /** Create global->local mapping */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_VIRTUAL))
    {
        SHR_IF_ERR_EXIT(dnx_flow_ingress_virtual_mapping_add(unit, BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_GET
                                                             (flow_handle_info->flow_id), 1, &local_lif_id));
    }
    else
    {
        lif_mapping_local_lif_payload_t local_lif_payload;

        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = local_lif_id;
        local_lif_payload.phy_table = physical_table_id;

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id),
                         &local_lif_payload, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_flow_lif_mgmt_terminator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    uint32 entry_handle_id,
    uint32 selectable_result_types)
{
    bcm_flow_l2_learn_info_t *l2_learn_info = NULL;
    flow_learn_mgmt_l2_learn_data_t *l2_learn_data_param = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)) && (flow_handle_info->core_bitmap))
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_symmetric_create
                        (unit, flow_handle_info, l2_learn_info, l2_learn_data_param, entry_handle_id,
                         selectable_result_types));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_terminator_info_symmetric_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    lif_mngr_local_inlif_info_t inlif_info;
    dbal_tables_e dbal_table_id = gport_hw_resources->inlif_dbal_table_id;
    uint32 result_type = gport_hw_resources->inlif_dbal_result_type;
    int local_lif_id = gport_hw_resources->local_in_lif;
    uint32 core_bitmap = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    /** clear HW */
    SHR_IF_ERR_EXIT(dnx_lif_mgmt_lif_clear_hw(unit, core_bitmap, TRUE, local_lif_id, dbal_table_id, result_type));

    /** Deallocate the Local Lif. Lif IDs are the same on all cores */
    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));
    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.dbal_result_type = result_type;
    inlif_info.core_id = core_bitmap;
    inlif_info.local_inlif = local_lif_id;

    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &inlif_info));

    /** delete SW state mapping */
    if (gport_hw_resources->virtual_id == DNX_ALGO_GPM_LIF_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources->global_in_lif));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_ingress_virtual_mapping_clear(unit, gport_hw_resources->virtual_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_terminator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_terminator_info_symmetric_clear(unit, gport_hw_resources));

exit:
    SHR_FUNC_EXIT;
}

/************************************* Egress LIF related prototypes *************************************/
/**
 * \brief - Get the values of valid field from the src_entry_handle_id and copy them to the dst_entry_handle_id
 *   \param [in] unit - unit id
 *   \param [in] flow_app_info - FLOW per application
 *          definitions
 *   \param [in] flow_handle_info - FLOW handle that identifies
 *          the FLOW application
 *   \param [in] gport_hw_resources - gport DBAL HW resources
 *   \param [in] src_entry_handle_id - Optional DBAL entry
 *          handle to get the fields from.
 *   \param [in] dst_entry_handle_id - DBAL entry handle to fill
 *          only with flow valid fields.
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_flow_lif_mgmt_initiator_no_local_lif_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 *src_entry_handle_id,
    uint32 dst_entry_handle_id)
{
    dbal_tables_e dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    uint32 retrieve_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Retrieve the virtual match key from SW and set it on the handler */
    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_key_fields_set(unit, flow_handle_info->flow_id, dst_entry_handle_id));

    /** for GET need to update CORE_ID to core 0 */
    if ((dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_is_dpc)) &&
        (dbal_table_id != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE))
    {
        dbal_entry_key_field32_set(unit, dst_entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    /** If no source entry_handle supplied, allocate a retrieve DBAL handle_id for the same table with the same key
      and retrieve the data fields */
    if (src_entry_handle_id == NULL)
    {
        DBAL_HANDLE_COPY(unit, dst_entry_handle_id, &retrieve_entry_handle_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, retrieve_entry_handle_id, DBAL_GET_ALL_FIELDS));
    }
    else
    {
        retrieve_entry_handle_id = *src_entry_handle_id;
    }

    /** Get the values of valid field from retrieve_entry_handle_id and copy them to dst_entry_handle_id */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_info_from_entry_handle_get
                    (unit, flow_app_info, flow_handle_info, gport_hw_resources,
                     retrieve_entry_handle_id, dst_entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** clear all the fields that related to flow application, currenlty this function is supported only for the specific
 *  application see also dnx_flow_lif_mgmt_shared_db_management_set_value_fields   */
static shr_error_e
dnx_flow_clear_all_flow_related_fields(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncasecmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_ESEM_NAMESPACE_VSI_INITIATOR, FLOW_STR_MAX))
    {
        dbal_tables_e dbal_table_id;
        dbal_fields_e next_field_id;

        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

        SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                        (unit, dbal_table_id, DBAL_FIELD_EMPTY, FALSE, gport_hw_resources->outlif_dbal_result_type,
                         &next_field_id));
        while (next_field_id != DBAL_FIELD_EMPTY)
        {
            if (next_field_id == DBAL_FIELD_RESULT_TYPE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, next_field_id, INST_SINGLE,
                                             gport_hw_resources->outlif_dbal_result_type);
            }
            else if (next_field_id != DBAL_FIELD_FWD_DOMAIN_NWK_NAME)
            {
                /** this field is managed by VXLAN application */
                dbal_entry_value_field32_set(unit, entry_handle_id, next_field_id, INST_SINGLE, 0);
            }

            SHR_IF_ERR_EXIT(dbal_tables_next_field_get
                            (unit, dbal_table_id, next_field_id, FALSE, gport_hw_resources->outlif_dbal_result_type,
                             &next_field_id));
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "operation not supported for applicaiton %s", flow_app_info->app_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/** return the esem source according to the application  */
static shr_error_e
dnx_flow_lif_flow_app_source_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    algo_esem_entry_source_e * source)
{
    dbal_tables_e dbal_table_id = FLOW_LIF_DBAL_TABLE_GET(flow_app_info);
    dbal_tables_e ac_table = DBAL_TABLE_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_table_id == ac_table)
    {
        (*source) = DNX_ALGO_ESEM_ENTRY_SOURCE_AC;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "operation not supported for applicaiton %s", flow_app_info->app_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Perform no-local-LIF gport entry removal - DBAL 
 *        entry clear, but also valid fields sw state and sw
 *        match mapping. The function assumes that the virtual
 *        key is already set in the DBAL handle.
 *  
 *   \param [in] unit - unit id
 *   \param [in] no_local_lif_gport - The gport of the removed no local LIF entry
 *   \param [in] gport_hw_resources - The gport hw resources.
 *   \param [in] flow_app_info - The flow application info.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_flow_lif_mgmt_initiator_no_local_lif_clear(
    int unit,
    bcm_gport_t no_local_lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    algo_esem_entry_status_e entry_status = ALGO_ESEM_ENTRY_STATUS_CREATED_BY_SOURCE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Retrieve the virtual match key from SW and set it on the handler */
    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_key_fields_set(unit, no_local_lif_gport, entry_handle_id));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SHARED_PHY_DB_MANAGEMENT))
    {
        algo_esem_entry_source_e source;

        SHR_IF_ERR_EXIT(dnx_flow_lif_flow_app_source_get(unit, flow_app_info, &source));
        SHR_IF_ERR_EXIT(dnx_esem_entry_sw_source_status_get(unit, entry_handle_id, source, &entry_status));

        if ((entry_status == ALGO_ESEM_ENTRY_STATUS_NOT_EXISTS) ||
            (entry_status == ALGO_ESEM_ENTRY_STATUS_EXISTS_SOURCE_NOT_MATCH))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "entry not found for app %s", flow_app_info->app_name);
        }
        SHR_IF_ERR_EXIT(dnx_esem_entry_sw_source_delete(unit, entry_handle_id, source));
    }

     /** Delete the SW mapping entry from no-local-LIF gport to the match keys and the result-type */
    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_match_clear(unit, no_local_lif_gport));

    if (entry_status == ALGO_ESEM_ENTRY_STATUS_ADDED_BY_SOURCE)
    {
        /** if entry was not created by flow no need to deleted the entry just clear the fields */
        SHR_IF_ERR_EXIT(dnx_flow_clear_all_flow_related_fields
                        (unit, entry_handle_id, gport_hw_resources, flow_app_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_initiator_no_local_lif_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 selected_result_type,
    uint32 existing_result_type)
{
    uint8 is_replace_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id_for_hw_set;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    if (existing_result_type != selected_result_type)
    {
        is_replace_result_type = 1;
    }

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id, selected_result_type, NULL,
                     TRUE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_for_hw_set));

    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_key_fields_set
                    (unit, flow_handle_info->flow_id, entry_handle_id_for_hw_set));

    dbal_entry_value_field32_set(unit, entry_handle_id_for_hw_set, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 selected_result_type);

    if (is_replace_result_type)
    {
        SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_sw_info_update
                        (unit, flow_handle_info->flow_id, -1, selected_result_type));
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_replace_result_type
                        (unit, dbal_table_id, FALSE, _SHR_CORE_ALL, FALSE, entry_handle_id_for_hw_set, entry_handle_id,
                         existing_result_type, selected_result_type, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, -1));
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SHARED_PHY_DB_MANAGEMENT))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_shared_db_management_set_value_fields
                        (unit, flow_app_info, entry_handle_id, entry_handle_id_for_hw_set, dbal_table_id,
                         selected_result_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                        (unit, entry_handle_id, entry_handle_id_for_hw_set, dbal_table_id, selected_result_type));
    }

    if (dbal_table_id != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_for_hw_set, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_for_hw_set, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_initiator_no_local_lif_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    uint32 selected_result_type)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id_for_hw_set;
    dbal_entry_action_flags_e flags = DBAL_COMMIT;
    algo_esem_entry_status_e entry_status = ALGO_ESEM_ENTRY_STATUS_NOT_EXISTS;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_SHARED_PHY_DB_MANAGEMENT))
    {
        algo_esem_entry_source_e source;

        SHR_IF_ERR_EXIT(dnx_flow_lif_flow_app_source_get(unit, (dnx_flow_app_config_t *) flow_app_info, &source));
        SHR_IF_ERR_EXIT(dnx_esem_entry_sw_source_status_get(unit, entry_handle_id, source, &entry_status));
        if ((entry_status == ALGO_ESEM_ENTRY_STATUS_CREATED_BY_SOURCE)
            || (entry_status == ALGO_ESEM_ENTRY_STATUS_ADDED_BY_SOURCE))
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "entry already exists for app %s", flow_app_info->app_name);
        }

        if (entry_status == ALGO_ESEM_ENTRY_STATUS_EXISTS_SOURCE_NOT_MATCH)
        {
            flags = DBAL_COMMIT_UPDATE;
            SHR_IF_ERR_EXIT(dnx_esem_entry_sw_source_add(unit, entry_handle_id, source));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_esem_entry_sw_source_create(unit, entry_handle_id, source));
        }
    }

    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_sw_info_add
                    (unit, flow_handle_info->flow_id, entry_handle_id, selected_result_type,
                     (dnx_flow_app_config_t *) flow_app_info));

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id, selected_result_type, NULL,
                     FALSE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id_for_hw_set));

    SHR_IF_ERR_EXIT(dnx_flow_egress_virtual_key_fields_set
                    (unit, flow_handle_info->flow_id, entry_handle_id_for_hw_set));

    dbal_entry_value_field32_set(unit, entry_handle_id_for_hw_set, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 selected_result_type);

    if (flags == DBAL_COMMIT_UPDATE)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_shared_db_management_set_value_fields
                        (unit, flow_app_info, entry_handle_id, entry_handle_id_for_hw_set, dbal_table_id,
                         selected_result_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_value_fields
                        (unit, entry_handle_id, entry_handle_id_for_hw_set, dbal_table_id, selected_result_type));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_for_hw_set, flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_lif_mgmt_outlif_allocate(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    uint32 existing_result_type,
    uint8 is_next_pointer_valid,
    uint8 is_replace,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int *local_lif_id,
    int *entry_reuse)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * RIF has special allocation procedure
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_rif_allocate(unit, *local_lif_id, result_type, 0));
    }
    else
    {
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
        outlif_info.dbal_table_id = dbal_table_id;
        outlif_info.logical_phase = logical_phase;
        outlif_info.dbal_result_type = result_type;
        outlif_info.fixed_entry_size = 0;
        if (is_next_pointer_valid)
        {
            outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST;
        }
        else
        {
            outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST;
        }
        if (!is_replace)
        {
            /*
             * New local Lif
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

            *local_lif_id = outlif_info.local_outlif;
        }
        else
        {
            /*
             * Replace existing result type with new one
             */
            outlif_info.alternative_result_type = existing_result_type;
            outlif_info.local_outlif = *local_lif_id;
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_can_be_reused(unit, &outlif_info, entry_reuse));
            /*
             * If can reuse entry no need to allocate new entry,
             * Else reserve a new one.
             */
            if (*entry_reuse == FALSE)
            {
                outlif_info.local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE;
                SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

                *local_lif_id = outlif_info.local_outlif;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* Replace the LIF attribute of the LIF entry, special handling of when result type or local LIF ID is changed.
 * Replace virtual LIFs can change the payload but not the match keys, so there's no need to store the match in the SW again. 
 * dnx_flow_virtual_result_type_update is called from dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw() 
 * to store a replaced result-type in SW.
 */
shr_error_e
dnx_flow_lif_mgmt_initiator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 prev_entry_handle_id,
    uint32 selectable_result_types)
{
    uint32 selected_result_type = 0;
    uint32 existing_result_type = gport_hw_resources->outlif_dbal_result_type;
    uint32 egr_pointed_id = 0;
    dbal_tables_e dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0, curr_local_lif_id = gport_hw_resources->local_out_lif;
    flow_lif_mgmt_replace_mode_e replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE;
    int entry_reuse;
    uint8 next_pointer_required;
    uint8 existing_result_type_has_ll;
    dbal_entry_handle_t *entry_handle;
    int nof_result_types;
    int flow_timer_idx = 0;
    const dnx_flow_app_config_t *flow_app_info;
    inlif_array_t local_lifs_array = { {0} };
    int is_local_lif;

    SHR_FUNC_INIT_VARS(unit);

    FLOW_TIMER_SET("FULL FLOW LIF-MGMT Initiator API", flow_timer_idx++);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

    /*
     * Decide on smallest result type: 
     * If there's only one result-type - Use it Otherwise, perform the full algorithm 
     */
    FLOW_TIMER_SET("select_result_type", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_types = entry_handle->table->nof_result_types;

    if (nof_result_types != 1)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                        (unit, flow_handle_info, NULL, NULL, selectable_result_types,
                         gport_hw_resources->logical_phase, entry_handle_id, &selected_result_type, FALSE,
                         is_local_lif));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    if (!is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_no_local_lif_replace
                        (unit, flow_handle_info, flow_app_info, entry_handle_id, selected_result_type,
                         existing_result_type));
        SHR_EXIT();
    }

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(flow_handle_info->flow_id))
    {
        egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(flow_handle_info->flow_id);
    }

    FLOW_TIMER_SET("Retrieve SW Info for Replace", flow_timer_idx);
    old_local_lif_id = curr_local_lif_id;
    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, curr_local_lif_id, &dbal_table_id, NULL, NULL,
                                                    NULL, &existing_result_type_has_ll, NULL));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Check if user is asking for next pointer, this info is required by lif allocation manager
     */
    FLOW_TIMER_SET("dbal_entry_handle_is_field_on_handle", flow_timer_idx);

    SHR_IF_ERR_EXIT(dbal_entry_handle_is_field_on_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0, &next_pointer_required));

    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * local-LIF allocation
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_outlif_allocate", flow_timer_idx);
    if (((existing_result_type != selected_result_type) || (existing_result_type_has_ll != next_pointer_required)))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_outlif_allocate
                        (unit, dbal_table_id, selected_result_type, existing_result_type,
                         next_pointer_required, TRUE, gport_hw_resources->logical_phase, &curr_local_lif_id,
                         &entry_reuse));
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
    is_new_local_lif = (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF);

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_store_valid_fields", flow_timer_idx);
    local_lifs_array.local_lifs[0] = curr_local_lif_id;
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields(unit,
                                                         flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id,
                                                         selected_result_type, &local_lifs_array, !is_new_local_lif));

    /** Set the journal only in case of a result-type change. Otherwise, leave this task to the DBAL */
    if (replace_mode != FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE)
    {
        FLOW_TIMER_STOP(flow_timer_idx++);

        FLOW_TIMER_SET("dnx_flow_lif_mgmt_initiator_journal_replace_rt_update", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_journal_replace_rt_update
                        (unit, prev_entry_handle_id, entry_handle_id, curr_local_lif_id, selected_result_type,
                         is_new_local_lif));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /** set HW */
    FLOW_TIMER_SET("Set/Copy HW", flow_timer_idx);
    if (is_new_local_lif)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_copy_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, existing_result_type, old_local_lif_id,
                         curr_local_lif_id));
    }
    else if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF)
    {
        /** Result-Type has changed */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_replace_rt_hw
                        (unit, _SHR_CORE_ALL, FALSE, gport_hw_resources->logical_phase, dbal_table_id, entry_handle_id,
                         selected_result_type, existing_result_type, curr_local_lif_id));
    }
    else
    {
        /** replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE, No RT change */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id,
                         entry_handle_id, selected_result_type, curr_local_lif_id));
    }

    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * In case of replacing with new local lif: Change all EEDB pointers to point on the new lif
     */
    if (is_new_local_lif)
    {
        FLOW_TIMER_SET("dnx_lif_table_mngr_update_eedb_next_pointer", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_eedb_next_pointer(unit, old_local_lif_id, curr_local_lif_id));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /** Write to GLEM if required */
    if ((egr_pointed_id == 0) && is_new_local_lif)
    {
        lif_mapping_local_lif_payload_t local_lif_payload;
        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = curr_local_lif_id;

        /** In case of replacing with new local lif: Update GLM and remove all mapping from SW */
        FLOW_TIMER_SET("GLEM add/replace", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem
                        (unit, _SHR_CORE_ALL, gport_hw_resources->global_out_lif, curr_local_lif_id,
                         LIF_LIB_GLEM_KEEP_OLD_VALUE, TRUE));
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources->global_out_lif));
        FLOW_TIMER_STOP(flow_timer_idx++);

        /** Add new global to local lif mapping */
        FLOW_TIMER_SET("Update GLEM mapping", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources->global_out_lif, &local_lif_payload, TRUE));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }
    else if (egr_pointed_id != 0)
    {
        if (old_local_lif_id != curr_local_lif_id)
        {
            /*
             * It is a 'replace' operation but 'local_lif' has been replaced then
             * update mapping between 'egr_pointed' and 'local_lif':
             * Remove exiting mapping and add the new one.
             */
            SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_mapping_remove(unit, egr_pointed_id));
            SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_mapping_add(unit, egr_pointed_id, curr_local_lif_id));
        }
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry 
     * Not applicable for virtual LIFs 
     */
    if (is_new_local_lif)
    {
        FLOW_TIMER_SET("Update new LIF", flow_timer_idx);

        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_fill_updated_lif
                        (unit, _SHR_CORE_ALL, FALSE, gport_hw_resources->logical_phase, dbal_table_id,
                         entry_handle_id, existing_result_type, selected_result_type, curr_local_lif_id, FALSE));
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_mgmt_lif_clear_hw
                        (unit, _SHR_CORE_ALL, FALSE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local old Lif clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, old_local_lif_id));

        /*
         * LIF table valid fields clear
         */
        SHR_IF_ERR_EXIT(flow_lif_mgmt_valid_fields_clear_by_out_lif(unit, old_local_lif_id));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    FLOW_TIMER_STOP(flow_timer_idx++);
    FLOW_TIMER_STOP(0);

exit:
    SHR_FUNC_EXIT;
}

/* 
 * See .h file for details
 */
shr_error_e
dnx_flow_lif_mgmt_initiator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 selectable_result_types)
{
    int local_lif_id = 0;
    uint32 selected_result_type = 0;
    uint32 egr_pointed_id = 0, global_lif = 0;
    dbal_tables_e dbal_table_id;
    int entry_reuse;
    uint8 next_pointer_required;
    dbal_entry_handle_t *entry_handle;
    int nof_result_types;
    int flow_timer_idx = 0;
    int is_local_lif;
    const dnx_flow_app_config_t *flow_app_info;
    inlif_array_t local_lifs_array = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    FLOW_TIMER_SET("FULL FLOW LIF-MGMT Initiator create API", flow_timer_idx++);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

    FLOW_TIMER_SET("select_result_type", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_types = entry_handle->table->nof_result_types;
    if (nof_result_types != 1)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                        (unit, flow_handle_info, NULL, NULL, selectable_result_types,
                         logical_phase, entry_handle_id, &selected_result_type, FALSE, is_local_lif));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    if (!is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_no_local_lif_create
                        (unit, flow_handle_info, flow_app_info, entry_handle_id, selected_result_type));
        SHR_EXIT();
    }

    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(flow_handle_info->flow_id))
    {
        egr_pointed_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(flow_handle_info->flow_id);
    }
    else
    {
        global_lif = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id);
    }

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /** Check if user is asking for next pointer, this info is required by lif allocation manager */
    FLOW_TIMER_SET("dbal_entry_handle_is_field_on_handle", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_is_field_on_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0, &next_pointer_required));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * local-LIF allocation
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_outlif_allocate", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_outlif_allocate
                    (unit, dbal_table_id, selected_result_type, 0,
                     next_pointer_required, FALSE, logical_phase, &local_lif_id, &entry_reuse));
    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_store_valid_fields", flow_timer_idx);
    local_lifs_array.local_lifs[0] = local_lif_id;
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields(unit,
                                                         flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id,
                                                         selected_result_type, &local_lifs_array, FALSE));

    /*
     * set HW
     */
    FLOW_TIMER_SET("Set HW", flow_timer_idx);

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                    (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, entry_handle_id, selected_result_type, local_lif_id));

    FLOW_TIMER_STOP(flow_timer_idx++);

    /*
     * Write to GLEM if required
     */
    if (egr_pointed_id == 0)
    {
        lif_mapping_local_lif_payload_t local_lif_payload;
        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = local_lif_id;

        FLOW_TIMER_SET("GLEM add/replace", flow_timer_idx);

        /*
         * New lif: add new GLEM entry
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, global_lif, local_lif_id, TRUE));

        FLOW_TIMER_STOP(flow_timer_idx++);

        /** Add new global to local lif mapping */
        FLOW_TIMER_SET("Update GLEM mapping", flow_timer_idx);

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_EGRESS, global_lif, &local_lif_payload, TRUE));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }
    else if (egr_pointed_id != 0)
    {
        SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_mapping_add(unit, egr_pointed_id, local_lif_id));
    }

    FLOW_TIMER_STOP(flow_timer_idx++);
    FLOW_TIMER_STOP(0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_initiator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info)
{
    dbal_tables_e dbal_table_id;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_no_local_lif_clear
                        (unit, flow_handle_info->flow_id, gport_hw_resources, flow_app_info));
        SHR_EXIT();
    }

    dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    result_type = gport_hw_resources->outlif_dbal_result_type;

    /** Clear GLEM if required - this is done before deleting the EEDB entry in order to avoid corruption*/

    if (gport_hw_resources->global_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        /** Update the  GLEM. **/
        SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources->global_out_lif));
        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources->global_out_lif));
    }

    if (gport_hw_resources->virtual_id != DNX_ALGO_GPM_LIF_INVALID)
    {
        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_flow_egr_pointed_virtual_mapping_remove(unit, gport_hw_resources->virtual_id));
    }

    /** clear HW */
    SHR_IF_ERR_EXIT(dnx_lif_mgmt_lif_clear_hw
                    (unit, _SHR_CORE_ALL, FALSE, gport_hw_resources->local_out_lif, dbal_table_id, result_type));

    /** Local Lif clear */
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, gport_hw_resources->local_out_lif));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_valid_fields_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF))
        {
            SHR_IF_ERR_EXIT(algo_flow_lif.
                            valid_fields_per_no_local_lif_egress_htb.delete(unit, &flow_handle_info->flow_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_lif_htb.delete(unit, &gport_hw_resources->local_out_lif));
        }
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        int local_lif_id = gport_hw_resources->local_in_lif;
        uint32 core_bitmap = _SHR_CORE_ALL;
        dbal_physical_tables_e physical_table_id;

        /** Get physical table id */
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, gport_hw_resources->inlif_dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

        /** Clear VALID FIELDS SW state */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                        (unit, physical_table_id, core_bitmap, local_lif_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported app type %d", flow_app_info->flow_app_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_flow_lif_mgmt_initiator_info_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id)
{
    uint32 lif_table_entry_handle_id;
    uint32 result_type = gport_hw_resources->outlif_dbal_result_type;
    dbal_tables_e dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    int local_lif_id = gport_hw_resources->local_out_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF))
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_initiator_no_local_lif_get
                        (unit, flow_app_info, flow_handle_info, gport_hw_resources, NULL, entry_handle_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /*
     * Retrieve the Out-LIF entry for futher use
     */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_key_fields_set
                    (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the values of valid field from lif_table_entry_handle_id and copy them to entry_handle_id */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_info_from_entry_handle_get
                    (unit, flow_app_info, flow_handle_info, gport_hw_resources,
                     lif_table_entry_handle_id, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_info_from_entry_handle_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 src_entry_handle_id,
    uint32 dst_entry_handle_id)
{
    dbal_tables_e dbal_table_id;
    local_lif_info_t local_lif_info;
    uint32 result_type, field_ids_bitmap[2];
    int local_lif_id, core_id = DBAL_CORE_DEFAULT;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Retrieve DBAL table info from gport_hw_resources */
    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
        result_type = gport_hw_resources->outlif_dbal_result_type;
        local_lif_id = gport_hw_resources->local_out_lif;
    }
    else
    {
        dbal_table_id = gport_hw_resources->inlif_dbal_table_id;
        result_type = gport_hw_resources->inlif_dbal_result_type;
        local_lif_id = gport_hw_resources->local_in_lif;
    }

    /** Retrieve the valid field bitmap */
    SHR_IF_ERR_EXIT(flow_lif_mgmt_valid_fields_bm_get
                    (unit, core_id, gport_hw_resources, flow_handle_info, flow_app_info, &local_lif_info));
    field_ids_bitmap[0] = local_lif_info.valid_fields;
    field_ids_bitmap[1] = 0;

    /** Get the values of valid field from the src_entry_handle_id and copy them to the dst_entry_handle_id */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_table_fields_get
                    (unit, local_lif_id, dbal_table_id, result_type, src_entry_handle_id,
                     field_ids_bitmap, dst_entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*** EGRESS POINTED data entries LIF APIs */
shr_error_e
dnx_flow_egress_pointed_data_entry_lif_create(
    int unit,
    uint32 entry_handle_id,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int *local_lif_id)
{
    uint32 result_type;
    dbal_tables_e table_id;
    int entry_reuse;
    uint8 next_pointer_required;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_current_result_type_get(unit, entry_handle_id, &result_type));

    /** Check if user is asking for next pointer, this info is required by lif allocation manager */
    SHR_IF_ERR_EXIT(dbal_entry_handle_is_field_on_handle
                    (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, 0, &next_pointer_required));

    /** LIF allocation */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_outlif_allocate
                    (unit, table_id, result_type, FALSE, next_pointer_required, FALSE, logical_phase, local_lif_id,
                     &entry_reuse));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, (*local_lif_id));

    /** LIF HW setting */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egress_pointed_data_entry_lif_update(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id)
{
    uint32 result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    int curr_local_lif_id = *local_lif_id;
    uint8 existing_result_type_has_ll;

    SHR_FUNC_INIT_VARS(unit);

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &table_id));

    SHR_IF_ERR_EXIT(dbal_entry_current_result_type_get(unit, entry_handle_id, &result_type));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, curr_local_lif_id, &table_id, &existing_result_type, NULL, NULL,
                     &existing_result_type_has_ll, NULL));

    if (result_type != existing_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Replacing result type is not supported yet existing result type %d, new result type %d",
                     existing_result_type, result_type);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, curr_local_lif_id);

    /** LIF HW setting */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egress_pointed_data_entry_lif_delete(
    int unit,
    int local_lif_id)
{
    dbal_tables_e table_id = DBAL_TABLE_EMPTY;
    uint32 result_type, entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, local_lif_id, &table_id, &result_type, NULL, NULL, NULL, NULL));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, local_lif_id));

exit:
    DBAL_FUNC_FREE_VARS SHR_FUNC_EXIT;
}

/*
 * }
 */
