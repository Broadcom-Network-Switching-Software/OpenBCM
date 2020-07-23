/**
 * \file diag_sand_framework.c
 *
 * Framework for sand shell commands development
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#endif

#include <sal/appl/sal.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include "diag_sand_framework_autocomplete.h"

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif /* BCM_WARM_BOOT_API_TEST */

#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_device.h>
#endif

#if defined(BCM_DNXF_SUPPORT)
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_device.h>
#endif

#if defined(BCM_DPP_SUPPORT)
#include<soc/dpp/drv.h>
#endif /* defined(BCM_DPP_SUPPORT) */

#include "diag_sand_framework_internal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/* Initialized by global init_init */
static sh_sand_cmd_t *sh_sand_cmd_global[SOC_MAX_NUM_DEVICES] = { NULL };

/**
 * \brief Set root shell command pointer
 * \param [in] unit             - unit id
 * \param [in] sh_sand_cmd_a    - root shell command pointer
 * \return
 *   \retval _SHR_E_NONE      - If the device is DNX
 *   \retval _SHR_E_INTERNAL  - If root already set
 */
static shr_error_e
sh_sand_cmd_root_set(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((sh_sand_cmd_a != NULL) && (sh_sand_cmd_global[unit] != NULL))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Shell root already set for unit:%d\n", unit);
    }

    sh_sand_cmd_global[unit] = sh_sand_cmd_a;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_cmd_root_get(
    int unit,
    sh_sand_cmd_t ** sh_sand_cmd_a_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd_a_p, _SHR_E_PARAM, "sh_sand_cmd_a_p");

    if (sh_sand_cmd_global[unit] == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Shell root was not set for unit:%d\n", unit);
    }

    *sh_sand_cmd_a_p = sh_sand_cmd_global[unit];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Add device specific set of commands to general dynamic list
 * \param [in] unit - unit id
 * \param [in] sh_sand_cmd_a - pointer to command list to start from
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 */
static shr_error_e
sh_sand_cmd_add_to_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_cmd_t *sh_sand_cmd;
    int flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_CMD_ITERATOR_COND(sh_sand_cmd, sh_sand_cmd_a, flags)
    {
        cmd_t cmd;

         /* coverity[copy_paste_error : FALSE]  */
        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(unit, sh_sand_cmd->keyword, sh_sand_cmd->keyword,
                                                  &sh_sand_cmd->short_key, &sh_sand_cmd->full_key, SH_CMD_VERIFY), "");

        if (ISEMPTY(sh_sand_cmd->full_key))
        {
            cmd.c_cmd = sh_sand_cmd->keyword;
        }
        else
        {
            cmd.c_cmd = sh_sand_cmd->full_key;
        }
        cmd.c_f = cmd_callback;
        if (sh_sand_cmd->man == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "No man page for top level command %s\n", sh_sand_cmd->keyword);
        }
        cmd.c_help = sh_sand_cmd->man->brief;
        if (!ISEMPTY(sh_sand_cmd->man->full))
            cmd.c_usage = sh_sand_cmd->man->full;
        else
            cmd.c_usage = sh_sand_cmd->man->brief;

        if (cmdlist_add(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to add:\"%s\" command\n", sh_sand_cmd->keyword);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Remove device specific set of commands from general dynamic list
 * \param [in] unit - unit id
 * \param [in] sh_sand_cmd_a - pointer to command list to start from
 * \param [in] cmd_callback - command to be called by general shell, serving entry point to the the framework
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval Other            - Other errors as per shr_error_e
 * \remark
 */
static shr_error_e
sh_sand_cmd_remove_from_dyn_list(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_cmd_t *sh_sand_cmd;
    int flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_CMD_ITERATOR_COND(sh_sand_cmd, sh_sand_cmd_a, flags)
    {
        cmd_t cmd;

        if (ISEMPTY(sh_sand_cmd->full_key))
        {
            cmd.c_cmd = sh_sand_cmd->keyword;
        }
        else
        {
            cmd.c_cmd = sh_sand_cmd->full_key;
        }
        cmd.c_f = cmd_callback;
        if (sh_sand_cmd->man == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "No man page for top level command %s\n", sh_sand_cmd->keyword);
        }
        cmd.c_help = sh_sand_cmd->man->brief;
        if (!ISEMPTY(sh_sand_cmd->man->full))
            cmd.c_usage = sh_sand_cmd->man->full;
        else
            cmd.c_usage = sh_sand_cmd->man->brief;

        if (cmdlist_remove(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to remove:\"%s\" command\n", sh_sand_cmd->keyword);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_param_u full_param_mask = {
    .array_uint32[0] = 0xFFFFFFFF,
    .array_uint32[1] = 0xFFFFFFFF,
    .array_uint32[2] = 0xFFFFFFFF,
    .array_uint32[3] = 0xFFFFFFFF,
    .array_uint32[4] = 0xFFFFFFFF,
    .array_uint32[5] = 0xFFFFFFFF,
    .array_uint32[6] = 0xFFFFFFFF,
    .array_uint32[7] = 0xFFFFFFFF,
    .array_uint32[8] = 0xFFFFFFFF,
    .array_uint32[9] = 0xFFFFFFFF,
    .array_uint32[10] = 0xFFFFFFFF,
    .array_uint32[11] = 0xFFFFFFFF,
    .array_uint32[12] = 0xFFFFFFFF,
    .array_uint32[13] = 0xFFFFFFFF,
    .array_uint32[14] = 0xFFFFFFFF,
    .array_uint32[15] = 0xFFFFFFFF,
};

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t sand_bool_table[] = {
    {"False", FALSE},
    {"True", TRUE},
    {"Y", TRUE},
    {"N", FALSE},
    {"Yes", TRUE},
    {"No", FALSE},
    {"On", TRUE},
    {"Off", FALSE},
    {"Yep", TRUE},
    {"Nope", FALSE},
    {"1", TRUE},
    {"0", FALSE},
    {"OKay", TRUE},
    {"YOUBET", TRUE},
    {"NOWay", FALSE},
    {"YEAH", TRUE},
    {"NOT", FALSE},
    {NULL}
};

char *
sh_sand_bool_str(
    int bool)
{
    if (bool == FALSE)
        return sand_bool_table[0].string;
    else
        return sand_bool_table[1].string;
}

char *
sh_sand_enum_str(
    sh_sand_enum_t * enum_entry,
    int value)
{
    if (enum_entry == NULL)
    {
        return NULL;
    }

    for (; enum_entry->string != NULL; enum_entry++)
    {
        if (enum_entry->value == value)
        {
            return enum_entry->string;
        }
    }

    return NULL;
}

void
sh_sand_time_get(
    char *time_str)
{
#if defined(UNIX) && !defined(__KERNEL__)
    time_t time;
    struct tm *timestruct;

    time = sal_time();
    timestruct = sal_localtime(&time);
    sal_strftime(time_str, SH_SAND_MAX_TIME_SIZE, "%Y-%m-%d_%H.%M.%S", timestruct);
    return;
#endif
}

/**
 * \brief - parse 'port' var and convert to ports bitmap
 * See SAL_FIELD_TYPE_PORT for more info
 */
static shr_error_e
sh_sand_port_bitmap_get(
    int unit,
    char *diag_port_input,
    bcm_pbmp_t * logical_ports_bitmap)
{
    bcm_port_t logical_port;
    char *diag_port_input_copy = NULL;
    char *str_ptr;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear bitmap */
    BCM_PBMP_CLEAR(*logical_ports_bitmap);

    /** empty string */
    if (diag_port_input == NULL || *diag_port_input == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port: unexpected empty string.\n");
    }
    /** copy the input string - to be able to modify it */
    diag_port_input_copy = sal_strdup(diag_port_input);
    if (diag_port_input_copy == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "port: failed to copy string.\n");
    }

    /** Integer value */
    logical_port = sal_ctoi(diag_port_input_copy, &str_ptr);
    /*
     *  if str_ptr does not point to the end of the string failed to parse.
     *  if failed - continue to the other options
     */
    if (*str_ptr == 0)
    {
        BCM_PBMP_PORT_ADD(*logical_ports_bitmap, logical_port);
        SHR_EXIT();
    }

    /*
     * Logical port type -
     * Currently supporting 'tm_ing'/'tm_egr'/'tm_e2e'/'fabric'/'nif'/'all'
     * Supported only by DNX!
     */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        dnx_algo_port_logicals_type_e logicals_type;

        logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_INVALID;
        if (!sal_strcasecmp(diag_port_input_copy, "tm_ing"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "tm_egr"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "tm_e2e"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "fabric"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "nif"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_NIF;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "eth"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "sat"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_SAT;
        }
        else if (!sal_strcasecmp(diag_port_input_copy, "all"))
        {
            logicals_type = DNX_ALGO_PORT_LOGICALS_TYPE_VALID;
        }
        if (logicals_type != DNX_ALGO_PORT_LOGICALS_TYPE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, logicals_type, 0, logical_ports_bitmap));
            SHR_EXIT();
        }
    }
#endif

    /*
     * Parse by port name
     */
    if (SHR_FAILURE(parse_bcm_pbmp(unit, diag_port_input_copy, logical_ports_bitmap)))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Port string:%s is not supported\n", diag_port_input);
    }

exit:
    SHR_FREE(diag_port_input_copy);
    SHR_FUNC_EXIT;
}

char *
sh_sand_enum_value_text(
    sh_sand_control_t * sand_control,
    char *arg_keyword,
    int enum_value)
{
    sh_sand_arg_t *sand_arg;
    sh_sand_enum_t *enum_entry;

    if (sand_control == NULL)
    {
        return "No command control";
    }

    if ((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->stat_args_list, arg_keyword)) == NULL)
    {
        if ((sand_arg = utilex_rhlist_entry_get_by_name(sand_control->dyn_args_list, arg_keyword)) == NULL)
        {
            return "Command line option is not supported";
        }
    }

    for (enum_entry = sand_arg->ext_ptr; enum_entry->string != NULL; enum_entry++)
    {
        if (enum_entry->value == enum_value)
        {
            return enum_entry->string;
        }
    }
    if (sand_arg->type == SAL_FIELD_TYPE_ENUM)
        return "Value is not supported for option";
    else
        return "";
}

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/**
 * \brief
 *    Extract value from plugin string
 * \param [in] unit - unit id
 * \param [in] plugin_str - plugin string, currently only DNX_DATA is supported
 * \param [out] value_p - pointer to value, that will be extracted from plugin string
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - If input format does not match plugin rules
 *   \retval _SHR_E_NOT_FOUND - If plugin is not supported
 * \remark
 *   Currently of DNX_DATA plugin is supported see dnxc_data_utils_generic_value_get for supported formats
 */
static shr_error_e
sh_sand_extract_value(
    int unit,
    char *plugin_str,
    int *value_p)
{
    char **tokens = NULL;
    uint32 realtokens = 0;
    char **base_tokens = NULL;
    uint32 base_realtokens = 0;
    char *actual_str;
    int subtract_value = 0;
    char *end_ptr;
    
    int value = 0;
    uint32 actual_str_realtokens = 0;
    char **actual_str_tokens = NULL;
    int actual_str_tokens_index;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Parse the string, if there is '-' extract value and subtract it from DNX below
     * More arithmetic operations may be supported
     */
    if ((base_tokens = utilex_str_split(plugin_str, "-", 2, &base_realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", plugin_str);
    }
    if (base_realtokens == 1)
    {   /** No arithmetics involved */
        actual_str = plugin_str;
    }
    else if (base_realtokens == 2)
    {
        actual_str = base_tokens[0];
        subtract_value = sal_strtoul(base_tokens[1], &end_ptr, 0);
        if (!ISEMPTY(end_ptr))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Second token:\"%s\" in range string is not numeric value\n", base_tokens[1]);
        }
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", plugin_str);
    }

    
    if ((actual_str_tokens = utilex_str_split(actual_str, "+", 2, &actual_str_realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", actual_str);
    }

    
    for (actual_str_tokens_index = 0; actual_str_tokens_index < actual_str_realtokens; actual_str_tokens_index++)
    {
        /*
         * Parse the string, if first token is DNX_DATA - fetch value from DNX_DATA
         */
        if ((tokens = utilex_str_split(actual_str_tokens[actual_str_tokens_index], ".", 2, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", plugin_str);
        }

        if ((realtokens == 2) && !sal_strcasecmp(tokens[0], "DNX_DATA"))
        {
            const uint32 *value_str_p = dnxc_data_utils_generic_value_get(unit, tokens[1]);
            if (value_str_p == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR - Failed to obtain value on dnx data string:\"%s\"\n", plugin_str);
            }
            else
            {
                value += *value_str_p;
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "Plugin string is not supported\"%s\"\n", plugin_str);
        }

        if (tokens != NULL)
        {
            utilex_str_split_free(tokens, realtokens);
            tokens = NULL;
        }
    }

    
    *value_p = value - subtract_value;

exit:
    if (base_tokens != NULL)
    {
        utilex_str_split_free(base_tokens, base_realtokens);
    }
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    
    if (actual_str_tokens != NULL)
    {
        utilex_str_split_free(actual_str_tokens, actual_str_realtokens);
    }
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief
 *    Print all strings/value of the neum structure
 * \param [in] unit - unit id
 * \param [in] enum_entry - pointer to array of string/value pairs
 * \return
 *   \retval _SHR_E_NONE      - Always success
 * \remark
 */
static shr_error_e
sh_sand_enum_value_show(
    int unit,
    sh_sand_enum_t * enum_entry)
{
    int value;
    SHR_FUNC_INIT_VARS(unit);

    if (enum_entry == NULL)
    {
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("Supported enum values\n")));
    for (; enum_entry->string != NULL; enum_entry++)
    {
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        /*
         * Plugin support only for DNX/DNXF
         */
        if (ISEMPTY(enum_entry->plugin_str) ||
            (sh_sand_extract_value(unit, enum_entry->plugin_str, &value) != _SHR_E_NONE))
        {
            /*
             * If plugin does not exist or failed take it from explicit value
             */
            value = enum_entry->value;
        }
#else
        value = enum_entry->value;
#endif
        LOG_CLI((BSL_META("\tEnum:%s Value:%d\n"), enum_entry->string, value));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Fetch value from the string to value array according to provided string
 * \param [in] unit - unit id
 * \param [in] enum_entry - pointer to array of string/value pairs
 * \param [in] source - string to search for in array
 * \param [in,out] value_p - pointer value, that will be assigned by routine in case of success
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - If array is NULL
 *   \retval _SHR_E_NOT_FOUND - If source string was not found in array
 * \remark
 *    Calling function behavior may differ depending on the parameter type, when the parameter is bool or enum,
 *    failure here means general failure, if it is other (like int, uint) failure will just cause treatment source as
 *    value itself
 */
static shr_error_e
sh_sand_enum_value_get(
    int unit,
    sh_sand_enum_t * enum_entry,
    char *source,
    int *value_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value_p, _SHR_E_PARAM, "value_p");

    if (enum_entry == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        for (; enum_entry->string != NULL; enum_entry++)
        {
            if (!sal_strcasecmp(enum_entry->string, source))
            {   /** We have match on enum string */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
                /*
                 * Plugin support only for DNX/DNXF
                 */
                if (ISEMPTY(enum_entry->plugin_str) ||
                    (sh_sand_extract_value(unit, enum_entry->plugin_str, value_p) != _SHR_E_NONE))
                {
                    /*
                     * If plugin does not exist or failed take it from explicit value
                     */
                    *value_p = enum_entry->value;
                }
#else
                *value_p = enum_entry->value;
#endif
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Add Zero padding to the source if needed
 * \param [in] unit - unit id
 * \param [in] len - lenth of the source string
 * \param [in] count - lenth of the output string(after padding)
 * \param [in, out] source - string to be padded
 * \return
 *   \retval _SHR_E_NONE      - success.
 * \remark
 */
static shr_error_e
sh_sand_zero_padding_arr_string(
    int unit,
    int len,
    int count,
    char *source)
{
    char *buf = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if the padding is needed.
     */
    if (len < count)
    {
        /*
         * Allocate the "zero" buffer with the wanted length
         */
        buf = sal_alloc((count - len), "Zero Buffer");
        sal_memset(buf, 0, (count - len));

        /*
         * Add the padding to the source.
         */
        sal_sprintf(source, "%s%s", buf, source);
    }

    SHR_EXIT();
exit:
    SHR_FREE(buf);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Used to prepare the array format (array="0x100 0x200 0x300 0x0" -> array=0x000003000000020000000100)
 * \param [in] unit - unit id
 * \param [in, out] source - string to be formated.
 * \return
 *   \retval _SHR_E_NONE      - success.
 * \remark
 */
static shr_error_e
sh_sand_prepare_the_arr_string(
    int unit,
    char *source)
{
    int len;
    char **tokens = NULL;
    uint32 maxtokens = 100;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove the white spaces and split the string to tokens.
     */
    utilex_str_white_spaces_remove(source);
    utilex_str_to_upper(source);
    if ((tokens = utilex_str_split(source, "0X", maxtokens, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Wrong input\"%s\"\n", source);
    }

    /*
     * This means that the user is not using 0x...... format, so do nothing and exit.
     */
    if (realtokens == 1)
    {
        SHR_EXIT();
    }

    len = sal_strnlen(source, SH_SAND_MAX_TOKEN_SIZE);
    sal_memset(source, 0, len);

    /*
     * Add "0x" to the start of the string.
     */
    realtokens--;
    sal_strncpy(source, "0x", SH_SAND_MAX_TOKEN_SIZE);

    while (realtokens != 0)
    {
        if (tokens[realtokens] != NULL)
        {
            /*
             * Add zero padding to the token if its len is smaller than 8.
             */
            SHR_IF_ERR_EXIT(sh_sand_zero_padding_arr_string
                            (unit, sal_strnlen(tokens[realtokens], SH_SAND_MAX_TOKEN_SIZE), 8, tokens[realtokens]));
            /*
             * Append the token to the string.
             */
            sal_strncat(source, tokens[realtokens], sal_strnlen(tokens[realtokens], SH_SAND_MAX_TOKEN_SIZE));
            realtokens--;
        }
    }

    SHR_EXIT();
exit:
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}
shr_error_e
sh_sand_value_get(
    int unit,
    sal_field_type_e type,
    char *source,
    sh_sand_param_u * target,
    void *ext_ptr)
{
    char *end_ptr;
    char *source_buf = NULL;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case SAL_FIELD_TYPE_BOOL:
            SHR_CLI_EXIT_IF_ERR(sh_sand_enum_value_get(unit, sand_bool_table, source, &target->val_bool),
                                "Boolean string:%s is not supported\n", source);
            break;
        case SAL_FIELD_TYPE_INT32:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source, &target->val_int32));
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                target->val_int32 = sal_strtol(source, &end_ptr, 0);
                if (end_ptr[0] != 0)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
            }
            break;
        case SAL_FIELD_TYPE_UINT32:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source,
                                                       (int *) &target->val_uint32));
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                target->val_uint32 = sal_strtoul(source, &end_ptr, 0);
                if (end_ptr[0] != 0)
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                }
            }
            break;
        case SAL_FIELD_TYPE_IP4:
            if (parse_ipaddr(source, &target->ip4_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_IP6:
            if (parse_ip6addr(source, target->ip6_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_STR:
            sal_strncpy(target->val_str, source, SH_SAND_MAX_TOKEN_SIZE - 1);
            break;
        case SAL_FIELD_TYPE_MAC:
            if (parse_macaddr(source, target->mac_addr) != _SHR_E_NONE)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_ARRAY32:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source,
                                                       (int *) target->array_uint32));

            source_buf = sal_alloc(SH_SAND_MAX_TOKEN_SIZE, "source buffer");
            sal_memset(source_buf, 0, SH_SAND_MAX_TOKEN_SIZE);
            sal_strncpy(source_buf, source, SH_SAND_MAX_TOKEN_SIZE - 1);

            sh_sand_prepare_the_arr_string(unit, source_buf);
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                parse_long_integer(target->array_uint32, SH_SAND_MAX_ARRAY32_SIZE, source_buf);
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            }
            break;
        case SAL_FIELD_TYPE_BITMAP:
            break;
        case SAL_FIELD_TYPE_ENUM:
            SHR_SET_CURRENT_ERR(sh_sand_enum_value_get(unit, (sh_sand_enum_t *) ext_ptr, source, &target->val_enum));
            if (SHR_FUNC_VAL_IS(_SHR_E_PARAM))
            {
                SHR_CLI_EXIT(_SHR_E_INTERNAL, "Bad Enum option for:%s\n", source);
            }
            else if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
            {
                LOG_CLI((BSL_META("Enum:%s is not supported\n"), source));
                sh_sand_enum_value_show(unit, (sh_sand_enum_t *) ext_ptr);
                SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
            }
            break;
        case SAL_FIELD_TYPE_PORT:
            SHR_CLI_EXIT_IF_ERR(sh_sand_port_bitmap_get(unit, source, &target->ports_bitmap), "");
            break;
        case SAL_FIELD_TYPE_NONE:
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Unsupported parameter type:%d\n", type);
            break;
    }

exit:
    SHR_FREE(source_buf);
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_option_value_get(
    int unit,
    sal_field_type_e type,
    int *state_p,
    char *source,
    sh_sand_param_u * target1,
    sh_sand_param_u * target2,
    void *ext_ptr)
{
    char **tokens = NULL;
    uint32 realtokens = 0;
    char *value_1_p = NULL, *value_2_p = NULL;
    int state = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (state_p != NULL)
    {
        state = *state_p;
    }

    if (ISEMPTY(source))
    {
        if (type == SAL_FIELD_TYPE_BOOL)
        {       /* When boolean option appears on command line without value - meaning is TRUE */
            target1->val_bool = TRUE;
        }
        else if (type == SAL_FIELD_TYPE_STR)
        {
            SET_EMPTY(target1->val_str);
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Value of type:\"%s\" cannot be empty\n", sal_field_type_str(type));
        }
    }
    else
    {
        if ((type != SAL_FIELD_TYPE_STR) && (type != SAL_FIELD_TYPE_PORT) && (type != SAL_FIELD_TYPE_ENUM) &&
            (type != SAL_FIELD_TYPE_BOOL))
        {       /* Check mask presence */
            if ((tokens = utilex_str_split(source, ",", 2, &realtokens)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", source);
            }
            else if (realtokens == 1)
            {   /* free previous result tokens */
                utilex_str_split_free(tokens, realtokens);
                /*
                 * Now check range presence
                 */
                if ((tokens = utilex_str_split(source, "-", 2, &realtokens)) == NULL)
                {
                    SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", source);
                }
                else if (realtokens == 2)
                {
                    if (ISEMPTY(tokens[1]))
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Range end not provided\n");
                    }

                    if (ISEMPTY(tokens[0]))
                    {   /* Assuming single negative value, not range */
                        sal_free(tokens[1]);
                        realtokens = 1;
                    }
                    else
                    {
                        state |= SH_SAND_ARG_RANGE;
                    }
                }
            }
            else
            {   /* Now we know it is mask */
                if (ISEMPTY(tokens[0]) || ISEMPTY(tokens[1]))
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Comma ',' requires 2 values:data and mask\n");
                }
                state |= SH_SAND_ARG_MASKED;
                sal_memset(target2, 0xFF, sizeof(sh_sand_param_u));
                /*
                 * NO need to do anything more with mask value
                 */
                value_2_p = NULL;
            }

            if (realtokens == 2)
            {
                /*
                 * We had value string containing mask, so we assign both value pointers to be extracted below
                 */
                value_1_p = tokens[0];
                value_2_p = tokens[1];
            }
            else
            {
                value_1_p = source;
            }
        }
        else
        {
            value_1_p = source;
            value_2_p = NULL;
        }
        /*
         * First value will be always present and not empty
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_value_get(unit, type, value_1_p, target1, ext_ptr), "");
        /*
         * Second value is present only in special cases, like mask and range
         */
        if (value_2_p != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_value_get(unit, type, value_2_p, target2, ext_ptr), "");
        }
        /*
         * For range check that start is less or equal to end
         */
        if (state & SH_SAND_ARG_RANGE)
        {
            if (((type == SAL_FIELD_TYPE_INT32) && (target1->val_int32 > target2->val_int32)) ||
                ((type == SAL_FIELD_TYPE_UINT32) && (target1->val_uint32 > target2->val_uint32)))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "In range:%s start is larger than end\n", source);
            }
        }
    }
    /*
     * Return updated state to the user
     */
    if (state_p != NULL)
    {
        *state_p = state;
    }

exit:
    if (tokens != NULL)
    {   /* Means we left the loop on error */
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_option_valid_range_get(
    int unit,
    sh_sand_option_t * option_p,
    char *start_str,
    char *end_str)
{
    sh_sand_param_u start, end;

    SHR_FUNC_INIT_VARS(unit);

    SHR_SET_CURRENT_ERR(sh_sand_option_value_get(unit, option_p->type, NULL, option_p->valid_range, &start, &end,
                                                 option_p->ext_ptr));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        LOG_CLI((BSL_META("Error fetching valid range from:%s\n"), option_p->keyword));
    }
    else
    {
        switch (option_p->type)
        {
            case SAL_FIELD_TYPE_INT32:
                sal_snprintf(start_str, RHNAME_MAX_SIZE - 1, "%d", start.val_int32);
                sal_snprintf(end_str, RHNAME_MAX_SIZE - 1, "%d", end.val_int32);
                break;
            case SAL_FIELD_TYPE_UINT32:
                sal_snprintf(start_str, RHNAME_MAX_SIZE - 1, "%u", start.val_uint32);
                sal_snprintf(end_str, RHNAME_MAX_SIZE - 1, "%u", end.val_uint32);
                break;
            default:
                /*
                 * Type not supported - leave
                 */
                break;
        }
    }

    SHR_FUNC_EXIT;
}

static sh_sand_arg_t *
sh_sand_option_check_start(
    int unit,
    rhlist_t * args_list,
    char *option_name)
{
    sh_sand_arg_t *sh_sand_arg;

    RHITERATOR(sh_sand_arg, args_list)
    {
        if (RHNAME(sh_sand_arg) == sal_strcasestr(RHNAME(sh_sand_arg), option_name))
        {
            break;
        }
    }

    return sh_sand_arg;
}

static sh_sand_arg_t *
sh_sand_option_get(
    int unit,
    rhlist_t * args_list,
    char *option_name,
    sh_sand_option_cb_t option_cb,
    rhlist_t * dyn_args_list,
    int start_check)
{
    sh_sand_arg_t *sh_sand_arg;

    /*
     * First look into the predefined list
     */
    RHITERATOR(sh_sand_arg, args_list)
    {
        if (!sal_strcasecmp(option_name, RHNAME(sh_sand_arg))
            || ((sh_sand_arg->short_key != NULL) && (!sal_strcasecmp(option_name, sh_sand_arg->short_key))))
        {
            break;
        }
    }
    /*
     * Not found in static list - see if option callback will recognize this option and add it to dynamic one
     */
    if ((sh_sand_arg == NULL) && (option_cb != NULL))
    {
        sal_field_type_e type = SAL_FIELD_TYPE_NONE;
        uint32 id = 0;
        void *ext_ptr = NULL;
        rhhandle_t void_arg;

        if (option_cb(unit, option_name, &type, &id, &ext_ptr) == _SHR_E_NONE)
        {
            if (utilex_rhlist_entry_add_tail(dyn_args_list, option_name, id, &void_arg) != _SHR_E_NONE)
            {
                LOG_CLI((BSL_META("ERROR - Cannot add option:%s to dynamic list"), option_name));
                return NULL;
            }
            sh_sand_arg = void_arg;
            sh_sand_arg->type = type;
            sh_sand_arg->ext_ptr = ext_ptr;
            sh_sand_arg->state = SH_SAND_ARG_DYNAMIC;
        }
    }
    /*
     * Finally check if keyword starts from the string provided by option_name
     */
    if ((sh_sand_arg == NULL) && (start_check == TRUE))
    {
        sh_sand_arg = sh_sand_option_check_start(unit, args_list, option_name);
    }

    return sh_sand_arg;
}

static void
sh_sand_option_value_validate_init(
    int unit,
    sh_sand_arg_t * sh_sand_arg)
{
    /*
     * Called from trusted location, so no need to check sh_sand_arg
     */
    /*
     * Validation range is good only for INT32 or UINT32, additional types may be considered per request,
     * so no validation for them - Success returned
     */
    switch (sh_sand_arg->type)
    {
        case SAL_FIELD_TYPE_INT32:
            /*
             * Init low to lowest integer, high to highest
             */
            sh_sand_arg->low.val_int32 = (-2147483647 - 1);
            sh_sand_arg->high.val_int32 = 2147483647;
            break;
        case SAL_FIELD_TYPE_UINT32:
            sh_sand_arg->low.val_uint32 = 0;
            sh_sand_arg->high.val_uint32 = 0xFFFFFFFF;
            break;
        default:
            /*
             * Type not supported - leave
             */
            break;
    }
}

static shr_error_e
sh_sand_option_value_validate(
    int unit,
    sh_sand_arg_t * sh_sand_arg)
{
    char *option_name;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sh_sand_arg, _SHR_E_PARAM, "sh_sand_arg");

    if (sh_sand_arg->state & SH_SAND_ARG_DYNAMIC)
    {
        /*
         * No validation for dynamic options, leave peacefully
         */
        SHR_EXIT();
    }

    option_name = RHNAME(sh_sand_arg);
    /*
     * Validation range is good only for INT32 or UINT32, additional types may be considered per request,
     * so no validation for them - Success returned
     */
    switch (sh_sand_arg->type)
    {
        case SAL_FIELD_TYPE_INT32:
            if ((sh_sand_arg->param1.val_int32 < sh_sand_arg->low.val_int32) ||
                (sh_sand_arg->param1.val_int32 > sh_sand_arg->high.val_int32))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Value:\"%d\" for option:\"%s\" type:\"%s\" is out of range:%d-%d\n",
                             sh_sand_arg->param1.val_int32, option_name, sal_field_type_str(sh_sand_arg->type),
                             sh_sand_arg->low.val_int32, sh_sand_arg->high.val_int32);
            }
            /*
             * Validate second parameter only if it was range or mask
             */
            if (sh_sand_arg->state & (SH_SAND_ARG_RANGE | SH_SAND_ARG_MASKED))
            {
                if ((sh_sand_arg->param2.val_int32 < sh_sand_arg->low.val_int32) ||
                    (sh_sand_arg->param2.val_int32 > sh_sand_arg->high.val_int32))
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Value:\"%d\" for option:\"%s\" type:\"%s\" is out of range:%d-%d\n",
                                 sh_sand_arg->param2.val_int32, option_name, sal_field_type_str(sh_sand_arg->type),
                                 sh_sand_arg->low.val_int32, sh_sand_arg->high.val_int32);
                }
            }
            break;
        case SAL_FIELD_TYPE_UINT32:
            if ((sh_sand_arg->param1.val_uint32 < sh_sand_arg->low.val_uint32) ||
                (sh_sand_arg->param1.val_uint32 > sh_sand_arg->high.val_uint32))
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Value:\"%u\" for option:\"%s\" type:\"%s\" is out of range:%u-%u\n",
                             sh_sand_arg->param1.val_uint32, option_name, sal_field_type_str(sh_sand_arg->type),
                             sh_sand_arg->low.val_uint32, sh_sand_arg->high.val_uint32);
            }
            /*
             * Validate second parameter only if it was range or mask
             */
            if (sh_sand_arg->state & (SH_SAND_ARG_RANGE | SH_SAND_ARG_MASKED))
            {
                if ((sh_sand_arg->param2.val_uint32 < sh_sand_arg->low.val_uint32) ||
                    (sh_sand_arg->param2.val_uint32 > sh_sand_arg->high.val_uint32))
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Value:\"%u\" for option:\"%s\" type:\"%s\" is out of range:%u-%u\n",
                                 sh_sand_arg->param2.val_uint32, option_name, sal_field_type_str(sh_sand_arg->type),
                                 sh_sand_arg->low.val_uint32, sh_sand_arg->high.val_uint32);
                }
            }
            break;
        default:
            /*
             * Type not supported - leave
             */
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Analyze option and add the result to possible arguments list of specific command, represented by sand_args_list
 * \param [in] unit
 * \param [in] sand_args_list - allocated per leaf command and serving as possible arguments list
 * \param [in] option_p - pointer to option entry in either system or per command options array
 * \param [in] flags    - flags from command invocation
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval other errors as per shr_error_e *
 *
 * \remark
 */
static shr_error_e
sh_sand_option_init(
    int unit,
    rhlist_t * sand_args_list,
    sh_sand_option_t * option_p,
    int flags)
{
    rhhandle_t void_arg;
    sh_sand_arg_t *sh_sand_arg;
    char *option_n;

    SHR_FUNC_INIT_VARS(unit);

    if (ISEMPTY(option_p->full_key))
    {
        option_n = option_p->keyword;
    }
    else
    {
        option_n = option_p->full_key;
    }
    if (utilex_rhlist_entry_add_tail(sand_args_list, option_n, RHID_TO_BE_GENERATED, &void_arg) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_INIT, "Error - failed to add option:\"%s\" to the list\n", option_n);
    }
    sh_sand_arg = void_arg;
    sh_sand_arg->type = option_p->type;
    sh_sand_arg->ext_ptr = option_p->ext_ptr;
    sh_sand_arg->short_key = option_p->short_key;
    /** State and flags are sharing the same set of flags, so state is initialized to static flags */
    sh_sand_arg->state = option_p->flags;

    if (option_p->def != NULL)
    {
        if (sh_sand_option_value_get(unit, sh_sand_arg->type, &sh_sand_arg->def_state, option_p->def,
                                     &sh_sand_arg->def_param1, &sh_sand_arg->def_param2,
                                     sh_sand_arg->ext_ptr) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Default value:\"%s\" for option:\"%s\" type:\"%s\"\n",
                         option_p->def, RHNAME(sh_sand_arg), sal_field_type_str(sh_sand_arg->type));
        }
    }
    else
    {   /* 
         * Absence of default means that option presence is requested
         */
        sh_sand_arg->requested = TRUE;
    }
    /*
     * Initialize to theoretical min/max per type
     */
    sh_sand_option_value_validate_init(unit, sh_sand_arg);
    if (option_p->valid_range != NULL)
    {
        if ((sh_sand_arg->type == SAL_FIELD_TYPE_INT32) || (sh_sand_arg->type == SAL_FIELD_TYPE_UINT32))
        {       /* 
                 * Validation range is good only for INT32 or UINT32, additional types may be considered per request
                 */
            if (sh_sand_option_value_get(unit, sh_sand_arg->type, NULL, option_p->valid_range,
                                         (sh_sand_param_u *) & sh_sand_arg->low,
                                         (sh_sand_param_u *) & sh_sand_arg->high, sh_sand_arg->ext_ptr) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal Range:\"%s\" for option:\"%s\" type:\"%s\"\n",
                             option_p->valid_range, RHNAME(sh_sand_arg), sal_field_type_str(sh_sand_arg->type));
            }
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Range validation:\"%s\" is not supported for option:\"%s\" type:\"%s\"\n",
                         option_p->valid_range, RHNAME(sh_sand_arg), sal_field_type_str(sh_sand_arg->type));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See diag_sand_framework.h
 */
void
sh_sand_option_list_clean(
    int unit,
    sh_sand_control_t * sand_ctr)
{
    if (sand_ctr == NULL)
    {
        /*
         * Nothing to be freed
         */
        return;
    }
    if (sand_ctr->stat_args_list != NULL)
    {
        utilex_rhlist_clean(sand_ctr->stat_args_list);
        SHR_FREE(sand_ctr->stat_args_list);
    }
    if (sand_ctr->dyn_args_list != NULL)
    {
        utilex_rhlist_clean(sand_ctr->dyn_args_list);
        SHR_FREE(sand_ctr->dyn_args_list);
    }
}

static shr_error_e
sh_sand_option_list_add_internal(
    int unit,
    char *cur_arg,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_ctr,
    rhlist_t * args_list,
    sh_sand_arg_t * sh_sand_arg)
{

    char **tokens = NULL;
    uint32 realtokens = 0;
    char *option_value = NULL, *option_name = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd != NULL)
    {
        if ((sal_strlen(sand_ctr->options_str) + sal_strlen(cur_arg)) >= (SH_SAND_MAX_TOKEN_SIZE - 1))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Option string size is too long to include:\"%s\"\n", cur_arg);
        }
        utilex_str_append(sand_ctr->options_str, cur_arg);
    }

    /*
     * Analyze option
     */
    if ((tokens = utilex_str_split(cur_arg, "=", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\n");
    }
    /*
     * If there is no = in the token, we assume that it is free argument according the order of their
     * Appearance in argument list
     */
    option_name = tokens[0];
    switch (realtokens)
    {
        case 0:
            SHR_CLI_EXIT(_SHR_E_MEMORY, "No tokens in:%s\n", cur_arg);
            break;
        case 1:
            if ((sh_sand_arg = sh_sand_option_get(unit, args_list, option_name, sh_sand_cmd->option_cb,
                                                  sand_ctr->dyn_args_list, FALSE)) == NULL)
            {
                /** Check if we have arguments with free flag not occupied yet */
                RHITERATOR(sh_sand_arg, args_list)
                {
                    if ((sh_sand_arg->state & SH_SAND_ARG_FREE) && !(sh_sand_arg->state & SH_SAND_ARG_PRESENT))
                    {
                        option_value = tokens[0];
                        break;
                    }
                }
                /** Found - great - no - try the start of arguments */
                if (sh_sand_arg == NULL)
                {
                    if ((sh_sand_arg = sh_sand_option_check_start(unit, args_list, option_name)) == NULL)
                    {
                        SHR_CLI_EXIT(_SHR_E_PARAM, "Option:%s is not supported for:%s\n",
                                     tokens[0], sh_sand_cmd->cmd_only);
                    }
                }
            }
            else
            {   /* Find option, but only boolean are allowed to miss the value */
                if (sh_sand_arg->type != SAL_FIELD_TYPE_BOOL)
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "Option:\"%s\" requires a value\n", option_name);
                }
                else
                {       /* Boolean option with no explicit value */
                    option_value = NULL;
                }
            }
            break;
        case 2:        /* regular option with value */
            /*
             * Look for keyword in the args_list
             */
            if ((sh_sand_arg = sh_sand_option_get(unit, args_list, option_name, sh_sand_cmd->option_cb,
                                                  sand_ctr->dyn_args_list, TRUE)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Option:%s is not supported for:%s\n", option_name, sh_sand_cmd->cmd_only);
            }
            option_value = tokens[1];
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal option:%s\n", cur_arg);
            break;
    }

    if (sh_sand_arg->state & SH_SAND_ARG_PRESENT)
    {
        if (sh_sand_arg->state & SH_SAND_ARG_FREE)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Argument:\"%s\" appears twice on command line. Probably once as free one\n",
                         RHNAME(sh_sand_arg));
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Argument:\"%s\" appears twice on command line\n", RHNAME(sh_sand_arg));
        }
    }
    sh_sand_arg->state |= SH_SAND_ARG_PRESENT;

    if (sh_sand_option_value_get(unit, sh_sand_arg->type, &sh_sand_arg->state,
                                 option_value, &sh_sand_arg->param1, &sh_sand_arg->param2,
                                 sh_sand_arg->ext_ptr) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal value:\"%s\" for option:\"%s\" type:\"%s\"\n",
                     option_value, option_name, sal_field_type_str(sh_sand_arg->type));
    }
    /*
     * Validate the values
     */
    SHR_CLI_EXIT_IF_ERR(sh_sand_option_value_validate(unit, sh_sand_arg), "");

    utilex_str_split_free(tokens, realtokens);
    tokens = NULL;

exit:
    if (tokens != NULL)
    {   /* Means we left the function on error */
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

/**
 * See diag_sand_framework.h
 */
shr_error_e
sh_sand_option_list_process(
    int unit,
    int *core,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd,
    sh_sand_control_t * sand_ctr,
    int flags)
{
    sh_sand_arg_t *sh_sand_arg = NULL;
    char *cur_arg;
    int i;
    sh_sand_option_t *option_p;
    rhlist_t *args_list;

    uint8 _core_exist;
    sh_sand_arg_t *_sand_arg;

    SHR_FUNC_INIT_VARS(unit);

    if ((sand_ctr == NULL) || (sh_sand_cmd == NULL))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR - Null args_list or sh_sand_cmd\n");
    }

    if (flags & SH_CMD_LEGACY)
    {   /* No option processing for legacy commands */
        SHR_EXIT();
    }
    /*
     * If options == NULL, no command specific options provided, just system one will be relevant
     */
    if ((sand_ctr->stat_args_list = utilex_rhlist_create("ShellStatOptions", sizeof(sh_sand_arg_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for stat args list\n");
    }

    if ((sand_ctr->dyn_args_list = utilex_rhlist_create("ShellDynOptions", sizeof(sh_sand_arg_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for dyn args list\n");
    }

    args_list = sand_ctr->stat_args_list;
    for (option_p = sh_sand_cmd->options; (option_p != NULL) && (option_p->keyword != NULL); option_p++)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_option_init(unit, sand_ctr->stat_args_list, option_p, flags), "");
    }
    /*
     * Add all system options to the list
     */
    for (option_p = sh_sand_sys_arguments; option_p->keyword != NULL; option_p++)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_option_init(unit, sand_ctr->stat_args_list, option_p, flags), "");
    }
    /*
     * Initialize option string
     */
    SET_EMPTY(sand_ctr->options_str);
    /*
     * Take time stamp
     */
    sh_sand_time_get(sand_ctr->time_str);

    /*
     * If Command line options provided, all arguments stay on default
     */
    if (args != NULL)
    {

        /*
         * Check all tokens left - supposedly options
         */
        for (i = 0; i < ARG_CNT(args); i++)
        {
            if ((cur_arg = ARG_GET_WITH_INDEX(args, ARG_CUR_INDEX(args) + i)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "No option for:%d\n", ARG_CUR_INDEX(args) + i);
            }

            /*
             * Add the current option argument.
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_add_internal
                                (unit, cur_arg, sh_sand_cmd, sand_ctr, args_list, sh_sand_arg), "");
        }
    }

    if (_SHR_IS_FLAG_SET(flags, SH_CMD_EXEC_PER_CORE))
    {
        /*
         * Check if a core argument already exist.
         */
        _core_exist = FALSE;
        if ((_sand_arg = utilex_rhlist_entry_get_by_name(args_list, "CORE")) != NULL)
        {
            if (_sand_arg->state & SH_SAND_ARG_PRESENT)
            {
                _core_exist = TRUE;
            }
        }

        /*
         * In case SH_CMD_EXEC_PER_CORE flag is set add a core related argument(if it is not specified).
         */
        if (!_core_exist)
        {
            char buf[10];
            sal_memset(buf, 0, 10);
            sal_snprintf(buf, 10, "core=%d", *core);
            SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_add_internal
                                (unit, buf, sh_sand_cmd, sand_ctr, args_list, sh_sand_arg), "");
        }
        else
        {
            /*
             * Change the core variable to be equal to NOF_CORES to break the loop.
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_nof_cores_get(unit, core), "");
        }
    }

    if (args != NULL)
    {
        RHITERATOR(sh_sand_arg, args_list)
        {
            /*
             * If option was present - great
             */
            if (sh_sand_arg->state & SH_SAND_ARG_PRESENT)
            {
                continue;
            }
            /*
             * If not and its presence required - error
             */
            if (sh_sand_arg->requested == TRUE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Presence of option:\"%s\" is requested\n", RHNAME(sh_sand_arg));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_cmd_traverse(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_traverse_cb_t sh_sand_action,
    void *flex_ptr,
    int flags,
    char *label)
{
    sh_sand_cmd_t *sh_sand_cmd;
    uint8 label_match = TRUE;
    uint32 max_tokens = 100;
    uint32 tokens = 0;
    char **label_tokens = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (sh_sand_cmd_a == NULL)
    {
         /* coverity[copy_paste_error : FALSE]  */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: Command list is NULL in traverse\n");
    }
    if (label != NULL)
    {
        label_tokens = utilex_str_split(label, ",", max_tokens, &tokens);
    }

    SH_SAND_CMD_ITERATOR_COND(sh_sand_cmd, sh_sand_cmd_a, flags)
    {
        if (label != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sys_manual_label_match(unit, sh_sand_cmd->label, label_tokens, tokens,
                                                       &label_match), "Error in regex label match.");
        }
        if (label_match)
        {
            /*
             * Use cur_error to keep ERROR if it failed
             */
            shr_error_e cur_error;
            if ((sh_sand_cmd->action != NULL) || (flags & SH_CMD_TRAVERSE_ALL))
            {
                if ((cur_error = sh_sand_action(unit, args, sh_sand_cmd, flex_ptr, flags)) != _SHR_E_NONE)
                {
                    SHR_SET_CURRENT_ERR(cur_error);
                }
            }

            if (sh_sand_cmd->child_cmd_a != NULL)
            {
                if ((cur_error = sh_sand_cmd_traverse(unit, args, sh_sand_cmd->child_cmd_a, sh_sand_action, flex_ptr,
                                                      flags, label)) != _SHR_E_NONE)
                {
                    SHR_SET_CURRENT_ERR(cur_error);
                }
            }
        }
    }

exit:
    sal_free(label_tokens);
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_cmd_get(
    int unit,
    char *cmd_name,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t ** sh_sand_cmd_p,
    int prefix_lookup)
{
    sh_sand_cmd_t *sh_sand_cmd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd_p, _SHR_E_PARAM, "sh_sand_cmd_p");

    if (ISEMPTY(cmd_name))
    {
        cmd_name = "usage";
    }
    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: Command list is NULL for:%s\n", cmd_name);
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
    {
        /*
         * Call to the first command that is supported and match the cmd_name
         */
        if (!sal_strcasecmp(cmd_name, sh_sand_cmd->keyword) ||
            (!ISEMPTY(sh_sand_cmd->short_key) && (!sal_strcasecmp(cmd_name, sh_sand_cmd->short_key))))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            break;
        }
    }
    /*
     * Now try command prefix substring
     */
    if ((prefix_lookup != 0) && (sh_sand_cmd->keyword == NULL) && (sal_strlen(cmd_name) > 1))
    {
        SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
        {
            if (sh_sand_cmd->keyword == sal_strcasestr(sh_sand_cmd->keyword, cmd_name))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                break;
            }
        }
    }

    if ((sh_sand_cmd != NULL) && SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        /*
         * Command found, now verify conditions, no conditions or returned success - all is good continue as usual
         */
        if ((sh_sand_cmd->flags & SH_CMD_CONDITIONAL) && (sh_sand_cmd->invoke_cb != NULL) &&
            (sh_sand_cmd->invoke_cb(unit, NULL) != _SHR_E_NONE))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
        }
        *sh_sand_cmd_p = sh_sand_cmd;
    }
    else
    {
        *sh_sand_cmd_p = NULL;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_legacy_fetch(
    int unit,
    args_t * args,
    char *cmd_name,
    rhlist_t * legacy_list)
{
    sh_sand_legacy_entry_t *legacy_entry;

    SHR_FUNC_INIT_VARS(unit);

    if (ISEMPTY(cmd_name))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    RHITERATOR(legacy_entry, legacy_list)
    {
        if (!sal_strcasecmp(cmd_name, RHNAME(legacy_entry)) || !sal_strcasecmp(cmd_name, legacy_entry->short_key))
            break;
    }

    if (legacy_entry == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if ((legacy_entry->next_tokens_list) && ((cmd_name = ARG_GET(args)) != NULL))
    {
        SHR_SET_CURRENT_ERR(sh_sand_legacy_fetch(unit, args, cmd_name, legacy_entry->next_tokens_list));
        if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
        {
            /*
             * If token was not found - take it back to allow regular processing
             */
            ARG_PREV(args);
        }
    }
    else
    {
        if (!ISEMPTY(legacy_entry->substitute))
        {
            /*
             * 1. Find pointer to the token after the last command, if any.
             * 2. Check that command is not the last token in entire string
             * 3. Attached the rest of command to substitute
             * 4. Arguments should be ones supported by the substitute
             */
            char command[RHSTRING_MAX_SIZE];
            int i_a = args->a_arg;
            int max_append;

            sal_strncpy(command, legacy_entry->substitute, RHSTRING_MAX_SIZE - 1);
            for (i_a = args->a_arg; i_a < args->a_argc; i_a++)
            {
                max_append = RHSTRING_MAX_SIZE - 1 - sal_strlen(command);
                sal_strncat(command, " ", max_append);
                sal_strncat(command, args->a_argv[i_a], max_append - 1);
            }

            SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, command), "Substitute command:'%s' execution failed\n", command);
        }
        else if (!ISEMPTY(legacy_entry->message))
        {
            LOG_CLI((BSL_META("%s\n"), legacy_entry->message));
        }
        else
        {
            LOG_CLI((BSL_META("Partial Legacy Command:%s\n"), args->a_cmd));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_act(
    int unit,
    args_t * args,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a,
    rhlist_t * legacy_list)
{
    char *cmd_name;
    sh_sand_cmd_t *sh_sand_cmd = NULL;
    sh_sand_control_t sand_ctr_m;
    int nof_cores;
    int current_core;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sand_ctr_m, 0, sizeof(sh_sand_control_t));

    if (sh_sand_cmd_a == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: No further command provided\n");
    }

    /*
     * Get command name
     */
    cmd_name = ARG_GET(args);

    if (legacy_list != NULL)
    {
        /*
         * Check in legacy list if exists
         */
        SHR_SET_CURRENT_ERR(sh_sand_legacy_fetch(unit, args, cmd_name, legacy_list));
        if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
        {
            SHR_EXIT();
        }
    }
    /*
     * Now we can move after command word
     */
    SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sys_cmd_a, &sh_sand_cmd, 1));
    if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {   /** If not found in system list look in local one - not found - error   */
        SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd, 1));
        if (SHR_FUNC_VAL_IS(_SHR_E_UNAVAIL) && (sh_sand_cmd != NULL) && (!ISEMPTY(sh_sand_cmd->man->compatibility)))
        {
            /*
             * Command was found but is unavailable and compatibility statement is present, print it and preserve
             * UNAVAIL error
             */
            SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Command:\"%s\" - %s\n", sh_sand_cmd->cmd_only,
                         sh_sand_cmd->man->compatibility);
        }
        else if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "Command:\"%s\" is not supported for current device and/or configuration\n",
                         cmd_name);
        }
    }
    else
    {   /* 
         * System command found - fill pointer to upstream array
         */
        sand_ctr_m.sh_sand_cmd_a = sh_sand_cmd_a;
        sand_ctr_m.sh_sys_cmd_a = sh_sys_cmd_a;
        sand_ctr_m.sh_sand_cmd = NULL;
    }
    if (sh_sand_cmd->child_cmd_a != NULL)
    {   /* 
         * No nesting on system commands, so command is regular one, going down the tree
         */
        SHR_SET_CURRENT_ERR(sh_sand_act(unit, args, sh_sand_cmd->child_cmd_a, sh_sys_cmd_a, sh_sand_cmd->legacy_list));
    }
    else if (sh_sand_cmd->action)
    {
        sh_sand_control_t *sand_control = &sand_ctr_m;
        int timer;
        sal_usecs_t usec;

        /*
         * For regular command record it inside control structure, which may be overwritten for system commands
         */
        sand_ctr_m.sh_sand_cmd = sh_sand_cmd;
        
        if ((cmd_name = ARG_CUR(args)) != NULL)
        {
            sh_sand_cmd_t *sh_sand_cmd_sys = NULL;

            SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sys_cmd_a, &sh_sand_cmd_sys, 1));
            if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
            {
                ARG_NEXT(args);
                sand_ctr_m.sh_sand_cmd_a = NULL;
                sand_ctr_m.sh_sys_cmd_a = sh_sys_cmd_a;
                /*
                 * Record base command
                 */
                sand_ctr_m.sh_sand_cmd = sh_sand_cmd;
                /*
                 * Move to system one to preserve the general flow
                 */
                sh_sand_cmd = sh_sand_cmd_sys;
            }
        }
        /*
         * Set default nof_cores that will be used as exec count.
         */
        nof_cores = 1;
        if (_SHR_IS_FLAG_SET(sh_sand_cmd->flags, SH_CMD_EXEC_PER_CORE))
        {
            /*
             * if SH_CMD_EXEC_PER_CORE flag is set update the nof_cores.
             */
            SHR_CLI_EXIT_IF_ERR(sh_sand_nof_cores_get(unit, &nof_cores), "");
        }
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        for (current_core = 0; current_core < nof_cores; current_core++)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_option_list_process
                                (unit, &current_core, args, sh_sand_cmd, sand_control, sh_sand_cmd->flags), "");
            SH_SAND_GET_BOOL("time", timer);
            if (timer == TRUE)
            {
                usec = sal_time_usecs();
                SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->action(unit, args, sand_control),
                                    "%s. Duration - %u usec\n", shrextend_errmsg_get(_rv), sal_time_usecs() - usec);
                LOG_CLI((BSL_META("Success. Duration - %u usec\n"), sal_time_usecs() - usec));
            }
            else
            {
                SHR_CLI_EXIT_IF_ERR(sh_sand_cmd->action(unit, args, sand_control), "");
            }
        }
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR: command:%s had neither leaf nor branch\n", cmd_name);
    }

exit:
    /*
     * Clean allocations from sand_ctr
     */
    sh_sand_option_list_clean(unit, &sand_ctr_m);
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_verify_man(
    sh_sand_man_t * man)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    if ((man == NULL) || (man->brief == NULL))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief Routine serves to verify correctness of shell command tree and init requested resources
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd_parent - parent command to sh_sand_cmd_a array
 *     \param [in] sh_sand_cmd_a pointer to command list to start from
 *     \param [in] sh_sys_cmd_a pointer to array of system commands assigned per root command
 *     \param [in] command accumulated from shell tree traversing, usually starts from NULL
 *     \param [in] flags verification flags for shell command initialization
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
shr_error_e
sh_sand_init(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_parent,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_cmd_t * sh_sys_cmd_a,
    char *command,
    int flags)
{
    sh_sand_cmd_t *sh_sand_cmd, *sh_sys_cmd;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over command array, verify and fill shortcuts for all commands, even ones with conditions
     */
    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
    {   /* Verify that every command is either leaf or branch */
        /*
         * Keep previous command length
         */
        int cmd_size = strlen(command);
        /*
         * Save parent command pointer inside child
         */
        sh_sand_cmd->parent_cmd = sh_sand_cmd_parent;
        /*
         * Obtain shortcut string
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(unit, command, sh_sand_cmd->keyword, &sh_sand_cmd->short_key,
                                                  &sh_sand_cmd->full_key, flags), "Failure to verify command:\"%s\"\n",
                            command);
        SH_SAND_CMD_ITERATOR_ALL(sh_sys_cmd, sh_sys_cmd_a)
        {
            if (!sal_strcasecmp(sh_sand_cmd->keyword, sh_sys_cmd->keyword))
            {
                SHR_CLI_EXIT(_SHR_E_INIT, "Error - '%s' includes command:'%s' reserved for system usage\n",
                             command, sh_sand_cmd->keyword);
            }
        }
        /*
         * Include present keyword into command
         */
        if (cmd_size == 0)
            sal_strncpy(command, sh_sand_cmd->full_key, SH_SAND_MAX_TOKEN_SIZE - 1);
        else
            sal_snprintf(command + cmd_size, SH_SAND_MAX_TOKEN_SIZE - cmd_size - 1, " %s", sh_sand_cmd->full_key);

        /*
         * Include parent label, if any.
         */
        if (sh_sand_cmd->parent_cmd != NULL && strlen(sh_sand_cmd->parent_cmd->label) != 0)
        {
            sal_strncpy(sh_sand_cmd->label, sh_sand_cmd->parent_cmd->label, SH_SAND_MAX_TOKEN_SIZE - 1);
        }

        /*
         * Add SH_CMD_NO_XML_VERIFY flag if it is set for its parent.
         */
        if (sh_sand_cmd->parent_cmd != NULL && _SHR_IS_FLAG_SET(sh_sand_cmd->parent_cmd->flags, SH_CMD_NO_XML_VERIFY))
        {
            sh_sand_cmd->flags |= SH_CMD_NO_XML_VERIFY;
        }

        if (sh_sand_cmd->action)
        {       /* Command is leaf, verify that we have correct usage and options */
            sh_sand_option_t *option_p, *sys_option_p;
            if (sh_sand_cmd->child_cmd_a)
            {   /* Command is branch and leaf - currently not supported */
                LOG_CLI((BSL_META("ERROR - command:%s is both leaf and branch\n"), command));
                SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            }

            if (flags & SH_CMD_VERIFY)
            {
                SHR_CLI_EXIT_IF_ERR(sh_sand_verify_man(sh_sand_cmd->man),
                                    "Bad \"usage\" for command:\"%s\"\n", command);
            }
            for (option_p = sh_sand_cmd->options; (option_p != NULL) && (option_p->keyword != NULL); option_p++)
            {
                sh_sand_option_t *cur_option_p;

                SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(unit, command, option_p->keyword, &option_p->short_key,
                                                          &option_p->full_key, flags),
                                    "Failure to verify argument:\"%s\" on command:\"%s\"\n", option_p->keyword,
                                    command);
                /*
                 * Check there is no same option twice in the list
                 */
                for (cur_option_p = (option_p + 1); (cur_option_p != NULL) && !ISEMPTY(cur_option_p->keyword);
                     cur_option_p++)
                {
                    if (!sal_strcasecmp(option_p->keyword, cur_option_p->keyword))
                    {
                        SHR_CLI_EXIT(_SHR_E_INIT, "Error - in command:'%s' argument:'%s' appears more than once\n",
                                     command, option_p->keyword);
                    }
                }
                /*
                 * Verify that options are not the same as system ones
                 */
                for (sys_option_p = sh_sand_sys_arguments; sys_option_p->keyword != NULL; sys_option_p++)
                {
                    if (!sal_strcasecmp(option_p->keyword, sys_option_p->keyword))
                    {
                        /*
                         * Only case when this situation is allowed - is to overwrite system default and/or valid range
                         * Type should be the same - description and ext_ptr will be overwritten, so may be left NULL
                         */
                        if (sys_option_p->type == option_p->type)
                        {
                            /** In this case no further verification is required  */
                            option_p->desc = sys_option_p->desc;
                            option_p->ext_ptr = sys_option_p->ext_ptr;
                            /*
                             * Update valid range only if it is NULL
                             */
                            if (option_p->valid_range == NULL)
                            {
                                option_p->valid_range = sys_option_p->valid_range;
                            }
                            continue;
                        }
                        else
                        {
                            SHR_CLI_EXIT(_SHR_E_INIT, "Error - in command:'%s' argument:'%s' is reserved for system."
                                         " To overwrite 'system default' argument must have the same type as system one\n",
                                         command, option_p->keyword);
                        }
                    }
                }
                if (ISEMPTY(option_p->desc))
                {
                    SHR_CLI_EXIT(_SHR_E_INIT, "Error - option:\"%s\" has no description\n", option_p->keyword);
                }
                if ((option_p->type == SAL_FIELD_TYPE_ENUM) && (option_p->ext_ptr == NULL))
                {
                    SHR_CLI_EXIT(_SHR_E_INIT, "Error - enum option:\"%s\" requests enum list to be provided\n",
                                 option_p->keyword);
                }
            }
        }
        else if (sh_sand_cmd->child_cmd_a)
        {       /* Command is branch got into recursive call */
             /* coverity[copy_paste_error : FALSE]  */
            SHR_CLI_EXIT_IF_ERR(sh_sand_init(unit, sh_sand_cmd, sh_sand_cmd->child_cmd_a, sh_sys_cmd_a, command, flags),
                                "");
        }
        else
        {
            LOG_CLI((BSL_META("ERROR - command:%s is neither leaf nor branch\n"), command));
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        }
        /*
         * Fill command list from command string
         */
        if (sal_strlen(command) >= SH_SAND_MAX_TOKEN_SIZE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Command string:%s is longer than MAX:%d\n", command,
                         (int) sal_strlen(command));
        }
        sal_strncpy(sh_sand_cmd->cmd_only, command, SH_SAND_MAX_TOKEN_SIZE - 1);
        /*
         * Return previous command - cut off current keyword
         */
        command[cmd_size] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Routine serves to free resources for entire tree
 * \par DIRECT INPUT:
 *     \param [in] unit unit id
 *     \param [in] sh_sand_cmd_a pointer to command list to start from
 * \par DIRECT OUTPUT:
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 * \remark
 *     Place holder - Currently do nothing, but may be functionality will be assigned
 */
static shr_error_e
sh_sand_deinit(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over command array, even with conditions - no harm - no actual actions
     */
    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
    {
        if (sh_sand_cmd->child_cmd_a)
        {       /* Command is branch got into recursive call */
            SHR_CLI_EXIT_IF_ERR(sh_sand_deinit(unit, sh_sand_cmd->child_cmd_a), "Deinit Failure");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Routine serves to invoke root level command
 * \param [in] unit             - unit id
 * \param [in] args             - dnx shell arguments structure
 * \return
 *     \retval _SHR_E_NONE for success
 *     \retval _SHR_E_PARAM problem with input parameters, usage should be printed by calling procedure
 *     \retval other errors for other failure type
 */
static shr_error_e
sh_sand_root_act(
    int unit,
    args_t * args)
{
    char *cmd_name;
    sh_sand_cmd_t *sh_sand_cmd = NULL;
    sh_sand_cmd_t *sh_sand_cmd_a = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get root command for device
     */
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_get(unit, &sh_sand_cmd_a), "");
    /*
     * Get command name
     */
    cmd_name = ARG_CUR(args);
    /*
     * First check on root level
     */
    SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, cmd_name, sh_sand_cmd_a, &sh_sand_cmd, 1));
    /*
     * If not found switch to underlying command with SKIP_ROOT and let sh_sand_act to handle all problems
     */
    if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {
        SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
        {
            /*
             * Switch to first seen command with SKIP_ROOT - use ALL Iterator because conditions are not relevant here
             */
            if (sh_sand_cmd->flags & SH_CMD_SKIP_ROOT)
                break;
        }
    }
    else
    {
        /*
         * If command found on highest level take next argument - otherwise we stay with previous
         */
        ARG_NEXT(args);
    }

    SHR_SET_CURRENT_ERR(sh_sand_act(unit, args, sh_sand_cmd->child_cmd_a, (sh_sand_cmd_t *) sh_sand_cmd->invokes,
                                    sh_sand_cmd->legacy_list));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Routine allowing to treat all new commands in unified way.
 * All commands may be invoked through dnx - e.g. "dnx access list..."
 */

static cmd_result_t
cmd_all_invoke(
    int unit,
    args_t * args)
{
    cmd_result_t result;

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#ifdef BCM_WARM_BOOT_API_TEST
    /*
     * disabling wb regression so we don't perform reboot in the middle of a diagnostic doing reboot in middle of
     * diagnostic may lead to loss of state that is required for the diagnostic to succeed
     */
    int wb_test_mode = 0;
    dnxc_wb_test_mode_get(unit, &wb_test_mode);
    if (wb_test_mode)
    {
        dnxc_wb_no_wb_test_set(unit, 1);
    }
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

    /*
     * Move to previous token which is dnx in this case
     */
    ARG_PREV(args);
    result = diag_sand_error_get(sh_sand_root_act(unit, args));
    ARG_DISCARD(args);

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#ifdef BCM_WARM_BOOT_API_TEST
    if (wb_test_mode)
    {
        dnxc_wb_no_wb_test_set(unit, 0);
    }
#endif /* BCM_WARM_BOOT_API_TEST */
#endif /* defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) */

    return result;
}

shr_error_e
sh_sand_execute(
    int unit,
    char *command)
{
    args_t *exec_args = NULL;
    char *str_next;

    SHR_FUNC_INIT_VARS(unit);

    if (ISEMPTY(command))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Shell command to be executed cannot be empty\n");
    }

    if ((exec_args = sal_alloc(sizeof(args_t), "leafs")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }

    if (diag_parse_args(command, &str_next, exec_args))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_root_act(unit, exec_args), "");

exit:
    if (exec_args != NULL)
    {
        sal_free(exec_args);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_root_init(
    int unit,
    sh_sand_cmd_t * sh_root_commands,
    sh_sand_legacy_cmd_t * sh_sand_legacy_cmd_a)
{
    sh_sand_option_t *option_p;
    char command[SH_SAND_MAX_TOKEN_SIZE];
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_cmd_t *sh_sys_cmd_a;
    sh_sand_cmd_t *sh_device_commands;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_root_commands, _SHR_E_PARAM, "sh_sand_cmd_a");

    SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_list_init(unit), "");

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_set(unit, sh_root_commands), "");

    for (option_p = sh_sand_sys_arguments; option_p->keyword != NULL; option_p++)
    {
        SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_fetch(unit, "System", option_p->keyword, &option_p->short_key,
                                                  &option_p->full_key, SH_CMD_VERIFY), "");
        if (ISEMPTY(option_p->desc))
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - System option:\"%s\" has no description\n", option_p->keyword);
        }
        if ((option_p->type == SAL_FIELD_TYPE_ENUM) && (option_p->ext_ptr == NULL))
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Error - System enum option:\"%s\" requests enum list to be provided\n",
                         option_p->keyword);
        }
    }

#ifdef INCLUDE_AUTOCOMPLETE
    sh_sand_cmd_autocomplete_init(unit, NULL, sh_root_commands);
#endif /* INCLUDE_AUTOCOMPLETE */

    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_root_commands)
    {
        if (sh_sand_cmd->child_cmd_a == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Root Level for:\"%s\" cannot be leaf, only node\n", sh_sand_cmd->keyword);
        }
        if (sh_sand_cmd->flags & SH_CMD_SKIP_ROOT)
        {
            command[0] = 0;
        }
        else
        {
            sal_strncpy(command, sh_sand_cmd->keyword, SH_SAND_MAX_TOKEN_SIZE - 1);
        }
        sh_sys_cmd_a = (sh_sand_cmd_t *) sh_sand_cmd->invokes;
        SHR_CLI_EXIT_IF_ERR(sh_sand_init
                            (unit, sh_sand_cmd, sh_sand_cmd->child_cmd_a, sh_sys_cmd_a, command, sh_sand_cmd->flags),
                            "%s shell commands verification failed\n", sh_sand_cmd->keyword);
        /*
         * invokes is used because in root there may not be invokes and we need some field were we can transfer system
         */
        if (sh_sys_cmd_a != NULL)
        {
            command[0] = 0;
            SHR_CLI_EXIT_IF_ERR(sh_sand_init(unit, sh_sand_cmd, sh_sys_cmd_a, NULL, command, SH_CMD_VERIFY),
                                "DNX shell system command verification failed\n");
        }
#ifdef INCLUDE_AUTOCOMPLETE
        if (sh_sand_cmd->flags & SH_CMD_SKIP_ROOT)
        {
            sh_sand_cmd_autocomplete_init(unit, NULL, sh_sand_cmd->child_cmd_a);
        }
#endif /* INCLUDE_AUTOCOMPLETE */
    }

#ifdef INCLUDE_AUTOCOMPLETE

    SHR_IF_ERR_EXIT(sh_sand_access_read_autocomplete_init(unit));

#if defined(DNX_SW_STATE_LOGGING)

    sh_sand_sw_state_cmd_autocomplete_init(unit)
#endif /* DNX_SW_STATE_LOGGING */
#endif /* INCLUDE_AUTOCOMPLETE */
        if (ISEMPTY(sh_root_commands->keyword) || ((sh_device_commands = sh_root_commands->child_cmd_a) == NULL))
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Bad device command tree for unit:%d\n", unit);
    }
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_add_to_dyn_list(unit, sh_device_commands, cmd_all_invoke), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_add_to_dyn_list(unit, sh_root_commands, cmd_all_invoke), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_init(unit, sh_root_commands->keyword, sh_root_commands,
                                            sh_sand_legacy_cmd_a, cmd_all_invoke), "");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Routine creates dynamic legacy command list after we found split point from static tree.
 *      Highest point to such tree is when first token of legacy command is unknown and dynamic starts from it
 *      In this case list is build on device native command list(dnx, dnxf, ...)
 * \param [in] unit
 * \param [in] legacy_cmd - command, used here to pass message pointer, that should be attached to the leaf
 * \param [in] list_p - pointer to token list on parent object
 * \param [in] tokens - legacy command string tokens
 * \param [in] nof_tokens - number of tokens in command string
 * \param [in] cur_token - token index being handled on this step
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval other errors as per shr_error_e *
 *
 * \remark
 *      Routine is recursive and is keeping recursion until the last token in the legacy command string
 * \see
 *      sh_sand_legacy_attach
 */
shr_error_e
sh_sand_legacy_build(
    int unit,
    sh_sand_legacy_cmd_t * legacy_cmd,
    rhlist_t ** list_p,
    char **tokens,
    int nof_tokens,
    int cur_token)
{
    rhlist_t *legacy_list;
    rhhandle_t void_entry;
    sh_sand_legacy_entry_t *legacy_entry;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(list_p, _SHR_E_PARAM, "list_p");

    legacy_list = *list_p;

    if (legacy_list == NULL)
    {
        /*
         * If there was no tree yet - create 1
         */
        if ((legacy_list = utilex_rhlist_create("Legacy Commands", sizeof(sh_sand_legacy_entry_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for legacy command list\n");
        }
        /*
         * Fill list pointer to be kept in parent object - either static command for beginning or parent list
         */
        *list_p = legacy_list;
    }
    /*
     * Check if the token was already registered - it means that more than one command has the same first n tokens
     * It will also allow to avoid failure if we try to add same command twice for whatever reason is may be
     */
    if ((legacy_entry = utilex_rhlist_entry_get_by_name(legacy_list, tokens[cur_token])) == NULL)
    {
        /*
         * Not found in the list, add
         */
        if (utilex_rhlist_entry_add_tail(legacy_list, tokens[cur_token], RHID_TO_BE_GENERATED, &void_entry)
            != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add token:%s(%s) to legacy list", tokens[cur_token],
                         legacy_cmd->command);
        }
        legacy_entry = void_entry;
        /*
         * Fill shortcut
         */
        SHR_IF_ERR_EXIT(sh_sand_keyword_shortcut_get(tokens[cur_token], legacy_entry->short_key));
    }

    if (++cur_token == nof_tokens)
    {
        /*
         * If current token was the last one, assign message and substitute to the leaf and exit
         */
        legacy_entry->message = (char *) legacy_cmd->message;
        legacy_entry->substitute = (char *) legacy_cmd->substitute;
        SHR_EXIT();
    }
    else
    {
        /*
         * Otherwise continue the recursion until last token
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_build(unit, legacy_cmd, &legacy_entry->next_tokens_list,
                                                 tokens, nof_tokens, cur_token), "");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      Routine looks for legacy command token under parend node and either keep moving on static command tree if token
 *      is found or switches to build separate dynamic one
 * \param [in] unit
 * \param [in] sh_sand_parent_cmd - command under which token is looked up
 * \param [in] legacy_cmd - command that should be attached in some place to static tree
 * \param [in] tokens - legacy command string tokens
 * \param [in] nof_tokens - number of tokens in command string
 * \param [in] cur_token - token index being handled on this step
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval other errors as per shr_error_e *
 *
 * \remark
 *      Routine is recursive and is keeping recursion until split from static tree, then it goes to dynamic
 * \see
 *      sh_sand_legacy_build
 */
static shr_error_e
sh_sand_legacy_attach(
    int unit,
    sh_sand_cmd_t * sh_sand_parent_cmd,
    sh_sand_legacy_cmd_t * legacy_cmd,
    char **tokens,
    int nof_tokens,
    int cur_token)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if current token is found in parent command list
     */
    SHR_SET_CURRENT_ERR(sh_sand_cmd_get(unit, tokens[cur_token], sh_sand_parent_cmd->child_cmd_a, &sh_sand_cmd, 0));
    if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
    {
        /*
         * Not found
         * Add token to the legacy list of parent command and continue recursively adding tokens inside the list
         */
        SHR_SET_CURRENT_ERR(sh_sand_legacy_build(unit, legacy_cmd, &sh_sand_parent_cmd->legacy_list,
                                                 tokens, nof_tokens, cur_token));
    }
    else if (SHR_FUNC_VAL_IS(_SHR_E_NONE))
    {
        /*
         * Found - we continue journey on the static tree to find split point - until there will be no found
         */
        cur_token++;
        if (cur_token == nof_tokens)
        {
            /*
             * We reach the end of legacy string and all tokens were found on static tree - means command is registered
             * in the framework
             */
            SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR - Command:'%s' is implemented in framework cannot be legacy\n",
                         legacy_cmd->command);
        }
        /*
         * Continue recursion on next level and next token
         */
        SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_attach(unit, sh_sand_cmd, legacy_cmd, tokens, nof_tokens, cur_token), "");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_legacy_init(
    int unit,
    char *root_n,
    sh_sand_cmd_t * sh_sand_cmd_a,
    sh_sand_legacy_cmd_t * sh_sand_legacy_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_legacy_cmd_t *sh_sand_legacy_cmd;
    sh_sand_cmd_t *sh_sand_root_cmd;
    char **tokens = NULL;
    uint32 realtokens = 0;
    sh_sand_legacy_entry_t *legacy_entry;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd_a, _SHR_E_PARAM, "sh_sand_cmd_a");
    /*
     * First of all find command of root
     */
    SHR_IF_ERR_EXIT(sh_sand_cmd_get(unit, root_n, sh_sand_cmd_a, &sh_sand_root_cmd, 0));

    for (sh_sand_legacy_cmd = sh_sand_legacy_cmd_a; sh_sand_legacy_cmd->command != NULL; sh_sand_legacy_cmd++)
    {
        if ((tokens = utilex_str_split((char *) sh_sand_legacy_cmd->command, " ", 10, &realtokens)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input command\"%s\"\n", sh_sand_legacy_cmd->command);
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_attach(unit, sh_sand_root_cmd, sh_sand_legacy_cmd, tokens, realtokens, 0),
                            "Legacy attach failed on:%s\n", sh_sand_legacy_cmd->command);
        utilex_str_split_free(tokens, realtokens);
        tokens = NULL;
    }
    /*
     * Now we need to add all elements from root legacy list to shell command list
     */
    RHITERATOR(legacy_entry, sh_sand_root_cmd->legacy_list)
    {
        cmd_t cmd;
        cmd.c_cmd = RHNAME(legacy_entry);
        cmd.c_f = cmd_callback;
        cmd.c_help = "";
        cmd.c_usage = "";
        if (cmdlist_add(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to add:\"%s\" command\n", RHNAME(legacy_entry));
        }
    }

exit:
    /*
     * In case we leaving with error
     */
    if (tokens != NULL)
    {
        utilex_str_split_free(tokens, realtokens);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_root_deinit(
    int unit)
{
    sh_sand_cmd_t *sh_sand_cmd;
    sh_sand_cmd_t *sh_sys_cmd_a;
    sh_sand_cmd_t *sh_sand_cmd_a;
    sh_sand_cmd_t *sh_device_commands;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_get(unit, &sh_sand_cmd_a), "");

    if (ISEMPTY(sh_sand_cmd_a->keyword) || ((sh_device_commands = sh_sand_cmd_a->child_cmd_a) == NULL))
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Bad device command tree for unit:%d\n", unit);
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_deinit(unit, sh_sand_cmd_a->keyword, sh_sand_cmd_a, cmd_all_invoke), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_remove_from_dyn_list(unit, sh_sand_cmd_a, cmd_all_invoke), "");
    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_remove_from_dyn_list(unit, sh_device_commands, cmd_all_invoke), "");

#ifdef INCLUDE_AUTOCOMPLETE
    sh_sand_cmd_autocomplete_deinit(unit, sh_sand_cmd_a);
#endif /* INCLUDE_AUTOCOMPLETE */

    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
    {
        if (sh_sand_cmd->child_cmd_a == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Root Level for:\"%s\" cannot be leaf, only node\n", sh_sand_cmd->keyword);
        }
        SHR_CLI_EXIT_IF_ERR(sh_sand_deinit(unit, sh_sand_cmd->child_cmd_a), "%s shell command deinit failed\n",
                            sh_sand_cmd->keyword);
        /*
         * invokes is used because in root there may not be invokes and we need some field were we can transfer system
         */
        sh_sys_cmd_a = (sh_sand_cmd_t *) sh_sand_cmd->invokes;
        if (sh_sys_cmd_a != NULL)
        {
            SHR_CLI_EXIT_IF_ERR(sh_sand_deinit(unit, sh_sys_cmd_a), "DNX shell system command verification failed\n");
        }

#ifdef INCLUDE_AUTOCOMPLETE
        if (sh_sand_cmd->flags & SH_CMD_SKIP_ROOT)
        {
            sh_sand_cmd_autocomplete_deinit(unit, sh_sand_cmd->child_cmd_a);
        }
#endif /* INCLUDE_AUTOCOMPLETE */
    }

    SHR_CLI_EXIT_IF_ERR(sh_sand_keyword_list_deinit(unit), "");

    SHR_CLI_EXIT_IF_ERR(sh_sand_cmd_root_set(unit, NULL), "");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_legacy_tree_free(
    int unit,
    rhlist_t * legacy_list)
{
    sh_sand_legacy_entry_t *legacy_entry;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Iterate over command array
     */
    RHITERATOR(legacy_entry, legacy_list)
    {
        if (legacy_entry->next_tokens_list)
        {
            sh_sand_legacy_tree_free(unit, legacy_entry->next_tokens_list);
        }
    }
    utilex_rhlist_free_all(legacy_list);

    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_legacy_clean(
    int unit,
    sh_sand_cmd_t * sh_sand_cmd_a)
{
    sh_sand_cmd_t *sh_sand_cmd;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Iterate over command array
     */
    SH_SAND_CMD_ITERATOR_ALL(sh_sand_cmd, sh_sand_cmd_a)
    {
        if (sh_sand_cmd->legacy_list)
        {
            sh_sand_legacy_tree_free(unit, sh_sand_cmd->legacy_list);
            sh_sand_cmd->legacy_list = NULL;
        }
        if (sh_sand_cmd->child_cmd_a)
        {       /* Command is branch got into recursive call */
            SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_clean(unit, sh_sand_cmd->child_cmd_a), "Legacy Clean Failure");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_legacy_deinit(
    int unit,
    char *root_n,
    sh_sand_cmd_t * sh_sand_cmd_a,
    cmd_func_t cmd_callback)
{
    sh_sand_cmd_t *sh_sand_root_cmd;
    sh_sand_legacy_entry_t *legacy_entry;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(sh_sand_cmd_a, _SHR_E_PARAM, "sh_sand_cmd_a");
    /*
     * First of all find command of root
     */
    SHR_IF_ERR_EXIT(sh_sand_cmd_get(unit, root_n, sh_sand_cmd_a, &sh_sand_root_cmd, 0));
    /*
     * Using top level list as the one of commands added to shell dynamic command list, remove all of them
     */
    RHITERATOR(legacy_entry, sh_sand_root_cmd->legacy_list)
    {
        cmd_t cmd;
        cmd.c_cmd = RHNAME(legacy_entry);
        cmd.c_f = cmd_callback;
        if (cmdlist_remove(unit, &cmd) != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to remove:\"%s\" command\n", RHNAME(legacy_entry));
        }
    }
#ifdef SH_ABS_DEINIT
    /*
     * Now we can free root level list
     */
    sh_sand_legacy_tree_free(unit, sh_sand_root_cmd->legacy_list);
    sh_sand_root_cmd->legacy_list = NULL;
    /*
     * Clean all dynamic trees from static command tree
     */
    SHR_CLI_EXIT_IF_ERR(sh_sand_legacy_clean(unit, sh_sand_root_cmd->cmd), "Legacy Deinit Failure");
#endif
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_add_option_dyn_args(
    int unit,
    sh_sand_control_t * sand_control,
    char *option_name,
    sh_sand_option_cb_t option_cb)
{
    sal_field_type_e type = SAL_FIELD_TYPE_NONE;
    uint32 id = 0;
    void *ext_ptr = NULL;
    rhhandle_t void_arg;

    SHR_FUNC_INIT_VARS(unit);

    if (option_cb(unit, option_name, &type, &id, &ext_ptr) == _SHR_E_NONE)
    {
        if (utilex_rhlist_entry_add_tail(sand_control->dyn_args_list, option_name, id, &void_arg) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "ERROR - Cannot add option:%s to dynamic list", option_name);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_nof_cores_get(
    int unit,
    int *nof_cores)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init the nof_cores.
     */
    *nof_cores = 1;

#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit))
    {
        *nof_cores = dnx_data_device.general.nof_cores_get(unit);
    }
#endif /* defined(BCM_DNX_SUPPORT) */

#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit))
    {
        *nof_cores = dnxf_data_device.general.nof_cores_get(unit);
    }
#endif /* defined(BCM_DNX_SUPPORT) */

#if defined(BCM_DPP_SUPPORT)
    if (SOC_IS_DPP(unit))
    {
        *nof_cores = SOC_DPP_CORE_NOF_ACTIVE_CORES(unit);
    }
#endif /* defined(BCM_DPP_SUPPORT) */

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
