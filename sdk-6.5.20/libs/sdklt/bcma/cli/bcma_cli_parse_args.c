/*! \file bcma_cli_parse_args.c
 *
 * This module parses a raw command line into an array of tokens,
 * which can be passed to the CLI command implmentations as standard
 * argc/argv parameters.
 *
 * If support for environment variables has been enabled, the parser
 * will also expand any environment variables found in the command
 * line.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_redir.h>
#include <bcma/cli/bcma_cli_parse.h>

/*! Maximum depth of environment variables. */
#define PARSE_DEPTH      10

/*!
 * \brief Parser control.
 *
 * Controls the state of the ongoing parsing operation.
 */
typedef struct parse_s {

    /*! Tracks if we are currently inside double-quotes. */
    int in_dquote;

    /*! Tracks if we are currently inside single-quotes. */
    int in_squote;

    /*! Tracks if we are currently parsing a command token. */
    int in_word;

    /*! Tracks context stack level. */
    int var_depth;

    /*! Stack of parsing contexts (base command or variable.) */
    const char *var_stack[PARSE_DEPTH];

    /*! Tracks position within current context. */
    const char *var_ptr[PARSE_DEPTH];

    /*! Tracks destination string. */
    char *dst;

    /*! Marks end of destination string. */
    char *eob;

} parse_t;

/*
 * Get next character in current context.
 * Pop context if current context has been exhausted.
 */
static int
parse_getc(parse_t *pp)
{
    int ch;

    for (ch = 0; pp->var_depth >= 0; pp->var_depth--) {
        if ((ch = *pp->var_ptr[pp->var_depth]++) != 0) {
            break;
        }
    }
    return ch;
}

/*
 * Perform ungetc for current context.
 */
static void
parse_ungetc(parse_t *pp)
{
    if (pp->var_ptr[pp->var_depth]) {
        pp->var_ptr[pp->var_depth]--;
    }
}

/*
 * Push variable onto context stack.
 */
static int
parse_var_push(parse_t *pp, const char *str)
{
    if (pp->var_depth >= (PARSE_DEPTH - 1)) {
        return -1;
    }

    pp->var_depth++;
    pp->var_stack[pp->var_depth] = str;
    pp->var_ptr[pp->var_depth] = str;

    return 0;
}

/*
 * Get next character in current context without changing the stack depth.
 */
static int
parse_var_getc(parse_t *pp)
{
    if (*pp->var_ptr[pp->var_depth]) {
        return *pp->var_ptr[pp->var_depth]++;
    }
    return 0;
}

/*
 * Parse a variable name.
 *
 * Names preceded by a '?' will be converted to a Boolean string ("0"
 * or "1") depending on the value/presence of the variable.
 *
 * For example $?myvar will be converted to "1" if $myvar is defined,
 * otherwise it will be converted to "0".
 */
static int
parse_variable(bcma_cli_t *cli, parse_t *pp, const char **var_value)
{
    char *varname, *ptr;
    const char *varval;
    int varq;
    int ch;

    varname = sal_alloc(sal_strlen(pp->var_ptr[pp->var_depth]) + 1,
                        "bcmaCliParseVar");
    if (varname == NULL) {
        return 0;
    }
    ptr = varname;
    ch = parse_var_getc(pp);
    if (ch == '{') {
        while ((ch = parse_var_getc(pp)) != '}' && ch != 0) {
            *ptr++= ch;
        }
    } else {
        while (sal_isalnum(ch) || ch == '_' || ch == '?') {
            *ptr++ = ch;
            ch = parse_var_getc(pp);
        }
        if (ch != 0) {
            parse_ungetc(pp);
        }
    }
    *ptr = 0;
    /* Check for $?varname */
    varq = (varname[0] == '?' && varname[1] != 0);
    /* Get variable value */
    varval = bcma_cli_var_get(cli, varname + varq);
    if (varq) {
        /* Mark if variable is defined */
        parse_var_push(pp, (varval == NULL) ? "0" : "1");
    } else if (varval != NULL) {
        /* Push variable expansion onto stack */
        parse_var_push(pp, varval);
    }

    if (var_value != NULL) {
        *var_value = varval;
    }

    sal_free(varname);

    return 0;
}

/*
 * Initialize parser control.
 */
static int
parse_init(parse_t *pp, const char *str)
{
    if (!str) {
        return -1;
    }

    sal_memset(pp, 0, sizeof(*pp));
    pp->var_stack[0] = str;
    pp->var_ptr[0] = str;

    return 0;
}

/*
 * Start a new command token.
 */
static int
parse_start_word(bcma_cli_args_t *args, char *word)
{
    if (args->argc >= COUNTOF(args->argv)) {
        return -1;
    }

    args->argv[args->argc++] = word;

    return 0;
}

/*
 * Parse escaped (\) characters depending on context.
 */
static int
parse_escaped(parse_t *pp, bcma_cli_args_t *args)
{
    int ch;

    ch = parse_getc(pp);
    if (ch == 0) {
        cli_out("%sCannot escape EOL\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return -1;
    }
    if (!pp->in_word) {
        if (parse_start_word(args, pp->dst)) {
            return -1;
        }
        pp->in_word = TRUE;
    }
    if (pp->dst < pp->eob) {
        *pp->dst++ = ch;
    }

    return 0;
}

/*
 * Parse command token.
 */
static int
parse_token(parse_t *pp, bcma_cli_args_t *args, int ch)
{
    if (!pp->in_word) {
        if (parse_start_word(args, pp->dst)) {
            return -1;
        }
        pp->in_word = TRUE;
    }
    if (ch == '"' && !pp->in_squote) {
        pp->in_dquote = !pp->in_dquote;
    } else if (ch == '\'' && !pp->in_dquote) {
        pp->in_squote = !pp->in_squote;
    } else {
        if (pp->dst < pp->eob) {
            *pp->dst++ = ch;
        }
    }

    return 0;
}

/*
 * Check for redirection ('|')
 */
static bool
is_redir(bcma_cli_t *cli, char ch, parse_t *pp)
{
    int nch;
    const char *varval;
    parse_t parse, *pp_redir = &parse;

    if (ch != '|') {
        return false;
    }

    sal_memcpy(pp_redir, pp, sizeof(*pp));

    while (1) {
        nch = parse_getc(pp_redir);
        if (sal_isspace(nch)) {
            continue;
        } else if (sal_isalpha(nch)) {
            /* Redirect if the following character is a letter */
            return true;
        } else if (nch == '$' && !pp_redir->in_squote) {
            if (parse_variable(cli, pp_redir, &varval) < 0 || varval == NULL) {
                return false;
            }
            if (!bcma_cli_parse_is_int(varval) && sal_isalpha(varval[0])) {
                /*
                 * Redirect if the following variable value
                 * is not an integer and start with a letter.
                 */
                return true;
            }
            return false;
        } else {
            break;
        }
    }

    return false;
}

/*
 * Parse string into tokens token.
 */
int
bcma_cli_parse_args(bcma_cli_t *cli, const char *str,
                    const char **s_ret, bcma_cli_args_t *args)
{
    int ch;
    int eos;
    parse_t parse, *pp = &parse;

    args->argc = 0;
    args->arg  = 0;
    if (s_ret) {
        *s_ret = NULL;
    }

    /* Handle NULL string */
    if (parse_init(pp, str) < 0) {
        return 0;
    }

    pp->dst = args->buffer;
    pp->eob = args->buffer + sizeof(args->buffer) - 1;

    while (1) {
        ch = parse_getc(pp);
        eos = (is_redir(cli, ch, pp) || ch == ';' || ch == 0) ? TRUE : FALSE;
        if (sal_isspace(ch) || eos) {
            /* White-space or end of statement */
            if (pp->in_dquote || pp->in_squote) {
                if (ch == 0) {
                    cli_out("%sCommand line ended while in a quoted string\n",
                            BCMA_CLI_CONFIG_ERROR_STR);
                    return -1;
                }
                /* In quote - copy verbatim */
                if (pp->dst < pp->eob) {
                    *pp->dst++ = ch;
                }
                continue;
            }
            if (pp->in_word) {
                /* In word - mark end of word */
                *pp->dst = 0;
                if (pp->dst < pp->eob) {
                    pp->dst++;
                }
                pp->in_word = FALSE;
            }
            if (eos) {
                /* Set redirection statue */
                bcma_cli_redir_enable_set(cli, (ch == '|') ? 1 : 0);
                /* End of statement - done for now */
                break;
            }
        } else if (ch == '\\') {
            if (parse_escaped(pp, args) < 0) {
                return -1;
            }
        } else if (ch == '$' && !pp->in_squote) {
            if (parse_variable(cli, pp, NULL) < 0) {
                return -1;
            }
        } else {
            /* Build argument */
            if (parse_token(pp, args, ch) < 0) {
                return -1;
            }
        }
    }

    if (s_ret) {
        /* Update s_ret if non-NULL */
        *s_ret = NULL;
        if (ch != 0 && pp->var_depth == 0) {
            /* More statements in command line */
            *s_ret = (char *)pp->var_ptr[pp->var_depth];
        }
    }

    return 0;
}
