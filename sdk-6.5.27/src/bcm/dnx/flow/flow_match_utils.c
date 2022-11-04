/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow_match_utils.c
 *
 *  The file contains utility functions for flow match operations
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
#include <bcm_int/dnx/flow/flow.h>
#include <soc/dnx/dbal/dbal.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <bcm/types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_flow_lif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/auto_generated/dnx_flow_dispatch.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/flow/flow_virtual_lif_mgmt.h>
#include "flow_match_utils.h"

extern flow_special_field_info_t flow_special_fields_db[];
extern dnx_flow_app_config_t *dnx_flow_objects[];

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

extern shr_error_e dbal_entry_key_as_buffer_set(
    int unit,
    uint32 entry_handle_id,
    uint32 *key_buffer,
    int size_in_bits);

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

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
 * }
 */
/*************
 * FUNCTIONS *
 *************/
shr_error_e
dnx_flow_match_key_fields_get(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dbal_table_type_e table_type,
    bcm_flow_special_fields_t * key_special_fields,
    int nof_app_special_fields)
{
    int key_idx, nof_reduced_fields = 0;
    flow_special_field_info_t *curr_field;

    SHR_FUNC_INIT_VARS(unit);

/** Get each key, if the key has mask keeping the mask value and the field ID. at the end appending them to the array
 *  of special fields returning to the user */
    for (key_idx = 0; key_idx < nof_app_special_fields; key_idx++)
    {
        curr_field = &flow_special_fields_db[key_special_fields->special_fields[key_idx].field_id];

        if ((table_type == DBAL_TABLE_TYPE_TCAM)
            && dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED))
        {
            SHR_IF_ERR_EXIT(flow_special_fields_mask_field_value_get
                            (unit, entry_handle_id, curr_field, key_special_fields, key_idx));
        }
        else if (dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_IS_MASK) ||
                 dnx_flow_special_field_is_ind_set(unit, curr_field, FLOW_SPECIAL_FIELD_IND_MATCH_IGNORE_OPERATION))
        {
        /** do nothing */
            continue;
        }
        else if (curr_field->get != NULL)
        {
            int rv;

            rv = curr_field->get
                (unit, entry_handle_id, flow_handle_info, flow_app_info, NULL,
                 key_special_fields->special_fields[key_idx].field_id, &(key_special_fields->special_fields[key_idx]));
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_UNAVAIL);
            if (rv == _SHR_E_UNAVAIL)
            {
                continue;
            }
        }
        else
        {
            int rv;

            rv = flow_special_field_key_direct_mapping_get(unit, curr_field, entry_handle_id, table_type,
                                                           &(key_special_fields->special_fields[key_idx]));
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_UNAVAIL);
            if (rv == _SHR_E_UNAVAIL)
            {
                key_special_fields->special_fields[key_idx].field_id = 0;
                nof_reduced_fields++;
            }
        }
    }

    /** in case fields were reduced, need to shrink the array to return all the fields conscutive (without holes) */
    if (nof_reduced_fields)
    {
        int iter;
        for (key_idx = 0; key_idx < nof_app_special_fields - nof_reduced_fields; key_idx++)
        {
            /** we found a hole (field that is not set), lets replace with valid field */
            if (key_special_fields->special_fields[key_idx].field_id == 0)
            {
                for (iter = nof_app_special_fields - nof_reduced_fields; iter < nof_app_special_fields; iter++)
                {
                    if (key_special_fields->special_fields[iter].field_id != 0)
                    {
                        sal_memcpy(&key_special_fields->special_fields[key_idx],
                                   &key_special_fields->special_fields[iter], sizeof(bcm_flow_special_field_t));
                        sal_memset(&key_special_fields->special_fields[iter], 0, sizeof(bcm_flow_special_field_t));
                        break;
                    }
                }

                if (iter == nof_app_special_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "No field found to replace the hole pos %d\n", key_idx);
                }
            }
        }
        key_special_fields->actual_nof_special_fields -= nof_reduced_fields;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_match_entry_from_global_lif_get(
    int unit,
    int global_lif,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    int is_end, ii = 0;
    lif_mapping_local_lif_payload_t local_lif_payload;
    dnx_flow_app_config_t *flow_app_info;
    dbal_tables_e dbal_table;
    int max_nof_tables = FLOW_MAX_NOF_MATCH_DBAL_TABLES, bud_table_used = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FLOW_HANDLE_VALIDATE(flow_handle_info->flow_handle);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table = flow_app_info->flow_table[0];

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        /** not supported for multiple tables yet since the relevant dbal table is not known */
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported for MULTIPLE_DBAL_TABLES applications");
    }

    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags,
                         BCM_FLOW_HANDLE_INFO_BUD) && !dnx_flow_app_is_ind_set(unit,
                                                                               flow_app_info,
                                                                               FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
    {
        max_nof_tables++;
    }

    while ((dbal_table != DBAL_TABLE_EMPTY) && (ii < max_nof_tables))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                            (unit, DNX_ALGO_LIF_INGRESS, global_lif, &local_lif_payload));

            if (dbal_table != DBAL_TABLE_INGRESS_PP_PORT)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 local_lif_payload.local_lifs_array.local_lifs[0], 0));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                                (unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                 local_lif_payload.local_lifs_array.local_lifs[0], 0));
            }
        }
        else if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT_MATCH)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_global_to_local_get
                            (unit, DNX_ALGO_LIF_EGRESS, global_lif, &local_lif_payload));

            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE,
                                                                 DBAL_CONDITION_EQUAL_TO,
                                                                 local_lif_payload.local_lifs_array.local_lifs[0], 0));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flow handle (%s), only match applications supported ",
                         flow_app_info->app_name);
        }

        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        if (is_end)
        {
            /** no matching entry found */
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        else
        {
            int nof_app_special_fields = 0;
            dbal_table_type_e table_type;

            if (flow_app_info->is_entry_related_cb &&
                dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
            {
                int is_entry_related;

                SHR_IF_ERR_EXIT(flow_app_info->is_entry_related_cb(unit, entry_handle_id, &is_entry_related));
                if (!is_entry_related)
                {
                    /** no matching entry found */
                    continue;
                }
            }

            SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

            /** Get number of key special fields, serves as key for match applications Assign each key field id */
            while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
            {
                key_special_fields->special_fields[nof_app_special_fields].field_id =
                    flow_app_info->special_fields[nof_app_special_fields];
                nof_app_special_fields++;
            }

            key_special_fields->actual_nof_special_fields = nof_app_special_fields;

            SHR_IF_ERR_EXIT(dnx_flow_match_key_fields_get
                            (unit, entry_handle_id, flow_handle_info, flow_app_info, table_type, key_special_fields,
                             nof_app_special_fields));

            if (flow_app_info->dbal_table_to_key_field_map_cb)
            {
                /** used in traverse to map the dbal table to the key field. */
                SHR_IF_ERR_EXIT(flow_app_info->dbal_table_to_key_field_map_cb
                                (unit, entry_handle_id, flow_handle_info, FLOW_MAP_TYPE_TABLE_TO_KEY_FIELD,
                                 key_special_fields, &dbal_table));
            }

            /** If a specific application callback is defined - call it which traverse cmd */
            if (flow_app_info->app_specific_operations_cb)
            {
                dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_TRAVERSE, FLOW_COMMAND_CONTINUE };

                SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                                (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info,
                                 &flow_cmd_control, NULL, NULL, key_special_fields, NULL));
            }

        }
        if (bud_table_used && !dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
        {
            dbal_table = DBAL_TABLE_EMPTY;
        }
        else if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_BUD)
                 && (ii == FLOW_MAX_NOF_MATCH_DBAL_TABLES - 1))
        {
            if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
            {
                dbal_table = (dbal_tables_e) flow_app_info->second_pass_param;
                bud_table_used = 1;
            }
            else
            {
                dbal_table = DBAL_TABLE_EMPTY;
            }
        }
        else
        {
            dbal_table = flow_app_info->flow_table[++ii];
            if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_BUD) &&
                !dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD)
                && (dbal_table == DBAL_TABLE_EMPTY))
            {
                dbal_table = (dbal_tables_e) flow_app_info->second_pass_param;
                bud_table_used = 1;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_match_related_dbal_table_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * key_special_fields,
    dbal_tables_e * selected_dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        if (flow_app_info->dbal_table_to_key_field_map_cb)
        {
            SHR_IF_ERR_EXIT(flow_app_info->dbal_table_to_key_field_map_cb
                            (unit, 0, flow_handle_info, FLOW_MAP_TYPE_KEY_FIELD_TO_TABLE,
                             key_special_fields, selected_dbal_table));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MATCH with MULTIPLE_DBAL_TABLES with no cb function");
        }
    }
    else
    {
        (*selected_dbal_table) = flow_app_info->flow_table[0];
    }

    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD) &&
        (!dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD)))
    {
        (*selected_dbal_table) = (dbal_tables_e) flow_app_info->second_pass_param;
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 * Clear profiles used in specific applications.
 * Those profiles can't be cleared at app specific cb because they're required later.
 * use case:
 * - app specific cb deletes the profile
 * - the profile is part of the key and the key needs to be accessed
 *   when flow clears gport to match entry (see dnx_flow_gport_to_match_info_sw_delete).
 *   So here the profile can't be accessed.
 * Solution:
 * -  app specific cb doesn't delete the profile
 * - gport to match entry is cleared
 * - call this function to clear the profile for a dedicated application
 */
static shr_error_e
dnx_flow_match_application_profiles_clear(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_special_fields_t * key_special_fields)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/* Add match entry info by gport to a dedicated sw state
 * In case it is already full, only update the counter */
shr_error_e
dnx_flow_gport_to_match_info_sw_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id)
{
    uint8 found, success;
    ingress_gport_sw_info_t match_info;
    int match_idx = 0, nof_sw_match_entries_per_gport;
    dbal_entry_handle_t *entry_handle;
    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&match_info, 0, sizeof(ingress_gport_sw_info_t));

    gport = flow_handle_info->flow_id;

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.find(unit, &gport, &match_info, &found));

    nof_sw_match_entries_per_gport = dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit);

    if (match_info.counter < nof_sw_match_entries_per_gport)
    {
        /*
         * Find an empty spot 
         */
        for (match_idx = 0; match_idx < nof_sw_match_entries_per_gport; match_idx++)
        {
            if (match_info.single_match_info[match_idx].dbal_table == DBAL_TABLE_EMPTY)
            {
                break;
            }
        }
        if (match_idx >= nof_sw_match_entries_per_gport)
        {
            SHR_EXIT();
        }

        if (DBAL_TABLE_IS_TCAM(entry_handle->table))
        {
            uint32 entry_access_id;

            SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

            match_info.single_match_info[match_idx].match_key[0] = entry_access_id;
        }
        else
        {
            sal_memcpy(match_info.single_match_info[match_idx].match_key, entry_handle->phy_entry.key,
                       dnx_data_flow.general.match_entry_info_max_key_size_in_words_get(unit) * sizeof(uint32));
        }

        match_info.single_match_info[match_idx].dbal_table = entry_handle->table_id;
    }

    match_info.counter++;

    SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.insert(unit, &gport, &match_info, &success));

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error, failed updating sw state table for gport info for gport 0x%x",
                     gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief
 *  Function set the key fields for entry according to the key_special_fields. for TCAM match table there is a dedicated
 *  logic for mask fields, for those fields, first finding all the mask and maskable fields pos in
 *  key_special_fields, than looking for matching pairs and eventually setting them to DBAL */
/*
 * Internal match add entry
 */
shr_error_e
dnx_flow_match_info_add(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    uint32 lif_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    uint32 entry_access_id;
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    int lif_per_core = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FLOW_LOGGER_PRINT(unit, flow_handle_info, flow_app_info, flow_cmd_control, NULL, key_special_fields);

    SHR_IF_ERR_EXIT(dnx_flow_match_related_dbal_table_get(unit,
                                                          flow_handle_info,
                                                          flow_app_info, key_special_fields, &dbal_table));

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** Create entry access id to TCAM */
    if (table_type == DBAL_TABLE_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_set
                        (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, flow_handle_info->flow_priority));
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
    }
    else
    {
        lif_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, flow_handle_info->flow_id, lif_flags, &gport_hw_resources));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit, entry_handle_id,
                                                flow_handle_info,
                                                flow_app_info, &gport_hw_resources, key_special_fields));

    /** Setting BUD field */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, (dbal_fields_e) flow_app_info->second_pass_param,
                                   _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD));
    }

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
    {
        if (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc) &&
            (gport_hw_resources.local_in_lif_per_core.core_id[0] != _SHR_CORE_ALL))
        {
            lif_per_core = TRUE;
        }
        else
        {
            if (dbal_table != DBAL_TABLE_INGRESS_PP_PORT)
            {
                if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC)
                    || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS))
                {
                    /** The IPv6 TCAM tables has  field called IPv6_DIP_IDX_DATA */
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE,
                                                 gport_hw_resources.local_in_lif);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE,
                                                 DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE);
                }
                else
                {
                    /**  The INGRESS_PP_PORT has a different In-LIF field name - DEFAULT_LIF, this is handled in the specific app cb */
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                                 gport_hw_resources.local_in_lif);
                }
            }
        }
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE,
                                     gport_hw_resources.local_out_lif);
    }

    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control,
                         &gport_hw_resources, NULL, key_special_fields, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    if ((dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit) > 1) &&
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH))
    {
        /*
         * For replace - need to remove the old entry 
         */
        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            uint32 delete_entry_handle_id;
            bcm_flow_handle_info_t delete_flow_handle_info = { 0 };
            dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
            dbal_core_mode_e core_mode = DBAL_CORE_NONE;

            DBAL_HANDLE_COPY(unit, entry_handle_id, &delete_entry_handle_id);

            sal_memcpy(&delete_flow_handle_info, flow_handle_info, sizeof(bcm_flow_handle_info_t));

            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                            (unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), DBAL_PHY_DB_DEFAULT_INDEX,
                             &physical_table_id));
            core_mode = dbal_mdb_phy_table_core_mode_get(unit, physical_table_id);

            if (core_mode == DBAL_CORE_MODE_DPC)
            {
                if (lif_per_core)
                {
                    /*
                     * It's enough to take the first 
                     */
                    dbal_entry_key_field32_set(unit, delete_entry_handle_id, DBAL_FIELD_CORE_ID,
                                               gport_hw_resources.local_in_lif_per_core.core_id[0]);
                }
                else
                {
                    dbal_entry_key_field32_set(unit, delete_entry_handle_id, DBAL_FIELD_CORE_ID, 0);
                }
            }

            SHR_IF_ERR_EXIT(dnx_flow_gport_to_match_info_sw_delete
                            (unit, &delete_flow_handle_info, delete_entry_handle_id, key_special_fields));
        }
    }

    /** Check if errors occurred on handle, using DBAL validation to make sure that the user values are correct. */
    SHR_IF_ERR_EXIT(dnx_flow_commit_error_check(unit, entry_handle_id));

    if (!lif_per_core)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
        {
            if (table_type == DBAL_TABLE_TYPE_TCAM)
            {
                SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
                SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
    else
    {
        int core_iter;

        for (core_iter = 0; core_iter < gport_hw_resources.local_in_lif_per_core.nof_in_lifs; core_iter++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                       gport_hw_resources.local_in_lif_per_core.core_id[core_iter]);

            if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC) || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS))
            {
                /** The IPv6 TCAM tables has  field called IPv6_DIP_IDX_DATA */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE,
                                             gport_hw_resources.local_in_lif_per_core.local_in_lif[core_iter]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_TYPE, INST_SINGLE,
                                             DBAL_ENUM_FVAL_IPV6_DIP_IDX_TYPE_LIF_ELIGIBLE);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                                             gport_hw_resources.local_in_lif_per_core.local_in_lif[core_iter]);
            }
            if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_REPLACE))
            {
                if (table_type == DBAL_TABLE_TYPE_TCAM)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get
                                    (unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
                }
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    if ((dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit) > 1) &&
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH))
    {
        SHR_IF_ERR_EXIT(dnx_flow_gport_to_match_info_sw_add(unit, flow_handle_info, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Internal match entry get */
shr_error_e
dnx_flow_match_info_get(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    uint32 local_lif;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    int is_terminator = (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) ? TRUE : FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FLOW_LOGGER_PRINT(unit, flow_handle_info, flow_app_info, flow_cmd_control, NULL, key_special_fields);

    SHR_IF_ERR_EXIT(dnx_flow_match_related_dbal_table_get(unit,
                                                          flow_handle_info,
                                                          flow_app_info, key_special_fields, &dbal_table));
    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /** setting the key of the entry */
    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit,
                                                entry_handle_id,
                                                flow_handle_info, flow_app_info, NULL, key_special_fields));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table, &core_mode));
    if ((core_mode == DBAL_CORE_MODE_DPC) && !(dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)))
    {
        /** setting the core_id to default (get requires to be done on only one of the cores) */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    /** Setting BUD field */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, (dbal_fields_e) flow_app_info->second_pass_param,
                                   _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD));
    }

    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control, NULL,
                         NULL, key_special_fields, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    if (table_type == DBAL_TABLE_TYPE_TCAM)
    {
        uint32 tcam_handler_id, entry_access_id;
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_access_id, &flow_handle_info->flow_priority));
    }

    /** in case of entry was added not to all the cores need to loop over all cores and see if entry exists once found,
     *  need to return the related global LIF */
    if ((core_mode == DBAL_CORE_MODE_DPC) && is_terminator &&
        dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc))
    {
        int rv, core_iter, entry_found = 0;
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core_iter = 0; core_iter < nof_cores; core_iter++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            if (rv == _SHR_E_NONE)
            {

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_lif));

                /** Retrieve the flow id from the local lif */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_flow_gport_from_local_lif(unit, local_lif, TRUE, core_iter,
                                                                       &(flow_handle_info->flow_id)));
                entry_found = 1;
                break;
            }
        }

        if (!entry_found)
        {
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS),
                                           _SHR_E_NOT_FOUND);
        if (is_terminator)
        {
            if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC) || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS))
            {
                /** The IPv6 TCAM tables has  field called IPv6_DIP_IDX_DATA */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE, &local_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, &local_lif));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, &local_lif));
        }

        /** Retrieve the flow id from the local lif */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_flow_gport_from_local_lif(unit, local_lif, is_terminator, DBAL_CORE_DEFAULT,
                                                               &(flow_handle_info->flow_id)));
    }

    if (flow_app_info->is_entry_related_cb &&
        dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
    {
        int is_entry_related;

        SHR_IF_ERR_EXIT(flow_app_info->is_entry_related_cb(unit, entry_handle_id, &is_entry_related));
        if (!is_entry_related)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found for related key \n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_gport_to_match_entry_from_existing_table(
    int unit,
    bcm_gport_t gport,
    ingress_gport_sw_info_t * match_info,
    int match_info_idx,
    dbal_tables_e dbal_table,
    const dnx_flow_app_config_t * match_flow_app_info,
    uint8 *entry_updated)
{
    uint32 entry_handle_id, current_lif;
    int is_end;
    dbal_fields_e field_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *entry_updated = FALSE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    field_id = (dbal_table == DBAL_TABLE_INGRESS_PP_PORT) ? DBAL_FIELD_DEFAULT_LIF : DBAL_FIELD_IN_LIF;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, &gport_hw_resources));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, &current_lif));

        if (current_lif == gport_hw_resources.local_in_lif)
        {
            CONST dbal_logical_table_t *table;
            uint32 key_buffer[DNX_DATA_MAX_FLOW_GENERAL_MATCH_ENTRY_INFO_MAX_KEY_SIZE_IN_WORDS] = { 0 };
            uint32 entry_access_id;
            int match_idx;
            uint8 entry_exists_in_sw = FALSE;

            if (match_flow_app_info->is_entry_related_cb &&
                dnx_flow_app_is_ind_set(unit, match_flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
            {
                int is_entry_related;

                SHR_IF_ERR_EXIT(match_flow_app_info->is_entry_related_cb(unit, entry_handle_id, &is_entry_related));
                if (!is_entry_related)
                {
                    /** Receive next entry in table.*/
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    continue;
                }
            }

            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table, &table));

            if (DBAL_TABLE_IS_TCAM(table))
            {
                SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));
            }
            else
            {
                dbal_entry_handle_t *entry_handle;

                SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
                sal_memcpy(key_buffer, entry_handle->phy_entry.key,
                           dnx_data_flow.general.match_entry_info_max_key_size_in_words_get(unit) * sizeof(uint32));
            }

            for (match_idx = 0; match_idx < dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit); match_idx++)
            {
                if (match_info->single_match_info[match_idx].dbal_table == dbal_table)
                {
                    if ((DBAL_TABLE_IS_TCAM(table))
                        && (match_info->single_match_info[match_idx].match_key[0] != entry_access_id))
                    {
                        entry_exists_in_sw = TRUE;
                        break;
                    }
                    else if ((!DBAL_TABLE_IS_TCAM(table))
                             &&
                             (!sal_memcmp
                              (match_info->single_match_info[match_idx].match_key, key_buffer, sizeof(key_buffer))))
                    {
                        entry_exists_in_sw = TRUE;
                        break;
                    }
                }
            }

            if (!entry_exists_in_sw)
            {
                match_info->single_match_info[match_info_idx].dbal_table = dbal_table;

                if (DBAL_TABLE_IS_TCAM(table))
                {
                    match_info->single_match_info[match_info_idx].match_key[0] = entry_access_id;
                }
                else
                {
                    sal_memcpy(match_info->single_match_info[match_info_idx].match_key,
                               key_buffer,
                               dnx_data_flow.general.match_entry_info_max_key_size_in_words_get(unit) * sizeof(uint32));
                }
                *entry_updated = TRUE;
                SHR_EXIT();
            }
        }
        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* Traverse to find an entry to add in a given index */
static shr_error_e
dnx_flow_gport_to_match_entry_add_from_existing_entries(
    int unit,
    bcm_gport_t gport,
    ingress_gport_sw_info_t * match_info,
    int match_info_idx)
{
    int flow_app_idx, lif_app_idx, flow_number_of_apps;
    bcm_flow_handle_t flow_handle_id = -1;
    const dnx_flow_app_config_t *flow_app_info;
    uint8 entry_updated = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * First - find the non-match application which is related to this gport
     */
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_encoded_gport_get(unit, gport, &flow_handle_id));

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_id);

    flow_number_of_apps = dnx_flow_number_of_apps();

    /*
     * Run over the flow apps
     */
    for (flow_app_idx = FLOW_APP_FIRST_IDX; flow_app_idx < flow_number_of_apps; flow_app_idx++)
    {
        const dnx_flow_app_config_t *match_flow_app_info = dnx_flow_app_info_get(unit, flow_app_idx);
        if (match_flow_app_info->flow_app_type != FLOW_APP_TYPE_TERM_MATCH)
        {
            continue;
        }

        /*
         * Run over the match related init or term apps
         */
        for (lif_app_idx = 0; lif_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; lif_app_idx++)
        {
            if (!sal_strncasecmp
                (match_flow_app_info->match_payload_apps[lif_app_idx], flow_app_info->app_name, FLOW_STR_MAX))
            {
                int table_idx;

                /*
                 * Traverse over the entries, look for the requested gport
                 */
                for (table_idx = 0; table_idx < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_idx++)
                {
                    dbal_tables_e curr_table = match_flow_app_info->flow_table[table_idx];

                    if (curr_table == DBAL_TABLE_EMPTY)
                    {
                        break;
                    }
                    SHR_IF_ERR_EXIT(dnx_flow_gport_to_match_entry_from_existing_table
                                    (unit, gport, match_info, match_info_idx, curr_table, match_flow_app_info,
                                     &entry_updated));

                    /*
                     * If entry was found and updated - break
                     */
                    if (entry_updated)
                    {
                        SHR_EXIT();
                    }
                }
            }
        }
    }

    if (!entry_updated)
    {
        sal_memset(&match_info->single_match_info[match_info_idx], 0, sizeof(match_entry_info_t));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* Delete a match entry info by gport from sw state */
shr_error_e
dnx_flow_gport_to_match_info_sw_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint8 found, entry_found = FALSE;
    ingress_gport_sw_info_t match_info;
    int match_idx = 0;
    dbal_entry_handle_t *entry_handle;
    bcm_gport_t gport;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_GET, FLOW_COMMAND_CONTINUE };
    uint8 success = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&match_info, 0, sizeof(ingress_gport_sw_info_t));

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    SHR_IF_ERR_EXIT(dnx_flow_match_info_get(unit, flow_handle_info, &flow_cmd_control, key_special_fields));

    gport = flow_handle_info->flow_id;

    SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.find(unit, &gport, &match_info, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Couldn't find relevant entry for gport 0x%x", gport);
    }

    for (match_idx = 0; match_idx < dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit); match_idx++)
    {
        if (match_info.single_match_info[match_idx].dbal_table == entry_handle->table_id)
        {
            if (DBAL_TABLE_IS_TCAM(entry_handle->table))
            {
                uint32 entry_access_id;

                SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT));

                if (match_info.single_match_info[match_idx].match_key[0] == entry_access_id)
                {
                    entry_found = TRUE;
                    break;
                }
            }
            else
            {
                if (!sal_memcmp
                    (entry_handle->phy_entry.key, match_info.single_match_info[match_idx].match_key,
                     sizeof(match_info.single_match_info[match_idx].match_key)))
                {
                    entry_found = TRUE;
                    break;
                }
            }
        }
    }

    match_info.counter--;

    if (entry_found)
    {
        /*
         * If it was the last entry in the match info - delete the entry completely
         */
        if (match_info.counter == 0)
        {
            SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.delete(unit, &gport));
            success = 1;
        }
        else
        {
            /*
             * If the counter exceeded the max kept match info entries per gport, iterate and fill the empty slot with
             * another entry
             */
            if (match_info.counter >= dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_flow_gport_to_match_entry_add_from_existing_entries
                                (unit, gport, &match_info, match_idx));
            }
            /*
             * Simply zero the deleted entry
             */
            else
            {
                sal_memset(&match_info.single_match_info[match_idx], 0, sizeof(match_entry_info_t));
            }

            SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.insert(unit, &gport, &match_info, &success));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.insert(unit, &gport, &match_info, &success));
    }

    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error, failed to remove entry related to gport 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Internal match entry delete
 */
shr_error_e
dnx_flow_match_info_delete(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    bcm_flow_special_fields_t * key_special_fields)
{
    uint32 entry_handle_id;
    dnx_flow_app_config_t *flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];
    dbal_table_type_e table_type;
    dbal_tables_e dbal_table = DBAL_TABLE_EMPTY;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FLOW_LOGGER_PRINT(unit, flow_handle_info, flow_app_info, flow_cmd_control, NULL, key_special_fields);

    SHR_IF_ERR_EXIT(dnx_flow_match_related_dbal_table_get(unit,
                                                          flow_handle_info,
                                                          flow_app_info, key_special_fields, &dbal_table));

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table, &core_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_flow_special_fields_set(unit,
                                                entry_handle_id,
                                                flow_handle_info, flow_app_info, NULL, key_special_fields));

    /** Setting BUD field */
    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, (dbal_fields_e) flow_app_info->second_pass_param,
                                   _SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_BUD));
    }

    /** If a specific application callback is defined - call it */
    if (flow_app_info->app_specific_operations_cb)
    {
        SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                        (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info, flow_cmd_control, NULL,
                         NULL, key_special_fields, NULL));
        if (flow_cmd_control->flow_command != FLOW_COMMAND_CONTINUE)
        {
            SHR_EXIT();
        }
    }

    if ((dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit) > 1) &&
        (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH))
    {
        SHR_IF_ERR_EXIT(dnx_flow_gport_to_match_info_sw_delete
                        (unit, flow_handle_info, entry_handle_id, key_special_fields));
    }

    SHR_IF_ERR_EXIT(dnx_flow_match_application_profiles_clear
                    (unit, entry_handle_id, flow_app_info, key_special_fields));

    {
        int core_iter;
        int nof_cores = (core_mode == DBAL_CORE_MODE_SBC || (dnx_data_lif.in_lif.feature_get(unit,
                                                                                             dnx_data_lif_in_lif_phy_db_dpc)))
            ? 1 : dnx_data_device.general.nof_cores_get(unit);
        int entry_clear_rv = 0;
        int entry_cleared = 0;

        for (core_iter = 0; core_iter < nof_cores; core_iter++)
        {
            if (nof_cores == 1)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_iter);
            }
            if (table_type == DBAL_TABLE_TYPE_TCAM)
            {
                int rv;
                uint32 entry_access_id;
                rv = dbal_entry_access_id_by_key_get(unit, entry_handle_id, &entry_access_id, DBAL_COMMIT);

                if (rv == _SHR_E_NONE)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
                    /** Delete entry from DBAL table*/
                    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
                    entry_cleared = 1;
                }
            }
            else
            {
                entry_clear_rv |= dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
                if (entry_clear_rv == _SHR_E_NONE)
                {

                    entry_cleared = 1;
                }
            }
        }

        if (!entry_cleared)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry not found for related key \n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** returns is_designated true according to predefine logic, this function is only valid for applications that are
 *  using the indication FLOW_APP_TERM_MATCH_IND_PER_CORE. there are two options,
 *  a) user entry is mapped to one HW entry (in case of port) so in this case the entry is always designated
 *  b) user entry is mapped to multiple HW entries, (such as vlan domain or LAG) in this case the function should return
 *  true only for the designated entry
 *
 *  in case no indication for PER core we expect that for each user entry there will be added num_of_cores entries,
 *  and the iterator will run only over the default core entries */
static shr_error_e
dnx_flow_match_is_entry_designated(
    int unit,
    int core_id,
    dnx_flow_app_config_t * flow_app_info,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    int global_lif,
    int *is_designated)
{
    int port_found = 0;
    uint32 field_value = 0;
    int idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    (*is_designated) = 0;

    while (flow_app_info->special_fields[idx] != FLOW_S_F_EMPTY)
    {
        if (flow_app_info->special_fields[idx] == FLOW_S_F_IN_PORT)
        {
            uint32 pp_port;
            bcm_port_t phy_port_gport;
            int valid;

            port_found = 1;
            /** Retrieve the PP-Port */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit,
                                                                  entry_handle_id, DBAL_FIELD_IN_PP_PORT, &pp_port));

            /** Convert to a logical Port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.ref_count.get(unit, core_id, pp_port, &valid));

            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_gport_get(unit, core_id, pp_port, &phy_port_gport));
                field_value = phy_port_gport;
                break;
            }
            else
            {
                port_found = 0;
            }
        }
        idx++;
    }

    if (port_found)
    {
        dnx_algo_gpm_gport_phy_info_t gport_info;
        uint32 pp_port_index;
        uint32 min_core_id;

        /** Convert the local port to a list of PP-Port + Core-ID */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, field_value, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        min_core_id = gport_info.internal_port_pp_info.core_id[0];
        for (pp_port_index = 1; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            if (min_core_id > gport_info.internal_port_pp_info.core_id[pp_port_index])
            {
                min_core_id = gport_info.internal_port_pp_info.core_id[pp_port_index];
            }
        }

        if (min_core_id == core_id)
        {
            (*is_designated) = 1;
        }
    }
    else
    {
        
        (*is_designated) = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_match_info_traverse_per_table(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation,
    dbal_tables_e dbal_table,
    bcm_flow_special_fields_t * key_special_fields,
    int bud_table_in_use)
{
    dbal_table_type_e table_type;
    dbal_physical_tables_e physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;
    uint32 entry_handle_id;
    int is_end, global_lif, local_lif, is_phy_dpc =
        dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc), is_lif_dpc = 0;
    lif_mapping_local_lif_key_t local_lif_info;
    dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_TRAVERSE, FLOW_COMMAND_CONTINUE };
    uint32 core_id = DBAL_CORE_DEFAULT;
    uint32 orig_flags = flow_handle_info->flags;
    bcm_flow_special_fields_t orig_key_special_fields = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** keeping the original special fields, in case changed we will be able to restore */
    sal_memcpy(&orig_key_special_fields, key_special_fields, sizeof(bcm_flow_special_fields_t));

    if (is_phy_dpc)
    {
    /** lif is dpc if the app is DPC and the system is dpc */
        is_lif_dpc = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_PER_CORE);
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, dbal_table, &table_type));
    if (table_type != DBAL_TABLE_TYPE_DIRECT)
    {
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, dbal_table, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
        core_mode = dbal_mdb_phy_table_core_mode_get(unit, physical_table_id);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    if (!is_lif_dpc)
    {
        if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) && (core_mode != DBAL_CORE_MODE_SBC)
            && (dbal_table != DBAL_TABLE_INGRESS_PP_PORT))
        {
            /** Add KEY rule to skip duplicated entry */
            SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                                   DBAL_CONDITION_EQUAL_TO, &core_id, NULL));
        }
    }

    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        int rv;

        if (key_special_fields->actual_nof_special_fields != orig_key_special_fields.actual_nof_special_fields)
        {
            sal_memcpy(key_special_fields, &orig_key_special_fields, sizeof(bcm_flow_special_fields_t));
        }

        if (flow_app_info->is_entry_related_cb &&
            dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
        {
            int is_entry_related;

            SHR_IF_ERR_EXIT(flow_app_info->is_entry_related_cb(unit, entry_handle_id, &is_entry_related));
            if (!is_entry_related)
            {
            /** Receive next entry in table.*/
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dnx_flow_match_key_fields_get
                        (unit, entry_handle_id, flow_handle_info, flow_app_info, table_type, key_special_fields,
                         key_special_fields->actual_nof_special_fields));

        /** Getting BUD field  value and updating the flag   */
        if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD))
        {
            uint32 second_pass_val;

            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, (dbal_fields_e) flow_app_info->second_pass_param,
                             &second_pass_val));
            if (second_pass_val)
            {
                flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_BUD;
            }
        }

        if (flow_app_info->dbal_table_to_key_field_map_cb)
        {
            /** used in traverse to map the dbal table to the key field. */
            SHR_IF_ERR_EXIT(flow_app_info->dbal_table_to_key_field_map_cb
                            (unit, entry_handle_id, flow_handle_info, FLOW_MAP_TYPE_TABLE_TO_KEY_FIELD,
                             key_special_fields, &dbal_table));
        }
        /*
         * If a specific application callback is defined - call it
         */
        if (flow_app_info->app_specific_operations_cb)
        {
            SHR_IF_ERR_EXIT(flow_app_info->app_specific_operations_cb
                            (unit, entry_handle_id, FLOW_ENTRY_HANDLE_ID_INVALID, flow_handle_info,
                             &flow_cmd_control, NULL, NULL, key_special_fields, NULL));
        }

        if (flow_cmd_control.flow_command == FLOW_COMMAND_CONTINUE)
        {
            if (flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH)
            {
                if ((dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC)
                    || (dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS))
                {
                    /** The IPv6 TCAM tables has  field called IPv6_DIP_IDX_DATA */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_IPV6_DIP_IDX_DATA, INST_SINGLE,
                                     (uint32 *) &local_lif));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE, (uint32 *) &local_lif));
                }
                if (!is_phy_dpc)
                {
                    local_lif_info.phy_table = (physical_table_id == DBAL_PHYSICAL_TABLE_ISEM_2) ?
                        DBAL_PHYSICAL_TABLE_INLIF_2 : DBAL_PHYSICAL_TABLE_INLIF_1;
                }
                local_lif_info.local_lif = local_lif;
                local_lif_info.core_id = core_id;
                rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                if (rv == _SHR_E_NONE)
                {
                    BCM_GPORT_SUB_TYPE_IN_LIF_SET(flow_handle_info->flow_id, global_lif);
                }
                else
                {
                    int virtual_id = 0;
                    rv = dnx_flow_ingress_virtual_gport_get(unit, local_lif, core_id, &virtual_id);
                    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                    if (rv == _SHR_E_NONE)
                    {
                        BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_SET(flow_handle_info->flow_id, virtual_id);
                    }
                    else
                    {
                        LOG_WARN_EX(BSL_LOG_MODULE,
                                    "Travrese encountered a Local-LIF (0x%x) that has no gport association%s%s%s\n",
                                    local_lif, EMPTY, EMPTY, EMPTY);
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    }
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, (uint32 *) &local_lif));
                local_lif_info.local_lif = local_lif;
                local_lif_info.core_id = core_id;
                rv = dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
                if (rv == _SHR_E_NONE)
                {
                    BCM_GPORT_SUB_TYPE_OUT_LIF_SET(flow_handle_info->flow_id, global_lif);
                }
            }
            BCM_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, flow_handle_info->flow_id);
        }
        if (flow_cmd_control.flow_command != FLOW_COMMAND_SKIP_TO_END)
        {
            if (traverse_operation == FLOW_TRAVERSE_OPERATION_USE_CALLBACK)
            {
                int is_designated = 1;

                /** Invoke the callback function */
                if ((flow_app_info->flow_app_type == FLOW_APP_TYPE_TERM_MATCH) && (core_mode != DBAL_CORE_MODE_SBC)
                    && (dbal_table != DBAL_TABLE_INGRESS_PP_PORT) && (is_lif_dpc))
                {
                    uint32 core_id;
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, &core_id));

                    SHR_IF_ERR_EXIT(dnx_flow_match_is_entry_designated
                                    (unit, core_id, flow_app_info, entry_handle_id, flow_handle_info, global_lif,
                                     &is_designated));
                }

                if (is_designated)
                {
                    if (bud_table_in_use)
                    {
                        flow_handle_info->flags |= BCM_FLOW_HANDLE_INFO_BUD;
                    }
                    SHR_IF_ERR_EXIT((*cb) (unit, flow_handle_info, key_special_fields, user_data));
                    flow_handle_info->flags = orig_flags;
                }
            }

            if (traverse_operation == FLOW_TRAVERSE_OPERATION_DELETE)
            {
                dnx_flow_cmd_control_info_t flow_cmd_control = { FLOW_CB_TYPE_DELETE, FLOW_COMMAND_CONTINUE };

                SHR_IF_ERR_EXIT(dnx_flow_match_info_delete
                                (unit, flow_handle_info, &flow_cmd_control, key_special_fields));
                flow_handle_info->flags = orig_flags;
            }
        }

        /** Receive next entry in table.*/
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    flow_handle_info->flags = orig_flags;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_match_info_traverse(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_match_info_traverse_cb cb,
    void *user_data,
    dnx_flow_traverse_operation_e traverse_operation)
{
    bcm_flow_special_fields_t key_special_fields = { 0 };
    int table_idx = 0;
    dnx_flow_app_config_t *flow_app_info;
    int nof_app_special_fields = 0;
    dbal_tables_e dbal_table;
    int bud_table_used = 0, is_bud_uses_extra_table = 0;
    int nof_regular_tables = 1;

    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_objects[flow_handle_info->flow_handle];

    dbal_table = flow_app_info->flow_table[table_idx];

    if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, BCM_FLOW_HANDLE_INFO_BUD)
        && (!dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD)))
    {
        is_bud_uses_extra_table = 1;
    }

    if (dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES))
    {
        while ((nof_regular_tables < FLOW_MAX_NOF_MATCH_DBAL_TABLES) &&
               (flow_app_info->flow_table[nof_regular_tables] != DBAL_TABLE_EMPTY))
        {
            nof_regular_tables++;
        }
    }

     /** Get number of key special fields, serves as key for match applications Assign each key field id */
    while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
    {
        key_special_fields.special_fields[nof_app_special_fields].field_id =
            flow_app_info->special_fields[nof_app_special_fields];
        nof_app_special_fields++;
    }

    key_special_fields.actual_nof_special_fields = nof_app_special_fields;

    while (dbal_table != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(dnx_flow_match_info_traverse_per_table
                        (unit, flow_handle_info, flow_app_info, cb, user_data, traverse_operation, dbal_table,
                         &key_special_fields, bud_table_used));

       /** selecting the next table to handle, should run over all valid tables and BUD table if exists */
        table_idx++;
        if (bud_table_used)
        {
            break;
        }

        if (table_idx == nof_regular_tables)
        {
            if (!is_bud_uses_extra_table)
            {
               /** all tables traversed - exit */
                break;
            }
            else
            {
               /** need to traverse also the BUD table */
                bud_table_used = 1;
                dbal_table = (dbal_tables_e) flow_app_info->second_pass_param;
            }
        }
        else
        {
            dbal_table = flow_app_info->flow_table[table_idx];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* Match traverse cb, used by dnx_flow_match_info_by_gport_multi_get_using_traverse(). If entry gport matches the required gport - update relevant data */
static int
flow_match_info_by_gport_entry_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields,
    void *user_data)
{
    dnx_flow_match_info_by_gport_traverse_data_t *traverse_flow_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve prt_control_t from the user_data
     */
    traverse_flow_data = (dnx_flow_match_info_by_gport_traverse_data_t *) user_data;

    if (traverse_flow_data->gport != flow_handle_info->flow_id)
    {
        /*
         * We're only interested in entries with a matching gport
         */
        SHR_EXIT();
    }

    if (traverse_flow_data->nof_entries >= traverse_flow_data->max_size)
    {
        /*
         * No room left to dump entries, exit with what we've got
         */
        SHR_EXIT();
    }
    sal_memcpy(&traverse_flow_data->match_array[traverse_flow_data->nof_entries].key_special_fields,
               key_special_fields, sizeof(bcm_flow_special_fields_t));
    sal_memcpy(&traverse_flow_data->match_array[traverse_flow_data->nof_entries].flow_handle_info,
               flow_handle_info, sizeof(bcm_flow_handle_info_t));
    traverse_flow_data->nof_entries++;

exit:
    SHR_FUNC_EXIT;
}

/* Get match entries which payload is a given gport by traverse */
static shr_error_e
dnx_flow_match_info_by_gport_multi_get_using_traverse(
    int unit,
    bcm_gport_t gport,
    int size,
    bcm_flow_match_entry_info_t * match_array)
{
    int flow_app_idx, lif_app_idx, flow_number_of_apps;
    bcm_flow_handle_info_t flow_handle_info = { 0 };
    bcm_flow_handle_t flow_handle_id = -1;
    const dnx_flow_app_config_t *flow_app_info;
    dnx_flow_match_info_by_gport_traverse_data_t traverse_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * First - find the non-match application which is related to this gport
     */
    SHR_IF_ERR_EXIT(flow_lif_flow_app_from_encoded_gport_get(unit, gport, &flow_handle_id));

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_id);

    flow_number_of_apps = dnx_flow_number_of_apps();

    traverse_data.gport = gport;
    traverse_data.max_size = size;
    traverse_data.nof_entries = 0;
    traverse_data.match_array = match_array;

    /*
     * Run over the flow apps
     */
    for (flow_app_idx = FLOW_APP_FIRST_IDX; flow_app_idx < flow_number_of_apps; flow_app_idx++)
    {
        const dnx_flow_app_config_t *match_flow_app_info = dnx_flow_app_info_get(unit, flow_app_idx);
        if (match_flow_app_info->flow_app_type != FLOW_APP_TYPE_TERM_MATCH)
        {
            continue;
        }

        /*
         * Run over the match related lif apps
         */
        for (lif_app_idx = 0; lif_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; lif_app_idx++)
        {
            if (!sal_strncasecmp
                (match_flow_app_info->match_payload_apps[lif_app_idx], flow_app_info->app_name, FLOW_STR_MAX))
            {
                int table_idx;

                SHR_IF_ERR_EXIT(bcm_flow_handle_get
                                (unit, match_flow_app_info->app_name, &flow_handle_info.flow_handle));

                /*
                 * Traverse over the entries, look for the requested gport
                 */
                for (table_idx = 0; table_idx < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_idx++)
                {
                    dbal_tables_e curr_table = match_flow_app_info->flow_table[table_idx];

                    if (curr_table == DBAL_TABLE_EMPTY)
                    {
                        break;
                    }

                    SHR_IF_ERR_EXIT(bcm_flow_match_info_traverse
                                    (unit, &flow_handle_info, flow_match_info_by_gport_entry_cb, &traverse_data));
                    if (traverse_data.nof_entries >= traverse_data.max_size)
                    {
                        SHR_EXIT();
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* Given a gport, match info entry - find the related flow application:
 * Traverse all match applications. If there's more than one table pointing to the match info table, check if it is related.
 * If related, or if it's the only appliaction pointing at it - then application was found. */
static shr_error_e
dnx_flow_match_flow_handle_by_entry_get(
    int unit,
    bcm_gport_t gport,
    match_entry_info_t * match_info,
    bcm_flow_handle_t * flow_handle)
{
    int flow_app_idx, flow_number_of_apps, table_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    flow_number_of_apps = dnx_flow_number_of_apps();

    /*
     * Run over the flow apps
     */
    for (flow_app_idx = FLOW_APP_FIRST_IDX; flow_app_idx < flow_number_of_apps; flow_app_idx++)
    {
        const dnx_flow_app_config_t *match_flow_app_info = dnx_flow_app_info_get(unit, flow_app_idx);
        if (match_flow_app_info->flow_app_type != FLOW_APP_TYPE_TERM_MATCH)
        {
            continue;
        }

        for (table_idx = 0; table_idx < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_idx++)
        {
            dbal_tables_e curr_table = match_flow_app_info->flow_table[table_idx];

            if (curr_table == DBAL_TABLE_EMPTY)
            {
                break;
            }

            if (curr_table != match_info->dbal_table)
            {
                continue;
            }

            if (match_flow_app_info->is_entry_related_cb &&
                dnx_flow_app_is_ind_set(unit, match_flow_app_info, FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE))
            {
                int is_entry_related;
                uint32 entry_handle_id;
                CONST dbal_logical_table_t *table;
                dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

                SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, curr_table, &table));

                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, curr_table, &entry_handle_id));

                if (DBAL_TABLE_IS_TCAM(table))
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, match_info->match_key[0]));
                }
                else
                {
                   /** setting the HW entry Key */
                    SHR_IF_ERR_EXIT(dbal_entry_key_as_buffer_set
                                    (unit, entry_handle_id, match_info->match_key, table->key_size));
                }

                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

                if (gport_hw_resources.local_in_lif_per_core.core_id[0] == _SHR_CORE_ALL)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
                }

                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(match_flow_app_info->is_entry_related_cb(unit, entry_handle_id, &is_entry_related));
                if (!is_entry_related)
                {
                    continue;
                }
            }

            SHR_IF_ERR_EXIT(bcm_dnx_flow_handle_get(unit, match_flow_app_info->app_name, flow_handle));
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't find related application for table %s",
                 dbal_logical_table_to_string(unit, match_info->dbal_table));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* Get match entries which payload is a given gport from sw state */
static shr_error_e
dnx_flow_match_info_by_gport_multi_get_using_sw(
    int unit,
    bcm_gport_t gport,
    ingress_gport_sw_info_t * match_info,
    bcm_flow_match_entry_info_t * match_array)
{
    int match_idx;
    uint32 entry_handle_id;
    dbal_table_type_e table_type;
    const dnx_flow_app_config_t *flow_app_info;
    int nof_app_special_fields;
    CONST dbal_logical_table_t *table;
    bcm_flow_handle_t flow_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (match_idx = 0; match_idx < dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit); match_idx++)
    {
        if (match_info->single_match_info[match_idx].dbal_table == DBAL_TABLE_EMPTY)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, match_info->single_match_info[match_idx].dbal_table, &table));

        nof_app_special_fields = 0;

        match_array[match_idx].flow_handle_info.flow_id = gport;

        SHR_IF_ERR_EXIT(dbal_tables_table_type_get
                        (unit, match_info->single_match_info[match_idx].dbal_table, &table_type));

        SHR_IF_ERR_EXIT(dnx_flow_match_flow_handle_by_entry_get
                        (unit, gport, &match_info->single_match_info[match_idx], &flow_handle));

        flow_app_info = dnx_flow_app_info_get(unit, flow_handle);

        /** Get number of key special fields, serves as key for match applications Assign each key field id */
        while (flow_app_info->special_fields[nof_app_special_fields] != FLOW_S_F_EMPTY)
        {
            match_array[match_idx].key_special_fields.special_fields[nof_app_special_fields].field_id =
                flow_app_info->special_fields[nof_app_special_fields];
            nof_app_special_fields++;
        }
        match_array[match_idx].key_special_fields.actual_nof_special_fields = nof_app_special_fields;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, match_info->single_match_info[match_idx].dbal_table, &entry_handle_id));

        if (DBAL_TABLE_IS_TCAM(table))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set
                            (unit, entry_handle_id, match_info->single_match_info[match_idx].match_key[0]));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        }
        else
        {
           /** setting the HW entry Key */
            SHR_IF_ERR_EXIT(dbal_entry_key_as_buffer_set
                            (unit, entry_handle_id, match_info->single_match_info[match_idx].match_key,
                             table->key_size));
        }

        SHR_IF_ERR_EXIT(dnx_flow_match_key_fields_get
                        (unit, entry_handle_id, &match_array[match_idx].flow_handle_info,
                         (dnx_flow_app_config_t *) flow_app_info, table_type,
                         &match_array[match_idx].key_special_fields, nof_app_special_fields));
        match_array[match_idx].flow_handle_info.flow_handle = flow_handle;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  BCM API to get multiple match objects that are using the given gport.
 *  The actual number of match objects is returned by count.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] gport - the relevant gport.
 * \param [in] size - match array size.
 * \param [out] match_array - An array of the received match objects
 * \param [out] count - Actual number of valid match objects found
 *
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_flow_match_info_by_gport_multi_get(
    int unit,
    bcm_gport_t gport,
    int size,
    bcm_flow_match_entry_info_t * match_array,
    int *count)
{
    uint8 found;
    ingress_gport_sw_info_t match_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Match info by gport is not supported");
    }

    sal_memset(&match_info, 0, sizeof(ingress_gport_sw_info_t));

    SHR_IF_ERR_EXIT(flow_db.ingress_gport_to_match_info_htb.find(unit, &gport, &match_info, &found));

    if (!found)
    {
        *count = 0;
        SHR_EXIT();
    }

    *count = match_info.counter;

    if (size == 0)
    {
        /*
         * Get count only 
         */
        SHR_EXIT();
    }

    if ((match_info.counter > dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit)) &&
        (size > dnx_data_flow.general.match_per_gport_sw_nof_entries_get(unit)))
    {
        SHR_IF_ERR_EXIT(dnx_flow_match_info_by_gport_multi_get_using_traverse(unit, gport, size, match_array));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_flow_match_info_by_gport_multi_get_using_sw(unit, gport, &match_info, match_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
