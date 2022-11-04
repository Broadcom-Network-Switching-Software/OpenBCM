
/** \file src/appl/diag/dnxc/diag_dnx_swstate.h
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
#include <appl/diag/dnxc/swstate/diagnostic_config.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/system.h>

/*soc*/
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnxc_sw_state_print.h>
#endif /* DNX_SW_STATE_DIAGNOSTIC */
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/*shared*/
#include <shared/util.h>
#include <shared/utilex/utilex_str.h>
/*sal*/
#include <sal/appl/editline/autocomplete.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/*src*/
#include"diag_dnxc.h"

/*
 * }
 */
#if defined(DNX_SW_STATE_DIAGNOSTIC)

/* The macro is returning the correct path format needed for the dump call */
#define DIAG_DNXC_SW_STATE_DUMP_EXIT_AND_PRINT_CORRECT_FORMAT(unit, diag_info, param_idx)\
do { \
        diag_dnx_swstate_dump_param_correct_format(unit, diag_info, param_idx, correct_format_string);\
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid input. Correct format: %s \n", correct_format_string);\
}while(0)

/* The macro is used for max value for spliting the node_id name*/
#define DIAG_SW_STATE_MAX_MODULE_STRING 1024

/* Invalid value for array_indexes for dump command*/
#define DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID -2

/* All indexes of the array_dimension had to be printed for dump command*/
#define DIAG_SW_STATE_DUMP_ARRAY_INDEX_ALL_INDEXES -1

/*
 * The structure is holding information for the array_indexes format checks for dump command
 */
typedef struct dnxc_sw_state_dump_array_indexes_values_info_diag_t
{
    /*
     * Array index given by the user
     * for each array_dimension
     */
    int dimension_array_index;
    /*
     * Max number of elements for dimension
     */
    int max_dim_nof_element;
} dnxc_sw_state_dump_array_indexes_values_info_diag_t;

/*
 * The structure is holding information for the dump command
 */
typedef struct dnxc_sw_state_dump_diag_t
{
    /*
     * The whole string given by the user
     */
    char *dump_string;
    /*
     * Array which is holidng the array indexes given by the user
     * It is allocated during pre_process_dump function ->
     *  array_indexes[nof_tokens][DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS]
     */
    dnxc_sw_state_dump_array_indexes_values_info_diag_t **array_indexes;
    /*
     * Structure with the dump filters
     */
    dnx_sw_state_dump_filters_t filters;
    /*
     * Array which is holding parameter_idx
     * for each depth level of the given path
     * param_indexes_by_token[nof_tokens] = param_idx
     */
    uint32 *param_indexes_by_token;
    /*
     * It hold the parameter sw_state_roots_address
     */
    void *param_addr;
} dnxc_sw_state_dump_diag_t;

/*
 * The structure is holding information about the total_size of the given parameter
 */
typedef struct dnxc_sw_state_size_diag_t
{
    /*
     * The name of the module or parameter
     */
    char *name_str;
    /*
     * The total_size of the module or parameter
     */
    uint32 total_size;
} dnxc_sw_state_size_diag_t;

/*
 * The structure is holding data about each parameter
 */
typedef struct dnxc_sw_state_diag_t
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
     * Array which is holding the size of each element which is involved to the calculation
     * of the given parameter
     */
    dnxc_sw_state_size_diag_t size_get_list[NOF_MODULE_ID];
} dnxc_sw_state_diag_t;

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
    dnxc_sw_state_diag_t * diag_info,
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
    dnxc_sw_state_diag_t * diag_info)
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
    dnxc_sw_state_diag_t * diag_info,
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
    dnxc_sw_state_diag_t * diag_info)
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

static shr_error_e
diag_dnx_swstate_dump_param_correct_format(
    int unit,
    dnxc_sw_state_diag_t * diag_info,
    uint32 param_idx,
    char *print_string)
{
    int dimension, token, current_idx, value = 0;
    char *format = "%s%s";
    char *int_num_to_str = NULL, *temp_print_string = NULL, *temp_print_string2 = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(int_num_to_str, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "int_num_to_str", "%s%s%s",
                       EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(temp_print_string, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "temp_print_string",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(temp_print_string2, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "temp_print_string2",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    current_idx = param_idx = diag_info->param_idx;
    for (token = diag_info->nof_tokens - 1; token >= 0; token--)
    {
        sal_strncpy(temp_print_string, diag_info->datatokens[token], DIAG_SW_STATE_MAX_MODULE_STRING);
        for (dimension = 0; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
        {
            if (diag_info->layout_address[param_idx].array_indexes[dimension].num_elements == 0
                && diag_info->layout_address[param_idx].array_indexes[dimension].index_type ==
                DNXC_SWSTATE_ARRAY_INDEX_INVALID)
            {
                break;
            }
            else
            {
                value = diag_info->layout_address[param_idx].array_indexes[dimension].num_elements - 1;
                utilex_str_append(temp_print_string, "[0-");
                dnxc_sw_state_to_string_uint32_cb(unit, &value, &int_num_to_str);
                utilex_str_append(temp_print_string, int_num_to_str);
                utilex_str_append(temp_print_string, "]");
            }
        }
        utilex_str_white_spaces_remove(temp_print_string);
        sal_strncpy(temp_print_string2, print_string, DIAG_SW_STATE_MAX_MODULE_STRING);
        if (token != diag_info->nof_tokens - 1)
        {
            format = "%s.%s";
        }
        sal_snprintf(print_string, DIAG_SW_STATE_MAX_MODULE_STRING, format, temp_print_string, temp_print_string2);
        param_idx = diag_info->layout_address[current_idx].parent;
        current_idx = param_idx;
    }

    SHR_EXIT();
exit:
    SHR_FREE(int_num_to_str);
    SHR_FREE(temp_print_string);
    SHR_FREE(temp_print_string2);
    SHR_FUNC_EXIT;
}

/*
 * Function to verify given array_indexes and dimensions
 */
static shr_error_e
diag_dnx_swstate_dump_array_indexes_verify(
    int unit,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info)
{
    int token = 0, dimension = 0;
    uint32 param_idx = 0, current_idx = 0;
    dnxc_sw_state_layout_t *layout_address = NULL;
    char *correct_format_string = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(correct_format_string, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1,
                       "correct_format_string", "%s%s%s", EMPTY, EMPTY, EMPTY);

    layout_address = diag_info->layout_address;
    current_idx = param_idx = diag_info->param_idx;
    for (token = diag_info->nof_tokens - 1; token > 0; token--)
    {
        dump_diag_info->param_indexes_by_token[token] = param_idx;
        for (dimension = 0; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
        {
            /*
             * There is information for an array index dimension which is null
             */
            if (layout_address[param_idx].array_indexes[dimension].index_type == DNXC_SWSTATE_ARRAY_INDEX_INVALID
                && dump_diag_info->array_indexes[token][dimension].dimension_array_index !=
                DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID)
            {
                DIAG_DNXC_SW_STATE_DUMP_EXIT_AND_PRINT_CORRECT_FORMAT(unit, diag_info, param_idx);
            }
            /*
             * There is missing information for the array index dimension
             */
            if (layout_address[param_idx].array_indexes[dimension].num_elements != 0
                && dump_diag_info->array_indexes[token][dimension].dimension_array_index ==
                DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID)
            {
                /*
                 * In the layout array_indexes structure we hold information about the buffer size, number of instances
                 * and the length of the string
                 */
                if (layout_address[param_idx].array_indexes[dimension].index_type == DNXC_SWSTATE_ARRAY_INDEX_BITMAP
                    || layout_address[param_idx].array_indexes[dimension].index_type == DNXC_SWSTATE_ARRAY_INDEX_BUFFER
                    || layout_address[param_idx].array_indexes[dimension].index_type == DNXC_SWSTATE_ARRAY_INDEX_STRING)
                {
                    break;
                }
                else
                {
                    DIAG_DNXC_SW_STATE_DUMP_EXIT_AND_PRINT_CORRECT_FORMAT(unit, diag_info, param_idx);
                }
            }
            /*
             * Given input bigger than the max size of the array index dimension
             */
            if (layout_address[param_idx].array_indexes[dimension].num_elements - 1 <
                dump_diag_info->array_indexes[token][dimension].dimension_array_index
                && dump_diag_info->array_indexes[token][dimension].dimension_array_index !=
                DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID
                && dump_diag_info->array_indexes[token][dimension].dimension_array_index !=
                DIAG_SW_STATE_DUMP_ARRAY_INDEX_ALL_INDEXES)
            {
                DIAG_DNXC_SW_STATE_DUMP_EXIT_AND_PRINT_CORRECT_FORMAT(unit, diag_info, param_idx);
            }
            /*
             * Invalid combination reached for array
             */
            if (layout_address[param_idx].array_indexes[dimension].index_type == DNXC_SWSTATE_ARRAY_INDEX_INVALID
                && dump_diag_info->array_indexes[token][dimension].dimension_array_index ==
                DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID)
            {
                /*
                 * Resetting the array_index form -2 to 0 so
                 * the diag_dnx_swstate_dump_array_indexes_multidimension_static_calculation
                 * formula calculate correctly
                 */
                dump_diag_info->array_indexes[token][dimension].dimension_array_index = 0;
            }

        }

        param_idx = diag_info->layout_address[current_idx].parent;
        current_idx = param_idx;
    }

exit:
    SHR_FREE(correct_format_string);
    SHR_FUNC_EXIT;
}

/**
 * Function that verify the format of the brackets
 */
static shr_error_e
swstate_diag_dump_array_indexes_string_format_verify(
    int unit,
    char *array_indexes_string,
    dnxc_sw_state_diag_t * diag_info,
    uint32 current_string_token)
{
    uint32 close_brackets, token, nof_index_datatokens;
    char **index_datatokens = NULL;
    char **close_brackets_datatokens = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Example: array_indexes_str = "[1][2]"
     */
    /*
     * Split by '[' -> token[0] = "", token[1]="1]" token[2]="2]"
     */
    close_brackets_datatokens =
        utilex_str_split(array_indexes_string, "]", DIAG_SW_STATE_MAX_MODULE_STRING, &close_brackets);

    /*
     * Split by ']' -> token[0] = "[1", token[1]= "[2" token[2]= ""
     */
    index_datatokens =
        utilex_str_split(array_indexes_string, "[", DIAG_SW_STATE_MAX_MODULE_STRING, &nof_index_datatokens);

    if (index_datatokens != NULL)
    {
        if (close_brackets < nof_index_datatokens)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid input. Open brackets '[' = %u are more than close brackets ']' = %u in  '%s' \n",
                         nof_index_datatokens - 1, close_brackets - 1, diag_info->datatokens[current_string_token]);
        }
        else if (close_brackets > nof_index_datatokens)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid input. Close brackets ']' = %u are more than open brackets '[' = %u in  '%s'\n",
                         close_brackets - 1, nof_index_datatokens - 1, diag_info->datatokens[current_string_token]);
        }

        for (token = 1; token < nof_index_datatokens; token++)
        {
            /*
             * Cases: "number]", "]"
             */
            if (sal_strnstr(index_datatokens[token], "]", DIAG_SW_STATE_MAX_MODULE_STRING) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Invalid input. There is a open bracket in '%s' without number or close bracket. Please edit it!\n",
                             diag_info->datatokens[current_string_token]);
            }
        }
    }

exit:
    if (index_datatokens != NULL)
    {
        utilex_str_split_free(index_datatokens, nof_index_datatokens);
    }
    if (close_brackets_datatokens != NULL)
    {
        utilex_str_split_free(close_brackets_datatokens, close_brackets);
    }
    SHR_FUNC_EXIT;
}
/*
 * The function is:
 *  1) Converting the indexes from string to integer numbers
 *  2) For each empty brackets "[]' -  "DIAG_SW_STATE_DUMP_ARRAY_INDEX_ALL_INDEXES" is set as value
 *  3) Add them to a 2-dimensional array[input_string_token_idx][max_dimension]
 */
static shr_error_e
swstate_diag_dump_convert_string_to_array_indexes(
    int unit,
    char *array_indexes_string,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info,
    uint32 current_string_token)
{
    char **temp_datatokens = NULL;
    char **index_datatokens = NULL;
    uint32 close_brackets = 0, token = 0, nof_index_datatokens;

    SHR_FUNC_INIT_VARS(unit);

    index_datatokens =
        utilex_str_split(array_indexes_string, "[", DIAG_SW_STATE_MAX_MODULE_STRING, &nof_index_datatokens);

    /*
     * token = 0 after split by "[" will always be empty
     */
    for (token = 1; token < nof_index_datatokens; token++)
    {

        if (temp_datatokens != NULL)
        {
            utilex_str_split_free(temp_datatokens, close_brackets);
            temp_datatokens = NULL;
        }
        /*
         * Cases: "number]", "]"
         */
        if (index_datatokens != NULL && sal_strnstr(index_datatokens[token], "]", DIAG_SW_STATE_MAX_MODULE_STRING))
        {
            temp_datatokens =
                utilex_str_split(index_datatokens[token], "]", DIAG_SW_STATE_MAX_MODULE_STRING, &close_brackets);
            if (temp_datatokens != NULL)
            {
                if (!sal_strncasecmp("", temp_datatokens[0], DIAG_SW_STATE_MAX_MODULE_STRING)
                    && !sal_strncasecmp("", temp_datatokens[1], DIAG_SW_STATE_MAX_MODULE_STRING))
                {
                    /*
                     * If there is not specified array_index we set 'DIAG_SW_STATE_DUMP_ARRAY_INDEX_ALL_INDEXES'
                     * Example: param[]
                     */
                    dump_diag_info->array_indexes[current_string_token][token - 1].dimension_array_index =
                        DIAG_SW_STATE_DUMP_ARRAY_INDEX_ALL_INDEXES;
                }
                else if (!sal_strncasecmp("", temp_datatokens[1], DIAG_SW_STATE_MAX_MODULE_STRING))
                {
                    /*
                     * If there is a specified array_index we set the given value
                     * Example: param[2]
                     * The 'token' is redused by 1 because the token = 0 after split by "[" is ("") - empty
                     */
                    dump_diag_info->array_indexes[current_string_token][token - 1].dimension_array_index =
                        sal_strtol(temp_datatokens[0], NULL, 10);
                }
                else
                {
                    /*
                     * If there is a specified array_index but there is number out of the brackets
                     * Example: param[2]3
                     */
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Invalid input. There is a number without brackets in '%s'. Please edit it!\n",
                                 diag_info->datatokens[current_string_token]);
                }
            }
        }
    }

exit:
    if (temp_datatokens != NULL)
    {
        utilex_str_split_free(temp_datatokens, close_brackets);
    }
    if (index_datatokens != NULL)
    {
        utilex_str_split_free(index_datatokens, nof_index_datatokens);
    }
    SHR_FUNC_EXIT;
}

/*
 * The function is setting the data given by the user for the dump command
 * For the pre_process function is needed string without any array indexes information in it,
 * so they are cut and set into diag_info->array_indexes[nof_tokens][DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS]
 */
static shr_error_e
swstate_diag_dump_pre_process_arguments(
    int unit,
    sh_sand_control_t * sand_control,
    args_t * args,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info)
{
    char *var;
    char *remove_array_info = NULL;
    int token;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * get NAME
     */
    SH_SAND_GET_STR("name", var);
    diag_info->name_str = var;
    sal_strncpy(dump_diag_info->dump_string, var, DIAG_SW_STATE_MAX_MODULE_STRING - 1);
    /*
     * Splitting the string given by the user
     */
    diag_info->datatokens =
        utilex_str_split(dump_diag_info->dump_string, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info->nof_tokens);

    /*
     * Allocating memory for param_indexes_by_token
     */
    dump_diag_info->param_indexes_by_token =
        (uint32 *) sal_alloc(sizeof(uint32) * diag_info->nof_tokens, "dump_diag_infos_array_param_indexes_by_token");

    /*
     * Allocating memory for array_index
     */
    dump_diag_info->array_indexes =
        (dnxc_sw_state_dump_array_indexes_values_info_diag_t **)
        sal_alloc(sizeof(dnxc_sw_state_dump_array_indexes_values_info_diag_t *) * diag_info->nof_tokens,
                  "dump_diag_info_array_indexes_dim0");
    for (int i = 0; i < diag_info->nof_tokens; i++)
    {
        dump_diag_info->array_indexes[i] =
            (dnxc_sw_state_dump_array_indexes_values_info_diag_t *)
            sal_alloc(sizeof(dnxc_sw_state_dump_array_indexes_values_info_diag_t) *
                      DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS, "dump_diag_info_array_indexes_dim1");
        for (int dim = 0; dim < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dim++)
        {
            dump_diag_info->array_indexes[i][dim].dimension_array_index = DIAG_SW_STATE_DUMP_ARRAY_INDEX_INVALID;
        }
    }

    for (token = 0; token < diag_info->nof_tokens; token++)
    {
        remove_array_info = sal_strnstr(diag_info->datatokens[token], "[", DIAG_SW_STATE_MAX_MODULE_STRING);
        if (remove_array_info)
        {
            /*
             * Removing array_index information from the diag_info->name_str
             * which is going to be used for finding the module_id and the param_idx
             */
            utilex_sub_string_replace(diag_info->name_str, remove_array_info, "");
            /*
             * Verify function for brackets format
             */
            SHR_IF_ERR_EXIT(swstate_diag_dump_array_indexes_string_format_verify
                            (unit, remove_array_info, diag_info, token));
            /*
             * Converting the string values to integer numebrs and setting the them into an array
             */
            SHR_IF_ERR_EXIT(swstate_diag_dump_convert_string_to_array_indexes
                            (unit, remove_array_info, diag_info, dump_diag_info, token));
        }
    }
    diag_info->datatokens =
        utilex_str_split(diag_info->name_str, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info->nof_tokens);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * The function is setting the data given by the user
 */
static shr_error_e
swstate_diag_pre_process_arguments(
    int unit,
    sh_sand_control_t * sand_control,
    args_t * args,
    dnxc_sw_state_diag_t * diag_info)
{
    char *var;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get NAME
     */
    SH_SAND_GET_STR("name", var);
    diag_info->name_str = var;
    diag_info->datatokens =
        utilex_str_split(diag_info->name_str, ".", DIAG_SW_STATE_MAX_MODULE_STRING, &diag_info->nof_tokens);
exit:
    SHR_FUNC_EXIT;
}

/*
* The function calling several function:
* 1) diag_dnx_swstate_module_list_print - is printing all the init modules
* 2) swstate_module_data_get - which is getting the module_id and layout_address
* 3) dnxc_sw_state_param_idx_get - which is getting the parameter index in the module tree
*/
shr_error_e
swstate_diag_pre_process(
    int unit,
    sh_sand_control_t * sand_control,
    args_t * args,
    dnxc_sw_state_diag_t * diag_info)
{

    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Case when there isn't module name
     */
    if (diag_info->datatokens == NULL)
    {
        if (diag_info->nof_tokens == 0 && !diag_info->all_flag)
        {
            /*
             * Display list of modules
             */
            diag_dnx_swstate_module_list_print(unit, sand_control);
            SHR_IF_ERR_EXIT_NO_MSG(BCM_E_INTERNAL);
        }
    }
    else
    {
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
         * Setting the count_layout_tree_level(depth level) to 1 because we know the module 
         */
        diag_info->count_layout_tree_level = 1;
        /*
         * Get the parameter index
         */
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
         * Check if parameter index is invalid
         */
        if (diag_info->nof_tokens != diag_info->count_layout_tree_level)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter name '%s' \n", diag_info->name_str);
        }
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
    dnxc_sw_state_diag_t * diag_info,
    sh_sand_control_t * sand_control,
    args_t * args)
{
    int token, index, dimension = 0;
    uint32 nof_tokens = 0;
    char **datatokens = NULL;
    char buffer[DIAG_SW_STATE_MAX_MODULE_STRING];
    char *label_str = NULL, *print_data = NULL;
    char print_str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(label_str, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "label_str", "%s%s%s", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(print_data, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "print_data", "%s%s%s", EMPTY,
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
    if (diag_info->layout_address[diag_info->param_idx].to_string_cb == NULL)
    {
        PRT_CELL_SET("%s", "N/A");
    }
    else
    {
        if (diag_info->layout_address[diag_info->param_idx].default_value != NULL)
        {
            diag_info->layout_address[diag_info->param_idx].to_string_cb(unit,
                                                                         diag_info->layout_address
                                                                         [diag_info->param_idx].default_value,
                                                                         &print_data);
            PRT_CELL_SET("%s", print_data);
        }
        else
        {
            PRT_CELL_SET("%s", "0");
        }
    }
    SET_EMPTY(label_str);
    SET_EMPTY(print_data);
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
    SHR_FREE(print_data);
    SHR_FREE(label_str);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

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
    dnxc_sw_state_diag_t diag_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&diag_info, 0, sizeof(dnxc_sw_state_diag_t));

    SHR_IF_ERR_EXIT(swstate_diag_pre_process_arguments(unit, sand_control, args, &diag_info));
    rv = swstate_diag_pre_process(unit, sand_control, args, &diag_info);
    /*
     * "_SHR_E_INTERNAL" error is ignored because it is return only when the all active modules are printed
     */
    if (rv == _SHR_E_INTERNAL)
    {
        SHR_EXIT();
    }
    else
    {
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
    dnxc_sw_state_diag_t * diag_info)
{
    int index = 0, iteration;
    dnxc_sw_state_size_diag_t temp_size_get_struct;
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
    dnxc_sw_state_diag_t * diag_info)
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
            PRT_CELL_SET("%d.%03d KB.", BYTES_TO_KBS(diag_info->size_get_list[iteration].total_size),
                         BYTES_TO_KBS_CARRY(diag_info->size_get_list[iteration].total_size));
            PRT_CELL_SET("%d bytes", diag_info->size_get_list[iteration].total_size);
        }
        else
        {
            PRT_CELL_SET("%d.%06d MB.", BYTES_TO_MBS(diag_info->size_get_list[iteration].total_size),
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
static shr_error_e
cmd_dnx_swstate_size_get_verify(
    int unit,
    dnxc_sw_state_diag_t * diag_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (diag_info->nof_tokens > 0 && diag_info->all_flag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "All flag can't be use in combination with name option.\n The all flag can use only in this way: swstate_new size_get all");
    }
exit:
    SHR_FUNC_EXIT;
}

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
    dnxc_sw_state_diag_t diag_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&diag_info, 0, sizeof(dnxc_sw_state_diag_t));
    SHR_IF_ERR_EXIT(swstate_diag_pre_process_arguments(unit, sand_control, args, &diag_info));
    SH_SAND_IS_PRESENT("all", diag_info.all_flag);
    SHR_IF_ERR_EXIT(cmd_dnx_swstate_size_get_verify(unit, &diag_info));
    if (!diag_info.all_flag)
    {
        rv = swstate_diag_pre_process(unit, sand_control, args, &diag_info);
        /*
         * "_SHR_E_INTERNAL" error is ignored because it is return only when the all active modules are printed
         */
        if (rv == _SHR_E_INTERNAL)
        {
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
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
 * The function is setting the number of iteration
 * which has to be made so the whole data will be printed
 * and the offset needed to update the address in each iteration.
 */
uint32
max_iteration_of_special_type_print(
    int unit,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info,
    uint32 *nof_iteration)
{
    uint32 dimension = 0, nof_elements, element_size, size;
    SHR_FUNC_INIT_VARS(unit);
    *nof_iteration = 1;

    for (dimension = 0; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
    {
        if (diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].num_elements == 0
            && diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type ==
            DNXC_SWSTATE_ARRAY_INDEX_INVALID)
        {
            break;
        }
        else if (diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BITMAP ||
                 diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BUFFER)
        {
            *nof_iteration = diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].num_elements;
        }
        else if (diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][diag_info->module_id].alloc_size(unit,
                                                                                            DNXC_SW_STATE_LAYOUT_NODE_ID_GET
                                                                                            (diag_info->module_id,
                                                                                             diag_info->param_idx),
                                                                                            dump_diag_info->param_addr,
                                                                                            &nof_elements,
                                                                                            &element_size));
            size = nof_elements * element_size;
            *nof_iteration = size / 32;
            if (size % 32 != 0)
            {
                *nof_iteration += 1;
            }
            break;
        }
        else if (diag_info->layout_address[diag_info->param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BUFFER)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][diag_info->module_id].alloc_size(unit,
                                                                                            DNXC_SW_STATE_LAYOUT_NODE_ID_GET
                                                                                            (diag_info->module_id,
                                                                                             diag_info->param_idx),
                                                                                            dump_diag_info->param_addr,
                                                                                            &nof_elements,
                                                                                            &element_size));
            size = nof_elements * element_size;
            *nof_iteration = size;
            break;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - Dump of the given param index
 */
static shr_error_e
diag_dnx_swstate_dump_print(
    int unit,
    sh_sand_control_t * sand_control,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info)
{
    uint32 max_iteration = 0, iteration;
    char *print_data = NULL;
    char print_str_name[DIAG_SW_STATE_MAX_MODULE_STRING] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(print_data, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "label_str", "%s%s%s", EMPTY,
                       EMPTY, EMPTY);
    
    if (dnx_sw_state_compare(dump_diag_info->filters.typefilter, diag_info->layout_address[diag_info->param_idx].type)
        != TRUE && sal_strncasecmp(dump_diag_info->filters.typefilter, "", DIAG_SW_STATE_MAX_MODULE_STRING))
    {
        SHR_EXIT();
    }

    sal_strncpy(print_str_name, diag_info->layout_address[diag_info->param_idx].name,
                DIAG_SW_STATE_MAX_MODULE_STRING - 1);
    utilex_sub_string_replace(print_str_name, "__", ".");
    if (dnx_sw_state_compare(dump_diag_info->filters.namefilter, print_str_name) != TRUE
        && sal_strncasecmp(dump_diag_info->filters.namefilter, "", DIAG_SW_STATE_MAX_MODULE_STRING))
    {
        SHR_EXIT();
    }

    /*
     * Print parameter path with '.'
     */
    sal_strncpy(print_str_name, diag_info->layout_address[diag_info->param_idx].name,
                DIAG_SW_STATE_MAX_MODULE_STRING - 1);

    PRT_TITLE_SET("DUMP INFORMATION FOR :");
    PRT_COLUMN_ADD("INFO:");
    PRT_COLUMN_ADD("VALUE:");

    /*
     * The function is providing the size of the offset
     * and how many iteration are needed so
     * the whole data for special types is printed
     */
    max_iteration_of_special_type_print(unit, diag_info, dump_diag_info, &max_iteration);

    /*
     * The special types - buffer, bitmap had to be printed on several lines depending on the nof_bits/size
     */
    for (iteration = 0; iteration < max_iteration; iteration++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
        if (max_iteration > 1)
        {
            PRT_CELL_SET("%s[%d]", dump_diag_info->dump_string, iteration);
        }
        else
        {
            PRT_CELL_SET("%s", dump_diag_info->dump_string);
        }

        if (diag_info->layout_address[diag_info->param_idx].to_string_cb == NULL)
        {
            PRT_CELL_SET("%s", "to_string_cb is not provide");
        }
        else
        {
            if (iteration != 0)
            {
                diag_info->layout_address[diag_info->param_idx].to_string_cb(unit,
                                                                             ((uint8 *) dump_diag_info->param_addr +
                                                                              diag_info->layout_address
                                                                              [diag_info->param_idx].size_of),
                                                                             &print_data);
            }
            else
            {
                diag_info->layout_address[diag_info->param_idx].to_string_cb(unit, dump_diag_info->param_addr,
                                                                             &print_data);
            }
            PRT_CELL_SET("%s", print_data);
        }
    }

    PRT_COMMITX;
exit:
    SHR_FREE(print_data);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * The function is:
 *      1) Calling verification function for the array_indexes values
 */
static shr_error_e
diag_dnx_swstate_dump_argument_verify(
    int unit,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verifing the array_indexes given by the user
     */
    SHR_IF_ERR_EXIT(diag_dnx_swstate_dump_array_indexes_verify(unit, diag_info, dump_diag_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function which is calcuclating the given dimention offset
 *
 * Example if it is 3-dim static array [max=2][max=5][max=6] and the given is arr[2][3][1]
 *  It will be called 3 times in this case:
 *      The calc for dim0 = 2 * max_size_dim_1(5) * max_size_dim_2(6) * sizeof(type)
 *      The calc for dim1 = 3 * max_size_dim_2(6) * sizeof(type)
 *      The calc for dim2 = 1 * sizeof(type)
 */
static uint32
multiplication_static_array(
    int unit,
    uint32 current_array_idx,
    uint32 current_dim,
    uint32 param_idx,
    dnxc_sw_state_diag_t * diag_info)
{
    uint32 dimension = 0;
    uint32 dynamic_offset = 1;

    for (dimension = DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS - 1; dimension > current_dim; dimension--)
    {
        if (diag_info->layout_address[param_idx].array_indexes[dimension].num_elements != 0
            && diag_info->layout_address[param_idx].array_indexes[dimension].num_elements !=
            DNXC_SW_STATE_LAYOUT_INVALID)
        {
            dynamic_offset *= diag_info->layout_address[param_idx].array_indexes[dimension].num_elements;
        }
        else
        {
            dynamic_offset *= 1;
        }
    }

    dynamic_offset *= current_array_idx * diag_info->layout_address[param_idx].size_of;

    return dynamic_offset;
}

/*
 * Updating the address pointer of the reached parameter
 * depending on its array indexes information.
 */
static uint32
diag_dnx_swstate_dump_array_indexes_multidimension_calculation(
    int unit,
    uint32 token,
    uint32 curr_dim,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info,
    void **begin_ptr)
{
    void **temp_double_ptr = NULL;
    void *temp_ptr = NULL;
    uint32 param_idx, dimension;
    uint32 offset = 0;
    param_idx = dump_diag_info->param_indexes_by_token[token];

    for (dimension = curr_dim; dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS; dimension++)
    {

        if (diag_info->layout_address[param_idx].array_indexes[dimension].index_type ==
            DNXC_SWSTATE_ARRAY_INDEX_INVALID)
        {
            break;
        }
        /*
         * The current dimension is dynamic
         */
        else if (is_dynamic_array_dimension(unit, diag_info->layout_address, param_idx, dimension))
        {
            if (dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS - 1)
            {
                /*
                 * The current dimension is dynamic and the next is invalid.
                 * The pointer had to be updated and then the offset
                 * is updated depending on the array_index value
                 */
                if (diag_info->layout_address[param_idx].array_indexes[dimension + 1].index_type ==
                    DNXC_SWSTATE_ARRAY_INDEX_INVALID
                    || diag_info->layout_address[param_idx].array_indexes[dimension + 1].index_type ==
                    DNXC_SWSTATE_ARRAY_INDEX_STATIC)
                {
                    offset =
                        dump_diag_info->array_indexes[token][curr_dim].dimension_array_index *
                        diag_info->layout_address[param_idx].size_of;
                }
                /*
                 * The current dimension is dynamic and the next is static
                 * First the pointer had to be updated and then the offset
                 * had to be add the same way as for a static dimension
                 */
                else if (diag_info->layout_address[param_idx].array_indexes[dimension + 1].index_type ==
                         DNXC_SWSTATE_ARRAY_INDEX_STATIC)
                {
                    offset =
                        multiplication_static_array(unit,
                                                    dump_diag_info->
                                                    array_indexes[token][dimension].dimension_array_index, dimension,
                                                    param_idx, diag_info);
                }
                /*
                 * The current dimension is dynamic and the next is dynamic
                 */
                else if (is_dynamic_array_dimension(unit, diag_info->layout_address, param_idx, dimension + 1))
                {
                    offset = (dump_diag_info->array_indexes[token][curr_dim].dimension_array_index) * sizeof(void *);
                    temp_ptr = ((uint8 *) *begin_ptr + offset);
                    *begin_ptr = temp_ptr;
                    offset = 0;
                }
            }
            /*
             * The last dynamic dimension 
             */
            else
            {
                offset =
                    dump_diag_info->array_indexes[token][curr_dim].dimension_array_index *
                    diag_info->layout_address[param_idx].size_of;
            }
            /*
             * Updating pointer
             */
            temp_double_ptr = *begin_ptr;
            *begin_ptr = ((uint8 *) *temp_double_ptr + offset);
            dump_diag_info->param_addr = *begin_ptr;
        }
        /*
         * The current dimension is static
         */
        else if (diag_info->layout_address[param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_STATIC)
        {
            if (dimension < DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS - 1)
            {
                /*
                 * The current dimension is static and the next is dynamic
                 */
                if (is_dynamic_array_dimension(unit, diag_info->layout_address, param_idx, dimension + 1))
                {
                    offset = (dump_diag_info->array_indexes[token][dimension].dimension_array_index) * sizeof(void *);
                }
                /*
                 * The current dimension it is static and the next is invalid
                 */
                else if (diag_info->layout_address[param_idx].array_indexes[dimension + 1].index_type ==
                         DNXC_SWSTATE_ARRAY_INDEX_INVALID)
                {
                    offset =
                        multiplication_static_array(unit,
                                                    dump_diag_info->
                                                    array_indexes[token][dimension].dimension_array_index, dimension,
                                                    param_idx, diag_info);
                }
                /*
                 * The current dimension it is static and the next is static
                 */
                else if (diag_info->layout_address[param_idx].array_indexes[dimension + 1].index_type ==
                         DNXC_SWSTATE_ARRAY_INDEX_STATIC)
                {
                    offset =
                        multiplication_static_array(unit,
                                                    dump_diag_info->
                                                    array_indexes[token][dimension].dimension_array_index, dimension,
                                                    param_idx, diag_info);
                }
            }
            /*
             * The last static dimension and the biggest possible dimension value 
             */
            else
            {
                offset =
                    multiplication_static_array(unit,
                                                dump_diag_info->array_indexes[token][dimension].dimension_array_index,
                                                dimension, param_idx, diag_info);
            }

            /*
             * Updating pointer
             */
            temp_ptr = ((uint8 *) *begin_ptr + offset);
            *begin_ptr = temp_ptr;
            dump_diag_info->param_addr = temp_ptr;
        }
        /*
         * The current dimension is holding the nof_bits for BITMAP
         * or the size of BUFFER
         */
        else if (diag_info->layout_address[param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BITMAP
                 || diag_info->layout_address[param_idx].array_indexes[dimension].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BUFFER)
        {
            dump_diag_info->param_addr = *begin_ptr;
            break;
        }
    }

    return 0;
}

/*
 * The function is computing the address of the whole paramater_path.
 * The offset is updated at each depth path level.
 * The path level is splited to tokens and each part is added one by one.
 */
static shr_error_e
diag_dnx_swstate_dump_compute_addresses(
    int unit,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info,
    void *begin_ptr,
    uint32 begin_token)
{
    uint32 offset = 0, current_param_idx = 0, token;

    SHR_FUNC_INIT_VARS(unit);

    for (token = begin_token; token < diag_info->nof_tokens; token++)
    {
        current_param_idx = dump_diag_info->param_indexes_by_token[token];
        /*
         * Calculation each token offset
         */
        offset += sw_state_layout_parameter_address_offset_get(unit,
                                                               DNXC_SW_STATE_LAYOUT_NODE_ID_GET(diag_info->module_id,
                                                                                                current_param_idx));

        /*
         * Calculation each token array_indexes offset if they are any
         */
        /*
         * Currently "-1" is not supported will return an error
         */
        if (diag_info->layout_address[current_param_idx].array_indexes[0].num_elements != 0
            && diag_info->layout_address[current_param_idx].array_indexes[0].index_type !=
            DNXC_SWSTATE_ARRAY_INDEX_INVALID)
        {
            if ((has_dynamic_dim(unit, diag_info->layout_address, current_param_idx)
                 || diag_info->layout_address[diag_info->param_idx].array_indexes[0].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BUFFER
                 || diag_info->layout_address[diag_info->param_idx].array_indexes[0].index_type ==
                 DNXC_SWSTATE_ARRAY_INDEX_BITMAP) && offset % 8 != 0)
            {
                /*
                 * If the next parameter has at least one dynamic dimension - the offset had to be align
                 */
                offset += offset % 8;
            }
            begin_ptr = (void *) ((uint8 *) begin_ptr + offset);
            diag_dnx_swstate_dump_array_indexes_multidimension_calculation(unit, token, 0, diag_info, dump_diag_info,
                                                                           &begin_ptr);
            begin_ptr = dump_diag_info->param_addr;
            offset = 0;
        }
    }

    dump_diag_info->param_addr = (void *) ((uint8 *) begin_ptr + offset);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * The function is:
 *      1) Computing address
 *      2) Calling print function
 */
static shr_error_e
diag_dnx_swstate_dump_print_process(
    int unit,
    sh_sand_control_t * sand_control,
    dnxc_sw_state_diag_t * diag_info,
    dnxc_sw_state_dump_diag_t * dump_diag_info)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Computing param_address
     */
    SHR_IF_ERR_EXIT(diag_dnx_swstate_dump_compute_addresses
                    (unit, diag_info, dump_diag_info,
                     sw_state_module_root_struct_address_get(unit, diag_info->module_id), 1));

    diag_dnx_swstate_dump_print(unit, sand_control, diag_info, dump_diag_info);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump of the given param index
 * If the received string:
 *    1) is empty will be printed table with all of the active modules
 *    2) is invalid will be return an error
 *    3) is valid print table
 */
static shr_error_e
cmd_dnx_swstate_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnxc_sw_state_diag_t diag_info;
    dnxc_sw_state_dump_diag_t dump_diag_info;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&diag_info, 0, sizeof(dnxc_sw_state_diag_t));
    sal_memset(&dump_diag_info, 0, sizeof(dnxc_sw_state_dump_diag_t));
    SHR_ALLOC_SET_ZERO(dump_diag_info.dump_string, (DIAG_SW_STATE_MAX_MODULE_STRING * sizeof(char)) + 1, "dump_string",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Get dump options
     */
    SH_SAND_GET_BOOL("nocontent", dump_diag_info.filters.nocontent);
    SH_SAND_GET_STR("typefilter", dump_diag_info.filters.typefilter);
    SH_SAND_GET_STR("namefilter", dump_diag_info.filters.namefilter);
    /*
     *'swstate_diag_dump_pre_process_arguments' is setting the data given by the user for the dump command
     *      1) All the array indexes, if they are any, are removed from the main input string
     *      2) Verify for brackets format
     *      3) Array indexes are converted from string to integer
     */
    SHR_IF_ERR_EXIT(swstate_diag_dump_pre_process_arguments(unit, sand_control, args, &diag_info, &dump_diag_info));

    /*
     * 'swstate_diag_pre_process' is
     *      1) Finding the module_id, if not - returning an error
     *      2) Finding the parameter_idx, if not - returning an error
     *      3) Setting the layout address
     */
    rv = swstate_diag_pre_process(unit, sand_control, args, &diag_info);
    /*
     * "_SHR_E_INTERNAL" error is ignored because it is return only when the all active modules are printed
     */
    if (rv == _SHR_E_INTERNAL)
    {
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(diag_dnx_swstate_dump_argument_verify(unit, &diag_info, &dump_diag_info));

    
    SHR_IF_ERR_EXIT(diag_dnx_swstate_dump_print_process(unit, sand_control, &diag_info, &dump_diag_info));

exit:
    SHR_FREE(dump_diag_info.dump_string);
    for (int i = 0; i < diag_info.nof_tokens; i++)
    {
        sal_free(dump_diag_info.array_indexes[i]);
    }
    sal_free(dump_diag_info.array_indexes);
    if (diag_info.datatokens != NULL && diag_info.nof_tokens > 0)
    {
        utilex_str_split_free(diag_info.datatokens, diag_info.nof_tokens);
    }
    SHR_FUNC_EXIT;
}

/**
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
    dnxc_sw_state_diag_t diag_info = { 0 };

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
    {"all",  SAL_FIELD_TYPE_BOOL, "Print the total sizes for all active SW State modules. Use:'swstate_new size_get all'. Can't be use in combination with parameter.", "false"},
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

sh_sand_option_t dnx_swstate_dump_layout_options[] = {
    {"name",           SAL_FIELD_TYPE_STR, "[<module_name>[.<parent_name>[.<child_name>[.<grandchild_name>s]]]]", "", NULL, NULL, SH_SAND_ARG_FREE, .ac_option_cb=diag_dnx_swstate_autocomplete_cb},
    {"nocontent",      SAL_FIELD_TYPE_BOOL, "Decision if to perform content dump or to print the access tree.",  "false"},
    {"typefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable type. If the written type is correct - it will dump the data, if not  - it won't print anything.",         ""},
    {"namefilter",     SAL_FIELD_TYPE_STR,  "Filter for the variable name.",         ""},
    {NULL}
};


sh_sand_man_t dnx_swstate_dump_layout_man = {
    .brief = "swstate dump",
};

sh_sand_man_t sh_swstate_new_man = {
    .brief = "SW State diagnostic commands\n Expected format - swstate <command> <module>.<parent-param>.<child-param>.<child-param>",
};

sh_sand_cmd_t sh_dnxc_swstate_new_cmds[] = {
    /*
     * keyword,        action,             cmd,           options,                          man,                      optional_cb,     invoke,      flags
     */
    {"info_get",  cmd_dnx_swstate_info_get, NULL, dnx_swstate_info_get_layout_options, &dnx_swstate_info_get_layout_man,    NULL,       NULL,      SH_CMD_NO_XML_VERIFY},
    {"size_get",  cmd_dnx_swstate_size_get, NULL, dnx_swstate_size_get_layout_options, &dnx_swstate_size_get_layout_man,    NULL,       NULL,      SH_CMD_NO_XML_VERIFY},
    {"dump",      cmd_dnx_swstate_dump,     NULL, dnx_swstate_dump_layout_options,     &dnx_swstate_dump_layout_man,        NULL,       NULL,      SH_CMD_NO_XML_VERIFY},
    {NULL}
};
/* *INDENT-ON* */
/**
 * }
 */
#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef BSL_LOG_MODULE
