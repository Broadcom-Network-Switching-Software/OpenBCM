/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        context.c
 * Purpose:     Determine ident/value context
 */

#include "context.h"
#include "tokenizer.h"
#include "api_mode.h"
#include "api_mode_yy.h"
#include "api_grammar.tab.h"
#include "shared/util.h"
#include "sal/appl/sal.h"
#include "cint_interpreter.h"

typedef struct api_cint_db_s {
    api_mode_cint_dt_db_t func; /* CINT function database */
    api_mode_cint_dt_db_t tdef; /* CINT typedef database */
    api_mode_cint_var_db_t var; /* CINT global variables */
    int dt_changed;             /* CINT datatypes changed */
    int var_changed;            /* CINT global variables changed */
    const char *prefix;         /* API prefix */
    const char *prefix_u;       /* API prefix in uppercase */
    int prefix_len;             /* API prefix length */
} api_cint_db_t;

STATIC api_cint_db_t api_cint;

STATIC api_mode_private_t api_mode_private[] = {
    { "print",   PRINT,  api_mode_context_qual_print},
    { "create",  CREATE, api_mode_context_qual_create},
    { "var",     VAR,    api_mode_context_qual_variable}
};

#define PRIVATE_CMD(idx) { api_mode_private[idx].name, api_mode_private+idx }

STATIC api_mode_cint_dt_db_entry_t api_mode_command[COUNTOF(api_mode_private)];

STATIC void
_api_mode_init_private(void)
{
    int i;

    for (i=0; i<COUNTOF(api_mode_private); i++) {
        sal_memset(api_mode_command + i, 0, sizeof(api_mode_command[0]));
        api_mode_command[i].name = api_mode_private[i].name;
        api_mode_command[i].private = api_mode_private + i;
    }
}

/*
  Try to match identifier arguments in arg to a CINT function. Return
  TRUE if there is a match. 'out' is set to the first non-identfier
  argument from 'in', and 'dt' is the CINT function datatype.

  'buffer' contains the matched function name.

  If help, prompt-assignment, or keyword/value arguments, then only the
  identifiers are matched.  If positional arguments, then the arity must
  either match or be one greater if the first argument name is "unit".

 */



#define OP_PROMPT      1
#define OP_HELP        2
#define OP_KWARG       3

STATIC int
_pfn(void *item, void *target)
{
    const api_mode_cint_dt_db_entry_t *entry = item;
    api_mode_context_t *ctx = target;
    int match = sal_strcmp(entry->name, ctx->match);

    return match;
}


/* join IDENT args from 'tokens' separated by 'c' into ctx->join

   return API_MODE_E_NONE if successful, error if failed
*/
STATIC int
join_args(api_mode_context_t *ctx, api_mode_tokens_t *tokens, char c)
{
    int i, n, join, rv, len;
    char *s;
    const char *t;

    s = ctx->join;
    len = ctx->jlen;
    rv = API_MODE_E_NONE;

    /* sanity check */
    if (tokens->len != ctx->len) {
        sal_printf("join_args: token/context mismatchn\n");
        return API_MODE_E_FAIL;
    }

    for (i=0; i<tokens->len; i++) {
        if (i == 0 && ctx->info[i].grammar_type == INFO) {
            /* INFO */
            continue;
        }
        
        if (ctx->info[i].grammar_type != IDENT) {
            break;
        }

        t = tokens->token[i].str;
        n = sal_strlen(t);
        if (n < (len+2)) { /* one for joining, one for end-sf-string */
            sal_strcpy(s, t);
            s += n;
            len -= n;
            /* join if this is not the last token and the token
               following is an IDENT */
            join = i < ((tokens->len)-1) &&
                ctx->info[i+1].grammar_type == IDENT;

            if (join) {
                *s++ = c;
                len -= 1;
            }
        } else {
            /* out of room */
            rv = API_MODE_E_FAIL;
            break;
        }
    }
    *s = 0;

    return rv;
}

/* Join tokens and search for var

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/
STATIC int
_match_var_ctx(api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    return API_MODE_E_NONE; /* STUB */
}


/* Join tokens and search for var

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/
STATIC int
_match_create_ctx(api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    
    if (tokens->len == 3 && ctx->info[2].grammar_type == IDENT) {
        ctx->info[2].grammar_type = KEY;
    }
    return API_MODE_E_NONE; /* STUB */
}



/* Search for a matching function based on ctx

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/

/* reverse search horizon. Should be larger then the number of APIs that
   are strict substrings of each following API. */

#define HORIZON 20

STATIC int
_search_func_ctx(api_mode_context_t *ctx)
{
    int idx, rv;
    const char *name;

    rv = API_MODE_E_FAIL;
    idx = _shr_bsearch(api_cint.func.entry,
                       api_cint.func.count,
                       sizeof(*api_cint.func.entry),
                       ctx,
                       _pfn);

    if (idx < 0) {
        /* inexact match; see if the match buffer refers to the given
           index (if match buffer is shorter or equal) or the next
           index (if the match buffer is longer) */

        idx = -idx - 1;
        if (idx < api_cint.func.count) {
            name = api_cint.func.entry[idx].name;
            if (!strncmp(ctx->match, name, sal_strlen(name))) {
                ctx->exact = 1;
            } else {
                ctx->partial = !strncmp(ctx->match, name,
                                        sal_strlen(ctx->match));
            }
            rv = API_MODE_E_NONE;
        } else {
            rv = 1;
        }
    } else {
        ctx->exact = 1;
        rv = API_MODE_E_NONE;
    }

    if (rv == API_MODE_E_NONE) {
        int i;

        if (!ctx->partial && !ctx->exact) {
            /*
              'Over Matching' an API with identifiers as arguments

              There are five cases that can occur:

              (1) The function being matched is not a substring of a
                  subsequent function. The match index will point to the
                  function *following* the one corresponding to the
                  (ultimately) desired function.

                  Example: field qset t init qset
                           ----- ---- - ----

                  Match text:
                    field_qset_t_init_qset

                  Matches (*):
                    bcm_field_qset_t_init       [5/5]

              (2) The function being matched is a substring of a
                  subsequent function, and the desired function
                  concatenated with the identifier argument(s) sorts
                  *before* subsequent function.

                  Example: mpls info foo bar
                           ---- ----

                  Match text:
                    bcm_mpls_info_foo_bar

                  Matches (*):
                    bcm_mpls_info               [3/3]
                    bcm_mpls_info_t_init *      [3/5]
                    bcm_mpls_init               [2/3]

              (3) The function being matched is a substring of a
                  subsequent function, and the desired function
                  concatenated with the identifier argument(s) matches
                  exactly.

                  Example: mpls info t init
                           ---- ----
                  Match text:
                    bcm_mpls_info_t_init

                  Matches (*):
                    bcm_mpls_info               [3/3]
                    bcm_mpls_info_t_init *      [5/5]
                    bcm_mpls_init               [2/3]

              (4) The function being matched is a substring of a
                  subsequent function, and the desired function
                  concatenated with the identifier argument(s)
                  overmatches:
              
                  Example: mpls info test output
                           ---- ----
              
                  Match text:
                    bcm_mpls_info_test_output

                  Matches (*):
                    bcm_mpls_info               [3/3]
                    bcm_mpls_info_t_init        [3/5]
                    bcm_mpls_init *             [2/3]

              (5) The function being matched is a substring of a
                  subsequent function, and the desired function
                  concatenated with the identifier argument(s) sorts
                  after:
  
                  Example: mpls info zzz yyy
                           ---- ----
              
                  Match text:
                    bcm_mpls_info_zzz_yyy

                  Matches (*):
                    bcm_mpls_info               [3/3]
                    bcm_mpls_info_t_init        [3/5]
                    bcm_mpls_init *             [2/3]
              
            If the match is against the immediately following
            function, then the string being matched is longer, and a
            superset of the functions, i.e., it has arguments to the
            function.

            This means that an inexact match will overshoot the target
            by at least two entries. However, as in example (5) above,
            if the target is the shortest string of a sequence of
            larger strings, then the distance to search backwards
            increases by the number of 'superstrings', which is a
            property of API naming. For example:

              field group create qset
              
            initially matches against field_group_destroy, so the
            reverse search horizon is 4.
            
               field_group_create *
               field_group_create_id
               field_group_create_mode
               field_group_create_mode_id
               field_group_destroy


            */
            for (i=0; i < HORIZON && idx > 1; i++) {
                idx--;
                name = api_cint.func.entry[idx].name;
                ctx->more = !strncmp(ctx->match, name, sal_strlen(name));
                if (ctx->more) {
                    rv = 1; /* try again to reduce the overmatch */
                    break;
                }
            }
        }
        ctx->dt0 = api_cint.func.entry + idx;
        ctx->idx = idx;

    }

    return rv;
}

/* Join tokens and search for function

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/
STATIC int
_match_func_ctx(api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    int rv;

    rv = join_args(ctx, tokens, '_');
    if (API_MODE_SUCCESS(rv)) {
        rv = _search_func_ctx(ctx);
    }

    if (rv == API_MODE_E_NONE && ctx->exact && ctx->dt0->private != NULL) {
        /* private command handled by grammar */
        ctx->info[0].grammar_type = ctx->dt0->private->grammar_type;
        ctx->qual = ctx->dt0->private->qual;
    }

    return rv;
}

/* Initialize search context.

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/
STATIC int
_search_ctx(api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    int rv = API_MODE_E_FAIL;

    /* match based on context */
    switch (ctx->qual) {
    case api_mode_context_qual_variable:
    case api_mode_context_qual_print:
        rv = _match_var_ctx(tokens, ctx);
        break;
    case api_mode_context_qual_create:
        rv = _match_create_ctx(tokens, ctx);
        break;
    case api_mode_context_qual_unknown:
    case api_mode_context_qual_function:
        rv = _match_func_ctx(tokens, ctx);
        break;
    default:
        break;
    }

    if ((rv == API_MODE_E_NONE) && (!ctx->more || ctx->exact)) {
        if (ctx->len > 0 && ctx->dt0 != NULL && ctx->exact) {
            /* info structs are passed through to grammar; the first
               grammar token will carry the CINT datatype for the entire
               function. */
            ctx->info[0].dt = &ctx->dt0->dt;
        }
    } else if (rv > 0) {
        if (ctx->qual == api_mode_context_qual_function &&
            ctx->num_ident > 0 && ctx->more) {
            /* No match and there are arguments, so mark the last
               ident token as VALUE and try again. */
            
            ctx->info[ctx->num_ident-1].grammar_type = VALUE;
        }
    }
    return rv ;
}





STATIC int
_ctx_value(int idx, api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    /* VALUE+ident nodes only count towards command matching up to the
       first non-identifer, after which they count towards
       arguments. */
    if (tokens->token[idx].ident) {
        if (ctx->num_arg == 0) {
            ctx->info[idx].grammar_type = IDENT;
            ctx->num_ident++;
        } else {
            ctx->info[idx].grammar_type = VALUE;
            ctx->num_arg++;
        }
    } else {
        ctx->info[idx].grammar_type = CONSTANT;
        ctx->num_arg++;
    }

    return API_MODE_E_NONE;
}

STATIC int
_ctx_equal(int idx, api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    /* There is a potential shift/reduce conflict in the grammar when
       an '=' character is the last token in a sequence (used for
       prompt assignment). Explicitly make the last '=' as a PROMPT
       token to help disambiguate.
    */
    if (idx == tokens->len-1) {
        ctx->info[idx].grammar_type = PROMPT;
    } else {
        ctx->info[idx].grammar_type = tokens->token[idx].str[0];
        if (ctx->qual == api_mode_context_qual_function &&
            ctx->num_ident > 1 && idx > 0) {
            /* For function grammar, the token before '=' is key */
            if (ctx->num_arg == 0) {
                /* this is the first argument */
                ctx->num_arg++;
                ctx->num_ident--;
            }
            /* The token before '=' is a key */
            ctx->info[idx-1].grammar_type = KEY;
        }
    }

    return API_MODE_E_NONE;
}

STATIC int
_ctx_comma(int idx, api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    /* tokens surrounding comma are values */
    if (ctx->num_arg > 1 &&
        idx > 1 &&
        idx < tokens->len &&
        tokens->token[idx-1].token_type == API_MODE_TOKEN_TYPE_VALUE &&
        tokens->token[idx+1].token_type == API_MODE_TOKEN_TYPE_VALUE) {
        ctx->info[idx-1].grammar_type = ITEM;
        ctx->info[idx].grammar_type = tokens->token[idx].str[0];
        ctx->info[idx+1].grammar_type = ITEM;
    }
    return API_MODE_E_NONE;
}

STATIC int
_ctx_separator(int idx, api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    int info = 0;

    if (tokens->token[idx].str[0] == '=') {
        _ctx_equal(idx, tokens, ctx);
    } else if (tokens->token[idx].str[0] == ',') {
        _ctx_comma(idx, tokens, ctx);
    } else {
        /* all other separators stand as themselves */
        ctx->info[idx].grammar_type = tokens->token[idx].str[0];
        if (idx == 0 && tokens->token[idx].str[0] == INFO) {
            info = 1;
        }
    }

    if (tokens->token[idx].str[0] == '.') {
        ctx->num_ident++;
    } else if (!info) {
        ctx->num_arg++;
    }

    return API_MODE_E_NONE;
}
    
STATIC int
_ctx_quote(int idx, api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    ctx->info[idx].grammar_type = CONSTANT;
    ctx->num_arg++;

    return API_MODE_E_NONE;
}

STATIC void
_api_mode_check(void)
{
#if 0
    int i;
    const char *p, *q;

    for (i=0; i<api_cint.func.count-1; i++) {
        p = api_cint.func.entry[i].dt.desc.basetype;
        q = api_cint.func.entry[i+1].dt.desc.basetype;
        if (p != NULL && q != NULL && !strncmp(p, q, sal_strlen(p))) {
            sal_printf("%s substring of %s\n", p, q);
        }
    }
    for (i=0; i<api_cint.func.count; i++) {
        p = api_cint.func.entry[i].dt.desc.basetype;
        if (p != NULL) {
            sal_printf("::: %s\n", p);
        }
    }
#endif
}

STATIC int
_api_mode_local(api_mode_cint_dt_db_t *db)
{
    int i;

    for (i=0; i<COUNTOF(api_mode_command); i++) {
        sal_memcpy((void *)(db->entry + db->idx),
                   api_mode_command + i,
                   sizeof(*api_mode_command));
        db->idx++;
    }

    return i;
}

STATIC const char *
_api_mode_dt_name(const cint_datatype_t *dt)
{
    const char *name;

    if ((dt->flags & CINT_DATATYPE_F_CONSTANT) != 0) {
        name = dt->basetype.cp->name;
    } else if ((dt->flags & CINT_DATATYPE_F_TYPEDEF) != 0) {
        name = dt->desc.name;
    } else {
        name = dt->desc.basetype;
    }

    return name;
}

STATIC int
_api_mode_cmp_dt(void *a, void *b)
{
    const api_mode_cint_dt_db_entry_t *ea, *eb;
    int cmp;

    ea = (const api_mode_cint_dt_db_entry_t *)a;
    eb = (const api_mode_cint_dt_db_entry_t *)b;

    cmp = sal_strcmp(ea->name, eb->name);

    if (cmp == 0) {
        int pa, pb;

        pa = ea->name != _api_mode_dt_name(&ea->dt);
        pb = eb->name != _api_mode_dt_name(&eb->dt);

        /* sort full function names before aliases */
        cmp = (pa - pb);

        if (cmp == 0) {
            cmp = ea->dt.flags - eb->dt.flags;

            if (cmp == 0 && (ea->dt.flags & CINT_DATATYPE_F_TYPEDEF) != 0) {
                
                cmp = sal_strcmp(ea->dt.desc.basetype,
                                 eb->dt.desc.basetype);
            }
        }
    }

    return cmp;
}

/* return TRUE if function is eligible for API mode matching */
STATIC int
_api_mode_eligible_func(const cint_datatype_t* dt)
{
    cint_parameter_desc_t *desc = dt->basetype.fp->params;

    /* must have some parameters */
    if (desc == NULL) {
        return FALSE;
    }

    /* must have a return type specified */
    if (desc->basetype == NULL) {
        return FALSE;
    }

    for (desc++; desc->basetype != NULL; desc++) {
    /* The flags parameters for all arguments be non-zero.  This is
       the case for PAPI generated interfaces, but not necessarily the
       case for hand generated interfaces, or interfaces generated by
       other tools. */
        if (desc->flags == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

/* Add eligible CINT functions to internal database */
STATIC int
_api_mode_scan_dt(void* cookie, const cint_datatype_t* dt)
{
    api_mode_cint_dt_db_t *db = cookie;

    
    if (db == NULL) {
        return api_mode_unexpected();
    }

    if (_api_mode_eligible_func(dt)) {
        if (db->idx >= db->count) {
            return api_mode_unexpected();
        }
        
        sal_memcpy((void *)&db->entry[db->idx].dt, dt, sizeof(*dt));
        db->entry[db->idx].name = _api_mode_dt_name(dt);
        db->entry[db->idx].private = NULL;
        db->idx++;
    }

    return API_MODE_E_NONE;
}

/* Add CINT functions matching prefix to internal database with
   prefix stripped off. */
STATIC int
_api_mode_scan_at(void* cookie, const cint_datatype_t* dt)
{
    api_mode_cint_dt_db_t *db = cookie;
    const char *name;
    int t1;
    int t2;

    if (db == NULL) {
        return api_mode_unexpected();
    }

    if (_api_mode_eligible_func(dt)) {

        if (db->idx <  db->count) {
            return api_mode_unexpected();
        }

        if (db->idx >= (db->count*2)) {
            return api_mode_unexpected();
        }

        name = _api_mode_dt_name(dt);

        if (api_cint.prefix_len > 0) {
            t1 = !strncmp(name, api_cint.prefix,   api_cint.prefix_len);
            t2 = !strncmp(name, api_cint.prefix_u, api_cint.prefix_len);
        } else {
            t1 = 1;
            t2 = 0;
        }

        if (t2) {
            const char *p;
            /* Check to see if there are any lower case letters */
            for (p=name; *p; p++) {
                if (*p >= 'a' && *p <= 'z') {
                    sal_printf("@@@ mixed case: %s\n", name);
                    break;
                }
            }
        }
        if (sal_strlen(name) > api_cint.prefix_len && (t1 || t2)) {
            sal_memcpy((void *)&db->entry[db->idx].dt, dt,
                       sizeof(*dt));
            db->entry[db->idx].name =
                dt->desc.basetype + api_cint.prefix_len;
            db->entry[db->idx].private = NULL;
            db->idx++;
        }
    }

    return API_MODE_E_NONE;
}

STATIC int
_api_mode_count_dt(void* cookie, const cint_datatype_t* dt)
{
    api_cint_db_t *db = cookie;

    /* only count eligible functions */
    if (_api_mode_eligible_func(dt)) {
        db->func.count++;
    }

    return API_MODE_E_NONE;
}

STATIC int
_api_mode_function_update(void)
{
    int rv;
    int flags = CINT_DATATYPE_FLAGS_FUNC;
    int n;

    /* Build function list */
    api_cint.func.count = 0;
    api_cint.func.idx = 0;
    rv = cint_datatype_traverse(flags, _api_mode_count_dt, &api_cint);
    if (rv == CINT_E_NONE && api_cint.func.count > 0) {
        /* double allocation for worst case aliases, plus some local cmds */
        n  = api_cint.func.count * 2;
        n += COUNTOF(api_mode_command);
        if (n > api_cint.func.alloc) {
            if (api_cint.func.entry) {
                sal_free(api_cint.func.entry);
            }
            api_cint.func.entry =
                sal_alloc(n * sizeof(*api_cint.func.entry), "apimode");
            if (api_cint.func.entry == NULL) {
                api_cint.func.alloc = 0;
                return API_MODE_E_MEMORY;
            }
            api_cint.func.alloc = n;
        }
        rv = cint_datatype_traverse(flags, _api_mode_scan_dt, &api_cint);
        if (rv == CINT_E_NONE) {
            rv = cint_datatype_traverse(flags, _api_mode_scan_at, &api_cint);
        }
        if (rv == CINT_E_NONE) {
            /* update actual count */
            api_cint.func.count = api_cint.func.idx;

            /* add local commands */
            api_cint.func.count += _api_mode_local(&api_cint.func);

            _shr_sort(api_cint.func.entry, api_cint.func.count,
                      sizeof(*api_cint.func.entry), _api_mode_cmp_dt);
        }
    }

    return rv;
}

/* Determine command context

   returns
              -error: some error
     API_MODE_E_NONE: match found
                   1: no match
   
*/
int
api_mode_contextualizer(api_mode_tokens_t *tokens, api_mode_context_t *ctx)
{
    int i;
    int rv;
    int ctx_len, alloc_len;

    rv = API_MODE_E_FAIL;

    /* update CINT info if changed */
    if (api_cint.dt_changed != 0) {
        rv = _api_mode_function_update();
        if (API_MODE_FAILURE(rv)) {
            return rv;
        }
        api_cint.dt_changed = 0;
    }
    sal_memset(ctx, 0, sizeof(*ctx));

    ctx->jlen = CONTEXT_MAX_BUFFER;

    ctx_len = tokens->len * sizeof(ctx->info[0]);
    alloc_len = ctx_len;
    alloc_len += CONTEXT_MAX_PFX;
    alloc_len += CONTEXT_MAX_BUFFER;
    ctx->alloc = sal_alloc(alloc_len, "api_mode_contextualizer");

    if (ctx->alloc == NULL) {
        return API_MODE_E_MEMORY;
    }

    sal_memset(ctx->alloc, 0, alloc_len);
    ctx->qual = api_mode_context_qual_unknown;
    ctx->info = (void *)ctx->alloc;
    ctx->len = tokens->len;
    ctx->match = ctx->alloc + ctx_len;
    ctx->join = ctx->match;
    ctx->db = &api_cint.func;
    ctx->mlen = alloc_len - ctx_len - 1;

    /* determine grammar qualification if needed */
    if (tokens->len > 0 && tokens->token[0].ident) {
        const char *token0 = tokens->token[0].str;

        /* assume function */
        ctx->qual = api_mode_context_qual_function;
        /* There's at least one token ident */
        for (i=0; i<COUNTOF(api_mode_private); i++) {
            if (!sal_strcmp(token0, api_mode_private[i].name)) {
                ctx->qual = api_mode_private[i].qual;
                ctx->info[0].grammar_type =
                    api_mode_private[i].grammar_type;
                break;
            }
        }
    }

    /* set grammar types based on token types and token context */
    for (i=0; i<tokens->len; i++) {
        /* if grammar_type is already set, don't set it again */
        if (ctx->info[i].grammar_type != 0) {
            continue;
        }
        switch (tokens->token[i].token_type) {
        case API_MODE_TOKEN_TYPE_VALUE:
            _ctx_value(i, tokens, ctx); 
            break;
        case API_MODE_TOKEN_TYPE_QUOTE:
            _ctx_quote(i, tokens, ctx); 
            break;
        case API_MODE_TOKEN_TYPE_SEPARATOR:
            _ctx_separator(i, tokens, ctx); 
            break;
        case API_MODE_TOKEN_TYPE_COMMENT:
            /* skip */
            break;
        case API_MODE_TOKEN_TYPE_ERROR:
        default:
            /* game over! */
            return API_MODE_E_FAIL;
        }
    }

    /* Now see if there is a matching function. Allow the "no
       identifiers" case so this will work correctly on empty inputs
       with a default prefix.
    */
    for (; ctx->num_ident>=0; ctx->num_ident--, ctx->num_arg++) {
        if ((rv=_search_ctx(tokens, ctx)) <= 0) {
            break;
        }
    }

    return rv;
}

int
api_mode_contextualizer_free(api_mode_context_t *ctx)
{
    if (ctx->alloc != NULL) {
        sal_free(ctx->alloc);
    }
    ctx->alloc = NULL;

    return API_MODE_E_NONE;
}

STATIC int
_api_mode_cint_event(void* cookie, cint_interpreter_event_t event)
{
    api_cint_db_t *db = (api_cint_db_t *)cookie;

    switch (event) {
    case cintEventReset:
        db->dt_changed = 1;
        db->var_changed = 1;
        break;
    case cintEventDatatypeAdded:
        db->dt_changed = 1;
        break;
    case cintEventGlobalVariableAdded:
        db->var_changed = 1;
        break;
    default:
        /* ignore */
        break;
    }

    return API_MODE_E_NONE;
}

int
api_mode_context_initialize(void)
{
    sal_memset(&api_cint, 0, sizeof(api_cint));

    api_cint.prefix   = "bcm_";
    api_cint.prefix_u = "BCM_";
    api_cint.prefix_len = sal_strlen(api_cint.prefix);
    api_cint.dt_changed = 1;
    api_cint.var_changed = 1;

    if (sal_strlen(api_cint.prefix_u) != api_cint.prefix_len) {
        return api_mode_unexpected();
    }

    _api_mode_init_private();

    cint_interpreter_event_register(_api_mode_cint_event, &api_cint);

    _api_mode_check();

    
    return API_MODE_E_NONE;
}

int
api_mode_context_uninitialize(void)
{
    cint_interpreter_event_unregister(_api_mode_cint_event);

    /* Destroy data */
    if (api_cint.func.entry != NULL) {
        sal_free(api_cint.func.entry);
    }
    sal_memset(&api_cint, 0, sizeof(api_cint));
    
    return API_MODE_E_NONE;
}

