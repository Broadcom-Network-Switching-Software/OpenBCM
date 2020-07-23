/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        completion.c
 * Purpose:     API mode completion
 */

#include "bcm/port.h"
#include "sal/core/alloc.h"
#include "sal/core/libc.h"
#include "sal/appl/io.h"
#include "shared/util.h"
#include "appl/diag/shell.h"
#include "appl/diag/system.h"
#include "sal/appl/editline/editline.h"
#include "sal/appl/sal.h"
#include "shared/util.h"
#include "context.h"
#include "completion.h"
#include "cint_porting.h"

#include <cint_porting.h>
#include <cint_internal.h>
#include <cint_ast.h>
#include <cint_eval_asts.h>
#include <cint_eval_ast_print.h>

/* forward references */

STATIC char *no_complete(char *pathname, int *unique);
STATIC int no_list(char *pathname, char ***avp);
STATIC char *cmd_api_complete(char *pathname, int *unique);
STATIC int cmd_api_list_possib(char *pathname, char ***avp);


/* Area to save editline info */

STATIC char *(*cmd_api_rl_complete_save)(char *, int *);
STATIC int  (*cmd_api_rl_list_possib_save)(char *, char ***);

typedef char *(*rl_complete_f)(char *, int *);
typedef int (*rl_list_possib_f)(char *, char ***);

typedef struct cmd_api_ctx_s {
    api_mode_completion_type_t ctype; /* completion type */
} cmd_api_ctx_t;

typedef struct rl_completion_handler_s {
    rl_complete_f       complete;
    rl_list_possib_f    list;
} rl_completion_handler_t;

STATIC rl_completion_handler_t rl_handler[] = {
    { no_complete,              no_list },
    { cmd_api_complete,         cmd_api_list_possib }
};

STATIC cmd_api_ctx_t cmd_api_context;

#define MAX_FUNC_LEN CINT_CONFIG_MAX_VARIABLE_NAME

typedef struct {
    char *alloc;        /* memory allocation */
    char *line;         /* input line to complete */
    char *match;        /* tokenized and reassembled input line */
    int ilen;           /* length of input line */
    int ispace;         /* input line space terminated */
    int mlen;           /* length of match */
    int *start;         /* completion stem start */
    int *end;           /* completion stem end */
    char *func;         /* func scratch buffer */
    api_mode_scanner_t scan;    /* tokens */
    api_mode_context_t ctx;     /* token context */
} completion_data_t;

STATIC int
completion_func(api_mode_cint_dt_db_entry_t *entry)
{
    return
        (entry->private != NULL) ||
        ((entry->dt.flags & CINT_DATATYPE_FLAGS_FUNC) != 0);
}

STATIC int
completion_data_free(completion_data_t *cd)
{
    if (cd->alloc != NULL) {
        sal_free(cd->alloc);
        cd->alloc = NULL;
    }
    api_mode_contextualizer_free(&cd->ctx);
    (void)api_mode_tokenizer_free(&cd->scan.tok);

    return 0;
}

STATIC int
completion_data_init(completion_data_t *cd)
{
    sal_memset(cd, 0, sizeof(*cd));
    cd->alloc = sal_alloc(MAX_FUNC_LEN, "api_mode");

    if (cd->alloc != NULL) {
        cd->func = cd->alloc;
    }

    return (cd->alloc == NULL) ? -1 : 0;
}

STATIC void
strtr(char *s, int from, int to)
{
    for (;*s != 0;s++) {
        if (*s == from) {
            *s = to;
        }
    }
}

STATIC int
try_match(const char *s1, const char *s2, int start, int end)
{
    int try;

    for (try = start; try < end; try++) {
        if (s1[try] != s2[try]) {
            /* char didn't match. back up and try the next name. */
            end = try;
            break;
        }
    }

    return end;
}

STATIC char *
cmd_api_try_completion(completion_data_t *cd)
{
    int start, end, idx, midx;
    const char *p;
    char *cpl;

    idx = cd->ctx.idx;
    cpl = NULL;
    sal_strncpy(cd->func, cd->ctx.db->entry[idx].name, MAX_FUNC_LEN-1);
    start = cd->mlen;
    end = sal_strlen(cd->func);

    midx = -1;
    for (;idx < cd->ctx.db->count; idx++) {
        if (!completion_func(cd->ctx.db->entry + idx)) {
            continue;
        }
        p = cd->ctx.db->entry[idx].name;
        /*
          If this is still a substring of what is being completed,
          then find a completion suffix. This will be the longest
          substring in common with all function names that have the
          string being completed as a prefix.

          The completion will not be longer than the first function
          name at the search point, so it is sufficient to base the
          completion buffer size string on that. The reason it won't
          be any longer is because any longer completion would
          necessarily have the current name as a substring.
           
        */
        if (strncmp(p, cd->match, cd->mlen)) {
            /*
             Input is not a substring of the current function at all,
             so a match is no longer possible.
            */
            break;
        }

        end = try_match(cd->func, p, start, end);

        if (end <= start) {
            /* nothing common, so nothing to complete */
            cpl = NULL;
            break;
        } else {
            /* found something that will complete. terminate and return
               the substring */
            cd->func[end] = 0;
            cpl = cd->func + start;
            midx = idx; /* save index of match */
        }
    }

    if (cd->line && cpl) {
        int trim = 1;
        /* if this completion fully matched something, then append a
           trailing space */
        if (cpl && midx >= 0) {
            /* if the function is fully the same as the function matched,
               and is not a substring of a subsequent match, ass a separator */
            if (!sal_strcmp(cd->func, cd->ctx.db->entry[midx].name) &&
                strncmp(cd->func, cd->ctx.db->entry[midx+1].name,
                        sal_strlen(cd->func))) {
                    CINT_STRCAT(cd->func, " ");
                    trim = 0;
            }
        }
        
        /* Sorry, there *was* nothing to complete if the input line
           ended in whitespace and the completion was more
           whitespace. */
        if (trim && cd->ispace && cpl[0] == '_') {
            cpl = NULL;
        }
    }

    return cpl;
}

STATIC int
near_match(completion_data_t *cd, char *line)
{
    int err;

    err = api_mode_tokenizer(line, &cd->scan.tok);
    if (!err) {
        /* contextualizer may not match anything, but that's OK;
           it returns enough information to allow listing and
           completion to work. */
        (void)api_mode_contextualizer(&cd->scan.tok, &cd->ctx);
    }

    if (cd->ctx.dt0 == NULL || !completion_func(cd->ctx.dt0)) {
        /* no datatype found, or datatype was not a function */
        return -1;
    }

    cd->line = line;
    cd->ilen = sal_strlen(cd->line);
    cd->match = cd->ctx.match;
    cd->mlen = sal_strlen(cd->match);
    cd->start = NULL;
    cd->end = NULL;
    cd->ispace = (cd->ilen > 0) && (line[cd->ilen - 1] == ' ');
    return 0;
}

STATIC int
over_match(completion_data_t *cd, char ***listp)
{
    char **list;
    int len;

    len = 0;
    list = sal_alloc(sizeof(char *)*1, "api");
    if (list) {
        list[0] = sal_strdup("complete but not unique");
        len++;
        *listp = list;
    }
    return len;
}



STATIC int
exact_match(completion_data_t *cd, char ***listp)
{
    char **list;
    int len;

    len = 0;
    list = sal_alloc(sizeof(char *)*1, "api");
    if (list) {
        list[0] = sal_strdup("complete");
        len++;
        *listp = list;
    }

    return len;
}

#define MAXMATCH 100

STATIC int
list_collect(completion_data_t *cd, int start, char **list)
{
    int idx, g_start, fnlen;
    int add_group, g_end;
    char *group;
    const char *fn;
    int count;

    group = NULL;
    g_start = -1;
    count = 0;
    for (idx = start; idx < cd->ctx.db->count; idx++) {

        /* only match functions */
        if (!completion_func(cd->ctx.db->entry + idx)) {
            continue;
        }
        fn = cd->ctx.db->entry[idx].name;
        
        if (strncmp(fn, cd->match, cd->mlen)) {
            /* no longer matching prefix */
            break;
        }

        fnlen = sal_strlen(fn);
        if (fnlen <= cd->mlen) {
            /* function is the same length or shorter than the input,
               to there's nothing additional that can match. */
            continue;
        }

        g_start = cd->mlen;
        /* if not at the end of a word */
        if (fn[g_start] != '_') {
            /* search backwards for beginning of word */
            for (; g_start > 0; g_start--) {
                if (fn[g_start] == '_') {
                    g_start++;
                    break;
                }
            }
        } else /* if (cd->ispace) */ {
            /* at a word break, so skip the break and start on the
               following word. */
            g_start++;
        }
        add_group = 0;

        /*

        Find the end of the word in the current function. Match
          against the current group if there is one to see if the word
          in this function needs to be added as a new group.
         */
        for (g_end = g_start; g_end < fnlen; g_end++) {
            if (fn[g_end] == 0 || fn[g_end] == '_') {
                /* end-of-string or word boundary */
                break;
            } else if (!add_group &&
                       (group == NULL ||
                        group[g_end - g_start] != fn[g_end])) {
                add_group = 1;
            }
        }

        if (add_group) {
            sal_strncpy(cd->func, fn, MAX_FUNC_LEN-1);
            cd->func[g_end] = 0;
            group = sal_strdup(cd->func + g_start);
            list[count] = group;
            count++;
            if (count >= MAXMATCH) {
                break;
            }
        }
    }

    return count;
}


/* Function names are composed of words separated by '_' characters.
   List are either lists of suffix words that are at the same level
   as the input being matched, or lists at the next level.

*/

STATIC int
list_matches(completion_data_t *cd, char ***listp)
{
    int count;
    char **list;

    *cd->func = 0; /* terminate 'func' buffer */
    list = sal_alloc(sizeof(char *)*MAXMATCH, "api");
    if (list == NULL) {
        return 0;
    }

    count = list_collect(cd, cd->ctx.idx, list);

    *listp = list;
    return count;
}

/* Called from <TAB> */
STATIC char *
cmd_api_complete(char *pathname, int *unique)
{
    rl_input_state_t state;
    char *cpl;
    const char *target;
    completion_data_t cd;
    int err;

    COMPILER_REFERENCE(pathname);
    rl_input_state(&state);

    if (completion_data_init(&cd) < 0) {
        return NULL;
    }

    cpl = NULL;
    err = near_match(&cd, (char *)state.line);

    if (err) {
        completion_data_free(&cd);
        return NULL;
    }

    if (!completion_func(cd.ctx.dt0)) {
        /* matched, but not a function */
        return NULL;
    }

    if (!cd.ctx.exact) {
        /* No match; determine if there is a completion possible */
        target = cd.ctx.dt0->name;
        if (!strncmp(target, cd.match, cd.mlen)) {
            cpl = cmd_api_try_completion(&cd);
            if (cpl) {
                /* copy and convert to 'command line' format */
                cpl = sal_strdup(cpl);
                if (cpl) {
                    strtr(cpl, '_', ' ');
                }
            }
        }
    } else {
        /* there is an exact match, so expand the rest of the entry,
           and add a trailing space. */
        char *rest = (char *)cd.ctx.dt0->name + cd.mlen;
        int rlen = sal_strlen(rest) + 1 + 1;

        cpl = sal_alloc(rlen, "apimode");
        sal_strncpy(cpl,rest,rlen-1);
        if (cd.line[cd.ilen - 1] != ' ') {
            CINT_STRCAT(cpl, " ");
        }
    }
    *unique = 0;
    completion_data_free(&cd);
    return cpl;
}

/* Called from ^[^[ */
STATIC int
cmd_api_list_possib(char *pathname, char ***avp)
{
    rl_input_state_t state;
    char **list = NULL;
    completion_data_t cd;
    int llen, err;

    COMPILER_REFERENCE(pathname);
    rl_input_state(&state);

    if (completion_data_init(&cd) < 0) {
        return 0;
    }

    err = near_match(&cd, (char *)state.line);
    if (err) {
        completion_data_free(&cd);
        return 0;
    }
    if (!cd.ctx.exact) {
        /* inexact match */
        llen = list_matches(&cd, &list);
    } else if (cd.ctx.more) {
        llen = over_match(&cd, &list);
    } else {
        llen = exact_match(&cd, &list);
    }
    *avp = list;
    completion_data_free(&cd);
    return llen;
}

STATIC char *
no_complete(char *pathname, int *unique)
{
    COMPILER_REFERENCE(pathname);
    COMPILER_REFERENCE(unique);
    return NULL;
}

/* Called from ^[^[ */
STATIC int
no_list(char *pathname, char ***avp)
{
    COMPILER_REFERENCE(pathname);
    COMPILER_REFERENCE(avp);
    return 0;
}

STATIC char *
call_complete(char *pathname, int *unique)
{
    return rl_handler[cmd_api_context.ctype].complete(pathname, unique);
}

/* Called from ^[^[ */
STATIC int
call_list_possib(char *pathname, char ***avp)
{
    return rl_handler[cmd_api_context.ctype].list(pathname, avp);
}

int
api_mode_completion_initialize(void)
{
     /* Save readline state. This is called during initialization and
        after command processing (in case other commands use their own
        completion functions), so this function is very conservative
        and only saves an existing completion function if it is not a
        API mode function.  */

    if (rl_complete != call_complete) {
        /* Use our completion function */
        cmd_api_rl_complete_save = rl_complete;
        rl_complete = call_complete;
    }

    if (rl_list_possib != call_list_possib) {
        /* Use our listing function */
        cmd_api_rl_list_possib_save = rl_list_possib;
        rl_list_possib = call_list_possib;
    }

    api_mode_completion_set(api_mode_completion_api);

    return BCM_E_NONE;
}

int
api_mode_completion_uninitialize(void)
{
    /* Restore readline state */
    rl_complete = cmd_api_rl_complete_save;
    rl_list_possib = cmd_api_rl_list_possib_save;

    return BCM_E_NONE;
}

/* set completion type to that indicated, returning previous */
int
api_mode_completion_set(api_mode_completion_type_t ctype)
{
    api_mode_completion_type_t current = cmd_api_context.ctype;
    cmd_api_context.ctype = ctype;

    return current;
}

