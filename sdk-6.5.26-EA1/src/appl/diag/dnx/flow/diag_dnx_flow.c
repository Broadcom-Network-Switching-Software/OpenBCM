/** \file diag_dnx_flow.c
 *
 * Main diagnostics for all application CLI commands that are
 * related to FLOW are gathered in this file.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

/*************
 * INCLUDES  *
 *************/
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
#include <src/bcm/dnx/flow/flow_def.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <soc/dnx/swstate/auto_generated/access/flow_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <include/bcm_int/dnx/flow/flow.h>

/*************
 * TYPEDEFS  *
 *************/
typedef struct
{
    prt_control_t *prt_ctr;
} dnx_diag_flow_traverse_data_t;

/********************
 * EXTERN FUNCTIONS *
 ********************/

extern const char *strcaseindex(
    const char *s,
    const char *sub);

#define DNX_DIAG_FLOW_MAX_ENUMS_PER_FIELD   (25)

typedef enum
{
    DIAG_FLOW_FIELD_TYPE_COMMON,
    DIAG_FLOW_FIELD_TYPE_SPECIAL,

    DIAG_NOF_FLOW_FIELD_TYPES
} diag_flow_field_type_e;

/*************
 * FUNCTIONS *
 *************/

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

        if (FLOW_APP_IS_NOT_VALID(flow_app_info))
        {
            continue;
        }
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
     * Get the next partially matching application until an exact match is achieved.
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
 * Display FLOW initiator encap access related information of a specific application.
 *
 * \return
 * _SHR_E_NONE - Successful print
 */
static shr_error_e
diag_flow_app_init_encap_info_print(
    int unit,
    bcm_flow_handle_t flow_handle,
    dbal_tables_e flow_table,
    sh_sand_control_t * sand_control)
{
    bcm_encap_access_t encap_access;
    uint32 encap_access_bitmap[1];
    uint32 res_type_bitmap[1];
    uint8 found;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    char result_type_str[1000];
    flow_app_encap_info_t flow_app_encap_info;
    int result_type, char_count;
    char *cur_str_pos;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Application encap access");
    PRT_COLUMN_ADD("Encap access");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Possible result types");

    /*
     * Get the possible encap access bitmap
     */
    SHR_IF_ERR_EXIT(flow_db.valid_phases_per_dbal_table.find(unit, &flow_table, &encap_access_bitmap[0], &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "entry not found in applications valid encap access hash table for %s",
                     dbal_logical_table_to_string(unit, flow_table));
    }

    sal_memset(&flow_app_encap_info, 0, sizeof(flow_app_encap_info_t));
    logical_phase = 0;
    /*
     * Iterate on all encap access from bitmap, display possible result type per each
     */
    while (encap_access_bitmap[0])
    {
        if (encap_access_bitmap[0] & 0x1)
        {
            sal_memset(result_type_str, 0, sizeof(result_type_str));

            res_type_bitmap[0] = 0;
            cur_str_pos = &result_type_str[0];

            flow_app_encap_info.dbal_table = flow_table;
            flow_app_encap_info.encap_access = logical_phase;

            SHR_IF_ERR_EXIT(flow_db.
                            dbal_table_to_valid_result_types.find(unit, &flow_app_encap_info, &res_type_bitmap[0],
                                                                  &found));

            if (!found)
            {
                encap_access_bitmap[0] = (encap_access_bitmap[0] >> 1);
                logical_phase++;

                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            SHR_IF_ERR_EXIT(dnx_lif_lib_logical_phase_to_encap_access_convert(unit, logical_phase, &encap_access));
            PRT_CELL_SET("%s", flow_encap_access_to_string(unit, encap_access));

            result_type = 0;
            while (res_type_bitmap[0])
            {
                if (res_type_bitmap[0] & 0x1)
                {
                    char_count =
                        sal_sprintf(cur_str_pos, "%s\n", dbal_result_type_to_string(unit, flow_table, result_type));
                    cur_str_pos += char_count;
                }
                res_type_bitmap[0] = (res_type_bitmap[0] >> 1);
                result_type++;
            }
            PRT_CELL_SET("%s", result_type_str);
        }
        encap_access_bitmap[0] = (encap_access_bitmap[0] >> 1);
        logical_phase++;
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Prints the special_fields data
 */
static shr_error_e
diag_flow_app_info_special_fields_table_print(
    int unit,
    const flow_special_fields_e * special_fields_arr,
    uint32 nof_special_fields,
    char *title_string,
    sh_sand_control_t * sand_control)
{
    uint8 field_idx;
    uint8 idx;
    flow_special_fields_e field_id;
    const flow_special_field_info_t *special_field_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", title_string);
    PRT_COLUMN_ADD("FLOW Field Name");
    PRT_COLUMN_ADD("Field ID");
    PRT_COLUMN_ADD("Payload type");
    PRT_COLUMN_ADD("DBAL mapped field");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Indications");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ENUM values");

    for (field_idx = 0; field_idx < nof_special_fields; field_idx++)
    {
        char indications_to_print[1024] = { 0 };

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        field_id = special_fields_arr[field_idx];

        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_id, &special_field_info));

        /** Flow Field Name */
        PRT_CELL_SET("%s", dnx_flow_special_field_to_string(unit, field_id));

        /** Flow Field ID */
        PRT_CELL_SET("%d", field_id);

        /** Payload type */
        PRT_CELL_SET("%s", dnx_flow_payload_types_to_string(unit, special_field_info->payload_type));

        /** mapped DBAL field */
        PRT_CELL_SET("%s", dbal_field_to_string(unit, special_field_info->mapped_dbal_field));

        /** Indications */
        for (idx = 0; idx < FLOW_NOF_SPECIAL_FIELDS_IND; idx++)
        {
            uint8 is_external = FALSE;
            SHR_IF_ERR_EXIT(dnx_flow_special_ind_is_external
                            (unit, (special_field_info->indications_bm & (1 << idx)), &is_external));
            if (is_external)
            {
                sal_strcat(indications_to_print, dnx_flow_special_fields_indications_to_string(unit, idx));
                sal_strcat(indications_to_print, "\n");
            }
        }
        if (sal_strlen(indications_to_print))
        {
            PRT_CELL_SET("%s", indications_to_print);
        }
        else
        {
            PRT_CELL_SET("None");
        }

        /** Description */
        if (special_field_info->description != NULL)
        {
            PRT_CELL_SET("%s", special_field_info->description(unit));
        }
        else
        {
            PRT_CELL_SKIP(1);
        }

        /** ENUM values */
        if ((special_field_info->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM) ||
            (special_field_info->payload_type == FLOW_PAYLOAD_TYPE_ENUM))
        {
            char enum_field_str[1024] = { 0 };
            char *cur_str_pos;
            int enum_idx;
            int nof_enums = 0;
            int char_count = 0;

            cur_str_pos = &enum_field_str[0];
            for (enum_idx = 0; enum_idx < FLOW_ENUM_MAX_VALUE; enum_idx++)
            {
                char print_string[FLOW_STR_MAX] = { '0' };
                shr_error_e rv;
                bcm_flow_special_field_t special_field_value;

                special_field_value.field_id = field_id;
                special_field_value.symbol = enum_idx;
                rv = special_field_info->print(unit, &special_field_value, print_string);

                if (!rv)
                {
                    char_count = sal_sprintf(cur_str_pos, "%s\n", print_string);
                    cur_str_pos += char_count;
                    nof_enums++;
                }
                /** Avoid reaching the cell max size */
                if (nof_enums > DNX_DIAG_FLOW_MAX_ENUMS_PER_FIELD)
                {
                    break;
                }
            }
            PRT_CELL_SET("%s", enum_field_str);
        }
        else
        {
            PRT_CELL_SET("");
        }
    }
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
    int field_idx, nof_common_fields, flag_idx;
    const dnx_flow_app_config_t *flow_app_info;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    uint8 is_verify_disabled, is_er_disabled;
    dbal_table_type_e table_type;
    int is_local_lif;
    int table_pos;

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
    for (table_pos = 0; table_pos < FLOW_MAX_NOF_MATCH_DBAL_TABLES; table_pos++)
    {
        if (flow_app_info->flow_table[table_pos] != DBAL_TABLE_EMPTY)
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_type_get(unit, flow_app_info->flow_table[table_pos], &table_type));
            LOG_CLI((BSL_META("\tRelated DBAL table: %-10s, table type: %-10s \n"),
                     dbal_logical_table_to_string(unit, flow_app_info->flow_table[table_pos]),
                     dbal_table_type_to_string(unit, table_type)));
        }
        else
        {
            break;
        }
    }

    LOG_CLI((BSL_META("\tIs validation Enabled: %s\n"), (is_verify_disabled ? "FALSE" : "TRUE")));
    LOG_CLI((BSL_META("\tIs error recovery Enabled: %s\n"), ((is_er_disabled) ? "FALSE" : "TRUE")));

    if (flow_app_info->flow_app_type == FLOW_APP_TYPE_INIT)
    {
        if (flow_app_info->encap_access_default_mapping == bcmEncapAccessInvalid)
        {
            LOG_CLI((BSL_META("\tNo default encap access\n")));
        }
        else
        {
            LOG_CLI((BSL_META("\tDefault encap access: %-10s\n"),
                     flow_encap_access_to_string(unit, flow_app_info->encap_access_default_mapping)));
        }
        /*
         * Encap access info, not applicable for no-local-LIF applications
         */
        is_local_lif = !(dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF));
        if (is_local_lif)
        {
            SHR_IF_ERR_EXIT(diag_flow_app_init_encap_info_print
                            (unit, flow_handle, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), sand_control));
        }
    }

    /*
     * Display any defined payload application for a Match application
     */
    if (FLOW_APP_IS_MATCH(flow_app_info->flow_app_type))
    {
        int payload_app_idx, nof_actual_app_payloads = 0;
        LOG_CLI((BSL_META("\tMay perform a lookup for applications:")));

        for (payload_app_idx = 0; payload_app_idx < FLOW_MAX_NOF_MATCH_PAYLOAD_APPS; payload_app_idx++)
        {
            if (flow_app_info->match_payload_apps[payload_app_idx][0])
            {
                LOG_CLI((BSL_META("\n\t\t%s\n"), flow_app_info->match_payload_apps[payload_app_idx]));
                nof_actual_app_payloads++;
            }
        }
        if (!nof_actual_app_payloads)
        {
            LOG_CLI((BSL_META(" None defined\n")));
        }
    }

    if (flow_app_info->app_indications_bm)
    {
        int ind_iter;
        LOG_CLI((BSL_META("\tApplication specific indications: \n")));
        for (ind_iter = 0; ind_iter < FLOW_MAX_NOF_APP_INDICATIONS; ind_iter++)
        {
            if (dnx_flow_app_is_ind_set(unit, flow_app_info, ind_iter))
            {
                LOG_CLI((BSL_META("\t\t %s \n"),
                         dnx_flow_app_indications_to_string(unit, flow_app_info->flow_app_type, ind_iter)));
            }
        }
        LOG_CLI((BSL_META("\n")));
    }
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
    if (flow_app_info->special_fields[0] != FLOW_S_F_EMPTY)
    {
        uint32 nof_special_fields = 0;

        while (flow_app_info->special_fields[nof_special_fields] != FLOW_S_F_EMPTY)
        {
            nof_special_fields++;
        }

        SHR_IF_ERR_EXIT(diag_flow_app_info_special_fields_table_print
                        (unit, flow_app_info->special_fields, nof_special_fields, "Application special fields",
                         sand_control));
    }
    else
    {
        LOG_CLI((BSL_META("Application special fields: NONE\n")));
    }

    /*
     * Print the flow handler flags that are supported by the application.
     */
    LOG_CLI((BSL_META("\nApplication valid flow handle flags:\n")));
    for (flag_idx = 0; flag_idx <= FLOW_HANDLE_FLAGS_MAX; flag_idx++)
    {
        if (_SHR_IS_FLAG_SET(flow_app_info->valid_flow_flags, (1 << flag_idx)))
        {
            LOG_CLI((BSL_META("\t%-20s\n"), flow_app_valid_flag_to_string(unit, flag_idx)));
        }
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
    uint8 is_print_virtual_keys)
{
    int app_field_idx, field_idx, nof_found_fields = 0, is_no_local_lif_app;
    char print_buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    is_no_local_lif_app = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF);

    /*
     * Retrieve the superset result-type number for display purposes
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, FLOW_LIF_DBAL_TABLE_GET(flow_app_info), &table));

    /*
     * Traverse all the special fields that are defined for the application and 
     * print the value only for those which were set and supplied by the 
     * special_fields struct.
     * For non supplied fields print "NOT SET"
     */
    for (app_field_idx = 0; app_field_idx < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; app_field_idx++)
    {
        flow_special_fields_e app_special_field_id = flow_app_info->special_fields[app_field_idx];
        if (app_special_field_id == FLOW_S_F_EMPTY)
        {
            break;
        }

        /*
         * Continue looking for special fields as long as the number of user supplied special 
         * fields wasn't reached. 
         * After that, the rest of the fields can be marked as "NOT SET" 
         */
        if (special_fields->actual_nof_special_fields > nof_found_fields)
        {
            /** For a Virtual application with no local-LIFs, print the match keys before the payload fields */
            if (is_no_local_lif_app)
            {
                /** Print key fields or payload fields according to the stage (is_print_virtual_keys) */
                const flow_special_field_info_t *special_field_info;
                int is_key;

                SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, app_special_field_id, &special_field_info));
                is_key = dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_IS_KEY);

                if (is_print_virtual_keys != is_key)
                {
                    continue;
                }
            }

            /*
             * Traverse the user supplied special fields structure to find the current application special field. 
             * If found, print the field's value. 
             * If not found, print "NOT SET"
             */
            for (field_idx = 0; field_idx < special_fields->actual_nof_special_fields; field_idx++)
            {
                if (app_special_field_id == special_fields->special_fields[field_idx].field_id)
                {
                    SHR_IF_ERR_EXIT(flow_special_field_printable_string_get
                                    (unit, app_special_field_id,
                                     &special_fields->special_fields[field_idx], print_buffer));
                    PRT_CELL_SET("%s", print_buffer);

                    nof_found_fields++;
                    break;
                }
            }
            if (field_idx >= special_fields->actual_nof_special_fields)
            {
                PRT_CELL_SET("NOT SET");
            }
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
    PRT_CELL_SKIP(1);

    /*
     * Traverse the valid common terminator fields and print each field 
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
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb(unit, (void *) terminator_info, print_string));
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
                case BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->dest_info)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_ADDITIONAL_DATA_VALID:
                    PRT_CELL_SET("0x%x", COMPILER_64_LO(terminator_info->additional_data));
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_ACTION_GPORT_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(terminator_info->action_gport)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_VSI_VALID:
                    PRT_CELL_SET("%d", terminator_info->vsi);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_L2_INGRESS_INFO_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb(unit, (void *) terminator_info, print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_ID_VALID:
                    PRT_CELL_SET("%d", terminator_info->failover_id);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_FAILOVER_STATE_VALID:
                    PRT_CELL_SET("%d", terminator_info->failover_state);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_CLASS_ID_VALID:
                    PRT_CELL_SET("%d", terminator_info->class_id);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_FIELD_CLASS_ID_VALID:
                    PRT_CELL_SET("%d", terminator_info->field_class_id);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_L2_LEARN_INFO_VALID:
                    PRT_CELL_SET("%d", terminator_info->l2_learn_info.dest_port);
                    break;
                case BCM_FLOW_TERMINATOR_ELEMENT_LEARN_ENABLE_VALID:
                    PRT_CELL_SET("%d", terminator_info->learn_enable);
                    break;
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
    bcm_flow_initiator_info_t * initiator_info,
    bcm_flow_special_fields_t * special_fields,
    void *user_data)
{
    const dnx_flow_app_config_t *flow_app_info;
    int nof_common_fields, field_idx, is_no_local_lif_app;
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
     * For a Virtual application with no local-LIFs, print the match keys before the payload fields
     */
    is_no_local_lif_app = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF);
    if (is_no_local_lif_app)
    {
        SHR_IF_ERR_EXIT(diag_flow_app_entry_special_fields_print(unit, prt_ctr, special_fields, flow_app_info, TRUE));
    }
    PRT_CELL_SKIP(1);

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
                (_SHR_IS_FLAG_SET(initiator_info->valid_elements_set, common_fields_desc->init_field_enabler) == 0))
            {
                PRT_CELL_SET("NOT SET");
                continue;
            }

            switch (common_fields_desc->init_field_enabler)
            {
                case BCM_FLOW_INITIATOR_ELEMENT_STAT_ID_VALID:
                    PRT_CELL_SET("%d", initiator_info->stat_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_STAT_PP_PROFILE_VALID:
                    PRT_CELL_SET("%d", initiator_info->stat_pp_profile);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_QOS_MAP_ID_VALID:
                    PRT_CELL_SET("%d", initiator_info->qos_map_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initiator_info->l3_intf_id)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_ACTION_GPORT_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initiator_info->action_gport)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_MTU_PROFILE_VALID:
                    PRT_CELL_SET("%d", initiator_info->mtu_profile);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_QOS_EGRESS_MODEL_VALID:
                    SHR_IF_ERR_EXIT(common_fields_desc->print_cb
                                    (unit, (void *) (&(initiator_info->egress_qos_model)), print_string));
                    PRT_CELL_SET("%s", print_string);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_L2_EGRESS_INFO_VALID:
                    PRT_CELL_SET("%d", initiator_info->l2_egress_info.egress_network_group_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_ID_VALID:
                    PRT_CELL_SET("%d", initiator_info->failover_id);
                    break;
                case BCM_FLOW_INITIATOR_ELEMENT_FAILOVER_STATE_VALID:
                    PRT_CELL_SET("%d", initiator_info->failover_state);
                    break;
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
     * Call a function to print the special fields part of a flow entry
     */
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    SHR_IF_ERR_EXIT(diag_flow_app_entry_special_fields_print(unit, prt_ctr, key_special_fields, flow_app_info, FALSE));

    /*
     * Print the FLOW ID last
     */
    PRT_CELL_SKIP(1);
    PRT_CELL_SET("0x%x", flow_handle_info->flow_id);

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
    int field_idx, nof_common_fields, is_no_local_lif_app;
    const dnx_flow_app_config_t *flow_app_info;
    const dnx_flow_common_field_desc_t *common_fields_desc;
    dnx_flow_app_type_e flow_app_type;
    bcm_flow_handle_info_t flow_handle_info;
    dnx_diag_flow_traverse_data_t diag_flow_data;
    flow_special_fields_e special_field_id;
    const flow_special_field_info_t *special_field_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Perform initializations  */
    bcm_flow_handle_info_t_init(&flow_handle_info);
    flow_handle_info.flow_handle = flow_handle;
    flow_app_info = dnx_flow_app_info_get(unit, flow_handle);
    flow_app_type = flow_app_info->flow_app_type;
    SHR_IF_ERR_EXIT(dnx_flow_number_of_common_fields(unit, flow_app_type, &nof_common_fields));
    is_no_local_lif_app = dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_NO_LOCAL_LIF);

    /*
     * For non-match applications, first display the Flow-ID. 
     * In case of a Virtual application with no local-LIF, display the key fields also 
     * before the payload fields. 
     */
    PRT_TITLE_SET("FLOW Entries Dump for App %s", flow_app_info->app_name);
    if ((flow_app_type == FLOW_APP_TYPE_TERM) || (flow_app_type == FLOW_APP_TYPE_INIT))
    {
        PRT_COLUMN_ADD("FLOW ID");

        /** In case of a Virtual application with no local-LIF */
        if (is_no_local_lif_app)
        {
            /** Traverse the special fields that are defined for the applications */
            for (field_idx = 0; field_idx < BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS; field_idx++)
            {
                special_field_id = flow_app_info->special_fields[field_idx];

                if (special_field_id == FLOW_S_F_EMPTY)
                {
                    break;
                }

                /** Print only the key fields */
                SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_field_id, &special_field_info));
                if (dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_IS_KEY))
                {
                    PRT_COLUMN_ADD("%s", dnx_flow_special_field_to_string(unit, special_field_id));
                }
            }
        }
        PRT_COLUMN_ADD("");
    }

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
        special_field_id = flow_app_info->special_fields[field_idx];

        if (special_field_id == FLOW_S_F_EMPTY)
        {
            break;
        }

        /** In case of a Virtual application with no local-LIF */
        if (is_no_local_lif_app)
        {
            /** Print only the payload fields (key fields were already printed) */
            SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, special_field_id, &special_field_info));
            if (!(dnx_flow_special_field_is_ind_set(unit, special_field_info, FLOW_SPECIAL_FIELD_IND_IS_KEY)))
            {
                PRT_COLUMN_ADD("%s", dnx_flow_special_field_to_string(unit, special_field_id));
            }
        }
        /** Print any special field if it's not a Virtual application with no local-LIF */
        else
        {
            PRT_COLUMN_ADD("%s", dnx_flow_special_field_to_string(unit, flow_app_info->special_fields[field_idx]));
        }
    }

    /** In case of a match application, the flow-id is printed last */
    if (FLOW_APP_IS_MATCH(flow_app_type))
    {
        PRT_COLUMN_ADD("");
        PRT_COLUMN_ADD("FLOW ID");
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
        case FLOW_APP_TYPE_TERM_MATCH:
        case FLOW_APP_TYPE_INIT_MATCH:
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
    {"terminator", FLOW_APP_TYPE_TERM, "an application that represents an Terminator LIF"},
    {"initiator", FLOW_APP_TYPE_INIT, "an application that represents an Initiator LIF"},
    {"term-match", FLOW_APP_TYPE_TERM_MATCH, "an application that represents a Terminator LIF match"},
    {"init-match", FLOW_APP_TYPE_INIT_MATCH, "an application that represents an Initiator LIF match"},
    {"all", FLOW_APP_TYPE_NOF_TYPES, "default - any application type"},
    {NULL}
};

static sh_sand_option_t diag_flow_app_info_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "Application type INITIATOR/TERMINATOR/TERM-MATCH/INIT-MATCH/ALL", "ALL",
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
    ******************************************************************************************************************/
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
    sh_sand_control_t * sand_control,
    uint8 *found_field)
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
        if ((common_fields_desc != NULL) && strcaseindex(common_fields_desc->field_name, field_name_str_match))
        {
            *found_field = TRUE;

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", common_fields_desc->field_name);
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
 *  Print Special fields info for fields that match the a string
 *  name by calling a function that is given an application
 *  type.
 * \return
 * _SHR_E_NONE - Successful Print
 */
static shr_error_e
diag_flow_special_fields_info_print(
    int unit,
    char *field_name_str_match,
    sh_sand_control_t * sand_control)
{
    int field_idx = 0;
    int found_field = 0;
    flow_special_fields_e special_fields_arr[NOF_FLOW_SPECIAL_FIELDS] = { 0 };
    const flow_special_field_info_t *special_field_last_match;

    SHR_FUNC_INIT_VARS(unit);

    for (field_idx = FLOW_S_F_EMPTY + 1; field_idx < NOF_FLOW_SPECIAL_FIELDS; field_idx++)
    {
        const flow_special_field_info_t *special_field;

        SHR_IF_ERR_EXIT(flow_special_field_info_get(unit, field_idx, &special_field));

        if (strcaseindex(special_field->name, field_name_str_match))
        {
            special_fields_arr[found_field] = field_idx;
            special_field_last_match = special_field;
            found_field++;
        }
    }

    if (found_field)
    {
        SHR_IF_ERR_EXIT(diag_flow_app_info_special_fields_table_print
                        (unit, special_fields_arr, found_field, "FLOW special fields", sand_control));
    }

    /** in case found only one field and the field is enum dump all possible values */
    if (found_field == 1)
    {
        if ((special_field_last_match->payload_type == FLOW_PAYLOAD_TYPE_ENUM) ||
            (special_field_last_match->payload_type == FLOW_PAYLOAD_TYPE_BCM_ENUM))
        {
            int enum_val;
            char print_string[FLOW_STR_MAX];
            LOG_CLI((BSL_META("\nFields possible values \n")));

            for (enum_val = 0; enum_val < FLOW_ENUM_MAX_VALUE; enum_val++)
            {
                int rv;
                bcm_flow_special_field_t special_field_value;

                special_field_value.field_id = field_idx;
                special_field_value.symbol = enum_val;
                rv = special_field_last_match->print(unit, &special_field_value, print_string);

                if (!rv)
                {
                    LOG_CLI((BSL_META("%s\n"), print_string));
                }
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
diag_flow_fields_info_print(
    int unit,
    char *field_name_str_match,
    dnx_flow_app_type_e app_type_match,
    sh_sand_control_t * sand_control)
{
    uint8 found_field = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FLOW Common Fields attributes");
    PRT_COLUMN_ADD("FLOW Field Name");
    PRT_COLUMN_ADD("FLOW App Type");
    PRT_COLUMN_ADD("Is Profile");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Description");

    /*
     * Print the relevant common terminator fields if they weren't fileterd by the application type 
     */
    if ((app_type_match == FLOW_APP_TYPE_NOF_TYPES) || (app_type_match == FLOW_APP_TYPE_TERM))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_by_app_type_info_print
                        (unit, field_name_str_match, FLOW_APP_TYPE_TERM, prt_ctr, sand_control, &found_field));
    }

    /*
     * Print the relevant common initiator fields if they weren't fileterd by the application type 
     */
    if ((app_type_match == FLOW_APP_TYPE_NOF_TYPES) || (app_type_match == FLOW_APP_TYPE_INIT))
    {
        SHR_IF_ERR_EXIT(diag_flow_common_fields_by_app_type_info_print
                        (unit, field_name_str_match, FLOW_APP_TYPE_INIT, prt_ctr, sand_control, &found_field));
    }

    if (found_field)
    {
        PRT_COMMITX;
    }

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
    diag_flow_field_type_e field_type_match;
    dnx_flow_app_type_e app_type_match;
    char *field_name_str;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("Type", field_type_match);
    SH_SAND_GET_ENUM("AppType", app_type_match);
    SH_SAND_GET_STR("Name", field_name_str);

    /*
     * Print the relevant common fields
     */
    if ((field_type_match == DIAG_NOF_FLOW_FIELD_TYPES) || (field_type_match == DIAG_FLOW_FIELD_TYPE_COMMON))
    {
        SHR_IF_ERR_EXIT(diag_flow_fields_info_print(unit, field_name_str, app_type_match, sand_control));
    }

    /*
     * Special fields dump 
     */
    if ((field_type_match == DIAG_NOF_FLOW_FIELD_TYPES) || (field_type_match == DIAG_FLOW_FIELD_TYPE_SPECIAL))
    {
        SHR_IF_ERR_EXIT(diag_flow_special_fields_info_print(unit, field_name_str, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sand_flow_field_type_enum_table[] = {
    {"common", DIAG_FLOW_FIELD_TYPE_COMMON, "Field type for fields that are part of the Common fields"},
    {"special", DIAG_FLOW_FIELD_TYPE_SPECIAL, "Field type for fields that are part of the Special fields"},
    {"all", DIAG_NOF_FLOW_FIELD_TYPES, "Default - any field type"},
    {NULL}
};

static sh_sand_option_t diag_flow_field_dump_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "Field type COMMON/SPECIAL/ALL", "ALL",
     (void *) sand_flow_field_type_enum_table},
    {"AppType", SAL_FIELD_TYPE_ENUM, "Application type INITIATOR/TERMINATOR/TERM-MATCH/INIT-MATCH/ALL", "ALL",
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
    ******************************************************************************************************************/
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
    ********************************************************************************************************************************/
    {"APPLication", NULL, diag_flow_app_cmds, NULL, &dnx_flow_app_man},
    {"Field", NULL, diag_flow_field_cmds, NULL, &dnx_flow_field_man},
    {NULL}
};
