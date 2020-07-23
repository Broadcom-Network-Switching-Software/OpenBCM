/** \file diag_sand_mbist.c
 * 
 * diagnostic pack for sand mbist interface
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/shrextend/shrextend_debug.h>

/*appl*/
#include <appl/diag/diag.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_mbist.h>

#define SAND_MBIST_FILE  "mbist_bin.dat"
#define SAND_MBIST_SENTINELS 0xdeadbeef

#define CHECK_SENTINEL \
do { \
    if ((p - buf) + sizeof(uint32) > filesize) { \
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "passed file end when verifying sentinel value in file %s%s%s\n", fname, EMPTY, EMPTY); \
    } else if (diag_sand_read_uint32(&p) != SAND_MBIST_SENTINELS) { \
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "found a wrong sentinel value in file %s location %d%s\n", fname, (int)(p - sizeof(uint32) - buf), EMPTY); \
    } \
} while (0)

static diag_sand_mbist_info_t diag_sand_mbist_info[BCM_MAX_NUM_UNITS] = { {{0}} };

static int
sand_mbist_default_device_data_get(
    int unit,
    sand_mbist_device_t * mbist_device)
{
    int rc = SOC_E_NONE;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        /*
         * see arad_mbist_device 
         */
        mbist_device->sleep_divisor = 10;
        mbist_device->reg_tap_command = ECI_REG_0270r;
        mbist_device->reg_tap_data_in = ECI_REG_0271r;
        mbist_device->reg_tap_data_out = ECI_REG_0272r;
    }
    else if (SOC_IS_QUX(unit))
    {
        /*
         * see qux_mbist_device 
         */
        mbist_device->sleep_divisor = 3;
        mbist_device->reg_tap_command = ECI_TAP_CPU_INTERFACE_COMMANDr;
        mbist_device->reg_tap_data_in = ECI_TAP_CPU_INTERFACE_DATA_INr;
        mbist_device->reg_tap_data_out = ECI_TAP_CPU_INTERFACE_DATA_OUTr;
    }
    else if (SOC_IS_QAX(unit))
    {
        /*
         * see qax_mbist_device 
         */
        mbist_device->sleep_divisor = 10;
        mbist_device->reg_tap_command = ECI_TAP_CPU_INTERFACE_COMMANDr;
        mbist_device->reg_tap_data_in = ECI_TAP_CPU_INTERFACE_DATA_INr;
        mbist_device->reg_tap_data_out = ECI_TAP_CPU_INTERFACE_DATA_OUTr;
    }
    else if (SOC_IS_JERICHO(unit))
    {
        /*
         * see jer_mbist_device 
         */
        mbist_device->sleep_divisor = 10;
        mbist_device->reg_tap_command = ECI_TAP_CPU_INTERFACE_COMMANDr;
        mbist_device->reg_tap_data_in = ECI_TAP_CPU_INTERFACE_DATA_INr;
        mbist_device->reg_tap_data_out = ECI_TAP_CPU_INTERFACE_DATA_OUTr;
    }
    else if (SOC_IS_RAMON(unit))
    {
        /*
         * see jer_mbist_device 
         */
        mbist_device->sleep_divisor = 10;
        mbist_device->reg_tap_command = ECI_TAP_CPU_INTERFACE_COMMANDr;
        mbist_device->reg_tap_data_in = ECI_TAP_CPU_INTERFACE_DATA_INr;
        mbist_device->reg_tap_data_out = ECI_TAP_CPU_INTERFACE_DATA_OUTr;
    }
    /*
     * else if (SOC_IS_JERICHO2_TYPE(unit)) { mbist_device->sleep_divisor = 10; mbist_device->reg_tap_command =
     * ECI_TAP_CPU_INTERFACE_COMMANDr; mbist_device->reg_tap_data_in = ECI_TAP_CPU_INTERFACE_DATA_INr;
     * mbist_device->reg_tap_data_out = ECI_TAP_CPU_INTERFACE_DATA_OUTr; }
     */
    else
    {
        rc = SOC_E_UNIT;
    }

    return rc;
}

/* read a uint32 from the file buffer and advance the buffer pointer */
static uint32
diag_sand_read_uint32(
    uint8 **buf)
{
    uint32 val = *((*buf)++) << 8;
    val |= *((*buf)++);
    val <<= 8;
    val |= *((*buf)++);
    val <<= 8;
    return val | *((*buf)++);
}

/*
 * Read a string from the file buffer and update the buffer pointer to point after it.
 * Return non zero in case of an error - string too big.
 * the input size does not include the needed terminating NULL character.
 */
static int
diag_sand_read_string(
    uint8 **buf,
    uint32 max_size)
{
    do
    {
        if (*((*buf)++) == 0)
            return 0;
    }
    while (max_size-- != 0);
    return 1;
}

/*
 * Read a binary MBIST script file, process it and store it in memory.
 * Stores the file content in allocated memory and one additional allocation
 * For the data structures of the MBIST script and comment array.
 * Stores the read script as the current loaded script for the device,
 * If a previous one is stored, its memory is freed.
 */
int
diag_sand_mbist_file_load(
    int unit,
    char *fname)
{
    FILE *fp = NULL;
    char *propval = NULL;
    int filesize, bytes_left;
    uint32 nof_commands, nof_comments, max_comment_size, string_size, i;
    uint8 *buf = NULL, *p;
    sand_mbist_script_t *script = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == fname)
    {
        propval = soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "mbist_file_path");
        if (NULL == propval)
        {
            fname = SAND_MBIST_FILE;
        }
        else
        {
            fname = propval;
        }
    }

    fp = sal_fopen(fname, "rb");
    if (fp == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "File %s not exist or cannot open!%s%s\n", fname, EMPTY, EMPTY);
    }

    filesize = sal_fsize(fp);
    if ((filesize < sizeof(uint32) * 9) || (filesize > 0x800000))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "File %s too large > 8 Mbytes or too small!%d%s\n", fname, filesize, EMPTY);
    }

    if ((p = buf = sal_alloc(filesize, "mbist script file")) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Allocate space %u for %s failed!%s\n", filesize, fname, EMPTY);
    }

     /* coverity[tainted_data_argument:FALSE]  */
    if (filesize != sal_fread(buf, 1, filesize, fp))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Read mbist script %s failed!%s%s\n", fname, EMPTY, EMPTY);
    }

    /*
     * check the file header 
     */
    CHECK_SENTINEL;
    nof_commands = diag_sand_read_uint32(&p);
    nof_comments = diag_sand_read_uint32(&p);
    if ((script = sal_alloc(sizeof(*script) + sizeof(char *) * nof_comments, "mbist script")) == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL,
                          "Allocate script data structures space for %s failed with %u comments failed!%s\n", fname,
                          nof_comments, EMPTY);
    }
    script->nof_commands = nof_commands;
    script->nof_comments = nof_comments;

    max_comment_size = diag_sand_read_uint32(&p);
    script->sleep_after_write = diag_sand_read_uint32(&p);
    string_size = diag_sand_read_uint32(&p);
    script->script_name = (char *) p;
    if (diag_sand_read_string(&p, string_size) || (p - (uint8 *) script->script_name) != string_size + 1)
    {
        p[-1] = 0;
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Found script name %s is not of the expected length %u.%s\n",
                          script->script_name, string_size, EMPTY);
    }
    CHECK_SENTINEL;

    /*
     * process the commands of the script 
     */
    script->commands = (const uint8 *) p;
    p += nof_commands;
    CHECK_SENTINEL;

    /*
     * process the comment strings of the script 
     */
    script->comments = (const char **) ((char *) script + sizeof(*script));
    for (i = 0; i < nof_comments; ++i)
    {
        bytes_left = filesize - ((p - buf) + (sizeof(uint32) + 1));     /* bytes left till the end of the last string */
        if (bytes_left <= 0)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "string number %d is outside of file %s%s\n", i, fname, EMPTY);
        }
        string_size = max_comment_size > bytes_left ? bytes_left : max_comment_size;
        script->comments[i] = (char *) p;
        if (diag_sand_read_string(&p, string_size))
        {
            p[-1] = 0;
            SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "Failed to process string number %u that starts with: %s%s\n", i,
                              script->comments[i], EMPTY);
        }

    }
    CHECK_SENTINEL;
    if (p - buf != filesize)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "the MBIST script finished before the end of file %s%s%s\n", fname, EMPTY,
                          EMPTY);
    }

    if (diag_sand_mbist_info[unit].file_buf != NULL)
    {
        sal_free(diag_sand_mbist_info[unit].file_buf);
        diag_sand_mbist_info[unit].file_buf = NULL;
    }
    diag_sand_mbist_info[unit].file_buf = buf;
    if (diag_sand_mbist_info[unit].script != NULL)
    {
        sal_free(diag_sand_mbist_info[unit].script);
        diag_sand_mbist_info[unit].script = NULL;
    }
    diag_sand_mbist_info[unit].script = script;
    buf = NULL;
    script = NULL;
    cli_out("mbist load file %s successfully, file_size=%d\n", fname, filesize);
exit:
    if (fp != NULL)
    {
        sal_fclose(fp);
    }

     /* coverity[tainted_data:FALSE]  */
    if (buf != NULL)
    {
         /* coverity[tainted_data:FALSE]  */
        sal_free(buf);
    }
    if (script != NULL)
    {
        sal_free(script);
    }
    SHR_FUNC_EXIT;
}

#ifndef NO_FILEIO
/* print an MBIST script to a text file*/
int
diag_sand_mbist_script_print(
    int unit,
    sand_mbist_script_t * script,
    char *fname)
{
    FILE *fp = NULL;
    uint32 i, err = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (script == NULL || (script->commands == NULL && script->nof_commands) ||
        (script->comments == NULL && script->nof_comments) || script->script_name == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "current script is invalid.%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    fp = sal_fopen(fname, "w");
    if (fp == NULL)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_INTERNAL, "File %s could not be opened for writing.%s%s\n", fname, EMPTY, EMPTY);
    }

    if (sal_fprintf(fp, "script name: %s\nsleep after write: %u\n", script->script_name, script->sleep_after_write) < 0)
    {
        err = 1;
    }
    if (sal_fprintf(fp, "COMMANDS binary data:\n") < 0)
    {
        err = 1;
    }
    for (i = 0; i < script->nof_commands;)
    {
        if (sal_fprintf(fp, "%.2x", script->commands[i]) < 0)
        {
            err = 1;
        }
        ++i;
        if (i % 4 == 0)
        {       /* handle line formatting */
            if (i % 32)
            {
                if (sal_fprintf(fp, " ") < 0)
                {
                    err = 1;
                }
            }
            else
            {
                if (sal_fprintf(fp, "\n") < 0)
                {
                    err = 1;
                }
            }
        }
    }

    if (sal_fprintf(fp, "\nCOMMENTS:\n") < 0)
    {
        err = 1;
    }
    for (i = 0; i < script->nof_comments; ++i)
    {
        if (sal_fprintf(fp, "%u: <<<%s>>>\n", i, script->comments[i]) < 0)
        {
            err = 1;
        }
    }
    if (sal_fprintf(fp, "\nEND\n") < 0)
    {
        err = 1;
    }

    cli_out("mbist script %s print to file %s %s\n", script->script_name, fname, err ? "failed" : "succeeded");
exit:
    if (fp != NULL)
    {
        sal_fclose(fp);
    }
    SHR_FUNC_EXIT;
}

/* print the current loaded MBIST script to a file */
int
diag_sand_mbist_file_print(
    int unit,
    char *fname)
{
    return diag_sand_mbist_script_print(unit, diag_sand_mbist_info[unit].script, fname);
}
#endif /* NO_FILEIO */

int
diag_sand_mbist_run(
    const int unit)
{
    sand_mbist_dynamic_t dynamic = { 0 };
    sand_mbist_device_t *mbist_device = &diag_sand_mbist_info[unit].mbist_device;
    SHR_FUNC_INIT_VARS(unit);

    if (diag_sand_mbist_info[unit].file_buf == NULL)
    {
        cli_out("Mbist file was not loaded.\n");
        SHR_EXIT();

    }
    if ((mbist_device->sleep_divisor | mbist_device->reg_tap_command |
         mbist_device->reg_tap_data_in | mbist_device->reg_tap_data_out) == 0)
    {   /* IF the mbist device was not defined by the user, use default values */
        SHR_IF_ERR_EXIT(sand_mbist_default_device_data_get(unit, mbist_device));
    }

    dynamic.skip_errors = diag_sand_mbist_info[unit].skip_errors;
    dynamic.measured_time = sal_time_usecs();
    SHR_IF_ERR_EXIT(soc_sand_run_mbist_script(unit, diag_sand_mbist_info[unit].script, mbist_device, &dynamic));
    cli_out("mbist run script result: nof_errors=%d, Memory BIST done in %u Microseconds\n\n",
            dynamic.nof_errors, sal_time_usecs() - dynamic.measured_time);

exit:
    SHR_FUNC_EXIT;
}

int
diag_sand_mbist_write_command(
    int unit,
    uint32 write_command)
{
    soc_reg_t reg_tap_command;

    SHR_FUNC_INIT_VARS(unit);

    reg_tap_command = diag_sand_mbist_info[unit].mbist_device.reg_tap_command;
    if (SOC_REG_IS_VALID(unit, reg_tap_command))
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, reg_tap_command, REG_PORT_ANY, 0, write_command));
    }
    else
    {
        cli_out("The tap_command register is not specified\n");
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

int
diag_sand_mbist_stop_on_error(
    int unit,
    int stop_on_error)
{
    if (stop_on_error)
    {
        diag_sand_mbist_info[unit].skip_errors = 0;
    }
    else
    {
        diag_sand_mbist_info[unit].skip_errors = 1;
    }
    return SOC_E_NONE;
}

sh_sand_man_t sh_sand_mbist_man = {
    .brief = "MBIST diagnostic commands",
    .full =
        "The dispatcher for the different MBIST diagnostics commands\n"
        "        load <script file name>\n"
        "        stop_on_error [0 | 1]\n"
        "        test write_command <val>\n"
        "        test run\n"
        "        device_data <sleep_divisor> CoMmanD_Reg=<regName> DaTaIN_Reg=<regName> DaTaOUT_Reg=<regName>\n",
    .examples = NULL
};

#ifndef NO_FILEIO
static sh_sand_man_t sh_sand_mbist_load_man = {
    .brief = "MBIST load commands \n",
    .examples = ""
};

static sh_sand_man_t sh_sand_mbist_print_man = {
    .brief = "MBIST print current loaded script \n",
    .examples = ""
};
#endif /* NO_FILEIO */

static sh_sand_man_t sh_sand_mbist_stop_on_error_man = {
    .brief = "MBIST Stop_On_Error commands \n",
    .examples = "1\n" "0"
};

static sh_sand_man_t sh_sand_mbist_test_man = {
    .brief = "MBIST test commands \n",
    .examples = "Write_Command=2\n" "Write_Command=3\n" "run"
};

static sh_sand_man_t sh_sand_mbist_device_data_man = {
    .brief = "MBIST device_data commands \n",
    .examples =
        "10 Command_Reg=ECI_TAP_CPU_INTERFACE_COMMAND DataIn_Reg=ECI_TAP_CPU_INTERFACE_DATA_IN DataOut_Reg=ECI_TAP_CPU_INTERFACE_DATA_OUT"
};

#ifndef NO_FILEIO
static sh_sand_option_t sand_mbist_load_options[] = {
    {"filename", SAL_FIELD_TYPE_STR, "Configure mbist load file name", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
static sh_sand_option_t sand_mbist_print_options[] = {
    {"filename", SAL_FIELD_TYPE_STR, "Select output file name", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
#endif /* NO_FILEIO */
static sh_sand_option_t sand_mbist_stop_on_error_options[] = {
    {"enable", SAL_FIELD_TYPE_BOOL, "Configure mbist stop_on_error", NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
static sh_sand_option_t sand_mbist_test_options[] = {
    {"Write_Command", SAL_FIELD_TYPE_UINT32, "Configure mbist test write_command", "-1"},
    {"run", SAL_FIELD_TYPE_BOOL, "Configure mbist test write_command", ""},
    {NULL}
};
static sh_sand_option_t sand_mbist_device_data_options[] = {
    {"divisor", SAL_FIELD_TYPE_UINT32, "Configure mbist device_data sleep_divisor", "-1", NULL, NULL, SH_SAND_ARG_FREE},
    {"CoMmanD_Reg", SAL_FIELD_TYPE_STR, "Configure mbist device_data TAP command register", NULL},
    {"DaTaIN_Reg", SAL_FIELD_TYPE_STR, "Configure mbist device_data TAP data in register", NULL},
    {"DaTaOUT_Reg", SAL_FIELD_TYPE_STR, "Configure mbist device_data TAP data out register", NULL},
    {NULL}
};

/*SH_SAND_IS_PRESENT("name", is_present);*/

#ifndef NO_FILEIO
static shr_error_e
sh_sand_mbist_load_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("filename", filename);
    if (ISEMPTY(filename))
    {
        cli_out("The mbist load filename must be specified.\n");
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_sand_mbist_file_load(unit, filename));
    }

exit:
    SHR_FUNC_EXIT;
}

/* print to file the current loaded script, useful for debugging by comparing to scripts in the code */
static shr_error_e
sh_sand_mbist_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("filename", filename);
    if (ISEMPTY(filename))
    {
        cli_out("The mbist print filename must be specified.\n");
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_sand_mbist_file_print(unit, filename));
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /* NO_FILEIO */

static shr_error_e
sh_sand_mbist_stop_on_error_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int stop_on_error;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("enable", stop_on_error);

    SHR_IF_ERR_EXIT(diag_sand_mbist_stop_on_error(unit, stop_on_error));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_mbist_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 write_command;
    int run_flag, is_write_command_present;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("Write_Command", is_write_command_present);
    SH_SAND_GET_BOOL("run", run_flag);

    if (is_write_command_present)
    {
        SH_SAND_GET_UINT32("Write_Command", write_command);
        SHR_IF_ERR_EXIT(diag_sand_mbist_write_command(unit, write_command));
    }
    else if (run_flag)
    {
        SHR_IF_ERR_EXIT(diag_sand_mbist_run(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_sand_mbist_device_data_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 sleep_divisor;
    char *tap_command_reg_name, *tap_data_in_reg_name, *tap_data_out_reg_name;
    soc_reg_t tap_command_reg, tap_data_in_reg, tap_data_out_reg;
    /*
     * void *tap_command_reg_h=NULL, *tap_data_in_h=NULL, *tap_data_out_h=NULL; int reg_match;
     */
    soc_reg_t reg;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("divisor", sleep_divisor);
    SH_SAND_GET_STR("CoMmanD_Reg", tap_command_reg_name);
    SH_SAND_GET_STR("DaTaIN_Reg", tap_data_in_reg_name);
    SH_SAND_GET_STR("DaTaOUT_Reg", tap_data_out_reg_name);

    /*
     * SHR_IF_ERR_EXIT(diag_sand_compare_init(tap_command_reg_name, &tap_command_reg_h));
     * SHR_IF_ERR_EXIT(diag_sand_compare_init(tap_data_in_name, &tap_data_in_h));
     * SHR_IF_ERR_EXIT(diag_sand_compare_init(tap_data_out_name, &tap_data_out_h));
     */
    tap_command_reg = INVALIDr;
    tap_data_in_reg = INVALIDr;
    tap_data_out_reg = INVALIDr;
    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }

        if (sal_strcasecmp(tap_command_reg_name, SOC_REG_NAME(unit, reg)) == 0)
        {
            tap_command_reg = reg;
        }
        else if (sal_strcasecmp(tap_data_in_reg_name, SOC_REG_NAME(unit, reg)) == 0)
        {
            tap_data_in_reg = reg;
        }
        else if (sal_strcasecmp(tap_data_out_reg_name, SOC_REG_NAME(unit, reg)) == 0)
        {
            tap_data_out_reg = reg;
        }
    }

    if ((tap_command_reg == INVALIDr) || (tap_data_in_reg == INVALIDr) || (tap_data_out_reg == INVALIDr))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "The mbist tap command|data_in|data_out register should be specified.\n");
    }

    diag_sand_mbist_info[unit].mbist_device.sleep_divisor = sleep_divisor;
    diag_sand_mbist_info[unit].mbist_device.reg_tap_command = tap_command_reg;
    diag_sand_mbist_info[unit].mbist_device.reg_tap_data_in = tap_data_in_reg;
    diag_sand_mbist_info[unit].mbist_device.reg_tap_data_out = tap_data_out_reg;

exit:
    /*
     * diag_sand_compare_close(tap_command_reg_h); diag_sand_compare_close(tap_data_in_h);
     * diag_sand_compare_close(tap_data_out_h);
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief DNXC MBIST diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for MBIST diagnostic commands
 */
sh_sand_cmd_t sh_sand_mbist_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
#ifndef NO_FILEIO
    {"LOAD", sh_sand_mbist_load_cmd, NULL, sand_mbist_load_options, &sh_sand_mbist_load_man}
    ,
#endif
    {"Stop_On_Error", sh_sand_mbist_stop_on_error_cmd, NULL, sand_mbist_stop_on_error_options,
     &sh_sand_mbist_stop_on_error_man}
    ,
    {"Test", sh_sand_mbist_test_cmd, NULL, sand_mbist_test_options, &sh_sand_mbist_test_man}
    ,
    {"Device_Data", sh_sand_mbist_device_data_cmd, NULL, sand_mbist_device_data_options, &sh_sand_mbist_device_data_man}
    ,
#ifndef NO_FILEIO
    {"Print", sh_sand_mbist_print_cmd, NULL, sand_mbist_print_options, &sh_sand_mbist_print_man}
    ,
#endif
    {NULL}
};

/*
 * This routine is for DPP/DFE only - For DNX/DNXF recursion starts from the top
 */
cmd_result_t
cmd_sand_mbist(
    int unit,
    args_t * args)
{
    cmd_result_t result;
    SH_SAND_VERIFY("mbist", sh_sand_mbist_cmds, result);

    sh_sand_act(unit, args, sh_sand_mbist_cmds, sh_sand_sys_cmds, NULL);
    ARG_DISCARD(args);
    /*
     * Always return OK - we provide all help & usage from inside framework
     */
exit:
    return result;
}

/*
 * General shell style usage
 */
const char cmd_sand_mbist_usage[] = "Please use \"mbist load/stop_on_error/test/device_data usage\" for help\n";

/*
 * General shell style description
 */
const char cmd_sand_mbist_desc[] = "MBIST diagnostic commands";

#undef BSL_LOG_MODULE
