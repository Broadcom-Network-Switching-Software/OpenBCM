/** \file diag_dnx_flow.c
 *
 * Main diagnostics for all application CLI commands that are
 * related to FLOW are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

/*************
 * INCLUDES  *
 */
#include <sal/appl/sal.h>
#include <appl/diag/diag.h>
#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/field/field_entry.h>
#include "diag_dnx_flow.h"
#include <appl/diag/sand/diag_sand_prt.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include "src/bcm/dnx/flow/flow_def.h"
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>

/*************
 * TYPEDEFS  *
 */
typedef struct
{
    prt_control_t *prt_ctr;
} dnx_diag_flow_traverse_data_t;

/********************
 * EXTERN FUNCTIONS *
 */

extern const char *strcaseindex(
    const char *s,
    const char *sub);

/*************
 * FUNCTIONS *
 */

/**
 * \brief
 * Retrieve the next flow handler that matches or includes the 
 * supplied application name string. The handler search is 
 * performed starting from the supplied flow handler. The search 
 * may be filtered by an application type.
 * \return 
 * CMD_NFND - If no next application is found 
 * CMD_OK - Application is found 
 */
static cmd_result_t
diag_flow_app_name_string_to_next_app_id_get(
    int unit,
    char *str_match,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_t * flow_handle_id,
    int *is_exact_match)
{
    int handle_idx, nof_flow_apps;
    const char *flow_app_name_str;
    SHR_FUNC_INIT_VARS(unit);

    nof_flow_apps = dnx_flow_number_of_apps();

    *is_exact_match = FALSE;
    handle_idx = (*flow_handle_id) + 1;

    /*
     * Traverse the flow handlers from the supplied handler ID
     */
    for (; handle_idx < nof_flow_apps; handle_idx++)
    {
        /*
         * Filter by application type
         */
        const dnx_flow_app_config_t *flow_app_info = dnx_flow_app_info_get(unit, handle_idx);
        if ((flow_app_type < FLOW_APP_TYPE_NOF_TYPES) && (flow_app_info->flow_app_type != flow_app_type))
        {
            continue;
        }

        /*
         * If no application string supplied, don't proceed to the string name filtering 
         */
        if (ISEMPTY(str_match))
        {
            break;
        }

        /*
         * Check whether there's an exact match or partial match
         */
        flow_app_name_str = dnx_flow_handle_to_string(unit, handle_idx);
        if (!sal_strncasecmp(str_match, flow_app_name_str, FLOW_STR_MAX))
        {
            *is_exact_match = TRUE;
            break;
        }

        if (strcaseindex(flow_app_name_str, str_match))
        {
            break;
        }
    }

    if (handle_idx >= nof_flow_apps)
    {
        return CMD_NFND;
    }

    *flow_handle_id = handle_idx;

    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Retrieve a flow handler whose application name matches or 
 * includes a supplied application name string. If more than one
 * application matches the supplied string, a list of the 
 * applicable applications will be displayed. 
 * Additional filtering may be achieved through supplied 
 * application type.
 * \return 
 * _SHR_E_NOT_FOUND - No matching handler found
 * _SHR_E_PARTIAL - A partial match is found 
 * _SHR_E_NONE - A matching flow handler is found
 */
shr_error_e
diag_flow_app_from_string(
    int unit,
    char *app_name_str,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_t * flow_handle_id,
    sh_sand_control_t * sand_control)
{
    int is_exact_match = FALSE;
    int nof_apps_found = 0;
    cmd_result_t cmd_rv;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Possible FLOW APPs");
    PRT_COLUMN_ADD("FLOW App Name");

    /*
     * Get the next partialy matching application untill an exact match is achieved. 
     * Store the names of partially matched applications in a buffer, but ignore the 
     * buffer in case an exact match is found.
     */
    *flow_handle_id = 0;
    while ((cmd_rv = diag_flow_app_name_string_to_next_app_id_get(unit,
                                                                  app_name_str, flow_app_type, flow_handle_id,
                                                                  &is_exact_match)) == CMD_OK)
    {
        nof_apps_found++;

        if (is_exact_match)
        {
            SHR_EXIT();
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_flow_handle_to_string(unit, *flow_handle_id));
    }

    /*
     * Determine if there was an exact match or no match at all
     */
    if (nof_apps_found == 0)
    {
        LOG_CLI((BSL_META("No matching FLOW APPs found\n\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else if (nof_apps_found == 1)
    {
        SHR_EXIT();
    }

    /*
     * In case of a partial match, display the buffer with the partially 
     * matched application names
     */
    SHR_SET_CURRENT_ERR(_SHR_E_PARTIAL);
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Display FLOW information of a specific application. 
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static shr_error_e
diag_flow_app_info_print(
    int unit,
    bcm_flow_handle_t flow_handle,
    sh_sand_control_t * sand_control)
{
    int field_idx, nof_common_fields;
    const dnx_flow_app_config_t *flow_app_info;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    uint8 is_verify_disabled, is_er_disabled;
    dbal_table_type_e table_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle);

    SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_verify_disabled.get(unit, flow_handle, &is_verify_disabled));
    SHR_IF_ERR_EXIT(flow_db.flow_application_info.is_error_recovery_disabled.get(unit, flow_handle, &is_er_disabled));

    /*
     * Print general application information
     */
    LOG_CLI((BSL_META("\nFLOW app %s information\n"), flow_app_info->app_name));
    LOG_CLI((BSL_META("\tApplication Type: %-10s \n"), flow_app_type_to_string(unit, flow_app_info->flow_app_type)));
    SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, flow_app_info->flow_table, &table_type));
    LOG_CLI((BSL_META("\tRelated DBAL table ID: %-10s, table type: %-10s \n"),
             dbal_logical_table_to_string(unit, flow_app_info->flow_table), dbal_table_type_to_string(unit,
                                                                                                      table_type)));
    LOG_CLI((BSL_META("\tIs validation Enabled: %s\n"), (is_verify_disabled ? "FALSE" : "TRUE")));
    LOG_CLI((BSL_META("\tIs error recovery Enabled: %s\n"), ((is_er_disabled) ? "FALSE" : "TRUE")));

    /*
     * Print the names of the Common fields that are supported by the application.
     * The supported fields evaluation is according to the application type
     */

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &nof_common_fields));

    if (flow_app_info->valid_common_fields_bitmap)
    {
        PRT_TITLE_SET("Application common fields");
        PRT_COLUMN_ADD("Field Name");
        PRT_COLUMN_ADD("Enabler ID");

        for (field_idx = 0; field_idx < nof_common_fields; field_idx++)
        {
            if (SAL_BIT(field_idx) & flow_app_info->valid_common_fields_bitmap)
            {
                SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                                (unit, flow_app_info->flow_app_type, field_idx, &common_fields_desc));
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", common_fields_desc->field_name);
                PRT_CELL_SET("%d", field_idx);
            }
        }
        PRT_COMMITX;
    }
    else
    {
        LOG_CLI((BSL_META("Application common fields: NONE\n")));
    }

    /*
     * Print the names of the Special fields that are supported by the application.
     */
    if (flow_app_info->special_fields[0] != DBAL_FIELD_EMPTY)
    {
        PRT_TITLE_SET("Application special fields");
        PRT_COLUMN_ADD("Field Name");
        PRT_COLUMN_ADD("Field ID");
        PRT_COLUMN_ADD("Set struct member");

        for (field_idx = 0; field_idx < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; field_idx++)
        {
            if (flow_app_info->special_fields[field_idx] == DBAL_FIELD_EMPTY)
            {
                break;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", dbal_field_to_string(unit, flow_app_info->special_fields[field_idx]));
            PRT_CELL_SET("%d", flow_app_info->special_fields[field_idx]);
            if (FLOW_FIELD_TYPE_IS_IPV4(flow_app_info->special_fields[field_idx]))
            {
                PRT_CELL_SET("shr_var_ipv4");
            }
            else
            {
                PRT_CELL_SET("shr_var_uint32");
            }
        }

        PRT_COMMITX;
    }
    else
    {
        LOG_CLI((BSL_META("Application special fields: NONE\n")));
    }

    LOG_CLI((BSL_META("\n")));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform the "flow app info" command. 
 * Display information for a specificaly matched application or 
 * a list of partially matched applications. 
 *  
 * \return 
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
diag_flow_app_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_flow_app_type_e app_type;
    char *app_name_str;
    bcm_flow_handle_t flow_handle_id = -1;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("Type", app_type);
    SH_SAND_GET_STR("Name", app_name_str);

    rv = diag_flow_app_from_string(unit, app_name_str, app_type, &flow_handle_id, sand_control);
    if (rv == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(diag_flow_app_info_print(unit, flow_handle_id, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * A function to print the special fields part of a flow entry 
 * dump. The input is the special fields structure from the FLOW 
 * traverse callback function. 
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static shr_error_e
diag_flow_app_entry_special_fields_print(
    int unit,
    prt_control_t * prt_ctr,
    bcm_flow_special_fields_t * special_fields,
    const dnx_flow_app_config_t * flow_app_info,
    uint8 is_key)
{
    int field_idx, actual_field_idx = 0;
    char print_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Traverse all the special fields that are defined for the application and 
     * print the value only for those which were set and supplied by the 
     * special_fields struct. 
     */
    for (field_idx = 0; field_idx < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; field_idx++)
    {
        if ((flow_app_info->special_fields[field_idx] == DBAL_FIELD_EMPTY) ||
            (special_fields->actual_nof_special_fields <= actual_field_idx))
        {
            break;
        }

        /*
         * If found an application special field that was actually set - Print its value. 
         * This logic assumes the actual special fields are ordered according to the 
         * application special fields.
         */
        if (flow_app_info->special_fields[field_idx] == special_fields->special_fields[actual_field_idx].field_id)
        {
            uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
            field_val[0] =
                (FLOW_FIELD_TYPE_IS_IPV4(special_fields->special_fields[actual_field_idx].field_id)) ?
                special_fields->special_fields[actual_field_idx].shr_var_ipv4 : special_fields->
                special_fields[actual_field_idx].shr_var_uint32;
            SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get
                            (unit, special_fields->special_fields[actual_field_idx].field_id, flow_app_info->flow_table,
                             field_val, NULL, 0, is_key, FALSE, print_buffer));
            PRT_CELL_SET("%s", print_buffer);
            actual_field_idx++;
        }
        else
        {
            PRT_CELL_SET("NOT SET");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * A flow traverse callback function to display a single FLOW 
 * Terminator entry.
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static int
diag_flow_app_term_entry_print_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    int nof_common_fields, field_idx;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    dnx_diag_flow_traverse_data_t *diag_flow_data;
    prt_control_t *prt_ctr;
    char print_string[FLOW_STR_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve prt_control_t from the user_data
     */
    diag_flow_data = user_data;
    prt_ctr = diag_flow_data->prt_ctr;

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &nof_common_fields));

    /*
     * Start a new Row and print the FLOW ID
     */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("0x%x", flow_handle_info->flow_id);

    /*
     * Traverse the valid common terminator field and print each field 
     * with its own format. 
     */
    for (field_idx = 0; field_idx < nof_common_fields; field_idx++)
    {
        if (SAL_BIT(field_idx) & flow_app_info->valid_common_fields_bitmap)
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, field_idx, &common_fields_desc));

            /*
             * If the field wasn't set by FLOW - Don't print it
             */
            if ((common_fields_desc->term_field_enabler == FLOW_ENABLER_INVALID) ||
                (_SHR_IS_FLAG_SET(terminator_info->valid_elements_set, common_fields_desc->term_field_enabler) == 0))
            {
                PRT_CELL_SET("NOT SET");
                continue;
            }

            switch (common_fields_desc->term_field_enabler)
            {
                case BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID:
                    PRT_CELL_SET("%d", terminator_info->vrf);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_STAT_ID_VALID:
                    PRT_CELL_SET("%d", terminator_info->stat_id);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->l3_ingress_info.urpf_mode)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_STAT_PP_PROFILE_VALID:
                    PRT_CELL_SET("%d", terminator_info->stat_pp_profile);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_QOS_MAP_ID_VALID:
                    PRT_CELL_SET("%d", terminator_info->qos_map_id);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_QOS_INGRESS_MODEL_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->ingress_qos_model)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->service_type)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->dest_info)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->action_gport)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Error, Unsupported BCM_FLOW_TERMINATOR_ELEMENT VALID flag %d for field %s",
                                 common_fields_desc->term_field_enabler, common_fields_desc->field_name);
            }

            /*
             * Verify that the attempted string to print, didn't exceed the expected limit
             */
            if (SHR_GET_CURRENT_ERR() > FLOW_STR_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_LIMIT,
                             "Error, Attempt to print a string of length %d for FLOW field %s exceeded the limit - %d",
                             SHR_GET_CURRENT_ERR(), common_fields_desc->field_name, FLOW_STR_MAX);
            }
        }
    }

    /*
     * Call a function to print the special fields part of a flow entry
     */
    SHR_IF_ERR_EXIT(diag_flow_app_entry_special_fields_print(unit, prt_ctr, special_fields, flow_app_info, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * A flow traverse callback function to display a single FLOW 
 * Initiator entry. 
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static int
diag_flow_app_init_entry_print_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_initiator_info_t * initator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    int nof_common_fields, field_idx;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    dnx_diag_flow_traverse_data_t *diag_flow_data;
    prt_control_t *prt_ctr;
    char print_string[FLOW_STR_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve prt_control_t from the user_data
     */
    diag_flow_data = user_data;
    prt_ctr = diag_flow_data->prt_ctr;

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_info->flow_app_type, &nof_common_fields));

    /*
     * Start a new Row and print the FLOW ID
     */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("0x%x", flow_handle_info->flow_id);

    /*
     * Traverse the valid common initiator field and print each field 
     * with its own format. 
     */
    for (field_idx = 0; field_idx < nof_common_fields; field_idx++)
    {
        if (SAL_BIT(field_idx) & flow_app_info->valid_common_fields_bitmap)
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_info->flow_app_type, field_idx, &common_fields_desc));

            /*
             * If the field wasn't set by FLOW - Don't print it
             */
            if ((common_fields_desc->init_field_enabler == FLOW_ENABLER_INVALID) ||
                (_SHR_IS_FLAG_SET(initator_info->valid_elements_set, common_fields_desc->init_field_enabler) == 0))
            {
                PRT_CELL_SET("NOT SET");
                continue;
            }

            switch (common_fields_desc->init_field_enabler)
            {
                case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
                    PRT_CELL_SET("%d", initator_info->stat_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
                    PRT_CELL_SET("%d", initator_info->stat_pp_profile);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
                    PRT_CELL_SET("%d", initator_info->qos_map_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initator_info->l3_intf_id)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initator_info->encap_access)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initator_info->action_gport)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
                    PRT_CELL_SET("%d", initator_info->mtu_profile);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Error, Unsupported BCM_FLOW_INITIATOR_ELEMENT VALID flag %d for field %s",
                                 common_fields_desc->init_field_enabler, common_fields_desc->field_name);
            }

            /*
             * Verify that the attempted string to print, didn't exceed the expected limit
             */
            if (SHR_GET_CURRENT_ERR() > FLOW_STR_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_LIMIT,
                             "Error, Attempt to print a string of length %d for FLOW field %s exceeded the limit - %d",
                             SHR_GET_CURRENT_ERR(), common_fields_desc->field_name, FLOW_STR_MAX);
            }
        }
    }

    /*
     * Call a function to print the special fields part of a flow entry
     */
    SHR_IF_ERR_EXIT(diag_flow_app_entry_special_fields_print(unit, prt_ctr, special_fields, flow_app_info, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * A flow traverse callback function to display a single FLOW 
 * Match entry. 
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static int
diag_flow_app_match_entry_print_cb(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_special_fields_t * key_special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    dnx_diag_flow_traverse_data_t *diag_flow_data;
    prt_control_t *prt_ctr;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve prt_control_t from the user_data
     */
    diag_flow_data = user_data;
    prt_ctr = diag_flow_data->prt_ctr;

    flow_app_info = dnx_flow_app_info_get(unit, flow_handle_info->flow_handle);

    /*
     * Start a new Row and print the FLOW ID
     */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("0x%x", flow_handle_info->flow_id);

    /*
     * Call a function to print the special fields part of a flow entry
     */
    SHR_IF_ERR_EXIT(diag_flow_app_entry_special_fields_print(unit, prt_ctr, key_special_fields, flow_app_info, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Display FLOW entries for a specific application. 
 * Use the FLOW traverse API to retrieve the entries. 
 *  
 * \return 
 * _SHR_E_NONE - Successful print
 */
static shr_error_e
diag_flow_app_dump_print(
    int unit,
    bcm_flow_handle_t flow_handle,
    sh_sand_control_t * sand_control)
{
    int field_idx, nof_common_fields;
    const dnx_flow_app_config_t *flow_app_info;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    dnx_flow_app_type_e flow_app_type;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_diag_flow_traverse_data_t diag_flow_data;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    bcm_flow_handle_info_t_init(&flow_handle_info);

    flow_handle_info.flow_handle = flow_handle;
    flow_app_info = dnx_flow_app_info_get(unit, flow_handle);
    flow_app_type = flow_app_info->flow_app_type;
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &nof_common_fields));

    /*
     * Determine the application type in order to call the appropriate 
     * travserse function 
     */
    PRT_TITLE_SET("FLOW Entries Dump for App %s", flow_app_type_to_string(unit, flow_app_type));
    PRT_COLUMN_ADD("FLOW ID");

    /*
     * Traverse all the common fields that are associated with this application type 
     * and add the columns to the print 
     */
    for (field_idx = 0; field_idx < nof_common_fields; field_idx++)
    {
        if (SAL_BIT(field_idx) & flow_app_info->valid_common_fields_bitmap)
        {
            SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get
                            (unit, flow_app_type, field_idx, &common_fields_desc));
            PRT_COLUMN_ADD("%s", common_fields_desc->field_name);
        }
    }

    /*
     * Traverse all the special fields that are associated with this application type 
     * and add the columns to the print 
     */
    for (field_idx = 0; field_idx < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; field_idx++)
    {
        if (flow_app_info->special_fields[field_idx] == DBAL_FIELD_EMPTY)
        {
            break;
        }

        PRT_COLUMN_ADD("%s", dbal_field_to_string(unit, flow_app_info->special_fields[field_idx]));
    }

    /*
     * Traverse the FLOW application entries
     */
    diag_flow_data.prt_ctr = prt_ctr;

    switch (flow_app_type)
    {
        case FLOW_APP_TYPE_TERM:
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_flow_terminator_info_traverse
                                      (unit, &flow_handle_info, diag_flow_app_term_entry_print_cb, &diag_flow_data),
                                      _SHR_E_NOT_FOUND);
            break;
        case FLOW_APP_TYPE_INIT:
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_flow_initiator_info_traverse
                                      (unit, &flow_handle_info, diag_flow_app_init_entry_print_cb, &diag_flow_data),
                                      _SHR_E_NOT_FOUND);
            break;
        case FLOW_APP_TYPE_MATCH:
            SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_flow_match_info_traverse
                                      (unit, &flow_handle_info, diag_flow_app_match_entry_print_cb, &diag_flow_data),
                                      _SHR_E_NOT_FOUND);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error, Unsupported app-type value (%d) for application %s", flow_app_type,
                         flow_app_info->app_name);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform the "flow app dump" command. 
 * Display the configured content entries of a whole 
 * application. 
 *  
 * \return 
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
diag_flow_app_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *app_name_str;
    bcm_flow_handle_t flow_handle_id = -1;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("Name", app_name_str);

    rv = diag_flow_app_from_string(unit, app_name_str, FLOW_APP_TYPE_NOF_TYPES, &flow_handle_id, sand_control);
    if (rv == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(diag_flow_app_dump_print(unit, flow_handle_id, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sand_flow_app_type_enum_table[] = {
    {"terminator", FLOW_APP_TYPE_TERM, "application related to LIF termination"},
    {"initiator", FLOW_APP_TYPE_INIT, "application related to LIF initiator"},
    {"match", FLOW_APP_TYPE_MATCH, "application related to LIF match)"},
    {"all", FLOW_APP_TYPE_NOF_TYPES, "default - any application type"},
    {NULL}
};

static sh_sand_option_t diag_flow_app_info_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "Application type INITIATOR/TERMINATOR/MATCH/ALL", "ALL",
     (void *) sand_flow_app_type_enum_table},
    {"Name", SAL_FIELD_TYPE_STR, "Application Name", ""},
    {NULL}
};

static sh_sand_option_t diag_flow_app_dump_options[] = {
    {"Name", SAL_FIELD_TYPE_STR, "Application Name", ""},
    {NULL}
};

static sh_sand_man_t diag_flow_app_info_man = {
    .brief = "Display application attributes or a list of matching application names",
    .full =
        "Display application attributes or a list of matching application names. User can add multiple filter rules. All the rules will apply to each filtered application",
    .synopsis = "[Type=<App type>] [Name=<App name>]",
    .examples = "Type=INITIATOR\n" "Name=GTP\n" "Type=TERMINATOR Name=VLAN"
};

static sh_sand_man_t diag_flow_app_dump_man = {
    .brief = "Dump all entries of an application or a list of matching applications",
    .full = "Dump all entries of an application or a list of matching applications",
    .synopsis = "[Name=<App name>]",
    .examples = "Name=GTP\n" "Name=TUN"
};

static sh_sand_cmd_t diag_flow_app_cmds[] = {
   /******************************************************************************************************************
    * CMD    *     CMD_ACTION           * Next *        Options               *            MAN             * CB      *
    * NAME   *                          * Level*                              *                            *         *
    *        *                          * CMD  *                              *                            *         *
 */
    {"Info", diag_flow_app_info, NULL, diag_flow_app_info_options, &diag_flow_app_info_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY},
    {"DuMP", diag_flow_app_dump, NULL, diag_flow_app_dump_options, &diag_flow_app_dump_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY},
    {NULL}
};

/**
 * \brief
 *  Print Common fields info for fields that match the a string
 *  name and application type.
 * \return 
 * _SHR_E_NONE - Successful Print 
 */
static shr_error_e
diag_flow_common_fields_by_app_type_info_print(
    int unit,
    char *field_name_str_match,
    dnx_flow_app_type_e app_type,
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control)
{
    int nof_common_fields, field_idx;
    const dnx_flow_common_field_desc_t *common_fields_desc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, app_type, &nof_common_fields));

    /*
     * Traverse all the fields that are associated with this application type
     */
    for (field_idx = 0; field_idx < nof_common_fields; field_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_flow_common_fields_desc_by_app_type_get(unit, app_type, field_idx, &common_fields_desc));

        /*
         * Print the entry if the field name matches at least partially the supplied string
         */
        if (strcaseindex(common_fields_desc->field_name, field_name_str_match))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", common_fields_desc->field_name);
            PRT_CELL_SET("%s", "Yes");
            PRT_CELL_SET("%s", flow_app_type_to_string(unit, app_type));
            PRT_CELL_SET("%s", (common_fields_desc->is_profile) ? "Yes" : "No");

            if (common_fields_desc->desc_cb != NULL)
            {
                PRT_CELL_SET("%s", common_fields_desc->desc_cb(unit));
            }
            else
            {
                PRT_CELL_SKIP(1);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Print Common fields info for fields that match the a string
 *  name by calling a function that is given an application
 *  type.
 * \return 
 * _SHR_E_NONE - Successful Print 
 */
static shr_error_e
diag_flow_common_fields_info_print(
    int unit,
    char *field_name_str_match,
    dnx_flow_app_type_e app_type_match,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FLOW Field attributes");
    PRT_COLUMN_ADD("FLOW Field Name");
    PRT_COLUMN_ADD("Is Common");
    PRT_COLUMN_ADD("FLOW App Type");
    PRT_COLUMN_ADD("Is Profile");
    PRT_COLUMN_ADD("Description");

    /*
     * Print the relevant common terminator fields if they weren't fileterd by the application type 
     */
    if ((app_type_match == FLOW_APP_TYPE_NOF_TYPES) || (app_type_match == FLOW_APP_TYPE_TERM))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_by_app_type_info_print
                        (unit, field_name_str_match, FLOW_APP_TYPE_TERM, prt_ctr, sand_control));
    }

    /*
     * Print the relevant common initiator fields if they weren't fileterd by the application type 
     */
    if ((app_type_match == FLOW_APP_TYPE_NOF_TYPES) || (app_type_match == FLOW_APP_TYPE_INIT))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_by_app_type_info_print
                        (unit, field_name_str_match, FLOW_APP_TYPE_INIT, prt_ctr, sand_control));
    }

    /*
     * Print the relevant common match fields if they weren't fileterd by the application type 
     */
    if ((app_type_match == FLOW_APP_TYPE_NOF_TYPES) || (app_type_match == FLOW_APP_TYPE_MATCH))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_by_app_type_info_print
                        (unit, field_name_str_match, FLOW_APP_TYPE_MATCH, prt_ctr, sand_control));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform the "flow field dump" command. 
 * Display information per FLOW field.
 *  
 * \return 
 * _SHR_E_NONE - Successful command
 */
static shr_error_e
diag_flow_field_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_flow_field_type_e field_type_match;
    dnx_flow_app_type_e app_type_match;
    char *field_name_str;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("Type", field_type_match);
    SH_SAND_GET_ENUM("AppType", app_type_match);
    SH_SAND_GET_STR("Name", field_name_str);

    /*
     * Print the relevant common fields if they weren't fileterd by the field type
     */
    if ((field_type_match == NOF_FLOW_FIELD_TYPES) || (field_type_match == FLOW_FIELD_TYPE_COMMON))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_info_print(unit, field_name_str, app_type_match, sand_control));
    }

    /*
     * TBD - Missing implementation for Special fields
     */

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sand_flow_field_type_enum_table[] = {
    {"common", FLOW_FIELD_TYPE_COMMON, "Field type for fields that are part of the Common fields"},
    {"special", FLOW_FIELD_TYPE_SPECIAL, "Field type for fields that are part of the Special fields"},
    {"all", NOF_FLOW_FIELD_TYPES, "Default - any field type"},
    {NULL}
};

static sh_sand_option_t diag_flow_field_dump_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "Field type COMMON/SPECIAL/ALL", "ALL",
     (void *) sand_flow_field_type_enum_table},
    {"AppType", SAL_FIELD_TYPE_ENUM, "Application type INITIATOR/TERMINATOR/MATCH/ALL", "ALL",
     (void *) sand_flow_app_type_enum_table},
    {"Name", SAL_FIELD_TYPE_STR, "Field Name", ""},
    {NULL}
};

static sh_sand_man_t diag_flow_field_dump_man = {
    .brief = "Dump matching common field and their attributes",
    .full =
        "Dump matching common field and their attributes. User can add multiple filter rules. All the rules will apply to each filtered field",
    .synopsis = "[Type=<Field type>] [AppType=<Application type>] [Name=<Field name>]",
    .examples = "Type=COMMON\n" "Name=VRF\n" "AppType=INITIATOR Name=STAT_ID"
};

static sh_sand_cmd_t diag_flow_field_cmds[] = {
   /******************************************************************************************************************
    * CMD    *     CMD_ACTION           * Next *        Options               *            MAN             * CB      *
    * NAME   *                          * Level*                              *                            *         *
    *        *                          * CMD  *                              *                            *         *
 */
    {"DuMP", diag_flow_field_dump, NULL, diag_flow_field_dump_options, &diag_flow_field_dump_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY},
    {NULL}
};

static sh_sand_man_t dnx_flow_app_man = {
    .brief = "Application operation menu",
    .full = NULL
};

static sh_sand_man_t dnx_flow_field_man = {
    .brief = "Field operation menu",
    .full = NULL
};

sh_sand_man_t sh_dnx_flow_man = {
    .brief = "FLOW diagnostic commands",
    .full = NULL
};

/**
 * \brief DNX FLOW diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for FLOW diagnostic commands
 */
sh_sand_cmd_t sh_dnx_flow_cmds[] = {
   /********************************************************************************************************************************
    * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *          *                           * Level                   *                                *                            *
    *          *                           * CMD                     *                                *                            *
 */
    {"APPLication", NULL, diag_flow_app_cmds, NULL, &dnx_flow_app_man},
    {"Field", NULL, diag_flow_field_cmds, NULL, &dnx_flow_field_man},
    {NULL}
};
