/*! \file bcma_cli_parse_data.c
 *
 * <description>
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

/* List of parse data drivers */
static bcma_cli_parse_data_t *parse_data_root;


/* Parse data driver for type "bool" */

static int
bool_parse_arg(const char *argstr, void *data, void **option)
{
    struct boolean_s {
        char *name;
        int value;
    };
    static struct boolean_s boolean_table[] = {
        {"Yes", TRUE},          {"OKay", TRUE},
        {"YOUBET", TRUE},       {"True", TRUE},
        {"1", TRUE},            {"0", FALSE},
        {"ON", TRUE},           {"OFF", FALSE},
        {"No", FALSE},          {"NOWay",FALSE},
        {"False", FALSE},       {"YEAH", TRUE},
        {"YEP", TRUE},          {"NOPE", FALSE},
        {"NOT", FALSE},
    };
    int cnt = COUNTOF(boolean_table);
    const struct boolean_s *bs = boolean_table;

    while (cnt--) {
        if (bcma_cli_parse_cmp(bs->name, argstr, 0)) {
            *((int *)data) = bs->value;
            return 0;
        }
        bs++;
    }
    return -1;
}

static char *
bool_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    len = sal_snprintf(buf, bufsz, "%s", val ? "True" : "False");

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_bool = {
    "bool",
    bool_parse_arg,
    bool_format_arg,
    NULL
};

/* Parse data driver for type "int" */

static int
int_parse_arg(const char *argstr, void *data, void **option)
{
    return bcma_cli_parse_int(argstr, (int *)data);
}

static char *
int_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    len = sal_snprintf(buf, bufsz, "%d", val);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_int = {
    "int",
    int_parse_arg,
    int_format_arg,
    NULL
};

/* Parse data driver for type "hex" */

static char *
hex_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *((int *)data);
    int len;

    if (val <= 9) {
        len = sal_snprintf(buf, bufsz, "%d", val);
    } else {
        len = sal_snprintf(buf, bufsz, "0x%x", val);
    }

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_hex = {
    "hex",
    int_parse_arg,
    hex_format_arg,
    NULL
};

/* Parse data driver for type "str" */

static int
str_parse_arg(const char *arg, void *data, void **option)
{
    char *str = sal_strdup(arg);

    if (str) {
        if (*option) {
            sal_free(*option);
        }
        /*
         * Save allocated string in unused "option" in order to allow
         * bcma_cli_parse_table_done to free it again.
         */
        *option = (void *)str;
    }

    *((char **)data) = (void *)str;
    return 0;
}

static char *
str_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    return *((char **)data);
}

static bcma_cli_parse_data_t parse_data_str = {
    "str",
    str_parse_arg,
    str_format_arg,
    NULL
};

/* Parse data driver for type "enum" */

static int
enum_parse_arg(const char *arg, void *data, void **option)
{
    int idx = 0;
    bcma_cli_parse_enum_t *penum = *(bcma_cli_parse_enum_t **)option;

    while (penum[idx].name != NULL) {
        if (bcma_cli_parse_cmp(penum[idx].name, arg, '\0')) {
            *(int *)data = penum[idx].val;
            return 0;
        }
        idx++;
    }

    cli_out("Invalid selection: %s\n", arg);
    idx = 0;
    while (penum[idx].name != NULL) {
        cli_out("\t%s\n", penum[idx].name);
        idx++;
    }

    return -1;
}

static char *
enum_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    int val = *(int *)data;
    int idx = 0;
    bcma_cli_parse_enum_t *penum = (bcma_cli_parse_enum_t *)option;

    while (penum[idx].name != NULL) {
        if (penum[idx].val == val) {
            return penum[idx].name;
        }
        idx++;
    }

    return "<overrun>";
}

static bcma_cli_parse_data_t parse_data_enum = {
    "enum",
    enum_parse_arg,
    enum_format_arg,
    NULL
};


int
bcma_cli_parse_data_init(void)
{
    parse_data_root = NULL;

    return 0;
}

int
bcma_cli_parse_data_add(bcma_cli_parse_data_t *pd)
{
    if (bcma_cli_parse_data_lookup(pd->arg_type)) {
        /* This arg type already exists */
        return -1;
    }

    /* Insert in driver list */
    pd->next = parse_data_root;
    parse_data_root = pd;

    return 0;
}

int
bcma_cli_parse_data_add_default(void)
{
    /* Skip if parsers have been added already */
    if (parse_data_root != NULL) {
        return 0;
    }

    /* Not really required */
    bcma_cli_parse_data_init();

    /* Add default data parsers */
    bcma_cli_parse_data_add(&parse_data_bool);
    bcma_cli_parse_data_add(&parse_data_int);
    bcma_cli_parse_data_add(&parse_data_hex);
    bcma_cli_parse_data_add(&parse_data_str);
    bcma_cli_parse_data_add(&parse_data_enum);

    return 0;
}

bcma_cli_parse_data_t *
bcma_cli_parse_data_lookup(const char *arg_type)
{
    bcma_cli_parse_data_t *pd = parse_data_root;

    while (pd) {
        if (sal_strcmp(arg_type, pd->arg_type) == 0) {
            return pd;
        }
        pd = pd->next;
    }

    return pd;
}

int
bcma_cli_parse_data_parse_arg(bcma_cli_parse_data_t *pd,
                         const char *argstr, void *data, void **option)
{
    if (pd == NULL || pd->parse_arg == NULL) {
        return -1;
    }

    return pd->parse_arg(argstr, data, option);
}

char *
bcma_cli_parse_data_format_arg(bcma_cli_parse_data_t *pd,
                          const void *data, const void *option,
                          char *buf, int bufsz)
{
    if (pd == NULL || pd->format_arg == NULL) {
        return "<nodata>";
    }

    return pd->format_arg(data, option, buf, bufsz);
}
