/**
 * \file diag_dnx_export.c
 *
 * Implementation of generic command(s) to export diagnostic
 * data and metadata to dump files.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
#include <stdlib.h>

/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
/*
 * soc
 */
#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
/*
 * bcm
 */
#include <bcm_int/dnx/cmn/dnxcmn.h>
/*
 * appl
 */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/shell.h>
#include "diag_dnx_export.h"
/*
 * }
 */

/*
 * LOCAL DEFINES
 * {
 */

/**
 * \brief - Keyword for the input file name of the script for the export command
 */
#define DNX_DIAG_EXPORT_OPTION_INPUT_SCRIPT         "from"
/**
 * \brief - Keyword for the output file name for the export command
 */
#define DNX_DIAG_EXPORT_OPTION_OUTPUT_FILENAME      "to"
/**
 * \brief - Default input file path/name
 */
#define DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPT        "dump_all.xml"
/**
 * \brief - Default output file path/name
 */
#define DNX_DIAG_EXPORT_DEFAULT_OUTPUT_FILENAME     "data_dump"
/**
 * \brief The file extension of the default output file.
 */
#define DNX_DIAG_EXPORT_DEFAULT_OUTPUT_EXT          "xml"
/**
 * \brief Environment variable name of the sdk base directory path.
 */
#define DNX_DIAG_EXPORT_SDK_PATH_ENV_VAR_NAME       "SDK"
/**
 * \brief Path relative to SDK where default input scripts reside
 */
#define DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPTS_REL_PATH "export/"
/**
 * \brief Default directory in which dumps will be created in case no output
 * dump path is supplied
 */
#define DNX_DIAG_EXPORT_DEFAULT_DUMP_DIR            ".dumps"
/**
 * \brief - Keyword for overwriting the output file instead of appending to it.
 */
#define DNX_DIAG_EXPORT_OPTION_OUTPUT_OVERWRITE     "overwrite"
/**
 * \brief - The maximal command line size allowed
 */
#define DNX_DIAG_EXPORT_CMD_LEN_MAX                 200
/**
 * \brief - Node type of commands in the input script
 */
#define DIAG_DNX_EXPORT_CMD_NODE_STR                "command"
/**
 * \brief - Property type of the command node in the input script
 */
#define DNX_DIAG_EXPORT_CMD_PROPERTY_STR            "line"
/**
 * \brief - The maximal category length
 */
#define DNX_DIAG_EXPORT_CATEGORY_LEN_MAX            200
/**
 * \brief - Attribute type for the command category
 */
#define DNX_DIAG_EXPORT_CATEGORY_PROPERTY_STR           "category"
/**
 * \brief - Attribute type for the command's core mode
 */
#define DNX_DIAG_EXPORT_CORE_MODE_PROPERTY_STR          "sbc"
#define EXPORT_CORE_MODE_SBC 1
#define EXPORT_CORE_MODE_DPC 0
/**
 * \brief - Keyword for creating a golden result of an export command in a certain scenario
 */
#define DNX_DIAG_EXPORT_OPTION_CREATE_GOLD_RESULT   "gold"
/*
 * }
 */

/*
 * GLOBAL DATA
 * {
 */

/**
 * \brief - Man page for 'export' shell command
 */
sh_sand_man_t dnx_export_script_man = {
    .brief = "Export data and meta data to a XML file by issuing shell commands with output file option.",
    .full =
        "This command is used to export data from multiple diagnostics and other shell commands into a "
        "single xml file. The input to the command is a xml file which should have a xml hierarchy of\n"
        "<top><command line=\"...\" category=\"...\"/><command line=\"...\" category=\"...\"/>...</top>.\n"
        "Each command found in the line property of the <command> nodes, is executed with the output file "
        "added to it as a 'file=' option and the category as a 'xpath=' option.\n"
        "For example: issuing the command 'export from=in.xml to=out.xml'\n"
        "where the in.xml file contain the node <command line=\"sig get all\" category=\"packet/metadata\"/>, "
        "would trigger the folowing command in the shell:\n"
        "'sig get all file=out.xml xpath=packet/metadata'.\n"
        "This assumes a convention in shell commands of responding to a 'file=<filename.xml>' option by writing"
        " the command output to the xml structure in the specified file instead of the CLI, under the first node "
        "with the hierarchy:\n"
        "<top><first-xpath-part><second-xpath-part>...<last-xpath-part>\n"
        "(where the parts are separated with '/' in the xpath string. The input file path can either be a full path,"
        " a relative to current directory (pwd) path or a path in the general DB directory under <device-name>/"
        DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPTS_REL_PATH ".\n"
        "For example: specifying input path 'in.xml' on a Jericho-2 device would cause a lookup of this file in the current"
        " directory and in:\n"
        "<DB directory>/jericho_2/" DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPTS_REL_PATH "\n"
        "If the output file is not specified, a new file is generated under $" DNX_DIAG_EXPORT_SDK_PATH_ENV_VAR_NAME
        "/.dumps/ " "with timestamp in the filename.",
    .synopsis = "[from=<input xml file>] [to=<output xml file>]",
    .examples = "from=metadata.xml to=metadump.xml\n" "from=metadata.xml to=metadump.xml overwrite"
};

/**
 * \brief - Options list for 'export' shell command
 */
sh_sand_option_t dnx_export_script_options[] = {
    {
     .keyword = DNX_DIAG_EXPORT_OPTION_INPUT_SCRIPT,
     .type = SAL_FIELD_TYPE_STR,
     .desc = "Input xml script to perform",
     .def = DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPT,
     }
    ,
    {
     .keyword = DNX_DIAG_EXPORT_OPTION_OUTPUT_FILENAME,
     .type = SAL_FIELD_TYPE_STR,
     .desc = "Output xml file name",
     .def = "",
     }
    ,
    {
     .keyword = DNX_DIAG_EXPORT_OPTION_OUTPUT_OVERWRITE,
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Overwrite the file to which everything is exported",
     .def = "Yes",
     }
    ,
    {
     .keyword = DNX_DIAG_EXPORT_OPTION_CREATE_GOLD_RESULT,
     .type = SAL_FIELD_TYPE_BOOL,
     .desc = "Create golden result for export in a certain scenario (to= must also be supplied)",
     .def = "No",
     .flags = SH_SAND_ARG_QUIET,
     }
    ,
    {NULL}      /* End of options list - must be last. */
};

/*
 * }
 */

/*
 * Types
 * {
 */
/**
 * \brief - Data for execution of a command
 */
typedef struct
{
    /*
     * Global data
     */
    char *in_file;
    char *out_file;
    int create_gold;

    /*
     * Per-cmd data
     */
    int overwrite_gold;
    char cmd[DNX_DIAG_EXPORT_CMD_LEN_MAX + 1];
    int core_mode;
    char category[DNX_DIAG_EXPORT_CATEGORY_LEN_MAX + 1];
} export_execution_info_t;

/*
 * }
 */
/*
 * LOCAL PROCEDURES
 * {
 */

/**
 * \brief
 *   Opens the input script file for the export command and retrieves
 *   the head node of the xml structure in it.
 * \param [in] unit - Unit #
 * \param [in] filename - input script file path (See remarks).
 * \param [out] top - xml node object of the top of the xml hierarchy in the input file
 * \return 
 *      Standard error handling
 * \remarks 
 *      The input file path can either be a full path, a relative to current directory (pwd) path or
 *      a path in the global db directory.
 */
static shr_error_e
dnx_export_input_script_top_get(
    int unit,
    char *filename,
    xml_node * top)
{
    xml_node in_top;
    char relative_filename[DNX_DIAG_EXPORT_CMD_LEN_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Safety first
     */
    SHR_NULL_CHECK(filename, _SHR_E_PARAM, "filename");
    SHR_NULL_CHECK(top, _SHR_E_PARAM, "top");

    sal_snprintf(relative_filename, DNX_DIAG_EXPORT_CMD_LEN_MAX, "%s%s",
                 DNX_DIAG_EXPORT_DEFAULT_INPUT_SCRIPTS_REL_PATH, filename);

    /*
     * Open input XML file. Make sure it opened.
     */
    if (NULL ==
        (in_top =
         dbx_pre_compiled_common_top_get(unit, relative_filename, "top",
                                         CONF_OPEN_CURRENT_LOC | CONF_OPEN_NO_ERROR_REPORT)))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "Input script (%s) for export not found locally or in the global directory.", filename);
    }

    /*
     * Return the result
     */
    *top = in_top;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Executes the command given, adding the output file inherent options
 * \param [in] unit - Unit #
 * \param [in] info - All relevant information
 * \return 
 *      Standard error handling.
 */
static shr_error_e
dnx_export_cmd_execute(
    int unit,
    export_execution_info_t * info)
{
    cmd_result_t rv;
    char *full_cmd_buffer = NULL;
    uint32 cmd_buffer_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Safety first
     */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    /*
     * Allocate command buffer. Size must include a terminating NULL ('\0')
     */
    /* *INDENT-OFF* */
    cmd_buffer_size = sal_strlen(info->cmd)
            + sal_strlen(" core=x")
            + sal_strlen(" file=")
            + sal_strlen(info->out_file)
            + sal_strlen(" xpath=")
            + sal_strlen(info->category)
            + sal_strlen(" gold new")
            + 1;
    /* *INDENT-ON* */
    full_cmd_buffer = sal_alloc(cmd_buffer_size, "export_script_full_command");
    if (NULL == full_cmd_buffer)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Out of memory while allocating command buffer of size %dB for export.",
                     cmd_buffer_size);
    }
    if (info->core_mode == EXPORT_CORE_MODE_SBC)
    {
        /*
         * Build full command
         */
        sal_snprintf(full_cmd_buffer, cmd_buffer_size, "%s %s %s file=%s xpath=%s", info->cmd,
                     (info->create_gold ? "gold" : ""), (info->overwrite_gold ? "new" : ""),
                     info->out_file, info->category);
        /*
         * Execute command, release the buffer and verify result value
         */
        LOG_CLI_EX("Executing: '%s' ...%s%s%s\n", info->cmd, EMPTY, EMPTY, EMPTY);
        rv = sh_process_command(unit, full_cmd_buffer);
        if (CMD_OK != rv)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Command '%s' failed.", full_cmd_buffer);
        }
        LOG_CLI_EX("CMD OK%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        /*
         * Dedicated Per Core
         */
        int core_i;
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_i)
        {
            /*
             * Build full command
             */
            sal_snprintf(full_cmd_buffer, cmd_buffer_size, "%s %s %s core=%d file=%s xpath=%s", info->cmd,
                         (info->create_gold ? "gold" : ""), (info->overwrite_gold ? "new" : ""),
                         core_i, info->out_file, info->category);
            /*
             * Execute command, release the buffer and verify result value
             */
            LOG_CLI_EX("Executing: '%s core=%d' ...%s%s\n", info->cmd, core_i, EMPTY, EMPTY);
            rv = sh_process_command(unit, full_cmd_buffer);
            if (CMD_OK != rv)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Command '%s' failed.", full_cmd_buffer);
            }
            LOG_CLI_EX("CMD OK%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    if (full_cmd_buffer)
    {
        sal_free(full_cmd_buffer);
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Creates a default path/name in the user's SDK directiry for an output file.
 *   The returned string must be freed afterwards using sal_free.
 * \param [in] unit - Unit #
 * \param [out] out_file_p - returned default path/name
 * \return shr_error_e Standard
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_export_default_out_file_name_get(
    int unit,
    char **out_file_p)
{
    char *sdk_path;
    char timestamp[SH_SAND_MAX_TIME_SIZE];
    size_t path_len;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get $SDK and timestamp
     */
    sdk_path = getenv(DNX_DIAG_EXPORT_SDK_PATH_ENV_VAR_NAME);
    if (sdk_path == NULL)
    {
        sdk_path = ".";
    }
    sh_sand_time_get(timestamp);
    /*
     * Calculate path max len
     */
    path_len = sal_strlen(sdk_path) + 1 /* '/' */  +
        sizeof(DNX_DIAG_EXPORT_DEFAULT_DUMP_DIR) + 1 /* '/' */  +
        sizeof(DNX_DIAG_EXPORT_DEFAULT_OUTPUT_FILENAME) + 1 /* '_' */  + sal_strlen(timestamp) + 1 /* '.' */  +
        sizeof(DNX_DIAG_EXPORT_DEFAULT_OUTPUT_EXT) + 1 /* '\0' */ ;
    /*
     * Allocate buffer to store the path
     */
    if ((*out_file_p = sal_alloc(path_len, "default_output_path")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not allocate buffer for default output file path.");
    }
    /*
     * Finally - compose the path
     */
    sal_snprintf(*out_file_p, path_len, "%s/%s/%s_%s.%s", sdk_path, DNX_DIAG_EXPORT_DEFAULT_DUMP_DIR,
                 DNX_DIAG_EXPORT_DEFAULT_OUTPUT_FILENAME, timestamp, DNX_DIAG_EXPORT_DEFAULT_OUTPUT_EXT);

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * PROCEDURE IMPLEMENTATIONS
 * {
 */

/**
 * \brief
 *      This is an implementation of the export shell command. This command reads an input xml file with shell
 *      commands. Each command read, is appended with the 'file=<output_file.xml>' where output_file.xml is the name
 *      of a xml file that will hold the output for all the commands.
 * \param [in] unit - Unit #
 * \param [in] args - Command line arguments, may be NULL if called from outside the shell
 * \param [in] sand_control - Shell command control structure
 * \return 
 *      Standard error handling.
 * \see 
 *  *   Command man page - for input file structure, and path lookup details. 
 */
shr_error_e
cmd_dnx_export_script(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    export_execution_info_t info_buf;
    export_execution_info_t *info = &info_buf;
    char *default_out_file_name = NULL;
    int overwrite = 1;
    xml_node in_top = NULL, cur_cmd = NULL;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(info, 0, sizeof(export_execution_info_t));

    /*
     * Safety first, but args may NULL if called not from shell, so it should be taken into account
     * SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
     */
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    /*
     * Get command options
     */
    SH_SAND_GET_STR(DNX_DIAG_EXPORT_OPTION_INPUT_SCRIPT, info->in_file);
    SH_SAND_GET_STR(DNX_DIAG_EXPORT_OPTION_OUTPUT_FILENAME, info->out_file);
    SH_SAND_GET_BOOL(DNX_DIAG_EXPORT_OPTION_OUTPUT_OVERWRITE, overwrite);
    SH_SAND_GET_BOOL(DNX_DIAG_EXPORT_OPTION_CREATE_GOLD_RESULT, info->create_gold);
    /*
     * Validate input
     */
    if (dbx_file_get_type(info->in_file) != DBX_FILE_XML)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Input file must be in XML format and have .xml suffix. File name was: '%s' .",
                     info->in_file);
    }
    if (info->out_file[0] == '\0')
    {
        if (info->create_gold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "When creating golden result, explicit output file must be set "
                         "using to= option\n");
        }
        /*
         * No output file name received from user
         */
        SHR_IF_ERR_EXIT(dnx_export_default_out_file_name_get(unit, &default_out_file_name));
        info->out_file = default_out_file_name;
    }
    else if (dbx_file_get_type(info->out_file) != DBX_FILE_XML)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Output file must have .xml suffix. File name was: '%s' .", info->out_file);
    }
    /*
     * Clear file if necessary
     */
    if (overwrite)
    {
        dbx_file_remove(info->out_file);
    }
    /*
     * Overwrite golden result if necessary
     */
    info->overwrite_gold = info->create_gold;
    /*
     * Open input XML file
     */
    SHR_IF_ERR_EXIT(dnx_export_input_script_top_get(unit, info->in_file, &in_top));
    /*
     * Execute script - read commands from the input file in format:
     * <command line="something to do" />
     * Execute each in shell, adding the output file option 'file=<out_file>'
     */
    cur_cmd = dbx_xml_child_get_first(in_top, DIAG_DNX_EXPORT_CMD_NODE_STR);
    while (cur_cmd != NULL)
    {
        /*
         * Found command to execute. Extract it from the xml node,
         * concatenate 'file=<out_file>' option and send to shell.
         */
        SHR_IF_ERR_EXIT(dbx_xml_property_get_str(cur_cmd,
                                                 DNX_DIAG_EXPORT_CMD_PROPERTY_STR,
                                                 info->cmd, DNX_DIAG_EXPORT_CMD_LEN_MAX));
        if (sal_strlen(info->cmd) > 0)
        {
            RHDATA_GET_STR_DEF_NULL(cur_cmd, DNX_DIAG_EXPORT_CATEGORY_PROPERTY_STR, info->category);
            RHDATA_GET_INT_DEF(cur_cmd, DNX_DIAG_EXPORT_CORE_MODE_PROPERTY_STR, info->core_mode, EXPORT_CORE_MODE_DPC);
#ifdef ADAPTER_SERVER_MODE
            info->core_mode = EXPORT_CORE_MODE_SBC;
#endif
            /*
             * Command successfully read from the input file. Execute it.
             * Continue even if the command failed since we don't want this to
             * cause regression tests failures.
             */
            SHR_IF_ERR_CONT_WITH_LOG(dnx_export_cmd_execute(unit, info),
                                     "Command '%s' failed while exporting to %s%s\n", info->cmd, info->out_file, EMPTY);
            /*
             * After the 1st command execution, there is no need to overwrite gold anymore.
             */
            info->overwrite_gold = 0;
        }
        cur_cmd = dbx_xml_child_get_next(cur_cmd);
    }
    /*
     * At this point the function should not return an error code, even if one
     * of the commands in the input files failed. Due to the use of
     * SHR_IF_ERR_CONT_WITH_LOG in the loop, the return value for the function
     * may be set to some error. Explicitly set it to NONE.
     */
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    LOG_CLI_EX("export finished successfully!%s%s%s%s\n", EMPTY, EMPTY, EMPTY, EMPTY);

exit:
    if (default_out_file_name)
    {
        sal_free(default_out_file_name);
    }
    SHR_FUNC_EXIT;
}
/*
 * }
 */
