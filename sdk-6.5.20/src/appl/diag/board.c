/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        board.c
 * Purpose:     Board driver CLI
 */

#include <sal/types.h>
#if !defined(NO_SAL_APPL)
#include <sal/appl/config.h>
#endif
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/ctrans.h>
#include <board/board.h>

#include <shared/bsl.h>

#if defined(INCLUDE_BOARD)

char board_cmd_usage[] =
    "board <subcmd> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Subcommands:\n"
    "    select [name=boardname] [flags=startflags]\n"
    "        Attach and start a board.\n"
    "        If no board name is provided, probe all registered board\n"
    "        drivers and select the first board driver that probes\n"
    "        successfully.\n"
    "        Start flags default to 0 (cold boot). Start flags can be\n"
    "        numeric, or one of 'Cold', 'Warm', or 'CLear'\n"
    "    detach\n"
    "        Stop and detach the current board.\n"
    "    list\n"
    "        Display all registered boards and attributes supported by\n"
    "        each board.\n"
    "    show\n"
    "        Display current board state and configuration name,\n"
    "        description, attributes, and connections.\n"
    "    register builtin\n"
    "        Register or re-register all built in drivers.\n"
    "    register Name=boardname\n"
    "        Register or re-register a built in driver with the given name.\n"
    "    unregister Name=boardname\n"
    "        Unregister a driver with the given name.\n"
    "    attribute name=val\n"
    "        Set the board attribute 'name' to 'value'.\n"
    "    attribute name\n"
    "        Display the current value of board attribute 'name'.\n"
    "    attribute\n"
    "        Display all board attributes supported by the current board.\n"
    "    clear\n"
    "        Clear devices on current board and preserve the current board\n"
    "        configuration.\n"
#endif
    ;

/* Macro to distinguish between NULL and empty strings */
#define S(s) ((s)?(strlen((s))?(s):"<EMPTY>"):"<NULL>")

/* Macro to convert BCM errors to CMD shell errors */
#define CMD_SUCCESS(x) (BCM_SUCCESS(x) ? CMD_OK : CMD_FAIL)

/* Position in board_cmd_flag_list where magic numeric flags land up */
#define NUM_FLAG_OFFSET 3

/* Decimal number string buffer length */
#define STRBUF 16

/* Board start string parameters */
STATIC char *board_cmd_flag_list[] = {
    "Cold",
    "Warm",
    "CLear",
    NULL, /* NUM_FLAG string */
    NULL
};

/* Flag values corresponding to the strings above in
   board_cmd_flag_list. This would normally have been combined with
   board_cmd_flag_list in a single structure, but such a structure
   would work with diag parser functions. */
STATIC uint32 board_cmd_flag_value[] = {
    BOARD_START_F_COLD_BOOT,
    BOARD_START_F_WARM_BOOT,
    BOARD_START_F_CLEAR,
    0 /* NUM_FLAG value */
};

/*
 * Function:
 *     board_cmd_parse_numeric_start_flag
 * Purpose:
 *     If there's a numeric start flag, add it to the flag list
 * Parameters:
 *     args  - (IN) argument structure to scan
 * Returns:
 *     void
 */
STATIC
void board_cmd_parse_numeric_start_flag(args_t *args)
{
    char *arg;
    char *value;
    int count;

    count=0;
    board_cmd_flag_list[NUM_FLAG_OFFSET] = NULL; /* remove any previous flag */
    while (ARG_CNT(args) > 0) {
        arg = ARG_CUR(args);

        /* flag format is f...=value */
        if (arg != NULL && (arg[0] == 'f' || arg[0] == 'F')) {
            value = sal_strchr(arg, '=');
            if (value) {
                value++;
                if (isint(value)) {
                    board_cmd_flag_list[NUM_FLAG_OFFSET] = value;
                    board_cmd_flag_value[NUM_FLAG_OFFSET] =
                        parse_integer(value);
                    break;
                }
            }
        }
        count++;
        ARG_NEXT(args);
    }
    /* put arg count back */
    while (count-- > 0) {
        ARG_PREV(args);
    }
}

/* Returns a string corresponding to the r/w access of a board attribute */
#define ATTR_ACCESS(a) ((a) ? "rw" : "ro")

/*
 * Function:
 *     board_cmd_list
 * Purpose:
 *     List names and attributes of all installed board drivers.
 * Parameters:
 *     none
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_list(void)
{
    board_driver_t **driver;
    int rv, count, i, j;

    rv = board_drivers_get(0, NULL, &count);

    if (BCM_FAILURE(rv)) {
        cli_out("board_drivers_get: %s (%d)\n", bcm_errmsg(rv), rv);
        return CMD_FAIL;
    }

    driver = sal_alloc(count * sizeof(board_driver_t *), "board_cmd");

    if (!driver) {
        return CMD_FAIL;
    }

    rv = board_drivers_get(count, driver, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_drivers_get: %s (%d)\n", bcm_errmsg(rv), rv);
        sal_free(driver);
        return CMD_FAIL;
    }

    for (i=0; i<count; i++) {

        cli_out("%s\n", driver[i]->name);

        for (j=0; j<driver[i]->num_attribute; j++) {
            cli_out("  %s [%s]\n",
                    driver[i]->attribute[j].name,
                    ATTR_ACCESS(driver[i]->attribute[j].set));
        }
    }

    sal_free(driver);
    return CMD_OK;
}

/*
 * Function:
 *     board_cmd_start
 * Purpose:
 *     Start a board based on parameters passed or config variables.
 *     If a name is given, use that name, otherwise if there's a
 *     "board_name" config variable, use that. When all else fails,
 *     probe for a board.  If the probe fails, then error out,
 *     otherwise attach the board.  Once the board is attached, start
 *     the board with the given board flags, the board flags config,
 *     or the default cold start flag. If the board start was
 *     successful, set board attributes if any attribute configs were
 *     found.
 * Parameters:
 *     name    - (IN) board name to start, or NULL to probe
 *     flagidx - (IN) start flag
 * Configs:
 *     board_name
 *     board_flags
 *     board_<attr>
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_start(char *name, int flagidx)
{
    int rv, cmd_rv, num_attr, i;
    uint32 flags = 0;
    board_attribute_t *attr;
#if !defined(NO_SAL_APPL)
    char config_name[SAL_CONFIG_STR_MAX];
#else
    char config_name[128];
#endif

    cmd_rv = CMD_FAIL;
    attr = NULL;

#if !defined(NO_SAL_APPL)
    if (name == NULL || sal_strlen(name) == 0) {
        /* if there's a config board_name, use that name */
        name = sal_config_get(spn_BOARD_NAME);
    }
#endif

    if (name == NULL || sal_strlen(name) == 0) {
        /* probe */
        name = board_probe();
        if (name == NULL) {
            cli_out("Board probe failed\n");
            goto fail;
        }
    } else {
        /* find */
        if (!board_find(name)) {
            cli_out("Cannot find board '%s'\n", name);
            goto fail;
        }
    }

    /* attach */
    rv = board_attach(name);
    if (BCM_FAILURE(rv)) {
        cli_out("Cannot attach board '%s':%s (%d)\n",
                name, bcm_errmsg(rv), rv);
        goto fail;
    }

    /* start */
    if (flagidx < 0) {
        char *flagstr = NULL;

#if !defined(NO_SAL_APPL)
        flagstr = sal_config_get(spn_BOARD_FLAGS);
#endif
        /* if there's a board_flags argument, use that, otherwise
           default to 'cold boot' */

        if (flagstr) {
            if (isint(flagstr)) {
                flags = parse_integer(flagstr);
            } else {
                cli_out("Could not parse board_flags=%s\n",flagstr);
                goto fail;
            }
        } else {
            flags = board_cmd_flag_value[0];
        }
    }

    rv = board_start(flags);
    if (BCM_FAILURE(rv)) {
        cli_out("Cannot start %s(%08x):%s (%d)\n",
                name, flags, bcm_errmsg(rv), rv);
        board_detach();
        goto fail;
    }

    /* Go though the writable attributes, and see if there are
       any matching configs. If there are, then set those
       attributes */

    rv = board_attributes_get(0, NULL, &num_attr);
    if (BCM_FAILURE(rv)) {
        cli_out("Cannot get board attributes for %s:%s (%d)\n",
                name, bcm_errmsg(rv), rv);
        goto fail;
    }

    attr = sal_alloc(num_attr * sizeof(board_attribute_t),
                       "board_cmd_start");

    if (attr == NULL) {
        cli_out("Cannot alloc memory for attributes\n");
        goto fail;
    }

    rv = board_attributes_get(num_attr, attr, &num_attr);
    if (BCM_FAILURE(rv)) {
        cli_out("Cannot get board attributes for %s:%s (%d)\n",
                name, bcm_errmsg(rv), rv);
        goto fail;
    }

    /* Set board attributes if matching config variables are found.
       Set variables for all attributes.
    */
    for (i=0; i<num_attr; i++) {
        sal_sprintf(config_name, "board_%s", attr[i].name);
#if !defined(NO_SAL_APPL)
        if (attr[i].set) {
            char *value;

            value = sal_config_get(config_name);
            if (value) {
                if (isint(value)) {
                    rv = board_attribute_set(config_name,
                                             parse_integer(value));
                    if (BCM_FAILURE(rv)) {
                        cli_out("Cannot set board attribute %s=%s:%s (%d)\n",
                                config_name, value, bcm_errmsg(rv), rv);
                        goto fail;
                    }
                } else {
                    cli_out("Could not parse %s=%s\n",
                            config_name, value);
                    goto fail;
                }
            }
        }
#endif
        if (attr[i].get) {
            int int_value;
            char int_value_str[STRBUF];

            rv = board_attribute_get(config_name, &int_value);
            if (BCM_SUCCESS(rv)) {
                sal_sprintf(int_value_str, "%d", int_value);
                if (var_set(config_name, int_value_str, FALSE, FALSE) < 0) {
                    goto fail;
                }
            }
        }
    }

    /* Set board variables */
    if (var_set("board_name", board_name(), FALSE, FALSE) < 0) {
        goto fail;
    }

    if (var_set(board_name(), "1", FALSE, FALSE) < 0) {
        goto fail;
    }

    cmd_rv = CMD_OK;

 fail:
    if (attr) {
        sal_free(attr);
    }

    return cmd_rv;
}

/*
 * Function:
 *     board_cmd_register
 * Purpose:
 *     Register a builtin driver of the given name
 * Parameters:
 *      name - (IN) board driver to register
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_register(char *name)
{
    board_driver_t **driver, *add;
    int count, i;
    int rv, cmd_rv = CMD_FAIL;

    driver = NULL;

    rv = board_driver_builtins_get(0, NULL, &count);

    if (BCM_FAILURE(rv)) {
        cli_out("board_driver_builtins_get: %s (%d)\n", bcm_errmsg(rv), rv);
        goto fail;
    }

    driver = sal_alloc(count * sizeof(board_driver_t *), "board_cmd");

    if (!driver) {
        cli_out("Out of memory\n");
        goto fail;
    }

    rv = board_driver_builtins_get(count, driver, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_driver_builtins_get: %s (%d)\n", bcm_errmsg(rv), rv);
        goto fail;
    }

    add = NULL;
    for (i=0; i<count; i++) {

        if (!sal_strcmp(name, driver[i]->name)) {
            add = driver[i];
            break;
        }
    }

    if (add) {
        int rv;

        rv = board_driver_add(add);
        if (BCM_SUCCESS(rv)) {
            cmd_rv = CMD_OK;
        } else {
            cli_out("Cannot add board driver %s:%s (%d)\n",
                    name, bcm_errmsg(rv), rv);
        }
    } else {
        cli_out("Board driver %s not found\n", name);
    }

 fail:
    if (driver) {
        sal_free(driver);
    }

    return cmd_rv;
}

/*
 * Function:
 *     board_cmd_unregister
 * Purpose:
 *     Register a driver of the given name
 * Parameters:
 *     name - (IN) board driver to unregister
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_unregister(char *name)
{
    board_driver_t **driver, *del;
    int count, i;
    int rv, cmd_rv = CMD_FAIL;

    rv = board_drivers_get(0, NULL, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_drivers_get: %s (%d)\n", bcm_errmsg(rv), rv);
        return CMD_FAIL;
    }

    driver = sal_alloc(count * sizeof(board_driver_t *), "board_cmd");

    if (!driver) {
        cli_out("Out of memory\n");
        return CMD_FAIL;
    }

    rv = board_drivers_get(count, driver, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_drivers_get: %s (%d)\n", bcm_errmsg(rv), rv);
        sal_free(driver);
        return CMD_FAIL;
    }

    del = NULL;
    for (i=0; i<count; i++) {

        if (!sal_strcmp(name, driver[i]->name)) {
            del = driver[i];
            break;
        }

    }
    if (del) {
        int rv;

        rv = board_driver_delete(del);
        if (BCM_SUCCESS(rv)) {
            cmd_rv = CMD_OK;
        } else {
            cli_out("Cannot delete board driver %s:%s (%d)\n",
                    name, bcm_errmsg(rv), rv);
        }
    } else {
        cli_out("Board driver %s not registered\n", name);
    }

    sal_free(driver);
    return cmd_rv;
}

/*
 * Function:
 *     board_cmd_attr_print
 * Purpose:
 *     Print short or long information about a board attribute
 * Parameters:
 *      attr - (IN) attribute to print
 *      full - (IN) TRUE if full information is to be printed
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - failed
 */
STATIC int
board_cmd_attr_print(board_attribute_t *attr, int full)
{
    int value, rv;
    char valstr[STRBUF], *spacer, *units;

    rv = board_attribute_get(attr->name, &value);

    if (BCM_SUCCESS(rv)) {

        /* print according to attribute format */
        sal_sprintf(valstr, attr->format ? attr->format : "%d", value);

        if (full) {
            /* get units if applicable */
            if (attr->units) {
                spacer = " ";
                units = attr->units;
            } else {
                spacer = units = "";
            }

            /* display attribute */
            cli_out("  %-10s [%s]=%s%s%s\n",
                    attr->name,
                    ATTR_ACCESS(attr->set),
                    valstr,
                    spacer,
                    units);
        } else {
            cli_out("%s\n", valstr);
        }
    } else {
        cli_out("  %s: %s (%d)\n", attr->name, bcm_errmsg(rv), rv);
    }

    return rv;
}

/*
 * Function:
 *     board_cmd_attribute_list
 * Purpose:
 *     Print arributes for the current board
 * Parameters:
 *     name - (IN) Specific attribute to print, or NULL for all attributes
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_attribute_list(char *name)
{
    board_attribute_t *attr;
    int rv, count, i;

    rv = board_attributes_get(0, NULL, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_attributes_get: %s (%d)\n", bcm_errmsg(rv), rv);
        return CMD_FAIL;
    }

    attr = sal_alloc(count * sizeof(board_attribute_t), "board_cmd");

    if (!attr) {
        cli_out("Out of memory\n");
        return CMD_FAIL;
    }

    rv = board_attributes_get(count, attr, &count);
    if (BCM_FAILURE(rv)) {
        cli_out("board_attributes_get: %s (%d)\n", bcm_errmsg(rv), rv);
        sal_free(attr);
        return CMD_FAIL;
    }

    for (i=0; i<count; i++) {
        if (name == NULL || !sal_strcmp(name, attr[i].name)) {
            board_cmd_attr_print(&attr[i], TRUE);
        }
    }

    sal_free(attr);
    return CMD_OK;
}

/*
 * Function:
 *     board_cmd_attribute_set
 * Purpose:
 *     Set the attribute of the given name to the given value
 * Parameters:
 *     name  - (IN) attribute name
 *     value - (IN) value
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_attribute_set(char *name, char *value)
{
    if (isint(value)) {
        return CMD_SUCCESS(board_attribute_set(name, parse_integer(value)));
    }

    return CMD_FAIL;
}

#define MAX_CALL 4
#define MAX_NAME 16

/*
 * Function:
 *     board_cmd_devport_name
 * Purpose:
 *     Pretty print a gport DEVPORT name into a circular buffer. This
 *     intended to allow multiple calls to this function from printfs.
 * Parameters:
 *      gport - (IN) gport to print
 * Returns:
 *     static string name of port
 */
STATIC char *
board_cmd_devport_name(bcm_gport_t gport)
{
    int unit;
    bcm_port_t port;
    char *s;
    static char name[MAX_CALL][MAX_NAME];
    static int idx;

    unit = BCM_GPORT_DEVPORT_DEVID_GET(gport);
    port = BCM_GPORT_DEVPORT_PORT_GET(gport);
    s = name[idx++];
    if (idx >= MAX_CALL) {
        idx = 0;
    }
    if (bcm_unit_valid(unit) && port >= 0) {
        sal_sprintf(s, "%d:%s", unit, bcm_port_name(unit, port));
    } else {
        sal_sprintf(s, "%08x", gport);
    }
    return s;
}

/*
 * Function:
 *     board_cmd_connection_print
 * Purpose:
 *     Pretty print a board connection
 * Parameters:
 *     connection  - (IN) connection to print
 * Returns:
 *     void
 */
STATIC void
board_cmd_connection_print(board_connection_t *connection)
{
    if (connection->to != BCM_GPORT_TYPE_NONE) {
        cli_out("  %8s -> %-8s\n",
                board_cmd_devport_name(connection->from),
                board_cmd_devport_name(connection->to));
    } else {
        cli_out("  %s [stack]\n", board_cmd_devport_name(connection->from));
    }
}

/*
 * Function:
 *     board_cmd_connection_list
 * Purpose:
 *     List all connections for the current board
 * Parameters:
 *     none
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_connection_list(void)
{
    board_connection_t *conn;
    int count, i;

    if (BCM_FAILURE(board_connections_get(0, NULL, &count))) {
        return CMD_FAIL;
    }

    conn = sal_alloc(count * sizeof(board_connection_t), "board_cmd");

    if (!conn) {
        return CMD_FAIL;
    }

    if (BCM_FAILURE(board_connections_get(count, conn, &count))) {
        sal_free(conn);
        return CMD_FAIL;
    }

    for (i=0; i<count; i++) {
        board_cmd_connection_print(&conn[i]);
    }

    sal_free(conn);
    return CMD_OK;
}

/*
 * Function:
 *     board_cmd_show
 * Purpose:
 *     Print all information about the current board
 * Parameters:
 *     none
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
STATIC cmd_result_t
board_cmd_show(void)
{
    cli_out("Name:\n  %s\n", S(board_name()));
    cli_out("Description:\n  %s\n", S(board_description()));
    cli_out("Attributes:\n");
    board_cmd_attribute_list(NULL);
    cli_out("Connections:\n");
    board_cmd_connection_list();
    return CMD_OK;
}


/*
 * Function:
 *     board_cmd
 * Purpose:
 *     Board Diag shell command handler
 * Parameters:
 *      unit - (IN) current diag shell unit
 *      args - arglist
 * Returns:
 *     CMD_OK - success
 *     CMD_FAIL - failed
 */
cmd_result_t
board_cmd(int unit, args_t *args)
{
    char *subcmd;
    int sublen;
    int rv, cmd_rv;
    static int init;
    parse_table_t pt;

    if (!init) {
        rv = board_init();
        if (BCM_FAILURE(rv)) {
            cli_out("board_init failed:%s (%d)\n", bcm_errmsg(rv), rv);
            return CMD_FAIL;
        }
        rv = board_driver_builtins_add();
        if (BCM_FAILURE(rv)) {
            cli_out("board_driver_builtins_add failed:%s (%d)\n",
                    bcm_errmsg(rv), rv);
            return CMD_FAIL;
        }
        init=TRUE;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return board_cmd_show();
    }

    sublen=sal_strlen(subcmd);
    if (sal_strncasecmp(subcmd, "show", sublen) == 0) {
        return board_cmd_show();

    } else if (sal_strncasecmp(subcmd, "select", sublen) == 0) {
        char *name;
        int  flagidx;

        name=NULL;
        flagidx=-1;
        /* if flags are numeric, add to flag multi list */

        if (ARG_CNT(args) > 0) {
            board_cmd_parse_numeric_start_flag(args);
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Name", PQ_STRING, 0,  &name, 0);
            parse_table_add(&pt, "Flags", PQ_DFL | PQ_MULTI, 0, &flagidx,
                            board_cmd_flag_list);

            if (parse_arg_eq(args, &pt)) {
                cli_out("select name:%s flag:%08x\n",
                        S(name), board_cmd_flag_value[flagidx]);
            } else {
                return CMD_USAGE;
            }
        } else {
            name = "";
        }
        return board_cmd_start(name, flagidx);

    } else if (sal_strncasecmp(subcmd, "detach", sublen) == 0) {
        cli_out("detach\n");
        rv = board_detach();
        return CMD_SUCCESS(rv);

    } else if (sal_strncasecmp(subcmd, "list", sublen) == 0) {
        return board_cmd_list();
    } else if (sal_strncasecmp(subcmd, "register", sublen) == 0) {
        char *builtin;

        builtin = ARG_CUR(args);
        if (builtin != NULL &&
            sal_strncasecmp(builtin, "builtin", sal_strlen(builtin)) == 0) {
            ARG_NEXT(args);
            cli_out("register builtin\n");
            rv = board_driver_builtins_add();
            return CMD_SUCCESS(rv);
        } else {
            char *name;

            name=NULL;
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Name", PQ_STRING, 0,  &name, 0);
            if (parse_arg_eq(args, &pt)) {
                cmd_rv = board_cmd_register(name);
            } else {
                cmd_rv = CMD_USAGE;
            }
        }
        return cmd_rv;
    } else if (sal_strncasecmp(subcmd, "unregister", sublen) == 0) {
        char *name;

        name=NULL;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Name", PQ_STRING, 0,  &name, 0);
        if (parse_arg_eq(args, &pt)) {
            cmd_rv = board_cmd_unregister(name);
        } else {
            cmd_rv = CMD_USAGE;
        }
        return cmd_rv;
    } else if (sal_strncasecmp(subcmd, "attribute", sublen) == 0) {
        char *name, *value;

        name = ARG_CUR(args);
        if (name == NULL) {
            cmd_rv = board_cmd_attribute_list(NULL);
        } else {
            value = sal_strchr(name, '=');

            if (!value) {
                board_cmd_attribute_list(name);
            } else {
                *value='\0';
                value++;
                board_cmd_attribute_set(name, value);
            }
            ARG_NEXT(args);
        }
        return CMD_OK;
    } else if (sal_strncasecmp(subcmd, "clear", sublen) == 0) {
        cli_out("clear\n");
        return CMD_OK;
    } else {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* INCLUDE_BAORD */
