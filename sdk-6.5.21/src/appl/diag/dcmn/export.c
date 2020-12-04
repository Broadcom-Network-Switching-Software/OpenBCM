/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNX Export facilities
 */

#include <shared/bsl.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/dcmn/diag.h>
#include <appl/diag/dcmn/export.h>
#include <appl/diag/dcmn/rate_calc.h>
#include <appl/diag/dcmn/diag_signals_dump.h>
#include <appl/diag/sand/diag_sand_utils.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_diagnostics.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dcmn/error.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>

#define TX_CMD_BUFFER_SIZE                  1000
#define PACKET_DATA_BUFFER_SIZE             1000
#define EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE   200
#define EXPORT_DCMN_PATH_MAX                256
#define EXPORT_DCMN_PATH_DEFAULT            ("DNX-Export.xml")

#include <appl/diag/dcmn/diag_pp_data.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

typedef struct {
    int ingress_core;
    int egress_core;
    char *drop_name;
    int is_olp_port;
    int is_oamp_port;
} path_dump_info_t;

/*Global variables
 */
extern debug_signals_t arad_debug_signals, arad_plus_debug_signals, jerichoplus_debug_signals, jericho_debug_signals, qax_debug_signals;

/*Local Functions
 */
STATIC cmd_result_t  diag_dnx_export_usage(int unit, args_t *args);

STATIC void diag_dnx_export_ready_usage(int unit);
STATIC cmd_result_t diag_dnx_export_ready(int unit, args_t* a);

STATIC void diag_dnx_export_property_usage(int unit);
STATIC cmd_result_t diag_dnx_export_property(int unit, args_t* a);

STATIC void diag_dnx_export_pp_usage(int unit);
STATIC cmd_result_t diag_dnx_export_pp(int unit, args_t* a);

STATIC void diag_dnx_export_diag_usage(int unit);
STATIC cmd_result_t diag_dnx_export_diag(int unit, args_t* a);

STATIC void diag_dnx_export_dump_usage(int unit);
STATIC cmd_result_t diag_dnx_export_dump(int unit, args_t* a);

STATIC void diag_dnx_export_script_usage(int unit);
STATIC cmd_result_t diag_dnx_export_script(int unit, args_t* a);

STATIC cmd_result_t diag_dnx_export_path(int unit, args_t* a);

STATIC cmd_result_t diag_dnx_export_dump_get_last_packet(int unit, char * last_data, int last_data_buff_size , bcm_port_t* last_port, path_dump_info_t *path_dump_info_p);
STATIC cmd_result_t diag_dnx_export_signals_dump(int unit, int core);

STATIC cmd_result_t diag_dnx_export_stat_path_clear(int unit);
STATIC cmd_result_t diag_dnx_export_device_info_dump(int unit);
STATIC cmd_result_t diag_dnx_export_stat_path_dump(int unit, path_dump_info_t *path_dump_info_p);


/*
 *Export diag pack types
 */
typedef enum diag_dnx_export_type_e
{
    diag_dnx_export_type_usage         = 0,
    diag_dnx_export_type_ready         = 1,
    diag_dnx_export_type_property      = 2,
    diag_dnx_export_type_dump          = 3,
    diag_dnx_export_type_pp            = 4,
    diag_dnx_export_type_diag          = 5,
    diag_dnx_export_type_script        = 6,
    diag_dnx_export_type_path          = 7,

    /*Must be last!*/
    diag_dnx_export_type_nof

} diag_dnx_export_type_s;

/*
 * DNX export diag pack
 */
const diag_dnx_table_t diag_dnx_export_pack[] = {
    /*CMD_NAME,    CMD_ACTION,                   CMD_USAGE,                              CMD_ID*/
    {"usage",      diag_dnx_export_usage,        NULL,                                   diag_dnx_export_type_usage},
    {"ready",      diag_dnx_export_ready,        diag_dnx_export_ready_usage,            diag_dnx_export_type_ready},
    {"property",   diag_dnx_export_property,     diag_dnx_export_property_usage,         diag_dnx_export_type_property},
    {"dump",       diag_dnx_export_dump,         diag_dnx_export_dump_usage,             diag_dnx_export_type_dump},
    {"pp",         diag_dnx_export_pp,           diag_dnx_export_pp_usage,               diag_dnx_export_type_pp},
    {"diag",       diag_dnx_export_diag,         diag_dnx_export_diag_usage,             diag_dnx_export_type_diag},
    {"script",     diag_dnx_export_script,       diag_dnx_export_script_usage,           diag_dnx_export_type_script},
    {"path",       diag_dnx_export_path,         NULL,                                   diag_dnx_export_type_path},
    {NULL,         NULL,                         NULL,                                   DNX_DIAG_ID_UNAVAIL}
};

STATIC int
diag_dnx_export_support(int unit, int diag_id)
{
    switch (diag_id)
    {
       default:
           return TRUE;
           break;
    }
}

/*DNX export diag pack usage*/
STATIC cmd_result_t
diag_dnx_export_usage(int unit, args_t *args)
{
    cli_out("Export diag pack usage:\n");
    cli_out("-----------------------\n");
    diag_dnx_usage_print(unit, diag_dnx_export_pack, diag_dnx_export_support);


    cli_out("export: ready for command\n");
    return CMD_OK;
}

char cmd_dpp_export_usage_str[] = "DNX export diag pack - type 'export usage' for additionl information\n";
/*DNX export pack main function*/
cmd_result_t
cmd_dpp_export(int unit, args_t *args)
{
    char *cmd_name;
    cmd_result_t res;

    if(!SOC_IS_ARAD(unit))
        return CMD_FAIL;

    cmd_name = ARG_GET(args);
    res = diag_dnx_dispatch(unit, diag_dnx_export_pack, diag_dnx_export_support, cmd_name, args);
    if (res != CMD_OK)
    {
        cli_out("EXPORT_CMD: FAILED\n");
    } else {
        cli_out("EXPORT_CMD: PASS\n");
    }
    if (res == CMD_USAGE)
    {
        diag_dnx_export_usage(unit, NULL);
        return CMD_FAIL;
    }

    return res;
}

/*Diag Export ready*/
STATIC void
diag_dnx_export_ready_usage(int unit)
{
    cli_out("'ready' - printout when the driver is ready for new command\n");
}

STATIC cmd_result_t
diag_dnx_export_ready(int unit, args_t* a)
{
    cli_out("export: ready for command\n");
    return CMD_OK;
}

/*Diag Export Property*/
STATIC void
diag_dnx_export_property_usage(int unit)
{
    cli_out("'property <soc property name> [...]' - display soc property value or NULL if undefined\n");
}

/*suffix types to parse*/
typedef enum diag_dnx_export_property_suffix_type_e {
    diag_dnx_export_property_suffix_type_none           = 0,
    diag_dnx_export_property_suffix_type_str            = 1,
    diag_dnx_export_property_suffix_type_port           = 2,
    diag_dnx_export_property_suffix_type_sp             = 3,
    diag_dnx_export_property_suffix_type_counter_engine = 4
} diag_dnx_export_property_suffix_type_t;

STATIC cmd_result_t
diag_dnx_export_property(int unit, args_t* args)
{
    char *name, *value, *value_port, *suffix = NULL;
    parse_table_t pt;
    diag_dnx_export_property_suffix_type_t suffix_type;
    bcm_port_t port;
    int i;

    name = ARG_GET(args);
    if (name == NULL)
    {
        return CMD_USAGE;
    }

    /* parsing input from user */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "suffix", PQ_DFL|PQ_STRING, NULL, &suffix, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
      parse_arg_eq_done(&pt);
      return CMD_USAGE;
    }

    if (suffix == NULL)
    {
        suffix_type = diag_dnx_export_property_suffix_type_none;
    }
    else
    {
        int strnlen_suffix;
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_suffix = sal_strnlen(suffix,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(suffix, "<port>", strnlen_suffix))
        {
            suffix_type = diag_dnx_export_property_suffix_type_port;
        }
        else if (!sal_strncasecmp(suffix, "<sp>", strnlen_suffix))
        {
            suffix_type = diag_dnx_export_property_suffix_type_sp;
        }
        else if (!sal_strncasecmp(suffix, "<engine-id>", strnlen_suffix))
        {
            suffix_type = diag_dnx_export_property_suffix_type_counter_engine;
        }
        else
        {
            suffix_type = diag_dnx_export_property_suffix_type_str;
        }
    }
    switch (suffix_type)
    {
       case diag_dnx_export_property_suffix_type_none:
           value  = soc_property_get_str(unit, name);
           cli_out("export_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           break;
       case diag_dnx_export_property_suffix_type_str:
           value  = soc_property_suffix_num_only_suffix_str_get (unit, -1, name, suffix);
           cli_out("export_soc_property:%s_%s:%s:\n", name, suffix, value != NULL ? value : "NULL");
           break;
       case diag_dnx_export_property_suffix_type_port:
           value  = soc_property_get_str(unit, name);
           cli_out("export_soc_property:%s:%s:\n", name, value != NULL ? value : "NULL");
           PBMP_PORT_ITER(unit, port)
           {
               value_port = soc_property_port_get_str(unit, port, name);
               if ((value == NULL && value_port != NULL) ||
                   (value != NULL && value_port != NULL && !sal_strncasecmp(value, value_port, strlen(value))))
               {
                   cli_out("export_soc_property:%s_%s:%s:\n", name, SOC_PORT_NAME(unit, port), value);
               }
           }
           break;
       case diag_dnx_export_property_suffix_type_counter_engine:
           for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_counter_processors); i++) {
               value = soc_property_suffix_num_only_suffix_str_get (unit, i, name, "");
               if (value != NULL) {
                   cli_out("export_soc_property:%s_%d:%s:\n", name, i, value);
               }
           }
           break;
       default:
           break;
    }
    parse_arg_eq_done(&pt);
    return CMD_OK;
}

/* Diag Export PP */
STATIC void
diag_dnx_export_pp_usage(int unit)
{
    cli_out("'pp [stage=<parser/vt/tt/flp>] [file=target_file ]' - export PP Stage programs&tables into targte_file.xml\n");
}

STATIC cmd_result_t
diag_dnx_export_pp(int unit, args_t* args)
{
    parse_table_t  pt;
    cmd_result_t res = CMD_OK;

    char *file_n = NULL;
    char *stage_n = NULL;

    /* Get parameters */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "file", PQ_STRING, 0, (void *)&file_n, NULL);
    parse_table_add(&pt, "stage", PQ_STRING, 0, (void *)&stage_n, NULL);

    if (0 > parse_arg_eq(args, &pt)) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    res = dpp_export_pp(unit, stage_n, file_n);
    parse_arg_eq_done(&pt);
    return res;
}

/* Diag commands wrapped with export */
STATIC void
diag_dnx_export_diag_usage(int unit)
{
    cli_out("'diag \"<diag command>\" \n");
}

STATIC cmd_result_t
diag_dnx_export_diag(int unit, args_t* args)
{
    return sh_process_command(unit, ARG_GET(args));
}

STATIC void
diag_dnx_export_script_usage(int unit)
{
    cli_out("'script infile=[script_file.xml] outfile=[output_file.xml]' - executes all the commands in [script_file],\n"
            "\t\tEach line is a command, adding 'file=[output_file.xml]' to each command.\n");
}

STATIC cmd_result_t
diag_dnx_export_script(int unit, args_t* args)
{
    cmd_result_t res = CMD_OK;
    int rv = _SHR_E_NONE;
    /*
     * Arguments
     */
    parse_table_t  pt;
    char *in_file = NULL;
    char *out_file = NULL;
    uint8 is_script_in_common_dir = FALSE;
    /*
     * XML nodes
     */
    xml_node inTop, outTop;
    /*
     * Parse arguments
     */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "infile", PQ_STRING, 0, (void *)&in_file, NULL);
    parse_table_add(&pt, "outfile", PQ_STRING, 0, (void *)&out_file, NULL);
    if (0 > parse_arg_eq(args, &pt))
    {
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    /*
     * Validate input
     */
    if (sal_strlen(out_file) > EXPORT_DCMN_PATH_MAX) {
        cli_out("Warning: output file name too long (%s), exporting to %s\n", out_file, EXPORT_DCMN_PATH_DEFAULT);
        out_file = EXPORT_DCMN_PATH_DEFAULT;
    }
    if (dbx_file_get_type(in_file) != DBX_FILE_XML) {
        cli_out("Error: input file must be XML file\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_file_get_type(out_file) != DBX_FILE_XML) {
        cli_out("Error: output file must have .xml suffix\n");
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Open input XML file
     */
    if((inTop = dbx_xml_top_get(in_file, "top", 0/*flags - open file to read*/)) == NULL) {
        /*
         * Local file open failed.
         * Try the commondir.
         */
        char common_file_path[EXPORT_DCMN_PATH_MAX];
        char* export_scripts_path = getenv("DPP_EXPORT_PATH");
        if (NULL == export_scripts_path) {
            cli_out("Error: Global input script folder not defined.\n");
            res = CMD_FAIL;
            goto exit;
        }
        if (sal_strlen(export_scripts_path) + 1 + sal_strlen(in_file) > EXPORT_DCMN_PATH_MAX - 1) {
            cli_out("failed to open (path too long): %s/%s\n", export_scripts_path, in_file);
            res = CMD_FAIL;
            goto exit;
        }
        sal_sprintf(common_file_path, "%s/%s", export_scripts_path, in_file);
        if((inTop = dbx_xml_top_get(common_file_path, "top", 0/*flags - open file to read*/)) == NULL) {
            cli_out("failed to open: %s or %s\n", in_file, common_file_path);
            res = CMD_FAIL;
            goto exit;
        }
        is_script_in_common_dir = TRUE;
    }
    /*
     * Open output XML file
     */
    if((outTop = dbx_xml_top_get(out_file, "top", CONF_OPEN_OVERWRITE|CONF_OPEN_CREATE)) == NULL) {
        cli_out("failed to open: %s\n", out_file);
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Add device properties to <top>
     */
    rv = dbx_xml_property_set_str(outTop, "device", (char *)soc_dev_name(unit));
    if (rv != _SHR_E_NONE) {
        res = CMD_FAIL;
        goto exit;
    }
    rv = dbx_xml_property_set_str(outTop, "family", SOC_CHIP_STRING(unit));
    if (rv != _SHR_E_NONE) {
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Add export description node
     */
    {
        xml_node expDesc;
        if ((expDesc = dbx_xml_child_add(outTop, "export", 1)) == NULL) {
            cli_out("Error: something went wrong\n");
        }
        rv = dbx_xml_property_set_str(expDesc, "type", "script");
        if (rv != _SHR_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        rv = dbx_xml_property_set_str(expDesc, "script", in_file);
        if (rv != _SHR_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        rv = dbx_xml_property_set_str(expDesc, "source", is_script_in_common_dir ? "common directory" : "local");
        if (rv != _SHR_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        rv = dbx_xml_node_end(expDesc, 1);
        if (rv != _SHR_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
    }
    /*
     * Save and close XML file
     */
    dbx_xml_top_save(outTop, out_file);
    dbx_xml_top_close(outTop);
    /*
     * Execute script
     */
    {
        char buffer[EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE + 10 + EXPORT_DCMN_PATH_MAX];
        xml_node curCmd;
        if ((curCmd = dbx_xml_child_get_first(inTop, "command")) == NULL) {
            /*
             * Nothing to do
             */
            goto exit;
        }
        while (curCmd != NULL) {
            dbx_xml_property_get_str(curCmd, "line", buffer, EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE);
            if (sal_strlen(buffer) > 0) {
                sal_strcat(buffer, " file=");
                sal_strcat(buffer, out_file);
                sh_process_command(unit, buffer);
            }
            curCmd = dbx_xml_child_get_next(curCmd);
        }
    }
exit:
    /*
     * Release argument parsing resources
     */
    parse_arg_eq_done(&pt);

    return res;
}

STATIC cmd_result_t
diag_dnx_export_path(int unit, args_t* args)
{
    cmd_result_t res = CMD_OK;
    /*
     * Buffers
     */
    char last_data[PACKET_DATA_BUFFER_SIZE];
    bcm_port_t port = -1;
    path_dump_info_t path_dump_info;
    /*
     * XML nodes
     */
    xml_node curTop, pathNode;
    /*
     * Arguments
     */
    parse_table_t  pt;
    char *filename = NULL;
    /*
     * Parse arguments
     */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "file", PQ_STRING, 0, (void *)&filename, NULL);
    if (0 > parse_arg_eq(args, &pt))
    {
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    if (filename == NULL) {
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Get path data
     */
    if(diag_dnx_export_stat_path_dump(unit, &path_dump_info) < 0) {
        cli_out("diag_dnx_export_stat_path_dump: failed");
        goto exit;
    }
    if(diag_dnx_export_dump_get_last_packet(unit, last_data, PACKET_DATA_BUFFER_SIZE , &port, &path_dump_info) < 0) {
        cli_out("diag_dnx_export_dump_get_last_packet: failed");
        goto exit;
    }
    /*
     * Open XML file
     */
    if((curTop = dbx_xml_top_get(filename, "top", CONF_OPEN_CREATE)) == NULL) {
        cli_out("failed to open/create:%s\n", filename);
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Export path to xml file - Add <path> node
     */
    if((pathNode = dbx_xml_child_add(curTop, "path", 1)) == NULL) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Export path - Add path attributes
     */
    if (dbx_xml_property_set_int(pathNode, "in_port",      port)                        != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_xml_property_set_int(pathNode, "ingress_core", path_dump_info.ingress_core) != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_xml_property_set_int(pathNode, "egress_core",  path_dump_info.egress_core)  != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_xml_property_set_str(pathNode, "drop",         path_dump_info.drop_name)    != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_xml_property_set_int(pathNode, "is_olp",       path_dump_info.is_olp_port)  != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    if (dbx_xml_property_set_int(pathNode, "is_oamp",      path_dump_info.is_oamp_port) != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Add path - close path node
     */
    if (dbx_xml_node_end(pathNode, 1)                                                   != _SHR_E_NONE) {
        cli_out("Failed to add path\n");
        res = CMD_FAIL;
        goto exit;
    }
    /*
     * Save and close XML file
     */
    dbx_xml_top_save(curTop, filename);
    dbx_xml_top_close(curTop);

exit:
    /*
     * Release argument parsing resources
     */
    parse_arg_eq_done(&pt);

    return res;
}

STATIC shr_error_e
diag_dnx_export_xml(int unit, char *output_file, bcm_port_t port, char *data, path_dump_info_t *path_dump_info_p)
{
    xml_node curTop, curSig, curCore, curBlock, curLine;
    int core;
    debug_signals_t *debug_signals;
    ARAD_PP_DIAG_REG_FIELD fld;
    uint32 sig_value[ARAD_PP_DIAG_DBG_VAL_LEN];
    dpp_export_pp_stage_info_t *stages = NULL;
    char tmp_str[120];
    int i, j, k, n, program_id;

    SHR_FUNC_INIT_VARS(unit);

    if((curTop = dbx_xml_top_get(output_file, "top", CONF_OPEN_CREATE | CONF_OPEN_OVERWRITE)) == NULL) {
        cli_out("failed to create:%s\n", output_file);
        goto exit;
    }

    RHDATA_SET_STR(curTop, "device", (char *)soc_dev_name(unit));
    RHDATA_SET_STR(curTop, "family", SOC_CHIP_STRING(unit));

    if(SOC_IS_QAX(unit))
        debug_signals = &qax_debug_signals;
    else if(SOC_IS_JERICHO_PLUS(unit))
        debug_signals = &jerichoplus_debug_signals;
    else if(SOC_IS_JERICHO(unit))
        debug_signals = &jericho_debug_signals;
    else if(SOC_IS_ARADPLUS(unit))
        debug_signals = &arad_plus_debug_signals;
    else
        debug_signals = &arad_debug_signals;

    if((curSig = dbx_xml_child_add(curTop, "signals", 1)) == NULL) {
        cli_out("Failed to add core\n");
        goto exit;
    }

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        if((curCore = dbx_xml_child_add(curSig, "core", 2)) == NULL) {
            cli_out("Failed to add core\n");
            goto exit;
        }
        RHDATA_SET_INT(curCore, "id", core);
        for (i = 0; i < BLOCK_NUM; ++i) {
            if((curBlock = dbx_xml_child_add(curCore, "block", 3)) == NULL) {
                cli_out("Failed to add core\n");
                goto exit;
            }
            RHDATA_SET_INT(curBlock, "id", debug_signals->block_id[i]);
            for (j = 0; j <= debug_signals->block_msb_max[i]; ++j) {
                for (k = 0; k <= debug_signals->block_lsb_max[i]; ++k) {
                    if((curLine = dbx_xml_child_add(curBlock, "reg", 4)) == NULL) {
                        cli_out("Failed to add core\n");
                        goto exit;
                    }
                    fld.base = (j << 16) + k;
                    fld.lsb  = 0;
                    fld.msb  = ARAD_DIAG_BLK_NOF_BITS - 1;

                    sprintf(tmp_str, "0x%08x", fld.base);
                    RHDATA_SET_STR(curLine, "address", tmp_str);

                    if(arad_pp_diag_dbg_val_get_unsafe(unit, core, debug_signals->block_id[i], &fld, sig_value) != SOC_SAND_OK)
                        goto exit;
                    for (n = 0; n < ARAD_DIAG_DBG_VAL_LEN; ++n) {
                      sprintf(tmp_str + n*11, "0x%08x ", sig_value[n]);
                    }
                    RHDATA_SET_STR(curLine, "data", tmp_str);
                }
            }
            dbx_xml_node_end(curBlock, 3);
        }
        dbx_xml_node_end(curCore, 2);
    }
    dbx_xml_node_end(curSig, 1);

    if((curSig = dbx_xml_child_add(curTop, "stages", 1)) == NULL) {
        cli_out("Failed to add core\n");
        goto exit;
    }
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        stages = sal_alloc(sizeof(dpp_export_pp_stage_info_t) * DPP_EXPORT_MAX_STAGE_NUM, "stages");
        if(stages == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation failure\n");
        }
        memset(stages, 0, sizeof(dpp_export_pp_stage_info_t) * DPP_EXPORT_MAX_STAGE_NUM);
        i = 0;
        dpp_dump_vt(unit, 0, &stages[i++]);
        dpp_dump_tt(unit, 0, &stages[i++]);
        dpp_dump_flp(unit, 0, &stages[i++]);

        if((curCore = dbx_xml_child_add(curSig, "core", 2)) == NULL) {
            cli_out("Failed to add core\n");
            goto exit;
        }
        RHDATA_SET_INT(curCore, "id", core);
        for(k = 0; k < i; k++) {
            if((curBlock = dbx_xml_child_add(curCore, "stage", 3)) == NULL) {
                cli_out("Failed to add core\n");
                goto exit;
            }

            RHDATA_SET_STR(curBlock, "name", stages[k].name);
            for (program_id = 0; program_id < DPP_EXPORT_PP_BUNDLE_TABLE_MAX; program_id++) {
                if (!ISEMPTY(stages[k].bundles[program_id].name) && stages[k].bundles[program_id].selected) {
                    if((curLine = dbx_xml_child_add(curBlock, "context", 4)) == NULL) {
                        cli_out("Failed to add program\n");
                        goto exit;
                    }
                    RHDATA_SET_STR(curLine, "name", stages[k].bundles[program_id].name);
                }
            }
            dbx_xml_node_end(curBlock, 3);
        }
        dbx_xml_node_end(curCore, 2);
    }
    dbx_xml_node_end(curSig, 1);

    if((curSig = dbx_xml_child_add(curTop, "path", 1)) == NULL) {
        cli_out("Failed to add core\n");
        goto exit;
    }

    RHDATA_SET_INT(curSig, "in_port",      port);
    RHDATA_SET_INT(curSig, "ingress_core", path_dump_info_p->ingress_core);
    RHDATA_SET_INT(curSig, "egress_core",  path_dump_info_p->egress_core);
    RHDATA_SET_STR(curSig, "drop",         path_dump_info_p->drop_name);
    RHDATA_SET_INT(curSig, "is_olp",       path_dump_info_p->is_olp_port);
    RHDATA_SET_INT(curSig, "is_oamp",      path_dump_info_p->is_oamp_port);
    RHDATA_SET_STR(curSig, "data",         data);
    dbx_xml_node_end(curSig, 1);

    dbx_xml_top_save(curTop, output_file);
    dbx_xml_top_close(curTop);

exit:
    if(stages != NULL)
        sal_free(stages);
    SHR_FUNC_EXIT;
}

/***********
* dump information for export
* Receives: port data
*
* The method of using a script to send a packet
* is currently not supported
 */
STATIC cmd_result_t
diag_dnx_export_dump(int unit, args_t* a){
    char
        tx_cmd[TX_CMD_BUFFER_SIZE],
        * data=NULL;
    char last_data[PACKET_DATA_BUFFER_SIZE];
    bcm_port_t port = -1;
    int core = -1;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int resend = 1;
    char *filename = NULL;
    char shell_cmd[EXPORT_DCMN_SHELL_CMD_BUFFER_SIZE];
    path_dump_info_t path_dump_info;

    cmd_result_t
        rv = CMD_OK;

    parse_table_t pt;

    sal_memset(&path_dump_info, 0, sizeof(path_dump_info_t));
    /*Parse args*/
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PSRC", PQ_DFL|PQ_INT, &port, &port, NULL);
    parse_table_add(&pt, "DATA", PQ_DFL|PQ_STRING, NULL, &data, NULL);
    parse_table_add(&pt, "FILE", PQ_DFL|PQ_STRING, NULL, &filename, NULL);
    parse_table_add(&pt, "RESEND", PQ_DFL|PQ_INT, &resend, &resend, NULL);
    if(parse_arg_eq(a, &pt) < 0) {
        cli_out("invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if(filename != NULL) {
        if(dbx_file_get_type(filename) != DBX_FILE_XML) {
            /* Start Log */
            sal_sprintf(shell_cmd, "log file=%s append=no", filename);
            rv = sh_process_command(unit,shell_cmd);
            ASSERT_EXIT(rv, shell_cmd);
        }
    }

    if ((port == -1) || (data == NULL))
    {
        /*Fill last_data buffer*/
        sal_memset(last_data,0,PACKET_DATA_BUFFER_SIZE);

        cli_out("INFO: PSRC or DATA was not received - using last packet\n");

        if(diag_dnx_export_stat_path_dump(unit, &path_dump_info) < 0) {
            cli_out("diag_dnx_export_stat_path_dump: failed");
            goto exit;
        }
        rv = diag_dnx_export_dump_get_last_packet(unit, last_data, PACKET_DATA_BUFFER_SIZE , &port, &path_dump_info);
        /* Even if we have failure here continue to get signals into XML to prevent test failure */
/*        ASSERT_EXIT(rv, "diag_dnx_export_dump_get_last_packet"); */

        data = last_data;
    }

    /*resend/send a packet - otherwise assume the the packet sent over cleared counters*/
    if((resend) && (rv == CMD_OK))
    {
        if(path_dump_info.is_olp_port){
            cli_out("Cannot resend olp packet\n");
            goto exit;
        }
        /* create tx command*/
        sal_memset(tx_cmd,0,TX_CMD_BUFFER_SIZE);
        sal_snprintf(tx_cmd,TX_CMD_BUFFER_SIZE - 1 ,"tx 1 PSRC=%d DATA=%s", port, data);

        rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
        ASSERT_EXIT(rv, "bcm_port_get failed");

        core = mapping_info.core;

        cli_out("INFO: %s", tx_cmd + 4 );

        rv = diag_dnx_export_stat_path_clear(unit);
        ASSERT_EXIT(rv, "diag_dnx_export_path_stat_clear");

        /* send tx command*/
        rv = sh_process_command(unit,tx_cmd);
        ASSERT_EXIT(rv, "tx");
        if(diag_dnx_export_stat_path_dump(unit, &path_dump_info) < 0) {
            cli_out("diag_dnx_export_stat_path_dump: failed\n");
            goto exit;
        }
    }

    if(dbx_file_get_type(filename) == DBX_FILE_XML) {
        return diag_sand_error_get(diag_dnx_export_xml(unit, filename, port, data, &path_dump_info));
    }

    cli_out("\n===export_dump===\n");

    /*device dump*/
    rv = diag_dnx_export_device_info_dump(unit);
    ASSERT_EXIT(rv, "diag_dnx_export_device_info_dump");

    /*signals dump*/
    rv = diag_dnx_export_signals_dump(unit, core);
    ASSERT_EXIT(rv, "diag_dnx_export_signals_dump");
    /*path stat dump*/
    cli_out("\n===dump_path_stat===\n");
    cli_out("in_port:%d:\n", port);
    cli_out("data:%s:\n", data);
    cli_out("ingress_core:%d:\n", path_dump_info.ingress_core);
    cli_out("egress_core:%d:\n", path_dump_info.egress_core);
    cli_out("drop:%s:\n", path_dump_info.drop_name);
    cli_out("is_olp:%d:\n", path_dump_info.is_olp_port);
    cli_out("is_oamp:%d:", path_dump_info.is_oamp_port);
    cli_out("\n===dump_path_stat===\n");

    ASSERT_EXIT(rv, "diag_dnx_export_path_stat_clear");


    cli_out("\n===export_dump===\n");

    if(filename != NULL) {
        sal_sprintf(shell_cmd, "log off");
        rv = sh_process_command(unit,shell_cmd);
        ASSERT_EXIT(rv, shell_cmd);
    }

exit:
    parse_arg_eq_done(&pt);
    return rv;
}

STATIC void
diag_dnx_export_dump_usage(int unit){
    cli_out("'dump [PSRC=<port number> DATA=<data>] [CORE=<core>] [FILE=<file name>] [RESEND=<0/1>]' - sends a packet and creates export dump; \n\
            To save output in a file use FILE option, \n\
            Use xml extension(*.xml) to have xml formatted one \n\
            To send a new packet, fill the PSRC and DATA fields. \n\
            To send the last packet fill the CORE field only. \n\
            To capture part of the data without resending new packet use 'RESEND=0'\n");
}

/***********
* dump signals for export
 */
STATIC cmd_result_t
diag_dnx_export_signals_dump(int unit, int core){

    uint32 sand_res;
    cmd_result_t
        rv = CMD_OK;
    int core_index;
    char shell_cmd[RHFILE_MAX_SIZE];

    cli_out("\n===dump_signal===\n");
    sand_res = arad_diag_signals_dump(unit, core, 0);
    if (SOC_SAND_FAILURE(sand_res))
    {
        cli_out("diag signals dump failed \n");
        return CMD_FAIL;
    }
    cli_out("\n===dump_signal===\n");

    if (SOC_DPP_DEFS_GET(unit, nof_cores) > 1) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
            cli_out("\n===dump_signal core_%d===\n", core_index);

            sand_res = arad_diag_signals_dump(unit, core_index, 0);
            if (SOC_SAND_FAILURE(sand_res))
            {
                cli_out("diag signals dump failed \n");
                return CMD_FAIL;
            }

            cli_out("\n===dump_signal core_%d===\n", core_index);
        }
    }

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
        cli_out("\n===table_view core_%d===\n", core_index);

        sprintf(shell_cmd, "diag pp vtt_dump last=1 core=%d", core);
        sh_process_command(unit,shell_cmd);

        sprintf(shell_cmd, "diag pp flp_dump last=1 core=%d", core);
        sh_process_command(unit,shell_cmd);

        cli_out("\n===table_view core_%d===\n", core_index);
    }
    return rv;
}

/*
* Input: unit number,
*           and pointers to copy information into
* Purpose: get last packet (port&data)
*           and copy it to the input pointers.
* Output: error detection
*/
STATIC cmd_result_t
diag_dnx_export_dump_get_last_packet(int unit, char * last_data, int last_data_buff_size , bcm_port_t* last_port, path_dump_info_t *path_dump_info)
{
    SOC_TMC_DIAG_LAST_PACKET_INFO info;
    unsigned i=0;
    char * soc_prop_port;
    int rv ,
        last_data_len = 0;

    sal_memset(&info, 0x0, sizeof(info));
    /* get last packet into info*/
    info.tm_port = 0;

    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_diag_last_packet_info_get, (unit, path_dump_info->ingress_core, &info));
    if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK) {
      cli_out("\nFailed to get the last packet information\n\n");
      return CMD_FAIL;
    }

    if (unit >= BCM_LOCAL_UNITS_MAX){
        return CMD_FAIL;
    }

    soc_prop_port = soc_property_port_get_str(unit, info.tm_port,  "tm_port_header_type_in");

    /*check the soc property port*/
    if(path_dump_info->is_olp_port ||
       path_dump_info->is_oamp_port) {
        *last_port = info.pp_port;
        return CMD_OK;
    }
    else if (soc_prop_port == NULL) {
        cli_out("ERROR: cannot get last packet - soc property port (NULL) is not supported tm_port:%d\n", info.tm_port);
        return CMD_FAIL;
    }
    else if (!sal_strcmp("ETH",  soc_prop_port  ) ){
        i = 0;
    }
    else if (!sal_strcmp("INJECTED_2_PP",  soc_prop_port  ) ){
        i = 2;
    }
    else if (!sal_strcmp("INJECTED_2",  soc_prop_port  ) ){
        i = 2;
    }
    else{
        cli_out("ERROR: cannot get last packet - soc property port (%s) is not supported\n", soc_prop_port);
        return CMD_FAIL;
    }

    cli_out("INFO: soc property port : %s \n", soc_prop_port);

    for (   ; i < SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX; ++i) {
       sal_sprintf(last_data + last_data_len, "%02x", info.buffer[i]);  /*print 2 chars exactly, pad with zeroes if necessary*/
       last_data_len += 2;

       if (last_data_len >= last_data_buff_size - 2) {
            break;
       }
    }

    *last_port = info.pp_port;

    return CMD_OK;
}


/***********
* dump device info for export
 */
STATIC cmd_result_t
diag_dnx_export_device_info_dump(int unit)
{
    cli_out("\n===device_info===\n");
    cli_out("device:%s:\n", soc_dev_name(unit));
    cli_out("\n===device_info===\n");
    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_is_port_special(int unit, SOC_PPC_PORT port, uint32 port_type, int* result){
    int rv, i;
    bcm_gport_t                             special_ports[BCM_PIPES_MAX];
    int                                     nof_special_ports;
    /* Get special ports array */
    rv = bcm_port_internal_get(unit,port_type,BCM_PIPES_MAX,special_ports,&nof_special_ports);
    if (rv != BCM_E_NONE) {
        cli_out("Error occurred while trying to retrieve ports\n");
        return CMD_FAIL;
    }

    /* In case OLP Ports were defined, check if packet originates from OLP */
    if (nof_special_ports > 0) {
        for (i = 0 ; i < nof_special_ports ; i++) {
            if (port == BCM_GPORT_LOCAL_GET(special_ports[i])) {
                *result = TRUE;
                return CMD_OK;
            }
        }
    }

    *result = FALSE;
    return CMD_OK;

}

/***********
* dump path stat info for export
 */
STATIC cmd_result_t
diag_dnx_export_stat_path_dump(int unit, path_dump_info_t *path_dump_info_p)
{
    soc_dpp_stat_path_info_t info;
    int rv;
    cmd_result_t ret;
    uint32 res;
    SOC_PPC_DIAG_RECEIVED_PACKET_INFO       prm_rcvd_pkt_info;
    SOC_PPC_DIAG_RESULT                     ret_val;
    int                                     is_olp_port = 0, is_oamp_port = 0;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_path_info_get, (unit, &info));
    if (BCM_FAILURE(rv))
    {
        cli_out("ERROR: stat path failed\n");
        return CMD_FAIL;
    }

    res = soc_ppd_diag_received_packet_info_get(
          unit,
          info.ingress_core,
          &prm_rcvd_pkt_info,
          &ret_val
        );

    rv = soc_sand_get_error_code_from_error_word(res);
    if(BCM_FAILURE(rv)){
        cli_out("ERROR: received packet info failed\n");
        return CMD_FAIL;
    }

    if (ret_val == SOC_PPC_DIAG_OK) {
        /* check if port is OLP */
        ret = diag_dnx_is_port_special(unit, prm_rcvd_pkt_info.in_tm_port, BCM_PORT_INTERNAL_OLP, &is_olp_port);
        if(ret != CMD_OK) {
            return ret;
        }

        /* check if port is OAMP */
        ret = diag_dnx_is_port_special(unit, prm_rcvd_pkt_info.in_tm_port, BCM_PORT_INTERNAL_OAMP, &is_oamp_port);
        if(ret != CMD_OK) {
            return ret;
        }

    } else {
        LOG_CLI((BSL_META_U(unit, "%s Packet not found\n\r"),get_core_str(unit, info.ingress_core)));
    }

    switch(info.drop)
    {
       case soc_dpp_stat_path_drop_stage_ingress_no_packet:
           path_dump_info_p->drop_name = "NIF";
           break;
       case soc_dpp_stat_path_drop_stage_ingress_tm:
           path_dump_info_p->drop_name = "ITM";
           break;
       case soc_dpp_stat_path_drop_stage_egress_tm:
           path_dump_info_p->drop_name = "ETM";
           break;
       case soc_dpp_stat_path_drop_stage_none:
       default:
           path_dump_info_p->drop_name = "NONE";
           break;
    }

    path_dump_info_p->ingress_core = info.ingress_core;
    path_dump_info_p->egress_core = info.egress_core;
    path_dump_info_p->is_olp_port = is_olp_port;
    path_dump_info_p->is_oamp_port = is_oamp_port;

    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_export_stat_path_clear(int unit)
{
    soc_dpp_stat_path_info_t info;
    int rv;

    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_path_info_get, (unit, &info));
    if (BCM_FAILURE(rv))
    {
        cli_out("ERROR: stat path failed\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}



