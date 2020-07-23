/*! \file bcma_cfgdbcmd_config.c
 *
 * CLI Configuration database shell commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/cfgdb/bcma_cfgdb.h>

#include <bcma/cfgdb/bcma_cfgdbcmd_config.h>

/*
 * Call-back function of bcma_cfgdb_var_traverse to get total variable numbers.
 */
static int
var_nums_get(void *cookie, const char *name, const char *value,
             uint32_t *tag)
{
    int *var_count = (int *)cookie;

    COMPILER_REFERENCE(name);
    COMPILER_REFERENCE(value);
    COMPILER_REFERENCE(tag);

    *var_count = *var_count + 1;

    return 0;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to get variable names and values.
 */
static int
var_entries_get(void *cookie, const char *name, const char *value,
                uint32_t *tag)
{
    char ***p_var_entry = (char ***)cookie;

    COMPILER_REFERENCE(tag);

    **p_var_entry = sal_strdup(name);
    if (**p_var_entry == NULL) {
        return -1;
    }
    *p_var_entry = *p_var_entry + 1;

    **p_var_entry = sal_strdup(value);
    if (**p_var_entry == NULL) {
        return -1;
    }
    *p_var_entry = *p_var_entry + 1;

    return 0;
}

/*
 * Function to process: config =
 */
static int
config_cmd_prompt(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    int size, idx, var_nums = 0;
    bcma_cli_parse_table_t pt;
    char **var_entries, **p_var_entry, **p_values;

    /* Get the total numbers of config vars */
    bcma_cfgdb_var_traverse(var_nums_get, &var_nums);
    if (var_nums == 0) {
        /* Consume '=' */
        BCMA_CLI_ARG_NEXT(args);
        return BCMA_CLI_CMD_OK;
    }

    size = 2 * var_nums * sizeof(char *);

    /* Allocate resources for vars information <name, value> */
    var_entries = sal_alloc(size, "bcmaCfgdbCmdPrompt");
    if (var_entries == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(var_entries, 0, size);

    size = var_nums * sizeof(char *);

    /* Allcate pointer arrays for variable value */
    p_values = sal_alloc(size, "bcmaCfgdbCmdPrompt");
    if (p_values == NULL) {
        sal_free(var_entries);
        return BCMA_CLI_CMD_FAIL;
    }
    sal_memset(p_values, 0, size);

    p_var_entry = var_entries;

    /* Get all vars names and values */
    if (bcma_cfgdb_var_traverse(var_entries_get, &p_var_entry) != 0) {
        /* Free allocated resources if trverse failed. */
        for (idx = 0; idx < 2 * var_nums; idx++) {
            if (var_entries[idx]) {
                sal_free(var_entries[idx]);
            }
        }
        sal_free(var_entries);
        sal_free(p_values);

        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);

    /* Add all variable entries to CLI parse table. */
    for (idx = 0; idx < var_nums; idx++) {
        p_values[idx] = var_entries[idx * 2 + 1];
        bcma_cli_parse_table_add(&pt, var_entries[idx * 2], "str",
                                 &p_values[idx], NULL);
    }

    /* Process the '=' argument */
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        cli_out("%s: Error: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
    }

    /* Update cfgdb variables with parse table entry value. */
    for (idx = 0; idx < var_nums; idx++) {
        bcma_cfgdb_var_set(var_entries[idx * 2], p_values[idx]);
    }

    bcma_cli_parse_table_done(&pt);

    /* Free allocated resources */
    for (idx = 0; idx < 2 * var_nums; idx++) {
        if (var_entries[idx]) {
            sal_free(var_entries[idx]);
        }
    }
    sal_free(var_entries);
    sal_free(p_values);

    return rv;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to show
 * cfgdb variable names and values.
 */
static int
var_entries_show(void *cookie, const char *name, const char *value,
                 uint32_t *tag)
{
    const char *substr = (const char *)cookie;

    COMPILER_REFERENCE(tag);

    if (substr == NULL || sal_strstr(name, substr)) {
        cli_out("\t%s=%s\n", name, value);
    }

    return 0;
}

/*
 * Function to process: config show
 */
static int
config_cmd_show(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *substr = NULL;

    if (BCMA_CLI_ARG_CNT(args) > 0) {
        substr = BCMA_CLI_ARG_GET(args);
    }

    /* Display all variables which contain 'substr' */
    bcma_cfgdb_var_traverse(var_entries_show, (void *)substr);

    return BCMA_CLI_CMD_OK;
}

/*
 * Parsing format <name>=<value>.
 * Return value is TRUE if there existing character '=' in string 's'.
 * 'name' and 'value' should be freed by the caller if returned non NULL.
 */
static int
parse_eq_string(const char *s, char **name, char **value)
{
    char *ptr, *c, *n, *v;

    *name = NULL;
    *value = NULL;

    ptr = sal_strchr(s, '=');
    if (ptr == NULL) {
        return FALSE;
    }

    n = sal_alloc(ptr - s + 1, "bcmaCfgdbParseEqName");
    if (n == NULL) {
        return TRUE;
    }

    c = n;
    while (s != ptr) {
        *c++ = *s++;
    }
    *c = '\0';

    v = sal_strdup(ptr + 1);
    if (v == NULL) {
        sal_free(n);
        return TRUE;
    }

    *name = n;
    *value = v;

    return TRUE;
}

/*
 * Internal function for config_cmd_add and config_cmd_set_existing.
 */
static int
config_set_variable(bcma_cli_t *cli, bcma_cli_args_t *args, int set_existing)
{
    const char *cmd;
    char *name, *value;

    while ((cmd = BCMA_CLI_ARG_GET(args)) != NULL) {
        /* Try config <var>=<value> first */
        if (parse_eq_string(cmd, &name, &value) == FALSE) {
            /* if no '=' sign then try config <var> <value> */
            if (BCMA_CLI_ARG_CNT(args) > 0) {
                name = sal_strdup(cmd);
                value = sal_strdup(BCMA_CLI_ARG_GET(args));
            } else {
                cli_out("%s: Invalid assignment: %s\n",
                        BCMA_CLI_ARG_CMD(args), cmd);
                break;
            }
        }

        if (name == NULL || value == NULL) {
            /* Memory allocation fail for strdup name or value. */
            if (name) {
                sal_free(name);
            }
            if (value) {
                sal_free(value);
            }
            return BCMA_CLI_CMD_FAIL;
        }

        if (set_existing) {
            if (bcma_cfgdb_var_get(name) != NULL) {
                bcma_cfgdb_var_set(name, value);
            } else {
                cli_out("%s: Must use 'add' to create new variable: %s\n",
                        BCMA_CLI_ARG_CMD(args), name);
            }
        } else {
            bcma_cfgdb_var_set(name, value);
        }

        sal_free(name);
        sal_free(value);
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function to process: config add <name>=<value> || config add <name> <value>
 */
static int
config_cmd_add(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return config_set_variable(cli, args, FALSE);
}

/*
 * Function to process: config <name>=<value> || config <name> <value>
 */
static int
config_cmd_set_existing(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return config_set_variable(cli, args, TRUE);
}

/*
 * Transform a pattern string to wildcard string.
 * The return value is the transformed wildcard string from pattern.
 * The returned string should be freed by the caller.
 * If pattern format is 'mem', wildcard string '*mem*' would be returned.
 * If pattern format is '^mem', wildcard string 'mem*' would be returned.
 */
static char *
pattern2wildcard_str(char *pattern)
{
    char *ws, *wc1;
    int size = 3 + sal_strlen(pattern);

    ws = sal_alloc(size, "bcmaCfgdbWildcard");
    if (ws == NULL) {
        return NULL;
    }

    if (pattern[0] == '^') {
        wc1 = "";
        /* Skip leading '^' in pattern. */
        pattern++;
    } else {
        wc1 = "*";
    }

    sal_snprintf(ws, size, "%s%s*", wc1, pattern);

    return ws;
}

/*
 * Function to process: delete [pattern=<substring>] [<var_name>]
 * <var_name> supports wildcard string.
 */
static int
config_cmd_delete(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_cli_parse_table_t pt;
    char *pattern = "";
    const char *var;

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "pattern", "str", &pattern, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Find the variables with "pattern" to delete. */
    if (sal_strlen(pattern) > 0) {
        char *ws = pattern2wildcard_str(pattern);

        bcma_cfgdb_var_set(ws, NULL);

        sal_free(ws);
    }

    bcma_cli_parse_table_done(&pt);

    /* Process the rest of the arguments */
    while ((var = BCMA_CLI_ARG_GET(args)) != NULL) {
        if (bcma_cfgdb_var_set(var, NULL) != 0) {
            cli_out("%s: Variable not found: %s\n",
                    BCMA_CLI_ARG_CMD(args), var);
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Function to process: config clear
 */
static int
config_cmd_clear(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;

    rv = bcma_cfgdb_var_set("*", NULL);

    return (rv == 0) ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

/*
 * Function to process: config refresh
 */
static int
config_cmd_refresh(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    /* Clear all variables. */
    bcma_cfgdb_var_set("*", NULL);

    /* Load variables from file. */
    bcma_cfgdb_load();

    return BCMA_CLI_CMD_OK;
}

/*
 * Function to process: config save
 */
static int
config_cmd_save(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_cli_parse_table_t pt;
    int append_file = 0;
    char *filename = "";
    char *append = "";
    char *pattern = "";

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "filename", "str", &filename, NULL);
    bcma_cli_parse_table_add(&pt, "append", "str", &append, NULL);
    bcma_cli_parse_table_add(&pt, "pattern", "str", &pattern, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp(append, "yes") == 0) {
        append_file = 1;
    }

    /*
     * Save to another file if filename is specified, otherwise flush to
     * orignal loaded file.
     */
    if (sal_strlen(filename)) {
        char *ws = pattern2wildcard_str(pattern);

        bcma_cfgdb_save(filename, ws, append_file);

        sal_free(ws);
    } else {
        bcma_cfgdb_flush();
    }

    bcma_cli_parse_table_done(&pt);

    return BCMA_CLI_CMD_OK;
}

/*
 * Call-back function of bcma_cfgdb_var_traverse to show cfgdb variable names and values
 */
static int
var_entries_dump(void *cookie, const char *name, const char *value,
                 uint32_t *tag)
{
    int *import_nums = (int *)cookie;

    if (*tag & BCMA_CFGDB_TAG_FLAG_IMPORTED) {
        *import_nums = *import_nums + 1;
    }

    cli_out("line:%4d, flag: %s%s%s, %s=%s\n",
            BCMA_CFGDB_TAG_LINE_NUM_GET(*tag),
            (*tag & BCMA_CFGDB_TAG_FLAG_IMPORTED) ? "I " : "- ",
            (*tag & BCMA_CFGDB_TAG_FLAG_FLUSHED) ? "F " : "- ",
            (*tag & BCMA_CFGDB_TAG_FLAG_HIT) ? "H " : "- ",
            name,
            value);

    return 0;
}

/*
 * Function to process: config dump
 */
static int
config_cmd_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char * filename;
    int import_nums = 0;

    bcma_cfgdb_file_name_get(&filename, NULL);

    bcma_cfgdb_var_traverse(var_entries_dump, &import_nums);

    cli_out("\nConfig file:\n\t%s\n", filename);

    cli_out("\nNumber of imported values from %s:\n\t%4d\n",
            filename, import_nums);

    return BCMA_CLI_CMD_OK;
}

static bcma_cli_command_t config_cmds[] = {
    {"show", config_cmd_show},
    {"add", config_cmd_add},
    {"delete", config_cmd_delete},
    {"clear", config_cmd_clear},
    {"refresh", config_cmd_refresh},
    {"save", config_cmd_save},
    {"dump", config_cmd_dump}
};

int
bcma_cfgdbcmd_config(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *cmd;
    int idx;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    cmd = BCMA_CLI_ARG_CUR(args);
    if (cmd == NULL) {
        /* Print all config variables */
        return config_cmd_show(cli, args);
    }

    if (sal_strcmp(cmd, "=") == 0) {
        return config_cmd_prompt(cli, args);
    }

    for (idx = 0; idx < COUNTOF(config_cmds); idx++) {
        if (sal_strcasecmp(config_cmds[idx].name, cmd) == 0) {
            BCMA_CLI_ARG_NEXT(args);
            return (*config_cmds[idx].func)(cli, args);
        }
    }

    /* No matching config paramters, try <var>=<value> format */
    return config_cmd_set_existing(cli, args);
}
