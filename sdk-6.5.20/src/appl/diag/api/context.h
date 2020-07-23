/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        context.h
 * Purpose:     API mode contextualizer interface
 */

#ifndef   _CONTEXT_H_
#define   _CONTEXT_H_

#include "tokenizer.h"
#include "api_mode_yy.h"
#include "api_grammar.tab.h"
#include "api_grammar.tab.h"
#include "cint_variables.h"

/* Return TRUE if 'c' is an uppercase letter */
#define UPPERCASE(c) (((c) >= 'A') && ((c) <= 'Z'))

#define CONTEXT_MAX_PFX 10
#define CONTEXT_MAX_BUFFER 256

typedef enum api_mode_context_qual_e {
    api_mode_context_qual_unknown,      /* Unknown grammar */
    api_mode_context_qual_function,     /* Function grammar */
    api_mode_context_qual_variable,     /* variable assignment grammar */
    api_mode_context_qual_print,        /* print grammar */
    api_mode_context_qual_create        /* type construction grammar */
} api_mode_context_qual_t;

typedef struct api_mode_private_s {
    const char *name;                   /* func name */
    enum yytokentype grammar_type;      /* grammar type */
    api_mode_context_qual_t qual;       /* qualification */
} api_mode_private_t;

/* Note - dt is a *copy*, not a pointer, because the dt passed to
   the traverse is constructed 'on the fly' and on the stack of the
   CINT traverse. */

typedef struct api_mode_cint_dt_db_entry_s {
    const char *name;                   /* CINT name */
    api_mode_private_t *private;        /* private API mode command */
    const cint_datatype_t dt;           /* CINT dt entry */
} api_mode_cint_dt_db_entry_t;

typedef struct api_mode_cint_dt_db_s {
    int count;                  /* total number of entries */
    int alloc;                  /* entried allocated */
    int idx;                    /* insertion index */
    int max;                    /* maximum length */
    api_mode_cint_dt_db_entry_t *entry;
} api_mode_cint_dt_db_t;

typedef struct api_mode_cint_var_db_s {
    int count;                  /* total number of entries */
    int alloc;                  /* entried allocated */
    int idx;                    /* insertion index */
    int max;                    /* maximum length */
    cint_variable_t *entry;
} api_mode_cint_var_db_t;

typedef struct api_mode_context_s {
    api_mode_context_qual_t qual; /* Context grammar qualification */
    char        *alloc;         /* allocation buffer */
    char        *match;         /* context match buffer */
    char        *join;          /* arg join pointer in match buffer */
    int         idx;            /* name table index */
    int         exact;          /* exact match */
    int         more;           /* overmatch */
    int         partial;        /* partial match */
    api_mode_cint_dt_db_entry_t *dt0; /* first DT match */
    api_mode_cint_dt_db_t *db;  /* function database */
    int         len;            /* number of info records */
    int         jlen;           /* join buffer length */
    int         mlen;           /* max match buffer length */
    int         num_ident;      /* number of leading identifiers */
    int         num_arg;        /* number of arguments */
    struct {
        enum yytokentype grammar_type;
        const cint_datatype_t *dt;
    } *info;
} api_mode_context_t;


extern int api_mode_contextualizer(api_mode_tokens_t *tokens,
                                   api_mode_context_t *ctx);

extern int api_mode_contextualizer_free(api_mode_context_t *ctx);

extern char *api_mode_bcm_prefix(char *buffer, int len, const char *name);

extern int api_mode_context_initialize(void);
extern int api_mode_context_uninitialize(void);

#endif /* _CONTEXT_H_ */
