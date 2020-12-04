/*! \file bcma_cli_parse_table.c
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

/*! Parse table entries are allocated in clumps of PT_ALLOC */
#define PT_ALLOC        32

/*!
 * \brief Look up command line argument in parse table
 *
 * <long-description>
 *
 * \param [in] str Command line argument
 * \param [in] pe Array of parse table entries
 * \param [in] sz Number of (valid) entries in array
 * \param [in] term Character which indicates end-of-string
 *
 * \return Matching parse table entry or NULL if no matches
 */
static bcma_cli_parse_entry_t *
parse_entry_lookup(const char *str, bcma_cli_parse_entry_t *pe, int sz,
                   char term)
{
    bcma_cli_parse_entry_t *rv;
    int len;

    if (str[0] == 0) {
        return NULL;
    }

    rv = NULL;
    while (sz--) {
        if (bcma_cli_parse_cmp(pe->name, str, term)) {
            if (rv == NULL) {
                rv = pe;
            }
            len = sal_strlen(pe->name);
            if (!sal_strncasecmp(str, pe->name, len)) {
                rv = pe;
                break;
            }
        }
        pe++;
    }
    return rv;
}

/*!
 * \brief Parse argument value into data object
 *
 * Lookup parse data driver and call the associated parser
 * function. Upon success, the parsed data is stored in the parse
 * table entry.
 *
 * \param [in] pe Parse table entry
 * \param [in] argstr Command line argument value string
 *
 * \retval 0 No errors
 * \retval -1 Malformed argument
 */
static int
parse_check_arg(bcma_cli_parse_entry_t *pe, const char *argstr)
{
    return bcma_cli_parse_data_parse_arg(pe->pd, argstr,
                                         pe->value, &pe->option);
}

/*!
 * \brief Parse argument "=" with prompted input
 *
 * Go through all the entries in the parse table with prompted input to
 * set each table entry value one by one.
 *
 * \param [in] pt Parse table
 *
 * \retval 0 No errors
 * \retval -1 Malformed argument
 */
static int
parse_prompt_table_entries(bcma_cli_parse_table_t *pt)
{
    int rv = 0, rv_gets, idx;
    bcma_cli_parse_entry_t *pe;
    char buf[256], prompt[128];
    char *str;

    /*
     * Go through the entries of the parse table to
     * get the prompted input value.
     */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];

        /* Format the prompt of the current parse entry with current value */
        str = bcma_cli_parse_data_format_arg(pe->pd, pe->value, pe->option,
                                             buf, sizeof(buf));
        sal_snprintf(prompt, sizeof(prompt), "%s[%s] ", pe->name,
                     str ? str : "N/A");

        /*
         * Get the prompted input of the parse table entry
         * until the input is valid.
         */
        do {
            rv = 0;
            rv_gets = bcma_cli_gets(pt->cli, prompt, sizeof(buf), buf);
            if (rv_gets == BCMA_CLI_CMD_INTR) {
                /* Exit the entries loop if Ctrl-C is pressed */
                idx = pt->num_valid;
                break;
            }
            if ((sal_strlen(buf) > 0)) {
                if (sal_strcmp(buf, "-") == 0) {
                    /*
                     * If the entry input is "-", go back to the previous one.
                     */
                    if (--idx >= 0) {
                        idx--;
                    }
                } else {
                    rv = parse_check_arg(pe, buf);
                }
            }
        } while (rv < 0);
    }

    return rv;
}

void
bcma_cli_parse_table_init(bcma_cli_t *cli, bcma_cli_parse_table_t *pt)
{
    pt->cli = cli;
    pt->num_valid = 0;
    pt->num_alloc = 0;
    pt->entries = NULL;

    /* Ensure the default parsers are configured */
    bcma_cli_parse_data_add_default();
}

int
bcma_cli_parse_table_add(bcma_cli_parse_table_t *pt,
                         const char *name, const char *arg_type,
                         void *value, void *option)
{
    bcma_cli_parse_entry_t *pe;
    bcma_cli_parse_entry_t *npe;
    bcma_cli_parse_data_t *pd;
    int nalloc;
    int copysize;

    pd = bcma_cli_parse_data_lookup(arg_type);
    if (pd == NULL) {
        cli_out("%sParse table unknown data type: %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, arg_type);
        return -1;
    }

    /* entries are allocated in clumps of PT_ALLOC */
    if (pt->num_valid + 1 >= pt->num_alloc) {
        nalloc = pt->num_alloc + PT_ALLOC;
        npe = sal_alloc(sizeof(bcma_cli_parse_entry_t) * nalloc,
                        "bcmaCliParseTab");
        if (npe == NULL) {
            cli_out("%sParse table cannot allocate %d entries\n",
                    BCMA_CLI_CONFIG_ERROR_STR, nalloc);
            return -1;
        }
        sal_memset(npe, 0, sizeof(bcma_cli_parse_entry_t) * nalloc);
        if (pt->num_alloc != 0) {
            /* Copy old contents to newly allocated table */
            copysize = sizeof(bcma_cli_parse_entry_t) * pt->num_alloc;
            sal_memcpy(npe, pt->entries, copysize);
            sal_free(pt->entries);
        }
        pt->num_alloc = nalloc;
        pt->entries = npe;
    }

    /* At this point we have room for the new entry */
    pe = &pt->entries[pt->num_valid++];

    pe->name = sal_strdup(name);
    if (pe->name == NULL) {
        cli_out("%sParse table cannot strdup entry name %s\n",
                BCMA_CLI_CONFIG_ERROR_STR, name);
        return -1;
    }
    pe->pd = pd;
    pe->value = value;
    pe->option = option;

    return 0;
}

int
bcma_cli_parse_table_do_args(bcma_cli_parse_table_t *pt, bcma_cli_args_t *args)
{
    int rv = 0;
    char *c, *eq;
    bcma_cli_parse_entry_t *pe;

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        /* No args, return 0 */
        return 0;
    }

    /* Prompt each table entry if the argument is "=" */
    c = BCMA_CLI_ARG_CUR(args);
    if (sal_strcmp(c, "=") == 0) {
        BCMA_CLI_ARG_NEXT(args);
        return parse_prompt_table_entries(pt);
    }

    while ((c = BCMA_CLI_ARG_CUR(args)) != NULL) {
        pe = parse_entry_lookup(c, pt->entries, pt->num_valid, '=');
        if (!pe) {
            /* Not found */
            return rv;
        }
        rv++;
        eq = sal_strchr(c, '=');
        if (!eq) {
            if (sal_strcmp(pe->pd->arg_type, "bool") == 0) {
                *((int *)pe->value) = 1;
            }
        } else {
            eq++;
            if (parse_check_arg(pe, eq)) {
                return -1;
            }
        }
        BCMA_CLI_ARG_NEXT(args);
    }
    return(rv);
}

void
bcma_cli_parse_table_done(bcma_cli_parse_table_t *pt)
{
    int idx;
    bcma_cli_parse_entry_t *pe;

    if (pt->entries == NULL) {
        return;
    }

    /* Free entry name and dynamic allocated strings */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];
        if (pe->name) {
            sal_free(pe->name);
        }
        if (pe->option && (sal_strcmp(pe->pd->arg_type, "str") == 0)) {
            sal_free(pe->option);
        }
    }

    sal_free(pt->entries);
    pt->entries = NULL;
    pt->num_alloc = 0;
}

void
bcma_cli_parse_table_show(bcma_cli_parse_table_t *pt, const char *prefix)
{
    int idx;
    bcma_cli_parse_entry_t *pe;
    char buf[128];
    char *str;

    /* Default prefix is TAB */
    if (prefix == NULL) {
        prefix = "\t";
    }

    /* Fill in defaults */
    for (idx = 0; idx < pt->num_valid; idx++) {
        pe = &pt->entries[idx];
        str = bcma_cli_parse_data_format_arg(pe->pd, pe->value, pe->option,
                                             buf, sizeof(buf));
        cli_out("%s%s=%s\n", prefix, pe->name, str ? str : "N/A");
    }
}
