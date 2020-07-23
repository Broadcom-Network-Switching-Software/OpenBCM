/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        parse.c
 * Purpose:     Defines routines used to break up a command line.
 */


#include <sal/core/libc.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm_int/api_xlate_port.h>

/*
 * Allow hook to plug a user-defined variable lookup routine into the
 * shell.  If non-NULL, the routine will be called when a shell variable
 * lookup fails.
 */

char *(*parse_user_var_get)(char *varname);

/*
 * scan routines
 *
 * These routines are for scanning an input stream from a string, with
 * the ability to push things onto the front of the stream at any time.
 * Used for expanding variables.
 */

#define SCAN_DEPTH      10

typedef struct scan_s {
    const char          *scan_stack[SCAN_DEPTH];
    const char          *scan_ptr[SCAN_DEPTH];
    int                 scan_tos;
} scan_t;

#define scan_start(scan)        ((scan)->scan_tos = -1)
#define scan_ungetc(scan)       ((scan)->scan_ptr[(scan)->scan_tos]--)

static void scan_push(scan_t *scan, const char *s)
{
    if (scan->scan_tos < SCAN_DEPTH - 1) {
        scan->scan_stack[++scan->scan_tos] = s;
        scan->scan_ptr[scan->scan_tos] = s;
    }
}

static int scan_getc(scan_t *scan)
{
    int         c;

    for (c = 0; scan->scan_tos >= 0; scan->scan_tos--)
        if ((c = *scan->scan_ptr[scan->scan_tos]++) != 0)
            break;

    return c;
}

int
parse_cmp(const char *p, const char *s, const char term)
/*
 * Function:    parse_cmp
 * Purpose:     Compare a command for a match against a string.
 * Parameters:  p - pointer to string to compare against, upper case
 *              letters indicate letters that MUST be present for a
 *              match; digits must also match
 *              s - pointer to string to compare for match (input).
 *              term - termination character, normally '\0', but may be
 *                      '=' for assignment type statements.
 * Returns:     TRUE - compare matched
 *              FALSE - compare failed.
 * Notes:       Initial substrings that consume ALL upper case letters
 *              will match. For example, "abc" will match "ABcd", but not
 *              "ABcD".
 */
{
    const char *tp, *ts;                /* Temp p/s pointers */

    /* Check for complete string match */

    tp = p;
    ts = s;

    while ((*ts != term) && (*tp) &&
           (sal_toupper((unsigned)*ts) == sal_toupper((unsigned)*tp))) {
        ts++;
        tp++;
    }

    /* We may terminate either on "term" character or end of string */
    if (!*tp && ((*ts == term) || !*ts)) {
        return(TRUE);
    }

    /*
     * If we failed above, check if we matched up to a point where
     * no required characters are required (i.e. "abc" will match "ABcd"
     * but not "ABcdE". This (so far) appears to always do what is
     * expected.
     */

    if (*ts == term) {                  /* Ok - check pattern */
        while (*tp && !isupper((unsigned)*tp) && !isdigit((unsigned)*tp)) {
            tp++;
        }
        if (!*tp) {                     /* Matched */
            return(TRUE);
        }
    }

    /* Check for Upper case match only */

    tp = p;
    ts = s;

    while (*tp) {
        if (isupper((unsigned)*tp) || isdigit((unsigned)*tp)) {
            if (*tp == sal_toupper((unsigned)*ts)) {
                ts++;
            } else {
                break;
            }
        }
        tp++;
    }

    return (!*tp && ((*ts == term) || !*ts));
}

static int
parse_cmp_exact(const char *p, const char *s, const char term)
/*
 * Function:    parse_cmp_exact
 * Purpose:     Compare a command for an exact match against a string.
 * Parameters:  p - pointer to string to compare against, upper case
 *              letters indicate letters that MUST be present for a
 *              match; digits must also match
 *              s - pointer to string to compare for match (input).
 *              term - termination character, normally '\0', but may be
 *                      '=' for assignment type statements.
 * Returns:     TRUE - compare matched
 *              FALSE - compare failed.
 */
{
    const char *tp, *ts;                /* Temp p/s pointers */

    /* Handle NULL string */
    if (p == NULL || s == NULL) {
        cli_out("Warning: Comparing NULL string: p=%s, s=%s\n", p, s);
        return (FALSE);
    }

    /* Check for complete string match */
    tp = p;
    ts = s;

    while ((*ts != term) && (*tp) &&
           (sal_toupper((unsigned)*ts) == sal_toupper((unsigned)*tp))) {
        ts++;
        tp++;
    }

    /* We may terminate either on "term" character or end of string */
    if (!*tp && ((*ts == term) || !*ts)) {
        return (TRUE);
    }

    return (FALSE);
}

void
parse_args_copy(args_t *dst, args_t *src)
/*
 * Function:    parse_args_copy
 * Purpose:     Copy an args structure
 */
{
    int                 i;

    sal_memcpy(dst, src, sizeof (*dst));

    dst->a_cmd = dst->a_buffer + (src->a_cmd - src->a_buffer);

    for (i = 0; i < src->a_argc; i++) {
        dst->a_argv[i] = dst->a_buffer + (src->a_argv[i] - src->a_buffer);
    }
}

static int
parse_start_word(args_t *a, char *w)
/*
 * Function:    parse_start_word
 * Purpose:     Start a new parsed word, checking for argv overflow.
 * Parameters:  a - pointer to args structure
 *              w - pointer to word to start.
 * Returns:     0 - OK
 *              -1 - failed (to many parameters).
 */
{
    if (a->a_argc >= ARGS_CNT) {
        return(-1);
    } else {
        a->a_argv[a->a_argc++] = w;
        return(0);
    }
}

#ifdef BROADCOM_DEBUG
void
parse_arg_dump(args_t *a)
{
    int         i;

    cli_out("parse_arg_dump: a=%p argc=%d next=%d\n",
            (void *)a, a->a_argc, a->a_arg);

    for (i = 0; i < a->a_argc; i++)
        cli_out("parse_arg_dump: arg[%d] = <%s>\n", i, a->a_argv[i]);
}
#endif  /* BROADCOM_DEBUG */

int
diag_parse_args(const char *s, char **s_ret, args_t *a)
/*
 * Function:    parse_args
 * Purpose:     Break up a command line into argv/argc format.
 * Parameters:  s - string to break up
 *              s_ret - updated to where processing finished (e.g. semicolon)
 *              a - argument structure to fill in.
 * Returns:     0 - success
 *              -1 failed (message printed)
 */
{
    int         inDQ = FALSE;           /* True if in double quote */
    int         inSQ = FALSE;           /* True if in single quote */
    int         inW = FALSE;            /* True if in word */
    char        *d;                     /* Destination of copy */
    char        *e;                     /* End of buffer (last char) */
    int         c;
    scan_t      scan;
    d = a->a_buffer;
    e = a->a_buffer + sizeof (a->a_buffer) - 1;

    a->a_argc = 0;                      /* Start at 0 please */
    a->a_arg  = 0;

    if (!s) {                           /* Handle NULL string */
        if (s_ret) {
            *s_ret = 0;
        }
        return 0;
    }

    scan_start(&scan);
    scan_push(&scan, s);

    while (1) {
        c = scan_getc(&scan);
        if ('\\' == c) {        /* Escape char */
            c = scan_getc(&scan);
            if (c == '\0') {
                cli_out("ERROR: Can't escape EOL\n");
                return(-1);
            } else {
                if (!inW) {
                    if (parse_start_word(a, d)) {
                        return(-1);
                    }
                    inW = TRUE;
                }
                if (d < e) {
                    *d++ = c;
                }
            }
        } else if (!inSQ && (c == PARSE_VARIABLE_PREFIX1 ||
                             c == PARSE_VARIABLE_PREFIX2)) {
            char varname[256], *p = varname;
            int varq;
            c = scan_getc(&scan);
            if (c == '{') {
                while ((c = scan_getc(&scan)) != '}' && c != 0) {
                    *p++= c;
                }
            } else {
                while (isalnum(c) || c == '_' || c == '?') {
                    *p++ = c;
                    c = scan_getc(&scan);
                }
                if (c != 0) {
                    /* COVERITY: Coverity reports negative array index write
                      using scan.scan_tos to scan.scan_ptr, but scan.scan.tos
                      would be set to non-negative in scan_push() call */

                    /*    coverity[negative_returns : FALSE]    */
                    scan_ungetc(&scan);
                }
            }
            *p = 0;
            varq = (varname[0] == '?' && varname[1] != 0);
            p = var_get(varname + varq);
            if (p == NULL && parse_user_var_get != NULL) {
                p = (*parse_user_var_get)(varname + varq);
            }
            if (varq) {
                scan_push(&scan, (p == NULL) ? "0" : "1");
            } else if (p != NULL) {
                scan_push(&scan, p);
            }
        } else if (isspace(c) || (';' == c) || (0 == c)) {
            if (inDQ || inSQ) {         /* In quote - copy */
                if (c == 0) {
                    cli_out("ERROR: Command line ended "
                            "while in a quoted string\n");
                    return(-1);
                }
                if (d < e) {
                    *d++ = c;
                }
                continue;
            } else if (inW) {           /* In word - break */
                *d = 0;
                if (d < e) {
                    d++;
                }
                inW = FALSE;
            }
            if (';' == c || 0 == c) {
                /*
                 * If end of line or statement, then update return
                 * pointer and finish up
                 */
                break;
            }
        } else {
            /*
             * Nothing special, if not in a word, then start new arg.
             */
            if (!inW) {
                if (parse_start_word(a, d)) {
                    return(-1);
                }
                inW = TRUE;
            }
            if ('"' == c && !inSQ) {
                inDQ = !inDQ;
            } else if ('\'' == c && !inDQ) {
                inSQ = !inSQ;
            } else {
                if (d < e) {
                    *d++ = c;
                }
            }
        }
    }

    if (s_ret) {        /* Update s_ret if non-NULL */
        *s_ret = ((c != 0 && scan.scan_tos == 0) ?
               (char *) scan.scan_ptr[scan.scan_tos] :
               NULL);
    }

    return(0);
}

int
parse_mask(const char *p, const parse_pm_t *pm, uint32 *mask)
/*
 * Function:    parse_mask
 * Purpose:     Parse an option of the form [+|-]string into an
 *              AND, OR, and XOR mask.
 * Parameters:  p - string to parse.
 *              pm - plus/minus table, last entry MUST have a string
 *                      pointer on NULL.
 *              mask - Mask to or/and/xor into.
 * Returns:     0 - success
 *              -1 failed.
 * Notes:       If the first character of an entry in pm is '@', the '@'
 *              is ignored (see mask format).
 */
{
    char        c = '\0';               /* Start NULL */
    char        *ps;                    /* String to compare */

    c = *p;
    if (('+' == c) || ('-' == c)) {
        p++;
    }

    while ((ps = pm->pm_s) != 0) {
        if ('@' == *ps) {               /* Skip alias character */
            ps++;
        }
        if (parse_cmp(ps, p, '\0')) {
            break;                      /* while */
        }
        pm++;
    }

    if (ps) {                           /* Found */
        switch(c) {
        case '+':                       /* OR */
            *mask |= pm->pm_value;
            break;
        case '-':                       /* AND */
            *mask &= ~pm->pm_value;
            break;
        default:                        /* XOR */
            *mask ^= pm->pm_value;
            break;
        }
        return(0);
    } else {
        return(-1);
    }
}

static void     *
parse_do_lookup(const char *s, void *t, const int size,
                int c, char term, const int cmd_flag)
/*
 * Function:    parse_do_lookup
 * Purpose:     Locate an entry in a table of the form "abc".
 * Parameters:  s - pointer to string to locate
 *              t - pointer to table to lookup in
 *              size - size of each entry.
 *              c - # of valid entries
 *              term - character which denotes end of match string.
 *              cmd_flag - flag which indicates the on-going config cmd
 * Returns:     Pointer to command table entry or NULL.
 * Notes:       If cmd_flag == CMD_ADD, only search for an exact match.
 *              Otherwise, in case of multiple matches, an exact match
 *              will be preferred over any partial matches.
 */
{
    void        *rv;
    int         len;

    if (s[0] == 0) {
        return NULL;
    }

    rv = NULL;
    while (c--) {
        if (cmd_flag == CMD_ADD) {
            /* Only search for an exact match */
            if (parse_cmp_exact(*(parse_key_t *)t, s, term)) {
                rv = t;
                break;
            }
        } else {
            if (parse_cmp(*(parse_key_t *)t, s, term)) {
                if (rv == NULL) {
                    rv = t;
                }
                len = sal_strlen(*(parse_key_t *)t);
                if (!sal_strncasecmp(s, *(parse_key_t *)t, len)) {
                    rv = t;
                    break;
                }
            }
        }
        t = (char *)t + size;
    }
    return(rv);
}

int
parse_check_eq_arg(parse_eq_t *pq, char *s)
/*
 * Function:    parse_check_eq
 * Purpose:     Check if an input parameter matches the pattern specified.
 * Parameters:  pq - pointer to parse_eq entry that matched.
 *              s - pointer to string to match against option.
 * Returns:     0 - success
 *              -1- failed, no error printed
 *              -2- failed, error printed.
 * Notes:       Fills in value in pq if OK.
 */
{
    struct boolean_s {
        char    *b_string;
        int     b_value;
    };
    static struct boolean_s boolean_table[] = {
        {"Yes", TRUE},          {"OKay", TRUE},
        {"YOUBET", TRUE},       {"True", TRUE},
        {"1", TRUE},            {"0", FALSE},
        {"ON", TRUE},           {"OFF", FALSE},
        {"No", FALSE},          {"NOWay",FALSE},
        {"False", FALSE},       {"YEAH", TRUE},
        {"YEP", TRUE},          {"NOPE", FALSE},
        {"NOT", FALSE},         /*{"Maybe",__TIME__[7]&1},*/
    };
    int boolean_cnt = COUNTOF(boolean_table);
    const struct boolean_s *bs;
    sal_mac_addr_t mac_addr;
    ip_addr_t   ip_addr;
    ip6_addr_t  ip6_addr;
    pbmp_t      pbm;
    void        *v = NULL;              /* Value */
    int         vs = 0;                 /* Value size if type == PTR */
    void        *fv = NULL;             /* Free value */
    int         rv = 0;
    char        **ms = NULL;
    uint32      flags = 0;
    uint64      val64;
    soc_port_mode_t pm;
    soc_port_t  port;
    bcm_mod_port_t mod_port;
    soc_phy_control_longreach_ability_t lr_pa;
    soc_port_ability_t pa;

    switch (PQ_TYPE(pq->pq_type)) {
    case PQ_INT64:
        val64 = parse_uint64(s);
        v = (void *)&val64;
        vs = sizeof(uint64);
        break;
    case PQ_INT:
    case PQ_HEX:
    case PQ_INT8:
    case PQ_INT16:
        if (!isint(s)) {
            return(-1);
        }
        v = INT_TO_PTR(parse_integer(s));
        if (pq->pq_type & PQ_LAB) {
            cli_out("WARNING: using deprecated zero based port parsing\n");
        }
        break;
    case PQ_STRING:
        fv = *(void **)pq->pq_value;
        v = sal_strdup(s ? s : "");
        flags = PQ_MALLOC;
        break;
    case PQ_BOOL:
        bs = parse_lookup(s, boolean_table, sizeof(boolean_table[0]),
                          boolean_cnt);
        if (!bs) {
            return(-1);
        }
        v = INT_TO_PTR(bs->b_value);
        break;
    case PQ_MAC:
        if (parse_macaddr(s, mac_addr)) {
            cli_out("Expected MAC address in the format "
                    "xx:xx:xx:xx:xx:xx or 0x<value>\n");
            return(-1);
        }
        v = mac_addr;
        vs = sizeof(mac_addr);
        break;
    case PQ_IP:
        if (parse_ipaddr(s, &ip_addr)) {
            cli_out("Expecting IP address in the format "
                    "w.x.y.z or 0x<value>\n");
            return(-1);
        }
        v = INT_TO_PTR(ip_addr);
        break;
    case PQ_IP6:
        if (parse_ip6addr(s, ip6_addr)) {
            cli_out("Expecting IPV6 address in the format "
                    "AAAA:BBBB:CCCC:DDDD:EEEE:FFFF:GGGG:HHHH\n");
            return(-1);
        }
        v = (void *)&ip6_addr;
        vs = sizeof(ip6_addr);
        break;
    case PQ_PBMP:
        /* Check for ? */
        if (sal_strcmp(s, "?") == 0 || parse_pbmp(pq->pq_unit, s, &pbm)) {
            /* AND with pbmp_valid */
            if (SOC_IS_RAPTOR(pq->pq_unit) || SOC_IS_RAVEN(pq->pq_unit)) {
                pbmp_t valid_pbm = soc_property_get_pbmp(pq->pq_unit, spn_PBMP_VALID, 0);
                (void)parse_pbmp_no_error(pq->pq_unit, s, &pbm);
                if (!SOC_PBMP_IS_NULL(valid_pbm)) {
                    SOC_PBMP_AND(pbm, valid_pbm);
                }
                cli_out("Warning: continuing with invalid bitmap\n");
            } else {
                if (sal_strcmp(s, "?") != 0) {
                    cli_out("Invalid port bitmap \"%s\"\n", s);
                }
                cli_out("Examples:\n");
                cli_out("  fe ge e fe5 fe8-fe15 fe0-fe46:2 fe:2 fe0,ge1,cpu\n");
                cli_out("  1,25,27 none 0x0 all 0xbffffff\n");
                return(-2);
            }
        } else if (pq->pq_type & PQ_BCM) {
            BCM_API_XLATE_PORT_PBMP_P2A(pq->pq_unit, &pbm);
        }
        v = (void *)&pbm;
        vs = sizeof(pbm);
        break;
    case PQ_PORTMODE:
        /* Check for ? */
        if (sal_strcmp(s, "?") == 0 || parse_port_mode(s, &pm)) {
            if (sal_strcmp(s, "?") != 0) {
                cli_out("Invalid port mode string.\n");
            }
            cli_out("Examples: 1000full,100,10,pause 100,pause_tx,pause_rx\n");
            return(-2);
        }
        v = INT_TO_PTR(pm);
        break;
    case PQ_PORTABIL:
        /* Check for ? */
        if (sal_strcmp(s, "?") == 0 || parse_port_ability(s, &pa)) {
            if (sal_strcmp(s, "?") != 0) {
                cli_out("Invalid port mode string.\n");
            }
            cli_out("Examples: 21g,1000full,100,10,pause 100,pause_tx,pause_rx,cl74|cl91|fec_none,short|long\n");
            return(-2);
        }
        v = (void *)&pa;
        vs = sizeof(pa);
        break;

    case PQ_PORT:
        /* Check for ? */
        if (!sal_strcmp(s, "?") || parse_port(pq->pq_unit, s, &port)) {
            if (sal_strcmp(s, "?"))
                cli_out("Invalid port \"%s\"\n", s);
            cli_out("Examples: fe0 ge1 3 cpu0 any\n");
            return(-2);
        }
        if (pq->pq_type & PQ_BCM) {
            BCM_API_XLATE_PORT_P2A(pq->pq_unit, &port);
        }
        v = INT_TO_PTR(port);
        break;
    case PQ_MOD_PORT:   /* Parse as <mod.port>; if no '.', parse as port */
        /* Check for ? */
        if (!sal_strcmp(s, "?") || parse_mod_port(pq->pq_unit, s, &mod_port)) {
            if (sal_strcmp(s, "?"))
                cli_out("Invalid module.port \"%s\"\n", s);
            cli_out("<mod>.<port> or port:  2.4 fe0 cpu0 any\n");
            return(-2);
        }
        v = (void *)&mod_port;
        vs = sizeof(mod_port);
        break;
    case PQ_MULTI:
        /* Check for ? */
        if (!sal_strcmp(s, "?")) {
            rv = -2;                    /* Say error, error msg printed */
        } else {
            for (rv = -1, ms = pq->pq_fm; *ms; ms++) {
                if (parse_cmp(*ms, s, '\0')) {
                    rv = 0;
                    break;
                }
            }
        }
        if (!rv) {
            v = (void *)(ms - pq->pq_fm); /* Return Index */
        } else {
            if (-1 == rv) {
                cli_out("Invalid selection: %s\n", s);
                rv = -2;
            }
            for (ms = pq->pq_fm; *ms; ms++) {
                cli_out("\t%s\n", *ms);
            }
            return(rv);
        }
        break;
    case PQ_LR_PHYAB:
        /* Check for ? */
        if (sal_strcmp(s, "?") == 0 ||
                parse_phy_control_longreach_ability(s, &lr_pa)) {
            if (sal_strcmp(s, "?") != 0) {
                cli_out("Invalid longreach phy ability string.\n");
            }
            cli_out("Examples: 100x4,100x2,100x1,50x2,50x1,33x2,33x1,25x2,25x1,20x2,20x1,10x2,10x1,pause_tx,pause_rx\n");
            return(-2);
        }
        v = INT_TO_PTR(lr_pa);
        break;
    default:
        /* Defensive default */
        cli_out("Unexpected parse qualifier 0x%03x\n", PQ_TYPE(pq->pq_type));
        return(-2);
    }
    /*
     * Call side effect function, and if result is OK, make the
     * assignment.
     */
        if (pq->pq_type & PQ_PTR) {
            sal_memcpy(pq->pq_value, v, vs);
        } else {
            switch (PQ_TYPE(pq->pq_type)) {
            case PQ_INT:
            case PQ_HEX:
            case PQ_BOOL:
            case PQ_IP:
            case PQ_PORTMODE:
            case PQ_PORT:
            case PQ_MULTI:
            case PQ_LR_PHYAB:
                *((int *)pq->pq_value) = PTR_TO_INT(v);   /* 64-bit support */
                break;
            case PQ_INT8:
                *((uint8 *)pq->pq_value) = PTR_TO_INT(v);
                break;
            case PQ_INT16:
                *((uint16 *)pq->pq_value) = PTR_TO_INT(v);
                 break;
            case PQ_STRING:
            case PQ_MAC:
            case PQ_PBMP:
            case PQ_INT64:
                *(void **)pq->pq_value = v;
                break;
            }
        }
        if (fv) {                       /* Free old string if required */
            sal_free(fv);
        }
        pq->pq_type |= flags;           /* Or in possible flags */
        pq->pq_type |= PQ_PARSED;       /* Mark as parsed */

        /*
         * COVERITY
         * pq->pq_value origin address has been set to fv and freed,
         * new address alloc by sal_strdup has been set to v;
         */
        /*    coverity[leaked_storage]    */

    return(rv);
}

static  int
parse_prompt_eq(parse_table_t *pt)
/*
 * Function:    parse_prompt_eq
 * Purpose:     Prompt from keyboard for each option.
 * Parameters:  pt - table to prompt from.
 *              pq_cnt - # of entries in table.
 * Returns:     # parameters or -1 for failed.
 * Notes:
 *              Allows entering - to go back an entry.
 *              Allows pressing ^D to default all the rest of the entries.
 */
{
    char        is[128], ds[128];       /* input/default strings */
    char        rds[128];               /* real default string */
    int         pq_cur;
    void        *defl;                  /* Pointer to default value */
    parse_eq_t  *pq;
    int         default_rest = 0;
    int         use_rds;
    char        pfmt[SOC_PBMP_FMT_LEN];
    uint64 *ds_ptr;
    uint64 *defl_ptr;

    pq_cur = 0;
    while (pq_cur < pt->pt_cnt) {
        pq = &pt->pt_entries[pq_cur];
        if (pq->pq_type & PQ_IGNORE) {
            pq_cur++;
            continue;
        }
        defl = (pq->pq_type & PQ_DFL) ?
            ((pq->pq_type & PQ_PTR) ? pq->pq_value : *(void **)pq->pq_value)
                : (void *)pq->pq_default;
        use_rds = 0;
        switch(PQ_TYPE(pq->pq_type)) {
        case PQ_INT64:
            ds_ptr = (uint64 *)((void *)ds);
            COMPILER_64_ZERO(*ds_ptr);
            defl_ptr = (uint64 *)((void *)defl);
            COMPILER_64_OR(*ds_ptr, *defl_ptr);
            break;
        case PQ_INT:
        case PQ_INT8:
        case PQ_INT16:
            sal_sprintf(ds, "%d", PTR_TO_INT(defl));
            break;
        case PQ_HEX:
            sal_sprintf(ds, "0x%x", PTR_TO_INT(defl));
            break;
        case PQ_BOOL:
            sal_strncpy(ds, PTR_TO_INT(defl) ? "True" : "False", 127);
            break;
        case PQ_STRING:
            sal_strncpy(ds, defl ? (char *)defl : "", 127);
            ds[127] = 0;
            break;
        case PQ_MAC:
            if (defl != NULL) {
                format_macaddr((void *)ds, (void *)defl);
            } else {
                ds[0] = 0;
            }
            break;
        case PQ_IP:
            format_ipaddr((void *)ds, (ip_addr_t)PTR_TO_INT(defl));
            break;
        case PQ_IP6:
            if (defl != NULL) {
                format_ip6addr((void *)ds, (void *)defl);
            } else {
                ds[0] = 0;
            }
            break;
        case PQ_PBMP:
            format_pbmp(pq->pq_unit, is, sizeof(is), *(pbmp_t *)pq->pq_value);
            if (defl != NULL) {
                sal_sprintf(rds, "%s", SOC_PBMP_FMT(*(pbmp_t *)defl, pfmt));
                sal_sprintf(ds, "%s; %s",
                        SOC_PBMP_FMT(*(pbmp_t *)defl, pfmt),
                        is);
            } else {
                rds[0] = 0;
                ds[0] = 0;
            }
            use_rds = 1;
            break;
        case PQ_PORTMODE:
            format_port_mode((void *)ds, sizeof (ds),
                             (soc_port_mode_t)PTR_TO_INT(defl), TRUE);
            break;
        case PQ_PORT:
            sal_sprintf(rds, "%s", SOC_PORT_NAME(pq->pq_unit, PTR_TO_INT(defl)));
            sal_sprintf(ds, "%d; %s",
                    PTR_TO_INT(defl),
                    SOC_PORT_NAME(pq->pq_unit, PTR_TO_INT(defl)));
            use_rds = 1;
            break;
        case PQ_MOD_PORT:
        {
            bcm_mod_port_t *mp = defl;

            if (mp->mod == -1) {  /* Parse as port */
                sal_sprintf(rds, "%s", SOC_PORT_NAME(pq->pq_unit, mp->port));
                sal_sprintf(ds, "%d; %s",
                        mp->port,
                        SOC_PORT_NAME(pq->pq_unit, mp->port));
                use_rds = 1;
            } else {  /* Parse as <mod>.<port> */
                sal_sprintf(ds, "%d.%d", mp->mod, mp->port);
            }
            break;
        }
        case PQ_MULTI:
            sal_strncpy(ds, pq->pq_fm[PTR_TO_INT(defl)], 127);
            ds[127] = 0;
            break;
        }

        if (default_rest) {
            cli_out("%s[%s]\n", pq->pq_s, ds);
            sal_strncpy(is, ds, sizeof(is) - 1);
            is[sizeof(is) - 1] = 0;
        } else if (sal_readline(pq->pq_s, is, sizeof(is), ds) == NULL) {
            cli_out("\n");
            default_rest = 1;
            sal_strncpy(is, ds, sizeof(is) - 1);
            is[sizeof(is) - 1] = 0;
        }

        /* Allow entering '-' to go back an entry. */

        if (sal_strcmp(is, "-") == 0 && !default_rest) {
            while (pq_cur > 0) {
                pq_cur--;
                pq--;
                if (!(pq->pq_type & PQ_IGNORE)) {
                    break;
                }
            }
            continue;
        }

        /*
         * if just return was hit, the wrong default string was
         * copied into the input string, so replace it
         */
        if (use_rds && sal_strcmp(ds, is) == 0) {
            sal_strncpy(is, rds, sizeof(is) - 1);
            is[sizeof(is) - 1] = 0;
        }

        switch (parse_check_eq_arg(pq, is)) {
        case 0:
            break;
        case -1:
            cli_out("Invalid response\n");
            /* Fall through */
        case -2:                        /* NO BREAK */
            if (!default_rest) {
                continue;               /* While */
            }
        }
        /* Mark all as parsed */
        pq->pq_type |= PQ_PARSED;
        pq_cur++;                       /* On to next */
    }

    return(pq_cur);
}


int
parse_default_fill(parse_table_t *pq_table)
/*
 * Function:    parse_default_fill
 * Purpose:     Fill default arguments
 * Parameters:  pq_table - pointer to parse table to lookup in.
 * Returns:     # parameters parsed, or -1 if failed.
 * Notes:       All default values are filled in in the table.
 *
 *              parse_args_eq_done MUST always be called when the argument
 *              list is no longer needed to free any allocated buffers.
 */
{
  int         i, rv = 0;
    parse_eq_t  *pq;

    for (i = 0; i < pq_table->pt_cnt; i++) {    /* Fill in defaults */
        pq = &pq_table->pt_entries[i];
        if (pq->pq_type & PQ_DFL) {
            continue;
        }
        if (PQ_TYPE(pq->pq_type) == PQ_STRING) {
            *(char **)pq->pq_value =
                sal_strdup(pq->pq_default ? (char *)pq->pq_default : "");
            if (!(pq->pq_type & PQ_STATIC)) {
                pq->pq_type |= PQ_MALLOC;
            }
        } else if (pq->pq_type & PQ_PTR) {
        int vs = 4;
        switch (PQ_TYPE(pq->pq_type)) {
        case PQ_MAC:    vs = 6;                break;
        case PQ_IP:        vs = sizeof (ip_addr_t);    break;
        case PQ_IP6:    vs = sizeof (ip6_addr_t);    break;
        case PQ_PBMP:    vs = sizeof (pbmp_t);        break;
        case PQ_MOD_PORT:    vs = sizeof (bcm_mod_port_t);    break;
        }
        if (pq->pq_default != NULL) {
        sal_memcpy(pq->pq_value, pq->pq_default, vs);
        } else {
        sal_memset(pq->pq_value, 0, vs);
        }
        } else {
        /* All others must be 4-byte 'int' type */
        *(int *)pq->pq_value = PTR_TO_INT(pq->pq_default);
    }
    }

    /* AFTER defaults are filled in */
    return rv;
}

int
parse_arg_eq(args_t *a, parse_table_t *pq_table)
/*
 * Function:    parse_arg_eq
 * Purpose:     Decode an argument of the form "abc=def".
 * Parameters:  a - pointer to arguments to parse.
 *              pq_table - pointer to parse table to lookup in.
 * Returns:     # parameters parsed, or -1 if failed.
 * Notes:       All default values are filled in in the table, then
 *              the options are parsed and the pq_value values are
 *              overwritten.
 *
 *              parse_args_eq_done MUST always be called when the argument
 *              list is no longer needed to free any allocated buffers.
 */
{
    int         i, rv = 0;
    char        *c, *eq;                /* Current/after '=' argument */
    parse_eq_t  *pq;

    for (i = 0; i < pq_table->pt_cnt; i++) {    /* Fill in defaults */
        pq = &pq_table->pt_entries[i];
        if (pq->pq_type & PQ_DFL) {
            continue;
        }
        if (PQ_TYPE(pq->pq_type) == PQ_STRING) {
            *(char **)pq->pq_value =
                sal_strdup(pq->pq_default ? (char *)pq->pq_default : "");
            if (!(pq->pq_type & PQ_STATIC)) {
                pq->pq_type |= PQ_MALLOC;
            }
        } else if (pq->pq_type & PQ_PTR) {
        int vs = 4;
        switch (PQ_TYPE(pq->pq_type)) {
        case PQ_MAC:    vs = 6;                break;
        case PQ_IP:        vs = sizeof (ip_addr_t);    break;
        case PQ_IP6:    vs = sizeof (ip6_addr_t);    break;
        case PQ_PBMP:    vs = sizeof (pbmp_t);        break;
        case PQ_MOD_PORT:    vs = sizeof (bcm_mod_port_t);    break;
        }
        if (pq->pq_default != NULL) {
        sal_memcpy(pq->pq_value, pq->pq_default, vs);
        } else {
        sal_memset(pq->pq_value, 0, vs);
        }
        } else {
        /* All others must be 4-byte 'int' type */
        *(int *)pq->pq_value = PTR_TO_INT(pq->pq_default);
    }
    }

    /* AFTER defaults are filled in */

    if (0 == ARG_CNT(a)) {              /* No args, return 0 */
        return(0);
    }

    /*
     * If only 1 argument, and it is '=', then prompt from keyboard for
     * each option.
     */

    if (!sal_strcmp("=", _ARG_CUR(a))) {
        ARG_NEXT(a);                    /* Consume '=' */
        return(parse_prompt_eq(pq_table));
    }

    while ((c = ARG_CUR(a)) != NULL) {
        pq = parse_do_lookup(c, pq_table->pt_entries, sizeof(parse_eq_t),
                             pq_table->pt_cnt, '=', pq_table->cmd_flag);
        if (!pq) {                      /* Not found */
            return(rv);
        }
        rv++;
        eq = sal_strchr(c, '=');
        if (!eq) {
            if (pq->pq_type & PQ_NO_EQ_OPT) {
                pq->pq_type |= PQ_SEEN; /* Mark as seen during parsing */
                if (PQ_TYPE(pq->pq_type) == PQ_BOOL) {
                    *((int *)pq->pq_value) = 1;
                }
            } else {
                return -1;
            }
        } else { /* = found */
            eq++;
            if (parse_check_eq_arg(pq, eq)) {
                return(-1);
            }
        }
        ARG_NEXT(a);                    /* On to next argument */
    }
    return(rv);
}

int
parse_arg_eq_keep_index(args_t *a, parse_table_t *pq_table)
/*
* Same as parse_arg_eq, but does not advance the a_arg parameter
* Allows multiple access to single command
*/
{
    int rv = 0;
    int arg_index_placeholder = ARG_CUR_INDEX(a);
    rv = parse_arg_eq(a, pq_table);
    while (arg_index_placeholder < ARG_CUR_INDEX(a)) {
        ARG_PREV(a);
    }
    return (rv);
}

void
parse_arg_eq_done(parse_table_t *pt)
/*
 * Function:    parse_arg_eq_done
 * Purpose:     Free memory associated/allocated for parsing a
 *              EQ table.
 * Parameters:  pq - pointer to table.
 *              pq_cnt - # of entries.
 * Returns:     Nothing
 */
{
    parse_eq_t  *pq;

    if (pt->pt_entries == NULL) {
        return;
    }
    for (pq = pt->pt_entries; pq < &pt->pt_entries[pt->pt_cnt]; pq++) {
        if ((pq->pq_type & PQ_MALLOC) &&
                !(pq->pq_type & PQ_STATIC) &&
                *((void **)pq->pq_value) != NULL) {
            sal_free(*(void **)pq->pq_value);
            *(void **)pq->pq_value = NULL;
            pq->pq_type &= ~PQ_MALLOC;
        }
    }
    sal_free(pt->pt_entries);
    pt->pt_entries = NULL;
    pt->pt_alloc = 0;
}

void
parse_eq_format(parse_table_t *pt)
/*
 * Function:    parse_eq_format
 * Purpose:     Print out current values from a EQ table, using the
 *              assigned values, NOT the default values.
 * Parameters:  pq - pointer to eq table
 * Returns:     Nothing.
 */
{
    char        fs[128];
    parse_eq_t  *pq;
    char        pfmt[SOC_PBMP_FMT_LEN];

    for (pq = pt->pt_entries; pq < &pt->pt_entries[pt->pt_cnt]; pq++) {
        switch(PQ_TYPE(pq->pq_type)) {
        case PQ_INT64:
            cli_out("\t%s=0x%x%x\n", pq->pq_s,
                    COMPILER_64_HI(*(uint64 *)pq->pq_value),
                    COMPILER_64_LO(*(uint64 *)pq->pq_value));
            break;
        case PQ_INT:
            cli_out("\t%s=%d\n", pq->pq_s, *(int *)pq->pq_value);
            break;
        case PQ_INT8:
            cli_out("\t%s=%d\n", pq->pq_s, *(uint8 *)pq->pq_value);
            break;
        case PQ_INT16:
            cli_out("\t%s=%d\n", pq->pq_s, *(uint16 *)pq->pq_value);
            break;
        case PQ_HEX:
            cli_out("\t%s=0x%x\n", pq->pq_s, *(int *)pq->pq_value);
            break;
        case PQ_BOOL:
            cli_out("\t%s=%s\n", pq->pq_s,
                    *(int *)pq->pq_value ? "True" : "False");
            break;
        case PQ_STRING:
            cli_out("\t%s=%s\n", pq->pq_s,
                    *(char **)pq->pq_value ?
                    *(char **)pq->pq_value : "<none>");
            break;
        case PQ_MULTI:
            cli_out("\t%s=%s\n", pq->pq_s,
                    pq->pq_fm[*(int *)pq->pq_value]);
            break;
        case PQ_MAC:
            format_macaddr(fs, (uint8 *)(pq->pq_value));
            cli_out("\t%s=%s\n", pq->pq_s, fs);
            break;
        case PQ_IP:
            format_ipaddr(fs, *(ip_addr_t *)(pq->pq_value));
            cli_out("\t%s=%s\n", pq->pq_s, fs);
            break;
        case PQ_IP6:
            format_ip6addr(fs, (uint8 *)(pq->pq_value));
            cli_out("\t%s=%s\n", pq->pq_s, fs);
            break;
        case PQ_PBMP:
            format_pbmp(pq->pq_unit, fs, sizeof(fs), *(pbmp_t *)pq->pq_value);
            cli_out("\t%s=%s, %s\n", pq->pq_s,
                    SOC_PBMP_FMT(*(pbmp_t *)pq->pq_value, pfmt),
                    fs);
            break;
        case PQ_PORTMODE:
            format_port_mode(fs, sizeof (fs),
                             (soc_port_mode_t)PTR_TO_INT(pq->pq_value), TRUE);
            cli_out("\t%s=%s\n", pq->pq_s, fs);
            break;
        case PQ_PORT:
        {
            char *pname;
            if (pq->pq_type & PQ_BCM) {
                pname = BCM_PORT_NAME(pq->pq_unit, *(int *)pq->pq_value);
            } else {
                pname = SOC_PORT_NAME(pq->pq_unit, *(int *)pq->pq_value);
            }
            cli_out("\t%s=%s, %d\n", pq->pq_s, pname, *(int *)pq->pq_value);
            break;
        }
        case PQ_MOD_PORT:
        {
            bcm_mod_port_t *mp = (bcm_mod_port_t *)pq->pq_value;
            if (mp->mod < 0) {  /* Display as local port */
                cli_out("\t%s=--.%s (%d)\n", pq->pq_s,
                        SOC_PORT_NAME(pq->pq_unit, mp->port), mp->port);
            } else {
                cli_out("\t%s=%d.%d\n", pq->pq_s, mp->mod, mp->port);
            }
            break;
        }
        }
    }
}


int
parse_table_add(parse_table_t *pq_table, char *key, uint32 type, void *def,
                void *value, void *func)
/*
 * Function:    parse_table_add
 * Purpose:     Initialize fields for a specific entry in the table
 * Parameters:  pq_table - pointer to pq table to fill in values for.
 *              key - keyword to fill in
 *              def - default value for entry
 *              value - value to assign to "value".
 *              func - pointer tp
 * Returns:     0 - success
 *              -1 - failed to find entry.
 *
 * Notes:       This routine is not efficient, but it works easily!
 */
{
    parse_eq_t  *pq;
    parse_eq_t  *npe;
    int         nalloc;

    /* pt_entries are allocated in clumps of PT_ALLOC */
    if (pq_table->pt_cnt + 1 >= pq_table->pt_alloc) {
        nalloc = pq_table->pt_alloc + PT_ALLOC;
        npe = sal_alloc(sizeof(parse_eq_t) * nalloc, "parse_tab");
        if (npe == NULL) {
            cli_out("parse_table_add: ERROR: "
                    "cannot allocate %d entries\n", nalloc);
            return -1;
        }
        sal_memset(npe, 0, sizeof(parse_eq_t) * nalloc);
        if (pq_table->pt_alloc != 0) {
            int         copysize;
            /*
             * Note: mips64 gcc 2.7.6 and 2.7.12 die with an internal
             * compiler error unless the copy size is calculated in a
             * local variable.
             */
            copysize = sizeof (parse_eq_t) * pq_table->pt_alloc;
            sal_memcpy(npe, pq_table->pt_entries, copysize);
            sal_free(pq_table->pt_entries);
        }
        pq_table->pt_alloc = nalloc;
        pq_table->pt_entries = npe;
    }
    pq = &pq_table->pt_entries[pq_table->pt_cnt];
    pq_table->pt_cnt += 1;

    pq->pq_unit = pq_table->pt_unit;
    pq->pq_s = key;
    pq->pq_type = type;
    pq->pq_default = def;
    pq->pq_value = value;
    pq->pq_fm = func;
    return 0;
}

void
parse_table_init(int unit, parse_table_t *pt)
/*
 * Function:    parse_table_init
 * Purpose:     Initialize parse table fields.
 * Parameters:  pt - pointer to table to initialize.
 * Returns:     Nothing
 */
{
    pt->pt_unit = unit;
    pt->pt_cnt = 0;
    pt->pt_alloc = 0;
    pt->pt_entries = NULL;
    pt->cmd_flag = 0;
}

void
parse_mask_format(const int ll, const parse_pm_t *pm, uint32 mask)
/*
 * Function:    parse_mask_format
 * Purpose:     Format mask bits into buffer.
 * Parameters:  ll - line length
 *              pm - pointer to pm table
 *              mask - mask value to format.
 * Returns:     Nothing.
 * Notes:       If the first character of an entry in pm is '@', the entry
 *              is ignored - this allows multiple entries to refer to the
 *              same bit(s) as an alias.
 *              For simplicity, we always append a space to the end of each
 *              entry.
 */
{
    int cl = 0;                         /* Current line length */
    int tl;                             /* Temp line length */
    int nl = FALSE;                     /* newline require */

    while (pm->pm_s) {
        if ('@' != *pm->pm_s) {         /* Ignore alias */
            if (pm->pm_value & mask) {
                tl = sal_strlen(pm->pm_s) + 1;
                if (cl + tl > ll) {
                    nl = TRUE;
                    cl = tl;
                } else {
                    cl += tl;
                }
                cli_out("%s%s%s",
                        nl ? "\n" : "",       /* Newline required */
                        cl == tl ? "" : " ",  /* First on line */
                        pm->pm_s);
                nl = FALSE;
            }
        }
        pm++;
    }
    cli_out("\n");
}

const void      *
parse_lookup(const char *s, const void *t, int size, int cnt)
/*
 * Function:    parse_lookup
 * Purpose:     Locate an entry in a table based on the key.
 * Parameters:  s - pointer to string to locate
 *              t - pointer to table to lookup in
 *              size - size of each entry.
 *              cnt - # of valid entries
 * Returns:     Pointer to command table entry or NULL.
 */
{
    return (parse_do_lookup(s, (void *)t, size, cnt, '\0', CMD_DC));
}

