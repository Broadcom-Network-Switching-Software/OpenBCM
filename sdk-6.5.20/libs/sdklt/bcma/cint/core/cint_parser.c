/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_parser.c
 * Purpose:     CINT parser
 */

#ifdef INCLUDE_EDITLINE
#include <sal/appl/editline/editline.h>
#endif
#include "cint_parser.h"
#include "cint_porting.h"

static const char* __prompt = NULL;
static int include_depth = 0;

/*
 * These are extern'ed manually to avoid conflicts between the scanner and parser headers.
 */
extern int cint_c_lex_init(void** scanner);
int cint_c_scanner_finish(void *scanner);

extern void* cint_c_pstate_new(void);
extern int   cint_c_pull_parse(void* handle, void* scanner, void* ctrl);
extern int   cint_c_pstate_delete(void* handle);
extern int cint_c_scanner_start(void*, void*);
extern void *cint_c__scan_bytes(const char *str, int len, void *scanner);
extern void cint_c_set_lineno(int line_number, void *yyscanner);
extern void cint_c_set_column(int column_no , void *yyscanner);


/*
 * Create a C Parser instance
 */
cint_cparser_t*
cint_cparser_create(void)
{
    cint_cparser_t* cp = CINT_MALLOC(sizeof(*cp));

    if(cp == NULL) {
        return NULL;
    }

    CINT_MEMSET(cp, 0, sizeof(*cp));

    /* Create the scanner instance */
    cint_c_lex_init(&cp->scanner);

    if(cp->scanner == NULL) {
        cint_cparser_destroy(cp);
        return NULL;
    }

    /* Create a parser instance */
    cp->parser = cint_c_pstate_new();

    if(cp->parser == NULL) {
        cint_cparser_destroy(cp);
        return NULL;
    }

    return cp;
}

/*
 * Destroy a C Parser instance
 */
int
cint_cparser_destroy(cint_cparser_t* cp)
{
    if (cp == NULL) {
        return 0;
    }

    if (cp->scanner) {
        cint_c_scanner_finish(cp->scanner);
    }

    if (cp->parser) {
        cint_c_pstate_delete(cp->parser);
    }

    CINT_FREE(cp);

    return 0;
}

int
cint_cparser_start_handle(cint_cparser_t* cp, void* handle)
{
    if(cp == NULL) {
        return -1;
    }

    cint_c_scanner_start(handle, cp->scanner);

    return 0;
}

int
cint_cparser_start_string(cint_cparser_t* cp, const char* string)
{
    if (cp == NULL) {
        return -1;
    }

    cint_c__scan_bytes(string, CINT_STRLEN(string), cp->scanner);
    /* cint_c__scan_buffer(), which is called by cint_c__scan_bytes,
       does not completely initialize YY_BUFFER_STATE; in particular,
       yylineno and yycolumn. This may cause accesses to uninitialized
       memory later on.  There's no Flex hook as of 2.5.35, so do
       further initialization here. */
    cint_c_set_lineno(1, cp->scanner);
    cint_c_set_column(0, cp->scanner);

    return 0;
}

cint_ast_t*
cint_cparser_parse(cint_cparser_t* cp)
{
    cint_ast_t* ast;

    if(cp == NULL) {
        return NULL;
    }

    cp->error = cint_c_pull_parse(cp->parser, cp->scanner, cp);
    ast = cp->result;
    cp->result = NULL;

    return ast;
}

cint_ast_t*
cint_cparser_parse_string(const char* string)
{
    int status;
    cint_ast_t* result = NULL;
    cint_cparser_t* cp = cint_cparser_create();

    cint_cparser_start_string(cp, string);

    for(;;) {

        status = cint_c_pull_parse(cp->parser, cp->scanner, cp);
        if(status == 0) {
            /* Successfully parsed a translation unit */

            if(cp->result == NULL) {
                /* EOF */
                break;
            }
            else {
                if(result == NULL) {
                    result = cp->result;
                }
                else {
                    if (cp->result != result) {
                        cint_ast_append(result, cp->result);
                    } else {
                        break;
                    }
                }
            }
        }
        else {
            /* Error */
            break;
        }
    }

    cint_cparser_destroy(cp);
    return result;
}


static int
cint_cparser_prompt(void)
{
    if(cint_cparser_interactive()) {
        CINT_PRINTF("%s", __prompt);
    }
    return 0;
}

int
cint_cparser_error(cint_cparser_t* cp)
{
    return cp->error;
}

void cint_cparser_fatal_error(char *msg)
{
    CINT_FATAL_ERROR(msg);
}

void *
cint_cparser_alloc(unsigned int size)
{
    return CINT_MALLOC(size);
}

#ifdef CINT_REALLOC

/* System fully supports malloc/realloc/free */

void
cint_cparser_free(void *ptr)
{
    CINT_FREE(ptr);
}

void *
cint_cparser_realloc(void *ptr,  unsigned int size)
{
    return CINT_REALLOC(ptr, size);
}

#else /* !CINT_REALLOC */

/*
   Some systems (like SDK SAL) do not support realloc. In that case,
   provide something that will work for FLEX, which is the only CINT
   component that needs it.

   This interface *just barely* supports the realloc semantics
   needed by the FLEX lexer.

     DO NOT USE FOR ANY OTHER PURPOSE

*/

typedef struct {
    void *ptr;
    unsigned int size;
} cint_alloc_info_t;

#define MAX_ALLOC_INFO 10

static cint_alloc_info_t cint_alloc_info[MAX_ALLOC_INFO];

static cint_alloc_info_t *
cint_cparser_realloc_find_slot(void *ptr)
{
    int i;
    cint_alloc_info_t *info = NULL;

    for (i=0; i<MAX_ALLOC_INFO; i++) {
        if (cint_alloc_info[i].ptr == ptr) {
            info = cint_alloc_info + i;
            break;
        }
    }

    return info;
}

void
cint_cparser_free(void *ptr)
{
    if (ptr != NULL) {
        cint_alloc_info_t *info;

        info = cint_cparser_realloc_find_slot(ptr);
        if (info != NULL) {
            info->ptr = NULL;
            info->size = 0;
        }
        CINT_FREE(ptr);
    }
}

void *
cint_cparser_realloc(void *ptr,  unsigned int size)
{
    cint_alloc_info_t *info;
    void *new_ptr = NULL;

    info = cint_cparser_realloc_find_slot(ptr);
    if (info != NULL) {
        new_ptr = cint_cparser_alloc(size);
        if (new_ptr != NULL) {
            if (ptr != NULL) {
                CINT_MEMCPY(new_ptr, ptr, info->size);
            }
            /* update info */
            info->ptr = new_ptr;
            info->size = size;
        }
    }

    cint_cparser_free(ptr);

    return new_ptr;
}

#endif /* CINT_REALLOC */

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1

int
cint_cparser_input_readline(void *in, char* buf,
                            int* result, int max_size, int prompt)
{
    FILE *yyin = (FILE *)in;
    char* b;

    if (cint_cparser_interactive()) {
        if (prompt) {
            cint_cparser_prompt();
        }
        b = CINT_READLINE("");
        if (b) {
            /* Copy readline buffer */
            CINT_STRNCPY(buf, b, max_size-1);

#if defined(CINT_ADD_HISTORY) && CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY == 1
            CINT_ADD_HISTORY(buf);
#endif
            /* Restore newline. This is necessary to complete some scanner rules */
            CINT_STRCAT(buf, "\n");

            *result = CINT_STRLEN(buf);

            CINT_READLINE_FREE(b);
        } else {
            /* Ctrl-D/EOF */
            char *e = "exit;";
            int len = CINT_STRLEN(e);
            if (max_size > len) {
                /* buf is malloc'ed and may not be initialized, so
                   string copy operations here may make valgrind complain */
                CINT_MEMCPY(buf, e, len+1);
                *result = CINT_STRLEN(buf);
            }
        }
    } else {
#if CINT_CONFIG_FILE_IO == 1
        if (((*result = CINT_FREAD( buf, 1, max_size, yyin )) == 0) &&
            CINT_FERROR( yyin ) ) {
            return -1;
        }
#else
        return -1;
#endif
    }

    return 0;
}

#else

/* The default YY_INPUT behavior  */
int
cint_cparser_input_default(void *in, char* buf,
                           int* result, int max_size, int prompt)
{
    FILE *yyin = (FILE *)in;

    if (cint_cparser_interactive()) {
        if (prompt) {
            cint_cparser_prompt();
        }
        int c = '*', n;
        for ( n = 0; n < max_size &&
                  (c = CINT_GETC( yyin )) != EOF && c != '\n'; ++n ) {
            buf[n] = (char) c;
        }
        if ( c == '\n' ) {
            buf[n++] = (char) c;
        }

        if ( c == EOF && CINT_FERROR( yyin ) ) {
            return -1;
        }
        *result = n;
    }
    else {
#if CINT_CONFIG_FILE_IO == 1
        if ( ((*result = CINT_FREAD( buf, 1, max_size, yyin )) == 0) &&
              CINT_FERROR( yyin ) ) {
        return -1;
    }
#else
        return -1;
#endif
    }

    return 0;
}

/* The default YY_INPUT behavior with optional character echo */
int
cint_cparser_input_default_echo(void *in, char* buf,
                                int* result, int max_size,
                                int prompt, int echo)
{
    FILE *yyin = (FILE *)in;

    if (cint_cparser_interactive()) {
        if (prompt) {
            cint_cparser_prompt();
        }
        int c = '*', n;
        for ( n = 0; n < max_size &&
                  (c = CINT_GETC( yyin )) != EOF && c != '\n';
              ++n ) {
            buf[n] = (char) c;
            if(echo==0) CINT_PRINTF("%c", c);
        }
        if ( c == '\n' ) {
            buf[n++] = (char) c;
            if(echo==0) CINT_PRINTF("%c", c);
        }

        if ( c == EOF && CINT_FERROR( yyin ) ) {
            return -1;
        }
        *result = n;
    } else {
#if CINT_CONFIG_FILE_IO == 1
        if ( ((*result = CINT_FREAD( buf, 1, max_size, yyin )) == 0) &&
              CINT_FERROR( yyin ) ) {
        return -1;
    }
#else
        return -1;
#endif /* CINT_CONFIG_FILE_IO */
    }
    return 0;
}
#endif

void *
cint_cparser_memcpy(void *dst, const void *src, int len)
{
    return CINT_MEMCPY(dst, src, len);
}

void *
cint_cparser_memset(void *dst, int c, int len)
{
    return CINT_MEMSET(dst, c, len);
}

void
cint_cparser_message(const char *msg, int len)
{
    (void)CINT_PRINTF("*%s", msg);
}

const char *
cint_cparser_set_prompt(const char *prompt)
{
    const char *old_prompt = __prompt;

    __prompt = prompt;

    return old_prompt;
}

int
cint_cparser_interactive(void)
{
    return (include_depth == 0 && __prompt != NULL);
}

int cint_cparser_include(int level)
{
    int prev_depth = include_depth;

    include_depth += level;
    if (include_depth < 0) {
        CINT_FATAL_ERROR("include stack underflow");
    }

    return prev_depth;
}
