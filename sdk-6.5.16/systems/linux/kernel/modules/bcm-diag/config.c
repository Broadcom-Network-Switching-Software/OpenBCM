/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * System configuration command.
 */

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <kconfig.h>

char sh_config_usage[] =
    "Parameters: show | refresh | save | [add] [<var>=<value>] | delete <var>\n\t"
    "If no parameters are given, displays the current config vars.\n\t"
    "  show                display current config vars\n\t"
    "  <var>=<value>       change the value of a config var\n\t"
    "  add <var>=<value>   create a new config var\n\t"
    "  delete <var>        deletes a config var\n\t"
    "  =                   prompt for new value for each config var\n\t"
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
    int	rv, add;
    char *name, *value, *c = ARG_CUR(a);

    rv = CMD_OK;

    if (c == NULL || !sal_strcasecmp(c, "show")) {
        name = NULL;
        while (kconfig_get_next(&name, &value) >= 0) {
            sal_printf("\t%s=%s\n", name, value);
        }
        if (NULL != c) {
            ARG_NEXT(a);
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "delete")) {
	ARG_NEXT(a);
	while ((c = ARG_GET(a)) != NULL) {
	    if (kconfig_set(c, 0) != 0) {
		printk("%s: Variable not found: %s\n", ARG_CMD(a), c);
		rv = CMD_FAIL;
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

    if (ARG_CNT(a)) {
	while ((c = ARG_GET(a)) != NULL) {
	    char *s;
	    s = strchr(c, '=');
	    if (s == NULL) {
		printk("%s: Invalid assignment: %s\n", ARG_CMD(a), c);
		rv = CMD_FAIL;
	    } else {
                *s++ = 0;
                if (!add && kconfig_get(c) == NULL) {
                    printk("%s: Must use 'add' to create new variable: %s\n",
                           ARG_CMD(a), c);
                    rv = CMD_FAIL;
                } else {
                    if (kconfig_set(c, s) < 0) {
                        rv = CMD_FAIL;
                    }
		}
	    }
	}
    }

    return rv;
}
