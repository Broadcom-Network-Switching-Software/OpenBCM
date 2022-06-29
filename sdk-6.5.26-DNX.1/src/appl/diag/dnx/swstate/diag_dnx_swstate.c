
/** \file src/appl/diag/dnx/swstate/diag_dnx_swstate.c
 * 
 * SWSTATE DIAG - Diagnostic pack for SW State modules.
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * INCLUDE FILES:
 * {
 */
/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/dnxc/swstate/diagnostic_config.h>
#include <appl/diag/system.h>
/*soc*/
#include <soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/*shared*/
#include <shared/util.h>
#include <shared/utilex/utilex_str.h>
/*sal*/
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <sal/appl/editline/autocomplete.h>
/*src*/
#include "diag_dnx_swstate.h"

/*
 * }
 */

/* The macro is used for max value for spliting the node_id name*/
#define DIAG_SW_STATE_MAX_MODULE_STRING 1024

/*
 * The structure 
 */
typedef struct dnxc_sw_state_layout_size_diag_t
{
    /*
     * The name of the module or parameter
     */
    char *name_str;
    /*
     * The total_size of the module or parameter
     */
    uint32 total_size;
} dnxc_sw_state_layout_size_diag_t;

/*
 * The structure is holding data about each parameter
 */
typedef struct dnxc_sw_state_layout_diag_t
{
    /*
     * The name of the module or parameter
     */
    char *name_str;
    /*
     * The current parameter index
     */
    uint32 param_idx;
    /*
     * The module id
     */
    uint32 module_id;
    /*
     * It is counting what is the layout tree level of the given param_idx
     */
    uint32 count_layout_tree_level;
    /*
     * The layout address
     */
    dnxc_sw_state_layout_t *layout_address;
    /*
     * Flag "all"
     * If the flag is set - all data for the current parameter will be print
     */
    int all_flag;
    /*
     * Splited name_str
     */
    char **datatokens;
    /*
     * Number of the splited tokes
     */
    uint32 nof_tokens;
    /*
     *
     */
    dnxc_sw_state_layout_size_diag_t size_get_list[NOF_MODULE_ID];
} dnxc_sw_state_layout_diag_t;

/**
 * \brief - Print list of modules
 * The function searches the module's names in the layout tree
 * and it is printing all of the modules which are acitve.
 */
static shr_error_e
diag_dnx_swstate_module_list_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 module_id = 0;
    dnxc_sw_state_layout_t *layout_address = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("LIST OF ACTIVE MODULES");
    PRT_COLUMN_ADD("Name");
    /*
     * Iterate over modules
     */
    for (; module_id < NOF_MODULE_ID; module_id++)
    {
        /*
         * Print name
         */
        layout_address = sw_state_layout_array[unit][module_id];
        if (layout_address != NULL)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", layout_address[0].name);
        }
    }
    PRT_COMMITX;
    SHR_EXIT();

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print list of options for current parameter
 * The function is printing in table all of the children parameters of the given module/parameter.
 * If there aren't any, it will print NONE.
 */
static shr_error_e
diag_dnx_swstate_children_list_print(
    int unit,
    prt_control_t * prt_ctr,
    dnxc_sw_state_layout_diag_t * diag_info,
    sh_sand_control_t * sand_control)
{
    uint32 idx, end, start;
    SHR_FUNC_INIT_VARS(unit);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Children list");
    if (diag_info->layout_address[diag_info->param_idx].first_child_index != DNXC_SW_STATE_LAYOUT_INVALID
        && diag_info->layout_address[diag_info->param_idx].last_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
    {
        start = diag_info->layout_address[diag_info->param_idx].first_child_index;
        end = diag_info->layout_address[diag_info->param_idx].last_child_index + 1;
        for (idx = start; idx < end; idx++)
        {
            diag_info->datatokens =
                utilex_str_split(diag_info->layout_address[idx].name, "__", DIAG_SW_STATE_MAX_MODULE_STRING,
                                 &diag_info->nof_tokens);
            if (diag_info->datatokens != NULL)
            {
                PRT_CELL_SET("%s", diag_info->datatokens[diag_info->nof_tokens - 1]);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }
        }
        PRT_ROW_DELETE();
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Creating list of the children option for the given parameter index
 * The function is only used by the autocomplete.
 */
static shr_error_e
diag_dnx_swstate_ac_children_list_print(
    int unit,
    sh_sand_control_t * sand_control,
    dnxc_sw_state_layout_diag_t * diag_info)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    PRT_TITLE_SET("Children of  %s:", diag_info->layout_address[diag_info->param_idx].name);
    PRT_COLUMN_ADD("List");
    PRT_COLUMN_ADD("Name");
    SHR_IF_ERR_EXIT(diag_dnx_swstate_children_list_print(unit, prt_ctr, diag_info, sand_control));
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Getting module data which is needed for the diagnostic
 * This function is getting the tokenized parameter name,
 * searches the module's name in the layout tree,
 * and outputs the module_id and a pointer to the layout structure
 * associated with the module's name.
 */
shr_error_e
swstate_module_data_get(
    int unit,
    dnxc_sw_state_layout_diag_t * diag_info,
    sh_sand_control_t * sand_control,
    args_t * args)
{

    char *module_name = NULL;
    uint32 module_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    module_name = diag_info->datatokens[0];
    diag_info->layout_address = NULL;
    /*
     * Get the module_id by the name of the module
     */
    for (; module_id < NOF_MODULE_ID; module_id++)
    {
        /*
         * Get the layout address
         */
        diag_info->layout_address = sw_state_layout_array[unit][module_id];
        if (diag_info->layout_address != NULL)
        {
            if (!sal_strncasecmp(diag_info->layout_address[0].name, module_name, DIAG_SW_STATE_MAX_MODULE_STRING))
            {
                diag_info->module_id = module_id;
                break;
            }
        }
    }

    /*
     * Check for incorrect syntax
     */
    if (diag_info->layout_address == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The module is not active or is not written correctly.\n Write 'swstate_new <command> ' to see the list of active modules.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the param index by the name
 * This function is getting the tokenized parameter name,
 * the layout structure address and output the parameter index
 * associated with the given string.
 */
int
dnxc_sw_state_param_idx_get(
    int unit,
    dnxc_sw_state_layout_diag_t * diag_info)
{
    int index = 0, iteartion = 0, start, end = 0;
    char *child = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(child, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "child", "%s%s%s", EMPTY, EMPTY,
                       EMPTY);
    utilex_str_append(child, (char *) diag_info->datatokens[0]);

    start = diag_info->layout_address[0].first_child_index;
    end = diag_info->layout_address[0].last_child_index;

    for (iteartion = 1; iteartion < diag_info->nof_tokens; iteartion++)
    {
        utilex_str_append(child, "__");
        utilex_str_append(child, (char *) diag_info->datatokens[iteartion]);
        utilex_str_white_spaces_remove(child);
        for (index = start; index <= end; index++)
        {
            if (!sal_strncmp(diag_info->layout_address[index].name, child, DIAG_SW_STATE_MAX_MODULE_STRING))
            {
                diag_info->count_layout_tree_level += 1;
                diag_info->param_idx = index;
                if (diag_info->layout_address[index].first_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
                {
                    start = diag_info->layout_address[index].first_child_index;
                    end = diag_info->layout_address[index].last_child_index;
                }
                break;
            }
        }
    }

exit:
    SHR_FREE(child);
    SHR_FUNC_EXIT;
}

/*
* The fucntion is calling two function
* 1) swstate_module_data_get - which is getting th module_id and layout_address
* 2) dnxc_sw_state_param_idx_get - which is getting the parameter index in the module tree
*/
shr_error_e
swstate_diag_pre_process(
    int unit,
    dnxc_sw_state_layout_diag_t * diag_info,
    sh_sand_control_t * sand_control,
    args_t * args)
{
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get module_id and layout address
     */
    rv = swstate_module_data_get(unit, diag_info, sand_control, args);
    SHR_IF_ERR_EXIT(rv);

    if (!sal_strncasecmp("", diag_info->datatokens[diag_info->nof_tokens - 1], DIAG_SW_STATE_MAX_MODULE_STRING))
    {
        /*
         * ignore the last token if it's an empty string
         */
        diag_info->nof_tokens--;
    }

    /*
     * Get the parameter index
     */
    diag_info->count_layout_tree_level = 1;
    if (diag_info->nof_tokens > 1)
    {
        dnxc_sw_state_param_idx_get(unit, diag_info);

    }
    /*
     * The param_idx will be set to 0 when there is only module_name 
     */
    else
    {
        diag_info->param_idx = 0;
    }

    /*
     *Check if parameter index is invalid
     */
    if (diag_info->nof_tokens != diag_info->count_layout_tree_level)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter name '%s' \n", diag_info->name_str);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print table with layout information about the given parameter
 *  The table contains:
 *     paramater name, parent name, type, sizeof the type, doc, labels, array information, default value of the parameter
 */
shr_error_e
diag_dnx_swstate_info_get_print(
    int unit,
    dnxc_sw_state_layout_diag_t * diag_info,
    sh_sand_control_t * sand_control,
    args_t * args)
{
    int token, index, dimension = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char buffer[DIAG_SW_STATE_MAX_MODULE_STRING];
    char *label_str = NULL;
    char print_str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(label_str, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "label_str", "%s%s%s", EMPTY,
                       EMPTY, EMPTY);
    /*
     * Print parameter path with '.' 
     */
    sal_strncpy(print_str_name, diag_info->layout_address[diag_info->param_idx].name,
                DIAG_SW_STATE_MAX_MODULE_STRING - 1);
    for (token = 0; token < diag_info->nof_tokens - 1; token++)
    {
        utilex_sub_string_replace(print_str_name, "__", ".");
    }

    PRT_TITLE_SET("LAYOUT INFORMATION FOR  %s:", print_str_name);
    PRT_COLUMN_ADD("Type of information");
    PRT_COLUMN_ADD("Data");

    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    datatokens =
        utilex_str_split(diag_info->layout_address[diag_info->param_idx].name, "__", DIAG_SW_STATE_MAX_MODULE_STRING,
                         &nof_tokens);
    if (nof_tokens > 1)
    {
        PRT_CELL_SET("%s", "Parent_Name");
        PRT_CELL_SET("%s", datatokens[nof_tokens - 2]);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", "Parameter_Name");
        PRT_CELL_SET("%s", datatokens[nof_tokens - 1]);
    }
    else
    {
        PRT_CELL_SET("%s", "Parameter_Name");
        PRT_CELL_SET("%s", datatokens[0]);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Type");
    PRT_CELL_SET("%s", diag_info->layout_address[diag_info->param_idx].type);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Size of type");
    PRT_CELL_SET("%d", diag_info->layout_address[diag_info->param_idx].size_of);

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Doc")
        PRT_CELL_SET("%s",
                     (diag_info->layout_address[diag_info->param_idx].doc !=
                      NULL) ? diag_info->layout_address[diag_info->param_idx].doc : "NONE");

    /*
     * Label information
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Labels");
    SET_EMPTY(datatokens);
    nof_tokens = 0;
    dnxc_sw_state_layout_flags_get(unit, label_str,
                                   DNXC_SW_STATE_LAYOUT_NODE_ID_GET(diag_info->module_id, diag_info->param_idx));
    datatokens = utilex_str_split(label_str, ",", DIAG_SW_STATE_MAX_MODULE_STRING, &nof_tokens);
    if (!sal_strncasecmp("*", datatokens[1], DIAG_SW_STATE_MAX_MODULE_STRING))
    {
        for (index = 0; index < nof_tokens; index++)
        {
            PRT_CELL_SET("%s", datatokens[index]);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SKIP(1);
        }
    }
    else
    {
        PRT_CELL_SET("%s", datatokens[0]);
    }
    /*
     * Array information
     */
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    buffer[0] = 0;
    for (; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
    {
        if (diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].name != NULL
            && diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type !=
            DNXC_SWSTATE_ARRAY_INDEX_INVALID
            && diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].num_elements !=
            DNXC_SW_STATE_LAYOUT_INVALID)
        {
            sal_snprintf(buffer, DIAG_SW_STATE_MAX_MODULE_STRING, "%s array_ind_%d[%d],", buffer, dimension,
                         diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].num_elements);
        }
        else
        {
            break;
        }
    }
    PRT_CELL_SET("%s", "Array");
    PRT_CELL_SET("%s", (buffer[0] != 0) ? buffer : "Not an array");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_CELL_SET("%s", "Default value");
    if (diag_info->layout_address[diag_info->param_idx].default_value != NULL)
    {
        PRT_CELL_SET("%p", diag_info->layout_address[diag_info->param_idx].default_value);
    }
    else
    {
        PRT_CELL_SET("%s", "NONE");
    }
    SET_EMPTY(label_str);
    SET_EMPTY(datatokens);
    
    /*
     * Children list options
     */
    diag_dnx_swstate_children_list_print(unit, prt_ctr, diag_info, sand_control);

    PRT_COMMITX;
exit:
    if (datatokens != NULL)
    {
        utilex_str_split_free(datatokens, nof_tokens);
    }
    SHR_FREE(label_str);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse args and call to cmd_dnx_swstate_info_get() with the requested data params
 */
/**
 *Print the information about the given param index
 *  If the received string:
 *      1) is empty will be printed table with all of the active modules
 *      2) is invalid will be return an error
 *      3) is valid will be printed table with layout information about the given parameter
 */
static shr_error_e
cmd_dnx_swstate_info_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *var;
    dnxc_sw_state_layout_diag_t diag_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    memset(&diag_info, 0, sizeof(dnxc_sw_state_layout_diag_t));
    /*
     * get NAME
     */
    SH_SAND_GET_STR("name", var);
    diag_info.name_str = var;
    diag_info.datatokens =
        utilex_str_split(diag_info.name_str, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info.nof_tokens);

    /*
     * Case when there isn't module name
     */
    if (diag_info.nof_tokens == 0)
    {
        /*
         * Display list of modules
         */
        diag_dnx_swstate_module_list_print(unit, sand_control);
        SHR_EXIT();
    }

    if (diag_info.datatokens != NULL)
    {
        rv = swstate_diag_pre_process(unit, &diag_info, sand_control, args);
        SHR_IF_ERR_EXIT(rv);
    }

    /*
     * Print the data
     */
    diag_dnx_swstate_info_get_print(unit, &diag_info, sand_control, args);

exit:
    if (diag_info.datatokens != NULL && diag_info.nof_tokens > 0)
    {
        utilex_str_split_free(diag_info.datatokens, diag_info.nof_tokens);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Sort the given list of total_sizes in ascending order
 */
void
size_get_bubble_sort_list(
    int count,
    dnxc_sw_state_layout_diag_t * diag_info)
{
    int index = 0, iteration;
    dnxc_sw_state_layout_size_diag_t temp_size_get_struct;
    for (; index < count - 1; index++)
    {
        for (iteration = 0; iteration < count - index - 1; iteration++)
        {
            if (diag_info->size_get_list[iteration].total_size > diag_info->size_get_list[iteration + 1].total_size)
            {
                temp_size_get_struct = diag_info->size_get_list[iteration];
                diag_info->size_get_list[iteration] = diag_info->size_get_list[iteration + 1];
                diag_info->size_get_list[iteration + 1] = temp_size_get_struct;
            }
        }
    }
}

/**
 * \brief - Print the size of the given param index
 *  1) all flag is set - will be printed table with the size for all of the modules
 *  2) all flag is not set - will be printed table with size of the given parameter and it children list
 */
static shr_error_e
diag_dnx_swstate_size_get_print(
    int unit,
    sh_sand_control_t * sand_control,
    dnxc_sw_state_layout_diag_t * diag_info)
{
    uint32 token, total_size = 0, module_id = 0, module_id_end = 0, node_id = 0;
    int iteration, count = 0;
    char print_str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setting range for the for-loop
     */
    if (diag_info->all_flag)
    {
        module_id = 0;
        module_id_end = NOF_MODULE_ID;
        diag_info->param_idx = 0;
    }
    else
    {
        module_id = diag_info->module_id;
        module_id_end = diag_info->module_id + 1;
    }

    for (; module_id < module_id_end; module_id++)
    {
        total_size = 0;
        diag_info->layout_address = sw_state_layout_array[unit][module_id];
        if (diag_info->layout_address != NULL)
        {
            if (!diag_info->all_flag)
            {
                diag_info->datatokens =
                    utilex_str_split(diag_info->name_str, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info->nof_tokens);
                if (diag_info->datatokens != NULL)
                {
                    if (!sal_strncasecmp
                        ("", diag_info->datatokens[diag_info->nof_tokens - 1], DIAG_SW_STATE_MAX_MODULE_STRING))
                    {
                        diag_info->nof_tokens--;
                    }
                    diag_info->size_get_list[count].name_str = diag_info->datatokens[diag_info->nof_tokens - 1];
                }
            }
            else
            {
                diag_info->size_get_list[count].name_str = diag_info->layout_address[diag_info->param_idx].name;
            }
            /*
             * Get total size of the given paramater/module 
             */
            node_id = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(module_id, diag_info->param_idx);
            dnxc_sw_state_layout_total_size_get(unit, node_id, &total_size);
            diag_info->size_get_list[count].total_size = total_size;
            count += 1;
        }
    }

    /*
     * Print parameter path with '.' 
     */
    if (diag_info->layout_address != NULL)
    {
        sal_strncpy(print_str_name, diag_info->layout_address[diag_info->param_idx].name,
                    DIAG_SW_STATE_MAX_MODULE_STRING - 1);
        for (token = 0; token < diag_info->nof_tokens - 1; token++)
        {
            utilex_sub_string_replace(print_str_name, "__", ".");
        }
    }

    /*
     * Table create
     */
    PRT_TITLE_SET("SIZE_GET FOR %s", print_str_name);
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Round size");
    PRT_COLUMN_ADD("Size in bytes");

    /*
     * Sort total_size in ascending order
     */
    if (count > 1)
    {
        size_get_bubble_sort_list(count, diag_info);
    }

    for (iteration = 0; iteration < count; iteration++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", diag_info->size_get_list[iteration].name_str);

        if (diag_info->size_get_list[iteration].total_size < 1 * 1024)
        {
            PRT_CELL_SET("%d bytes", diag_info->size_get_list[iteration].total_size);
            PRT_CELL_SET("%d bytes", diag_info->size_get_list[iteration].total_size);
        }
        else if (diag_info->size_get_list[iteration].total_size < 1 * 1024 * 1024)
        {
            PRT_CELL_SET("%d.%d KB.", BYTES_TO_KBS(diag_info->size_get_list[iteration].total_size),
                         BYTES_TO_KBS_CARRY(diag_info->size_get_list[iteration].total_size));
            PRT_CELL_SET("%d bytes", diag_info->size_get_list[iteration].total_size);
        }
        else
        {
            PRT_CELL_SET("%d.%d MB.", BYTES_TO_MBS(diag_info->size_get_list[iteration].total_size),
                         BYTES_TO_MBS_CARRY(diag_info->size_get_list[iteration].total_size));
            PRT_CELL_SET("%d bytes", diag_info->size_get_list[iteration].total_size);
        }
    }

    if (!diag_info->all_flag)
    {
        diag_dnx_swstate_children_list_print(unit, prt_ctr, diag_info, sand_control);
    }

    PRT_COMMITX;
    SHR_EXIT();

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse args and call to cmd_dnx_swstate_size_get() with the requested data params
 */
/**
 * \brief - Print the size of the given param index
 *  If the received string:
 *      1) is empty will be printed table with all of the active modules
 *      2) is invalid will be return an error
 *      3) is valid print table
 */
static shr_error_e
cmd_dnx_swstate_size_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *var;
    int all_flag = 0;
    dnxc_sw_state_layout_diag_t diag_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    memset(&diag_info, 0, sizeof(dnxc_sw_state_layout_diag_t));
    /*
     * get NAME
     */
    SH_SAND_GET_STR("name", var);
    diag_info.name_str = var;
    diag_info.datatokens =
        utilex_str_split(diag_info.name_str, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info.nof_tokens);
    SH_SAND_IS_PRESENT("all", all_flag);
    diag_info.all_flag = all_flag;

    /*
     * Case when there isn't module name
     */
    if (diag_info.nof_tokens == 0 && !diag_info.all_flag)
    {
        /*
         * Display list of modules
         */
        diag_dnx_swstate_module_list_print(unit, sand_control);
        SHR_EXIT();
    }
    else if (diag_info.nof_tokens > 0 && diag_info.all_flag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "All flag can't be use in combination with name option.\n The all flag can use only in this way: swstate_new size_get all");
    }
    else if (!diag_info.all_flag && diag_info.datatokens != NULL)
    {
        rv = swstate_diag_pre_process(unit, &diag_info, sand_control, args);
        SHR_IF_ERR_EXIT(rv);
    }
    /*
     * Print table with size of the given module/parameter
     */
    diag_dnx_swstate_size_get_print(unit, sand_control, &diag_info);

exit:
    if (diag_info.datatokens != NULL && diag_info.nof_tokens > 0)
    {
        utilex_str_split_free(diag_info.datatokens, diag_info.nof_tokens);
    }
    SHR_FUNC_EXIT;
}

/*
 * \brief - Auto-complete values for SW State diag commands.
 * It has 2 cases:
 *  1) Input string contains only one part/word(module name) without "." splitter
 *  2) Input string contains at least one part/word with "." splitter
 *  \param [in] unit -  Unit #
 *  \param [in] input_string - input string provided by the user
 *  \param [in,out] swstate_full_list - List which will be filled with of the matching auto_complete values based on the input string
 *  \param [in,out] num_matches_found - Number of the matching values
*/
shr_error_e
diag_dnx_swstate_autocomplete_cb(
    int unit,
    char *input_string,
    char *swstate_full_list,
    int *num_matches_found)
{

    int module_id, rv, param_idx, iteration;
    uint32 full_str_len, input_str_len;
    char *module_name;
    char str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };
    char print_str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };
    dnxc_sw_state_layout_diag_t diag_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Expected format - <module>.<parent>.<child>.<grandchild>
     * The depth of the layout tree is not symetrical
     * so the depth level can vary across tables/modules.
     */
    diag_info.datatokens = utilex_str_split(input_string, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info.nof_tokens);
    if (diag_info.datatokens != NULL)
    {
        module_name = diag_info.datatokens[0];
        /*
         * if nof_tokens is 1, only the module was provided
         */
        if (diag_info.nof_tokens == 1)
        {
            /*
             * iterate through the modules and find all those that are at least a partial match
             */
            for (module_id = 0; module_id < NOF_MODULE_ID; module_id++)
            {
                diag_info.layout_address = sw_state_layout_array[unit][module_id];
                if (diag_info.layout_address != NULL)
                {
                    input_str_len = sal_strnlen(module_name, DIAG_SW_STATE_MAX_MODULE_STRING);
                    full_str_len = sal_strnlen(diag_info.layout_address[0].name, DIAG_SW_STATE_MAX_MODULE_STRING);
                    if (sal_strncaseindex(diag_info.layout_address[0].name, module_name, full_str_len, input_str_len))
                    {
                        (*num_matches_found)++;
                        /*
                         * if the returned string isn't long enough to include all partially-matching modules, print table with all available modules
                         */
                        if (sal_strnlen(swstate_full_list, AUTOCOMPLETE_MAX_STRING_LEN) +
                            sal_strnlen(diag_info.layout_address[0].name,
                                        AUTOCOMPLETE_MAX_STRING_LEN) + 2 >= AUTOCOMPLETE_MAX_STRING_LEN)
                        {
                            (*num_matches_found) = AUTOCOMPLETE_PRINT_OUT_OF_SCOPE;
                            swstate_full_list[0] = 0;
                            cli_out("\n");
                            rv = diag_dnx_swstate_module_list_print(unit, NULL);
                            if (rv == _SHR_E_NONE)
                            {
                                cli_out
                                    ("\n\nThe module name provided (%s) partially matches many modules. Please choose a module from the table above and try again.\n",
                                     module_name);
                                SHR_EXIT();
                            }
                            SHR_IF_ERR_EXIT(rv);
                        }
                        if ((*num_matches_found) > 1)
                        {
                            sal_strncat_s(swstate_full_list, " ", AUTOCOMPLETE_MAX_STRING_LEN);
                        }
                        /*
                         * Add module to string containing all partially-matching modules in the form of 'module.' (module name followed by '.')
                         */
                        sal_strncat_s(swstate_full_list, diag_info.layout_address[0].name, AUTOCOMPLETE_MAX_STRING_LEN);
                        sal_strncat_s(swstate_full_list, ".", AUTOCOMPLETE_MAX_STRING_LEN);
                    }
                }
            }
        }
        /*
         * if nof_tokens is bigger or equal to 2, at least module name was provided
         */
        else if (diag_info.nof_tokens >= 2)
        {
            /*
             * Get module_id and layout address
             */
            swstate_module_data_get(unit, &diag_info, NULL, NULL);
            /*
             * Get parameter index
             */
            dnxc_sw_state_param_idx_get(unit, &diag_info);
            if (diag_info.count_layout_tree_level + 1 < diag_info.nof_tokens)
            {
                sal_strncpy(str_name, input_string, DIAG_SW_STATE_MAX_MODULE_STRING - 1);
                /*
                 * Replacing all the "=" with "__"
                 */
                for (iteration = 0; iteration < diag_info.nof_tokens - 1; iteration++)
                {
                    utilex_sub_string_replace(str_name, ".", "__");
                }
                input_str_len = sal_strnlen(str_name, DIAG_SW_STATE_MAX_MODULE_STRING);
                /*
                 * Check if the parameter is having children
                 */
                if (diag_info.layout_address[diag_info.param_idx].first_child_index != DNXC_SW_STATE_LAYOUT_INVALID
                    && diag_info.layout_address[diag_info.param_idx].last_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
                {
                    for (param_idx = diag_info.layout_address[diag_info.param_idx].first_child_index;
                         param_idx <= diag_info.layout_address[diag_info.param_idx].last_child_index; param_idx++)
                    {
                        if (!sal_strncasecmp(diag_info.layout_address[param_idx].name, str_name, input_str_len))
                        {
                            (*num_matches_found)++;
                            /*
                             * if the returned string isn't long enough to include all partially-matching modules, print table with all available modules
                             */
                            if (sal_strnlen(swstate_full_list, AUTOCOMPLETE_MAX_STRING_LEN) +
                                sal_strnlen(diag_info.layout_address[param_idx].name,
                                            AUTOCOMPLETE_MAX_STRING_LEN) + diag_info.nof_tokens + 1 >=
                                AUTOCOMPLETE_MAX_STRING_LEN)
                            {
                                (*num_matches_found) = AUTOCOMPLETE_PRINT_OUT_OF_SCOPE;
                                swstate_full_list[0] = 0;
                                cli_out("\n");
                                rv = diag_dnx_swstate_ac_children_list_print(unit, NULL, &diag_info);
                                if (rv == _SHR_E_NONE)
                                {
                                    cli_out
                                        ("\n\nThe module name provided (%s) partially matches many modules. Please choose a module from the table above and try again.\n",
                                         module_name);
                                    SHR_EXIT();
                                }
                                SHR_IF_ERR_EXIT(rv);
                            }
                            if ((*num_matches_found) > 1)
                            {
                                sal_strncat_s(swstate_full_list, "  ", AUTOCOMPLETE_MAX_STRING_LEN);
                            }
                            /*
                             * Add parameter path name to string containing all partially-matching parameter path name in the form of 'module.parent.child.grandchild' (module name followed by '.', parent name followed by '.' and ect.)
                             */
                            sal_strncpy(print_str_name, diag_info.layout_address[param_idx].name,
                                        DIAG_SW_STATE_MAX_MODULE_STRING - 1);
                            /*
                             * Replacing all the "=" with "__" so the call format to be correct
                             */
                            for (iteration = 0; iteration < diag_info.nof_tokens - 1; iteration++)
                            {
                                utilex_sub_string_replace(print_str_name, "__", ".");
                            }
                            sal_strncat_s(swstate_full_list, print_str_name, AUTOCOMPLETE_MAX_STRING_LEN);
                            /*
                             * If the child isn't having any children of its own, the print string shouldn't have "." at the end
                             */
                            if (diag_info.layout_address[param_idx].first_child_index != DNXC_SW_STATE_LAYOUT_INVALID
                                && diag_info.layout_address[param_idx].last_child_index != DNXC_SW_STATE_LAYOUT_INVALID)
                            {
                                sal_strncat_s(swstate_full_list, ".", AUTOCOMPLETE_MAX_STRING_LEN);
                            }
                        }
                    }
                }
            }
        }
    }

    SHR_EXIT();

exit:
    /*
     * Free resources
     */
    if (diag_info.datatokens != NULL && diag_info.nof_tokens > 0)
    {
        utilex_str_split_free(diag_info.datatokens, diag_info.nof_tokens);
    }
    SHR_FUNC_EXIT;
}
/*
 * DIAG PACK:
 * {
 */
/**
 * \brief SW State diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_option_t dnx_swstate_info_get_layout_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "[<module_name>[.<parent_name>[.<child_name>[.<grandchild_name>]]]]", "", NULL, NULL, SH_SAND_ARG_FREE, .ac_option_cb=diag_dnx_swstate_autocomplete_cb},
    {NULL}
};

sh_sand_man_t dnx_swstate_info_get_layout_man = {
    .brief = "Get layout information with 'swstate_new info_get'",
    .full = "Prints layout information: parameter name, doc, labels, default value, array - information.\nTo list all modules 'swstate_new info_get'\nNote that the 'info_get' data can be done with or without prefacing with 'name=', but autocomplete is supported only if prefaced with 'name='\n",
    .synopsis = "[<module_name>[.<parent_name>[.<child_name>[.<grandchild_name>]]]]'",
    .examples = "name=dnx_algo_port_db\n"
                "name=dnx_algo_port_db.signal_quality\n"
                "name=dnx_field_context_sw.context_info.context_ipmf1_info.compare_info\n"
                "name=trunk_db.trunk_system_port_db.trunk_system_port_entries\n"
};

sh_sand_option_t dnx_swstate_size_get_layout_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "[<module_name>[.<parent_name>[.<child_name>[.<grandchild_name>]]]]", "", NULL, NULL, SH_SAND_ARG_FREE, .ac_option_cb=diag_dnx_swstate_autocomplete_cb},
    {"all", SAL_FIELD_TYPE_BOOL, "Print the total sizes for all active SW State modules. Use:'swstate_new size_get all'. Can't be use in combination with parameter.", "false"},
    {NULL}
};

sh_sand_man_t dnx_swstate_size_get_layout_man = {
    .brief = "swstate size_get",
    .full = "Prints size of the given parameter/module.\nTo list all modules 'swstate_new size_get'\n'All' flag cannot be called together with name option.\nNote that the 'size_get' data can be done with or without prefacing with 'name=',but autocomplete is supported only if prefaced with 'name='\n",
    .synopsis = "[<module_name>[.<parent_name>[.<child_name>[.<grandchild_name>]]]]'",
    .examples = "name=dnx_algo_port_db\n"
                "name=dnx_algo_port_db.signal_quality\n"
                "name=dnx_field_context_sw.context_info.context_ipmf1_info.compare_info\n"
                "name=trunk_db.trunk_system_port_db.trunk_system_port_entries\n"
                "all\n"
};

sh_sand_man_t sh_swstate_new_man = {
    .brief = "SW State diagnostic commands\n Expected format - swstate <command> <module>.<parent-param>.<child-param>.<child-param>",
};

sh_sand_cmd_t sh_dnx_swstate_new_cmds[] = {
    /*
     * keyword,        action,             cmd,           options,                          man,                      optional_cb,     invoke,      flags
     */
    {"info_get",  cmd_dnx_swstate_info_get, NULL, dnx_swstate_info_get_layout_options, &dnx_swstate_info_get_layout_man,    NULL,       NULL,      SH_CMD_NO_XML_VERIFY},
    {"size_get",  cmd_dnx_swstate_size_get, NULL, dnx_swstate_size_get_layout_options, &dnx_swstate_size_get_layout_man,    NULL,       NULL,      SH_CMD_NO_XML_VERIFY},
    {NULL}
};
/* *INDENT-ON* */
/**
 * }
 */
#undef BSL_LOG_MODULE
