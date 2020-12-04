/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System configuration command.
 */

#ifndef NO_SAL_APPL

#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <sal/appl/config.h>
#include <shared/bsl.h>

#define ENV_CONFIG_VAR_VAL "config_var_val"
#define ENV_CONFIG_VAR_NOT_SET "config_var_not_set"

char sh_config_usage[] =
    "Parameters: show <substring> | dump | refresh |  \n\t"
    "                 save [filename=<filename>] [append=yes] [pattern=<substring>] | \n\t"
    "                 add [<var>=<value>] | \n\t"
    "                 delete [pattern=<substring>] |<var>\n\t"
    "If no parameters are given, displays the current config vars.\n\t"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  show                display current config vars. Next parameter (optional) maybe a substring to match\n\t"
    "  dump                display current config vars imported from config.bcm and used(hit) by SDK.\n\t"
    "                      e.g., Hash, Line #, Flag(Compiled/Flushed/Hit), Vars Name=Value \n\t"
    "  refresh             reload config vars from non-volatile storage\n\t"
    "  save                save config vars to non-volatile storage.\n\t"
    "                      it can optionally save current config to any given file \n\t"
    "                      providing the optional <pattern> will only save variables matching the pattern\n\t"
    "  append              append config vars to the end of the file.\n\t"
    "                      don\'t replace the old one.\n\t"
    "  <var>=<value>       change the value of a config var\n\t"
    "  add <var>=<value>   create a new config var\n\t"
    "  get <var>           print the value of a config var\n\t"
    "  delete <var>        deletes a config var\n\t"
    "  clear               deletes all config vars\n\t"
    "  =                   prompt for new value for each config var\n\t"
#endif
    "NOTE: changes are not retained permanently unless saved\n";

cmd_result_t
sh_config(int u, args_t *a)
/*
 * Function: 	sh_config
 * Purpose:	Configure switch management information.
 * Parameters:
 * Returns:
 */
{
    parse_table_t    pt;
    char        *name, *value;
    char        **parsed_value;
    int         i, rv, nconfig, add, get = 0;
    char        *c = ARG_CUR(a);
    char        *substr;
    char        *new_fname = NULL;
    char        *pattern = NULL;
    char        *append = NULL;
    char        *config_var = NULL;
    char        *var_val = NULL;


    rv = CMD_OK;

    if (c == NULL || !sal_strcasecmp(c, "show")) {
        name = NULL;
        ARG_NEXT(a);
        substr = ARG_CUR(a);
        while (sal_config_get_next(&name, &value) >= 0) {
            if ((substr == NULL) || (sal_strstr(name, substr) != NULL)) {
                cli_out("    %s=%s\n", name, value);
            }
        }
        if (NULL != c) {
            ARG_NEXT(a);
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "dump")) {
        ARG_NEXT(a);
        sal_config_dump();
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "refresh")) {
        ARG_NEXT(a);
        cli_out("%s: Refreshing configuration database\n", ARG_CMD(a));
        if (sal_config_refresh()) {
            cli_out("%s: Failed to refresh configuration database\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "save")) {
        parse_table_t    pt_save;
        parse_table_init(u, &pt_save);
        parse_table_add(&pt_save, "filename", PQ_STRING, NULL,
                                (void *)&new_fname, NULL);
        parse_table_add(&pt_save, "append", PQ_STRING, NULL,
                                (void *)&append, NULL);
        parse_table_add(&pt_save, "pattern", PQ_STRING, NULL,
                                (void *)&pattern, NULL);
        ARG_NEXT(a);
        if (0 > parse_arg_eq(a, &pt_save)) {
            parse_arg_eq_done(&pt_save);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt_save);
            return CMD_USAGE;
        }
        if (sal_strlen(new_fname)) {
            rv = sal_config_save(new_fname, pattern, sal_strcasecmp(append, "yes") ? 0 : 1);
        } else {
            rv = sal_config_flush();
        }
        parse_arg_eq_done(&pt_save);

        if (rv) {
            cli_out("%s: Warning: sal_config_flush failed\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "delete")) {
        parse_table_t    pt_delete;
        parse_table_init(u, &pt_delete);
        parse_table_add(&pt_delete, "pattern", PQ_STRING, NULL,
                                (void *)&pattern, NULL);
        ARG_NEXT(a);
        if (0 > parse_arg_eq(a, &pt_delete)) {
            parse_arg_eq_done(&pt_delete);
            return CMD_USAGE;
        }

        if (sal_strlen(pattern)) {
            name = NULL;
            for(;;) {
                 if (sal_config_get_next(&name, &value) < 0) {
                     break; 
                 }
                 if ((sal_strstr(name, pattern) != NULL)) {
                    LOG_VERBOSE(BSL_LS_APPL_SHELL, (BSL_META_U(u,
                                "Pattern(%s) Deleting %s=%s\n"),
                                pattern, name, value));
                    sal_config_set(name, 0);
                    name = NULL;
                 }
            }
        } 
        parse_arg_eq_done(&pt_delete);

        while ((c = ARG_GET(a)) != NULL) {
            if (sal_config_set(c, 0) != 0) {
                cli_out("%s: Variable not found: %s\n", ARG_CMD(a), c);
                rv = CMD_FAIL;
            }
        }
        return rv;
    }

    if (!sal_strcasecmp(c, "clear")) {
        ARG_NEXT(a);
        for (;;) {
            name = NULL;
            if (sal_config_get_next(&name, &value) < 0) {
                break;
            }
            if (sal_config_set(name, 0) < 0) {
                cli_out("%s: Variable not found: %s\n", ARG_CMD(a), name);
                rv = CMD_FAIL;
                break;
            }
        }
        return rv;
    }

    if (!sal_strcasecmp(c, "add")) {
        ARG_NEXT(a);
        add = 1;
    } else {
        add = 0;
    }

    if (!sal_strcasecmp(c, "get")) {
        ARG_NEXT(a);
        config_var = ARG_CUR(a);
        if (config_var == NULL) {
            cli_out("config variable cannot be empty!!\n");
            return CMD_USAGE;
        }
        get = 1;
    } else {
        get = 0;
    }

    /* count config variables first, to allocate parsed_value array */
    name = NULL;
    nconfig = 0;
    while (sal_config_get_next(&name, &value) >= 0) {
        nconfig += 1;
        /*
         * "config get <config_var> was issued.
         */
        if (get) {
            if (!sal_strcasecmp(config_var, name)) {
                if ((var_val = sal_config_get(config_var)) != NULL) {
                    /*
                     *  <config_var> found, et the env variables
                     *  "config_var_val" to the value in the config.
                     */
                    var_set_integer(ENV_CONFIG_VAR_VAL, 
                                    parse_integer(var_val), TRUE, FALSE);
                    var_set_integer(ENV_CONFIG_VAR_NOT_SET, 0, TRUE, FALSE);
                    cli_out("Environment variable (%s) was set\n", 
                           ENV_CONFIG_VAR_VAL);
                    ARG_DISCARD(a);
                    return  CMD_OK;
                }
            }
        }
    }

    /*
     *  if the shell command "config get <config_var>" was
     *  issued and code execution has reached here, means the
     *  the config_var is not set. Set the env variables
     *  "config_var_val" to 0 and "config_var_not_set to 0
     */
    if (get) {
        var_set_integer(ENV_CONFIG_VAR_VAL, 0, TRUE, FALSE);
        var_set_integer(ENV_CONFIG_VAR_NOT_SET, 1, TRUE, FALSE);
        cli_out("Environment variables (%s) (%s) was set\n", 
               ENV_CONFIG_VAR_VAL, ENV_CONFIG_VAR_NOT_SET);
        ARG_DISCARD(a);
        return CMD_OK;
    }

    if (nconfig) {
        parsed_value = sal_alloc(sizeof(*parsed_value) * nconfig,
                                 "config values");
        if (parsed_value == NULL) {
            cli_out("%s: cannot allocate memory for config values\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        }
        sal_memset(parsed_value, 0, sizeof(*parsed_value) * nconfig);

        name = NULL;
        parse_table_init(u, &pt);
        i = 0;
        while (sal_config_get_next(&name, &value) >= 0) {
            if (parse_table_add(&pt, name, PQ_STRING, value,
                                (void *)&parsed_value[i], NULL) < 0) {
                cli_out("Internal error in parsing\n");
                sal_free(parsed_value);
                return CMD_FAIL;
            }
            i += 1;
        }

        /* Set cmd_flag before calling parse_arg_eq() */
        pt.cmd_flag = (add == 1) ? CMD_ADD : CMD_DC;

        if (0 > parse_arg_eq(a, &pt)) {
            cli_out("%s: Invalid option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            sal_free(parsed_value);
            return CMD_USAGE;
        }

        /* For each that is modified, set new value */

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                if (sal_config_set(pt.pt_entries[i].pq_s,
                                   parsed_value[i]) < 0) {
                    rv = CMD_FAIL;
                }
            }
        }

        parse_arg_eq_done(&pt);
        if (parsed_value) {
            sal_free(parsed_value);
        }
    }

    if (ARG_CNT(a)) {
        while ((c = ARG_GET(a)) != NULL) {
            char *s;
            s = sal_strchr(c, '=');
            if (s == NULL) {
                cli_out("%s: Invalid assignment: %s\n", ARG_CMD(a), c);
                rv = CMD_FAIL;
            } else if (! add) {
                cli_out("%s: Must use 'add' to create new variable: %s\n",
                        ARG_CMD(a), c);
                rv = CMD_FAIL;
            } else {
                *s++ = 0;
                if (sal_config_set(c, s) < 0) {
                    rv = CMD_FAIL;
                }
            }
        }
    }

    return rv;
}

#endif /* NO_SAL_APPL */
