/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_parser.h
 * Purpose:     CINT parser interfaces
 */

/*******************************************************************************
 *
 * CINT C Parser Interface
 *
 * This is the object-oriented interface to the lexical scanner
 * and generated parser state machine.
 */

#ifndef __CINT_CPARSER_H__
#define __CINT_CPARSER_H__

#include "cint_config.h"
#include "cint_ast.h"

typedef struct cint_cparser_s {

    /* Private Implementation Members */
    cint_ast_t* result;
    void* scanner;
    void* parser;
    int error;

} cint_cparser_t;


/*
 * Create a C Parser Instance
 */
extern cint_cparser_t* cint_cparser_create(void);


/*
 * Set the input file handle
 */
int cint_cparser_start_handle(cint_cparser_t* cp, void* handle);

/*
 * Set the input string
 */
int cint_cparser_start_string(cint_cparser_t* cp, const char* string);

/*
 * Parse one C translation unit from the current input handle.
 */
cint_ast_t* cint_cparser_parse(cint_cparser_t* cp);

/*
 * Parse one C translation unit from the given input string.
 */
cint_ast_t* cint_cparser_parse_string(const char* string);

/*
 * Retrieve the error code from the last request for parsing
 */
int cint_cparser_error(cint_cparser_t* cp);

/*
 * Destroy a C Parser Instance
 */
int cint_cparser_destroy(cint_cparser_t* cp);

/*
 * Handle a parser fatal error
 */
extern void cint_cparser_fatal_error(char *msg);

#if CINT_CONFIG_INCLUDE_PARSER_READLINE == 1
extern int cint_cparser_input_readline(void *in, char* buf,
                                       int* result, int max_size, int prompt);
#else

/*
 * Default character input
 */
extern int cint_cparser_input_default(void *in, char* buf,
                                      int* result, int max_size, int prompt);

/*
 * Default character input with echo
 */
extern int cint_cparser_input_default_echo(void *in,
                                           char* buf,
                                           int* result, int max_size,
                                           int prompt, int echo);

#endif /* CINT_CONFIG_INCLUDE_PARSER_READLINE */

/*
   Interfaces to library functions.  Declaring here avoids painful
   interactions with other header files and helps keep track of
   portability interfaces.
*/

extern void *cint_cparser_alloc(unsigned int size);
extern void *cint_cparser_realloc(void *ptr,  unsigned int size);
extern void cint_cparser_free(void *ptr);
extern void *cint_cparser_memcpy(void *dst, const void *src, int len);
extern void *cint_cparser_memset(void *dst, int c, int len);
extern void cint_cparser_message(const char *msg, int len);
extern int cint_cparser_interactive(void);
extern int cint_cparser_include(int level);
extern const char *cint_cparser_set_prompt(const char *prompt);

#endif /* __CINT_CPARSER_H__ */

