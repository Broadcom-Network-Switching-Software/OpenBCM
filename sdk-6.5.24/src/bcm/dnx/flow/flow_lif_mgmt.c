/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>

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

 
typedef enum
{
    FLOW_LIF_MGMT_VIRTUAL_MATCH_INVALID = -1,
    FLOW_LIF_MGMT_VIRTUAL_MATCH_FIRST,
    FLOW_LIF_MGMT_VIRTUAL_MATCH_ESEM_AC = FLOW_LIF_MGMT_VIRTUAL_MATCH_FIRST,
    FLOW_LIF_MGMT_VIRTUAL_MATCH_DEFAULT,
    FLOW_LIF_MGMT_VIRTUAL_MATCH_LAST = FLOW_LIF_MGMT_VIRTUAL_MATCH_DEFAULT,
    FLOW_LIF_MGMT_VIRTUAL_MATCH_COUNT
} flow_lif_mgmt_virtual_match_type_e;

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

static shr_error_e dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 table_specific_flags,
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
 */
void
flow_lif_mgmt_virtual_match_type_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    flow_lif_mgmt_virtual_match_type_e * virtual_match_type)
{
    int sizeof_app_name = sizeof(flow_app_info->app_name);

    *virtual_match_type = FLOW_LIF_MGMT_VIRTUAL_MATCH_INVALID;

    if (sal_strncmp(flow_app_info->app_name, FLOW_APP_NAME_VLAN_PORT_ESEM_INITIATOR, sizeof_app_name) == 0)
    {
        *virtual_match_type = FLOW_LIF_MGMT_VIRTUAL_MATCH_ESEM_AC;
    }
}

shr_error_e
flow_lif_mgmt_is_gport_allocated(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint8 *is_allocated)
{
    int decoded_id = _SHR_GPORT_FLOW_LIF_ID_GET(flow_handle_info->flow_id);

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = 0;

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                        ingress_global_lif.is_allocated(unit, decoded_id, is_allocated));
    }
    else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        int is_virtual_lif = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_VIRTUAL_LIF);
        if (!is_virtual_lif)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                            egress_global_lif.is_allocated(unit, decoded_id, is_allocated));
        }
        else
        {
            if (BCM_GPORT_SUB_TYPE_GET(flow_handle_info->flow_id) == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH)
            {
                SHR_IF_ERR_EXIT(lif_table_mngr_db.
                                flow_egress_virtual_gport_id.is_allocated(unit, decoded_id, is_allocated));
            }
            else if (BCM_GPORT_SUB_TYPE_GET(flow_handle_info->flow_id) == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT)
            {
                SHR_IF_ERR_EXIT(algo_port_pp_db.
                                esem_default_result_profile.is_allocated(unit, decoded_id, is_allocated));
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
 * \brief - Retrieve the match keys of a virtual gport, from the
 *        SW table to the DBAL handler and return the retrieved
 *        result type.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the retrieved
 *          virtual LIF
 *   \param [in] entry_handle_id - The DBAL handler that stores
 *          the match keys and the result-type.
 *   \param [out] virtual_match_type - The stored virtual match
 *          type in the SW.
 *   \param [out] result_type - The SW stored result-type.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_flow_virtual_match_sw_get(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id,
    flow_lif_mgmt_virtual_match_type_e * virtual_match_type,
    uint32 *result_type)
{
    uint32 sw_entry_handle_id, sw_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take SW table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));

    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Retrieve the mapping entry from DBAL and get the result-type */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &sw_result_type));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type));

    /** Copy the fields from SW DBAL table to the handler */
    switch (sw_result_type)
    {
        case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
        {
            uint32 vsi, vlan_domain, c_vid;
            dbal_fields_e field_id;

            field_id = DBAL_FIELD_VLAN_DOMAIN;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, &vlan_domain));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &c_vid));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, vlan_domain);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);

            *virtual_match_type = FLOW_LIF_MGMT_VIRTUAL_MATCH_ESEM_AC;
        }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported sw result-type %s for virtual gport 0x%x\n",
                         dbal_result_type_to_string(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW,
                                                    sw_result_type), virtual_lif_gport);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Add Virtual match-info per gport to SW state.
 *  Currently applicable for ESEM entries.
 *  See .h file
 */
shr_error_e
dnx_flow_virtual_match_sw_add(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info)
{
    uint32 sw_entry_handle_id, result_type;
    flow_lif_mgmt_virtual_match_type_e virtual_match_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    flow_lif_mgmt_virtual_match_type_get(unit, flow_app_info, &virtual_match_type);

    /** Take SW table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));

    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Set the match info */
    
    switch (virtual_match_type)
    {
        case FLOW_LIF_MGMT_VIRTUAL_MATCH_ESEM_AC:
        {
            uint32 vsi, vlan_domain, c_vid;
            dbal_fields_e field_id;

            field_id = DBAL_FIELD_VLAN_DOMAIN;

        /** Set the SW table result type  */
            dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC);

        /** Retrieve the match keys from the DBAL handler */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, &vlan_domain));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_C_VID, &c_vid));

        /** Store the match keys in SW DBAL table */
            dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, vsi);
            dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain);
            dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, c_vid);
        }
            break;
        case FLOW_LIF_MGMT_VIRTUAL_MATCH_DEFAULT:
        /** Set the SW table result type  */
            dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_DEFAULT);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Unsupported virtual match type - %d", virtual_match_type);
    }

    /** Get the result type from the input entry_handle_id and stote it */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type);

    /** Insert the mapping entry to the SW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the actual result-type of a virtual gport. 
 *        The value is taken from the SW table per virtual
 *        gport.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the virtual
 *          LIF for which the result-type is retrieved.
 *   \param [out] result_type - The returned result-type.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_flow_virtual_result_type_get(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 *result_type)
{
    uint32 sw_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take SW table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));

    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Retrieve the mapping entry from the SW DBAL */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Retrieve the result type from the DBAL handler */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the result-type of a virtual gport within the
 *        SW table per virtual gport.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the virtual
 *          LIF for which the result-type is updated.
 *   \param [in] virtual_match_type - The type of virtual match
 *          implies the sw state result type.
 *   \param [in] result_type - The result-type value to set.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_flow_virtual_result_type_update(
    int unit,
    bcm_gport_t virtual_lif_gport,
    flow_lif_mgmt_virtual_match_type_e virtual_match_type,
    uint32 result_type)
{
    uint32 sw_entry_handle_id;
    dbal_fields_e sw_result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take SW table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));

    
    switch (virtual_match_type)
    {
        case FLOW_LIF_MGMT_VIRTUAL_MATCH_ESEM_AC:
            sw_result_type = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC;
            break;
        case FLOW_LIF_MGMT_VIRTUAL_MATCH_DEFAULT:
            sw_result_type = DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_DEFAULT;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Unsupported virtual match type %d", virtual_match_type);
    }

    dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, sw_result_type);

    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Set the modified result type */
    dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type);

    /** Insert the mapping entry to the SW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_entry_handle_id, DBAL_COMMIT_UPDATE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear an entry from a SW table that maps a virtual 
 *        gport to the match keys of the entry and the selected
 *        result-type.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the virtual
 *          LIF for which the mapping is cleared.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e
dnx_flow_virtual_match_sw_clear(
    int unit,
    bcm_gport_t virtual_lif_gport)
{
    uint32 sw_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_entry_handle_id));

    /** Set the virtual gport as the key for the mapping table */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Clear the entry from the SW DBAL mapping table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print debug info in case result type wasn't found.
 * Print options for fields that can be omitted to get valid result
 *
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle
 *   \param [in] dbal_table_id - table
 *   \param [in] table_specific_flags - specific flags requested by the user
 *   \param [in] selectable_result_types - bitmap of selectable
 *          result types following a Flow result-type cb
 *   \param [in] logical_phase - Out-LIF logical phase
 *   \param [in] entry_handle_id - original handle id containing the fields to set
 *   \param [in] is_local_lif - Indication whether it's a local
 *          LIF or a virtual gport
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
                            (unit, flow_handle_info, table_specific_flags, selectable_result_types,
                             logical_phase, test_entry_handle_id, &result_type_decision, TRUE, is_local_lif));
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
 *   \param [in] table_specific_flags - lif table specific information
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
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_flow_lif_mgmt_select_result_type(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 table_specific_flags,
    uint32 selectable_result_types,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 entry_handle_id,
    uint32 *selected_result_type,
    uint8 is_test,
    uint8 is_local_lif)
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

    /*
     * Use table specific filtering of result types for tables where additional rules should be applied or
     * filtering by the result-type cb 
     */
    if (selectable_result_types)
    {
        table_specific_rules.allowed_result_types_bmp = selectable_result_types;
    }
    else
    {
        /** in case no result type rule received use all possible result types.. */
        table_specific_rules.allowed_result_types_bmp = UINT32_MAX;
    }

    if (is_local_lif && (logical_phase != DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID))
    {
        SHR_IF_ERR_EXIT(flow_lif_mgmt_result_type_filter_by_encap_access
                        (unit, logical_phase, dbal_table_id, &(table_specific_rules.allowed_result_types_bmp)));
    }

    if (table->nof_result_types == 1)
    {
        /** only one result type, no need to do algorithm, just continue */
        *selected_result_type = 0;
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

    if (valid_result_type == FALSE)
    {
        /*
         * Result type not found - if not test return debug info,
         * else set selected_result_type to invalid
         */
        if (is_test == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_no_result_type_found_debug
                            (unit, flow_handle_info, dbal_table_id, table_specific_flags,
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

/**
 * \brief - Verify function for
 *        dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] flow_handle_info - Flow handle
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
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    const dnx_flow_app_config_t *flow_app_info;
    int is_local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

    if (is_local_lif)
    {
        SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
        if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
            && ((lif_info->global_lif == 0) || (lif_info->global_lif == DNX_ALGO_GPM_LIF_INVALID)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "global_lif must be allocated when calling dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set");
        }

        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
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
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace existing entry in lif table with new result 
 *          type. Not applicable for Vitrual gports with no LIF
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

    
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) && is_ingress)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id_existing, DBAL_FIELD_CORE_ID, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id_existing, DBAL_GET_ALL_FIELDS));

    
    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) && is_ingress)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id_existing, DBAL_FIELD_CORE_ID, core_id);
    }

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

static shr_error_e
flow_lif_mgmt_valid_fields_bm_set(
    int unit,
    uint32 core_bitmap,
    bcm_flow_handle_info_t * flow_handle_info,
    const dnx_flow_app_config_t * flow_app_info,
    int local_lif_id,
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
                SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_egress_lif_htb.delete(unit, &local_lif_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(lif_table_mngr_db.
                                valid_fields_per_egress_virtual_htb.delete(unit, &flow_handle_info->flow_id));
            }
        }

        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_lif_htb.insert(unit, &local_lif_id, local_lif_info, &found));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_virtual_htb.insert(unit, &flow_handle_info->flow_id, local_lif_info,
                                                                       &found));
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error: Failed to insert LIF Table manager valid fields for local LIF 0x%08X, found - %d",
                         local_lif_id, found);
        }
    }
    else if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
    }
    else
    {
#ifdef BCM_DNX2_SUPPORT
        if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_ingress_lif1.set(unit, local_lif_id, local_lif_info));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_ingress_lif2.set(unit, local_lif_id, local_lif_info));
        }
#endif
    }

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
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        int is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_lif_htb.find(unit, &gport_hw_resources->local_out_lif,
                                                                 local_lif_info, &found));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_egress_virtual_htb.find(unit, &flow_handle_info->flow_id, local_lif_info,
                                                                     &found));
        }
    }
    else if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
    }
    else
    {
#ifdef BCM_DNX2_SUPPORT

        dbal_physical_tables_e physical_table_id;
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, gport_hw_resources->inlif_dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

        if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_ingress_lif1.get(unit, gport_hw_resources->local_in_lif, local_lif_info));
            found = (local_lif_info->valid_fields) ? TRUE : FALSE;
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_table_mngr_db.
                            valid_fields_per_ingress_lif2.get(unit, gport_hw_resources->local_in_lif, local_lif_info));
            found = (local_lif_info->valid_fields) ? TRUE : FALSE;
        }
#endif
    }

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: Failed to retrieve LIF Table manager valid fields bitmap , found - %d",
                     found);
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
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] core_bitmap - The cores in which the LIF is defined. Relevant only for Ingress.
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
    uint32 core_bitmap,
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
                    (unit, core_bitmap, flow_handle_info, flow_app_info, local_lif_id, physical_table_id,
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
 *   \param [in] flow_handle_info - Flow handle
 *   \param [in] result_type - new entry result type
 *   \param [in] current_result_type - existing entry result
 *               type, relevant if is_replace_result_type is 1
 *   \param [in] local_lif_id - new entry local lif
 *   \param [in] is_replace_result_type - reuse existing entry
 *               for new result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
shr_error_e
dnx_flow_lif_mgmt_set_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    void *lif_info,
    dbal_tables_e dbal_table_id,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    uint32 result_type,
    uint32 current_result_type,
    int local_lif_id,
    uint8 is_replace_result_type)
{
    uint32 lif_table_entry_handle_id, last_result_type, set_result_type;
    int field_idx, is_local_lif = TRUE;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;
    uint8 next_pointer_required_by_user;
    const dnx_flow_app_config_t *flow_app_info;
    flow_lif_mgmt_virtual_match_type_e virtual_match_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    if (!is_ingress)
    {
        is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
    }

    /** Suppress the DBAL commit only, all swstate access in function (if any) must enter journal to ensure correct rollback. */
    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    set_result_type = (is_replace_result_type) ? current_result_type : result_type;
    if (is_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                        (unit, core_id, is_ingress, dbal_table_id, set_result_type,
                         local_lif_id, lif_table_entry_handle_id));
    }
    else
    {
        /** Retrieve the virtual match key from SW and set it on the handler */
        SHR_IF_ERR_EXIT(dnx_flow_virtual_match_sw_get
                        (unit, flow_handle_info->flow_id, lif_table_entry_handle_id, &virtual_match_type,
                         &last_result_type));

        dbal_entry_value_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     set_result_type);

        SHR_IF_ERR_EXIT(dnx_flow_virtual_result_type_update
                        (unit, flow_handle_info->flow_id, virtual_match_type, set_result_type));
    }

    if (is_replace_result_type)
    {
        /*
         * If replacing result type on same entry get the entry first
         */
        
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) && is_ingress)
        {
            if (core_id == _SHR_CORE_ALL)
            {
                dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, 0);
            }
            else
            {
                dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

        
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) && is_ingress)
        {
            dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        }

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
                             current_result_type, local_lif_id, is_replace_result_type));
        }
        else
        {
            /*
             * Check if user is asking for next pointer, this info is required by lif allocation manager
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_field_exist_on_dbal_handle
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, &next_pointer_required_by_user));

            if (is_local_lif)
            {
                SHR_IF_ERR_EXIT(dnx_lif_table_mngr_replace_result_type_outlif
                                (unit, dbal_table_id, (lif_table_mngr_outlif_info_t *) lif_info,
                                 result_type, current_result_type, next_pointer_required_by_user, local_lif_id,
                                 is_replace_result_type));
            }
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
    if ((is_local_lif) || (virtual_match_type == FLOW_LIF_MGMT_VIRTUAL_MATCH_DEFAULT))
    {
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dbal_entry_commit
                                                   (unit, lif_table_entry_handle_id, DBAL_COMMIT_FORCE));
    }

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
    local_lif_info_t local_lif_info;
    uint32 field_ids_bitmap[2];
    int core_idx = _SHR_CORE_ALL;
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

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_idx, TRUE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(flow_lif_mgmt_valid_fields_bm_get
                    (unit, core_idx, gport_hw_resources, flow_handle_info, flow_app_info, &local_lif_info));

    /** Create a unified field ID bitmap */
    field_ids_bitmap[0] = local_lif_info.valid_fields;
    field_ids_bitmap[1] = 0;

    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_table_fields_get
                    (unit, local_lif_id, dbal_table_id, result_type, lif_table_entry_handle_id,
                     field_ids_bitmap, entry_handle_id));

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
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 existing_result_type,
    uint32 selected_result_type,
    int local_lif_id,
    int old_local_lif_id,
    dbal_physical_tables_e physical_table_id)
{
    lif_mngr_local_inlif_info_t old_local_lif_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Write the new content to the new lif (up to now contained the old lif entry) */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw_replace_result_type
                    (unit, core_id, TRUE, (void *) lif_info, dbal_table_id,
                     entry_handle_id, existing_result_type, selected_result_type, local_lif_id, FALSE));
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
                    (unit, core_id, physical_table_id, old_local_lif_id, local_lif_id));
    /*
     * old LIF valid fields clear
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                    (unit, physical_table_id, core_id, old_local_lif_id));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_flow_lif_mgmt_terminator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 selectable_result_types)
{
    uint32 selected_result_type, existing_result_type = gport_hw_resources->inlif_dbal_result_type;
    uint32 core_bitmap = _SHR_CORE_ALL;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif;
    int old_local_lif_id = 0;
    dbal_physical_tables_e physical_table_id;
    flow_lif_mgmt_replace_mode_e replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_NO_REPLACE;
    int local_lif_id = gport_hw_resources->local_in_lif;

    SHR_FUNC_INIT_VARS(unit);

    if (lif_info->entry_size_to_allocate != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not supported \n",
                     lif_info->entry_size_to_allocate);
    }

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /** Get physical table id */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /** Decide on smallest result type: */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                    (unit, flow_handle_info, lif_info->table_specific_flags, selectable_result_types,
                     DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, entry_handle_id, &selected_result_type, FALSE, TRUE));

    old_local_lif_id = local_lif_id;

    /** Lif allocation */
    if (existing_result_type != selected_result_type)
    {
        int entry_reuse;
        
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_inlif
                        (unit, core_bitmap, dbal_table_id, lif_info, selected_result_type, existing_result_type,
                         &local_lif_id, &entry_reuse));
        replace_mode = entry_reuse ? FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF : FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF;
    }
    else
    {
        /** Get here is replacing entry with same result type, entry can be reused */
        replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
    }

    is_new_local_lif = (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF);

    /** Save in SW state all the valid fields requested by this action, clear undesired In case of using existing lif */
    
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, TRUE, dbal_table_id, physical_table_id, entry_handle_id,
                     selected_result_type, local_lif_id, core_bitmap, !is_new_local_lif));

    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update
                        (unit, local_lif_id, 1, _SHR_CORE_ALL, physical_table_id, is_new_local_lif));

        /*
         * set HW
         */
        if (is_new_local_lif)
        {
            /*
             * In case of new lif, first copy old lif contents to new lif,
             * redirect the pointers to the new lif and only then rewrite with new data
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                            (unit, _SHR_CORE_ALL, TRUE, dbal_table_id, existing_result_type, old_local_lif_id,
                             local_lif_id));

            /*
             * In case of replacing with new local lif: Change all ISEM, TCAM, PP-Port entries to point on the new lif
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_ingress_lookup_pointers
                            (unit, _SHR_CORE_ALL, dbal_table_id, (uint32) old_local_lif_id, (uint32) local_lif_id));

        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                            (unit, _SHR_CORE_ALL, TRUE, (void *) lif_info, dbal_table_id, flow_handle_info,
                             entry_handle_id, selected_result_type, existing_result_type, local_lif_id,
                             (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF)));
        }
    }

    /*
     * Create SW global->local mapping
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        
        lif_mapping_local_lif_payload_t local_lif_payload = {
            .local_lifs_array = {{0}
                                 }
            ,.phy_table = 0,.is_symmetric = 0
        };

        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = local_lif_id;
        local_lif_payload.phy_table = physical_table_id;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif));

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif, &local_lif_payload, TRUE));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (is_new_local_lif)
    {
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
        {
            
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_local_inlif_replace
                            (unit, _SHR_CORE_ALL, dbal_table_id, entry_handle_id, lif_info, existing_result_type,
                             selected_result_type, local_lif_id, old_local_lif_id, physical_table_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_local_inlif_replace
                            (unit, _SHR_CORE_ALL, dbal_table_id, entry_handle_id, lif_info, existing_result_type,
                             selected_result_type, local_lif_id, old_local_lif_id, physical_table_id));
        }
    }
    /*
     * Print debug info
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, TRUE, selected_result_type, existing_result_type));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_flow_lif_mgmt_terminator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 selectable_result_types)
{
    uint32 selected_result_type, existing_result_type = 0;
    uint32 core_bitmap = _SHR_CORE_ALL;
    dbal_tables_e dbal_table_id;
    dbal_physical_tables_e physical_table_id;
    int entry_reuse;

    SHR_FUNC_INIT_VARS(unit);

    if (lif_info->entry_size_to_allocate != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not supported \n",
                     lif_info->entry_size_to_allocate);
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_ALLOC_BY_CORE_BM))
    {
        core_bitmap = flow_handle_info->core_bitmap;
    }

    /** Take DBAL table ID from handle */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /** Get physical table id */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /** Decide on smallest result type: */
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                    (unit, flow_handle_info, lif_info->table_specific_flags, selectable_result_types,
                     DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_INVALID, entry_handle_id, &selected_result_type, FALSE, TRUE));

    /** Lif allocation */
    
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_inlif
                    (unit, core_bitmap, dbal_table_id, lif_info, selected_result_type, existing_result_type,
                     local_lif_id, &entry_reuse));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields
                    (unit, flow_handle_info, TRUE, dbal_table_id, physical_table_id, entry_handle_id,
                     selected_result_type, *local_lif_id, core_bitmap, FALSE));

    if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        int core_idx;
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);

        for (core_idx = 0; core_idx < nof_cores; core_idx++)
        {
            
            if (core_bitmap & SAL_BIT(core_idx))
            {
                SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update
                                (unit, *local_lif_id, 1, core_idx, physical_table_id, TRUE));

                /** set HW */
                SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                                (unit, core_idx, TRUE, (void *) lif_info, dbal_table_id, flow_handle_info,
                                 entry_handle_id, selected_result_type, existing_result_type, *local_lif_id, FALSE));

            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update
                        (unit, *local_lif_id, 1, _SHR_CORE_ALL, physical_table_id, TRUE));

        /** set HW */
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_set_hw
                        (unit, _SHR_CORE_ALL, TRUE, (void *) lif_info, dbal_table_id, flow_handle_info, entry_handle_id,
                         selected_result_type, existing_result_type, *local_lif_id, FALSE));
    }

    /*
     * Create SW global->local mapping
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING))
    {
        lif_mapping_local_lif_payload_t local_lif_payload = {
            .local_lifs_array = {{0}
                                 }
            ,.phy_table = 0,.is_symmetric = 0
        };

        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = *local_lif_id;
        local_lif_payload.phy_table = physical_table_id;
        

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif, &local_lif_payload, TRUE));
    }

    /** Print debug info */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_print_debug_info
                    (unit, dbal_table_id, FALSE, selected_result_type, existing_result_type));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_terminator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    lif_mngr_local_inlif_info_t inlif_info;
    dbal_physical_tables_e physical_table_id;
    dbal_tables_e dbal_table_id = gport_hw_resources->inlif_dbal_table_id;
    uint32 result_type = gport_hw_resources->inlif_dbal_result_type;
    int local_lif_id = gport_hw_resources->local_in_lif;
    uint32 core_bitmap = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    /** clear SW     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw(unit, core_bitmap, TRUE, local_lif_id, dbal_table_id, result_type));

    /** Clear VALID FIELDS SW state     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif
                    (unit, physical_table_id, core_bitmap, local_lif_id));

    /** Deallocate the Local Lif */
    
    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));
    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.dbal_result_type = result_type;
    inlif_info.core_id = core_bitmap;
    inlif_info.local_inlif = local_lif_id;

    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &inlif_info));

    /** delete SW state mapping */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, gport_hw_resources->global_in_lif));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_lif_mgmt_initiator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    dbal_tables_e dbal_table_id;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    dbal_table_id = gport_hw_resources->outlif_dbal_table_id;
    result_type = gport_hw_resources->outlif_dbal_result_type;

    /** Clear GLEM if required - this is done before deleting the EEDB entry in order to avoid corruption*/

    /** Update the  GLEM. **/
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, gport_hw_resources->global_out_lif));
    /** delete SW state mapping */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, gport_hw_resources->global_out_lif));

    /** clear HW */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                    (unit, _SHR_CORE_ALL, FALSE, gport_hw_resources->local_out_lif, dbal_table_id, result_type));

    /*
     * Clear VALID FIELDS SW state
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, gport_hw_resources->local_out_lif));

    /*
     * Local Lif clear
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, gport_hw_resources->local_out_lif));

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
    lif_table_mngr_outlif_info_t * lif_info,
    uint32 selectable_result_types)
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
    uint8 is_verify_disabled;
    dbal_entry_handle_t *entry_handle;
    int nof_result_types;
    int flow_timer_idx = 0;
    int is_replace, is_local_lif;
    const dnx_flow_app_config_t *flow_app_info;

    SHR_FUNC_INIT_VARS(unit);

    if (lif_info->entry_size_to_allocate != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Requested size to allocate %d is not supported \n",
                     lif_info->entry_size_to_allocate);
    }

    FLOW_TIMER_SET("FULL FLOW LIF-Table-Mngr Initiator API", flow_timer_idx++);
    FLOW_TIMER_SET("Verify func", flow_timer_idx);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    is_replace = _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE);
    is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));

    SHR_IF_ERR_EXIT(flow_db.flow_application_info.
                    is_verify_disabled.get(unit, flow_handle_info->flow_handle, &is_verify_disabled));
    if (!is_verify_disabled)
    {
        SHR_INVOKE_VERIFY_DNXC(dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw_verify
                               (unit, flow_handle_info, entry_handle_id, local_lif_id, lif_info));
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
     * Otherwise, perform the full algorithm 
     */
    FLOW_TIMER_SET("select_result_type", flow_timer_idx);
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    nof_result_types = entry_handle->table->nof_result_types;
    if (nof_result_types == 1)
    {
        selected_result_type = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_select_result_type
                        (unit, flow_handle_info, lif_info->table_specific_flags, selectable_result_types,
                         lif_info->logical_phase, entry_handle_id, &selected_result_type, FALSE, is_local_lif));
    }
    FLOW_TIMER_STOP(flow_timer_idx++);

    if (is_local_lif)
    {
        /*
         * In case of replace find existing result type
         */
        if (is_replace)
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
         * local-LIF allocation
         */
        FLOW_TIMER_SET("dnx_lif_table_mngr_allocate_outlif", flow_timer_idx);
        if (((!is_replace) || (existing_result_type != selected_result_type) ||
             (existing_result_type_has_ll != next_pointer_required_by_user)))
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_outlif
                            (unit, dbal_table_id, lif_info, selected_result_type, existing_result_type,
                             next_pointer_required_by_user, local_lif_id, &entry_reuse));
            if (is_replace)
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
    }
    else
    {
        /** Initiator entries with no local-LIF (ESEM) have only one entry size */
        if (is_replace)
        {
            SHR_IF_ERR_EXIT(dnx_flow_virtual_result_type_get(unit, flow_handle_info->flow_id, &existing_result_type));
        }
        if (is_replace && existing_result_type != selected_result_type)
        {
            replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_REUSE_LIF;
        }
        /** Can re-use lif, no need to update the lif mngr sw tables  */
        else
        {
            replace_mode = FLOW_LIF_MGMT_REPLACE_MODE_SAME_LIF_SIZE;
        }
    }

    /*
     * indications for new local lif that was allocated and new local lif with replace.
     * These indications will be useful later on
     */
    is_new_local_lif = ((!is_replace) || (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF));

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    FLOW_TIMER_SET("dnx_flow_lif_mgmt_store_valid_fields", flow_timer_idx);
    SHR_IF_ERR_EXIT(dnx_flow_lif_mgmt_store_valid_fields(unit,
                                                         flow_handle_info, FALSE, dbal_table_id, -1, entry_handle_id,
                                                         selected_result_type, *local_lif_id, 0, !is_new_local_lif));
    FLOW_TIMER_STOP(flow_timer_idx++);

    
    if (is_local_lif)
    {
        FLOW_TIMER_SET("dnx_lif_table_mngr_journal_update", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_journal_update(unit, *local_lif_id, 0, 0, 0, is_new_local_lif));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

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
                        (unit, _SHR_CORE_ALL, FALSE, (void *) lif_info, dbal_table_id, flow_handle_info,
                         entry_handle_id, selected_result_type, existing_result_type, *local_lif_id,
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
        && is_new_local_lif && is_local_lif)
    {
        lif_mapping_local_lif_payload_t local_lif_payload;
        sal_memset(&local_lif_payload, 0, sizeof(local_lif_payload));
        local_lif_payload.local_lifs_array.local_lifs[0] = *local_lif_id;

        FLOW_TIMER_SET("GLEM add/replace", flow_timer_idx);
        if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
        {
            /*
             * In case of replacing with new local lif: Update GLM and remove all mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, LIF_LIB_GLEM_KEEP_OLD_VALUE,
                             !is_verify_disabled));
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif));
        }
        else
        {
            /*
             * New lif: add new GLEM entry
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, !is_verify_disabled));
        }
        FLOW_TIMER_STOP(flow_timer_idx++);

        /** Add new global to local lif mapping */
        FLOW_TIMER_SET("Update GLEM mapping", flow_timer_idx);
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif, &local_lif_payload, !is_verify_disabled));
        FLOW_TIMER_STOP(flow_timer_idx++);
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry 
     * Not applicable for virtual LIFs 
     */
    if (replace_mode == FLOW_LIF_MGMT_REPLACE_MODE_NEW_LIF)
    {
        FLOW_TIMER_SET("Update new LIF", flow_timer_idx);

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

/** See .h file */
shr_error_e
dnx_flow_lif_mgmt_virtual_entry_on_handle_get(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id)
{
    dbal_tables_e dbal_table_id;
    uint32 field_ids_valid_bitmap[2];
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    CONST dbal_logical_table_t *table;
    local_lif_info_t entry_lif_info;
    uint32 retrieve_entry_handle_id, result_type;
    int field_idx;
    uint8 found;
    dbal_fields_e field_id;
    flow_lif_mgmt_virtual_match_type_e virtual_match_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Retrieve the virtual match key from SW and set it on the handler */
    SHR_IF_ERR_EXIT(dnx_flow_virtual_match_sw_get
                    (unit, virtual_lif_gport, entry_handle_id, &virtual_match_type, &result_type));

    /** Allocate a retrieve DBAL handle_id for the same table with the same key and retrieve the data fields */
    DBAL_HANDLE_COPY(unit, entry_handle_id, &retrieve_entry_handle_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, retrieve_entry_handle_id, DBAL_GET_ALL_FIELDS));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    /** Retrieve the valid fields bitmap from the SW State table */
    SHR_IF_ERR_EXIT(lif_table_mngr_db.
                    valid_fields_per_egress_virtual_htb.find(unit, &virtual_lif_gport, &entry_lif_info, &found));
    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Error: Failed to retrieve entry info for virtual LIG gport 0x%08X, found - %d",
                     virtual_lif_gport, found);
    }
    field_ids_valid_bitmap[0] = entry_lif_info.valid_fields;
    field_ids_valid_bitmap[1] = 0;

    /** Traverse the result-type fields and copy all encountered that are valid */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;
        if (field_id != DBAL_FIELD_RESULT_TYPE)
        {
            /** Check in the SW state if this field is valid */
            if (SHR_BITGET(field_ids_valid_bitmap, field_idx))
            {
                /** Field valid - copy its value from HW to the superset handle ??? */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, retrieve_entry_handle_id, field_id,
                                                                        INST_SINGLE, field_val));
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_flow_lif_mgmt_virtual_entry_clear(
    int unit,
    bcm_gport_t virtual_lif_gport,
    dbal_tables_e dbal_table_id)
{
    uint32 entry_handle_id;
    uint32 result_type;
    flow_lif_mgmt_virtual_match_type_e virtual_match_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Retrieve the virtual match key from SW and set it on the handler */
    SHR_IF_ERR_EXIT(dnx_flow_virtual_match_sw_get
                    (unit, virtual_lif_gport, entry_handle_id, &virtual_match_type, &result_type));

    /** Remove the virtual entry from DBAL */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Delete the valid sw state entry */
    SHR_IF_ERR_EXIT(lif_table_mngr_db.valid_fields_per_egress_virtual_htb.delete(unit, &virtual_lif_gport));

    /** Delete the SW mapping entry from virtual gport to the match keys and the result-type */
    SHR_IF_ERR_EXIT(dnx_flow_virtual_match_sw_clear(unit, virtual_lif_gport));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
