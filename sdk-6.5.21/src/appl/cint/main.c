/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        main.c
 * Purpose:     Standalone CINT application
 */

#include "cint_config.h"

#if CINT_CONFIG_INCLUDE_MAIN == 1

#include "cint_interpreter.h"
#include "cint_ast.h"
#include "cint_parser.h"
#include "cint_variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>

#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
extern int cint_test_data_init(void);
extern int cint_test_data_deinit(void);
#endif

/*
 * Program Options
 */

static int _opt_ast = 0; 
static int _opt_str = 0; 
static int _opt_prompt = 1; 

#ifdef YYDEBUG
extern int cint_c_debug; 
#endif

static struct option long_options[] = 
    {
        { "atrace", no_argument, &interp.debug.atrace, 1 }, 
        { "ftrace", no_argument, &interp.debug.ftrace, 1 }, 
        { "dtrace", no_argument, &interp.debug.dtrace, 1 }, 
        { "print",  no_argument, &interp.print_expr, 1 }, 
        { "ast", no_argument, &_opt_ast, 1 }, 
        { "str", no_argument, &_opt_str, 1 }, 
        { "noprompt", no_argument, &_opt_prompt, 0 }, 
#ifdef YYDEBUG
        { "yydebug", no_argument, &cint_c_debug, 1 }, 
#endif
        { 0, 0, 0, 0 }, 
    }; 
       

static char *
santize_filename(char *s)
{
    return s ? ((CINT_STRLEN(s) > PATH_MAX) ? NULL : s) : NULL;
}

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
static void
init_include_path(void)
{
    char *path;

    path = getenv("CINT_INCLUDE_PATH");
    if (path) {
        cint_interpreter_include_set(path); 
    }
}
#endif

int 
main(int argc, char* argv[])
{
    FILE* fp = NULL; 
    char *file_arg;

    while(1) {
        
        int opt_index; 
        int c; 
        
        c = getopt_long(argc, argv, "", long_options, &opt_index); 
        
        if(c == -1) {
            break; 
        }
    }   
    file_arg = santize_filename(argv[optind]);

    /*
     * What actions?
     */
    if(_opt_ast) {

        /* Do not interpret. Just parse and dump the ASTs */
        cint_cparser_t* cp = cint_cparser_create(); 
        cint_ast_t* ast; 

        if(cp == NULL) {
            CINT_PRINTF("** error creating parser object\n"); 
            return -1; 
        }

        if(file_arg) {
#if CINT_CONFIG_FILE_IO == 1
            /* Assume file argument */
            fp = CINT_FOPEN(file_arg, "r"); 
            
            if(fp) {
                cint_cparser_start_handle(cp, fp); 
            }
            else {
                /* Assume immediate string argument */
                cint_cparser_start_string(cp, file_arg); 
            }
#else
            CINT_PRINTF("File IO not supported\n");
            return 1;
#endif
        }
        else {
            /* Assume stdin */
            cint_cparser_start_handle(cp, NULL); 
        }       

        /*
         * Parse translation units until EOF or error
         */
        while((ast = cint_cparser_parse(cp))) {
            cint_ast_dump(ast, 0);            
        }       
        
        cint_cparser_destroy(cp); 

#if CINT_CONFIG_FILE_IO == 1
        if(fp) {
            CINT_FCLOSE(fp); 
        }
#endif

        return 0; 
    } else if (_opt_str) {
        int rv = -1;
        cint_ast_t* ast;

        /* interpret using string interface */
        if (optind < argc) {
            cint_interpreter_init(); 
            ast = cint_interpreter_parse_string(argv[optind]);
            if (ast) {
                rv = cint_interpreter_evaluate(ast);
            }
        }
        return (rv != 0);
    }
    
#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
    if (cint_test_data_init() != CINT_E_NONE) {
        return (1);
    }
#endif
    cint_interpreter_init(); 
#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
    init_include_path();
#endif
#if CINT_CONFIG_FILE_IO == 1
    if(file_arg) {
        fp = CINT_FOPEN(file_arg, "r"); 
    }   
#endif

    cint_interpreter_parse(fp, (_opt_prompt) ? "cint> " : NULL, 0, NULL); 
    
#if CINT_CONFIG_FILE_IO == 1
    if(fp) {
        CINT_FCLOSE(fp); 
    }  
#endif

    cint_datatype_clear();
    cint_variable_clear();
#if CINT_CONFIG_INCLUDE_TEST_DATA == 1
    if (cint_test_data_deinit() != CINT_E_NONE) {
        return (1);
    }
#endif

    return 0;
}

#else /* CINT_CONFIG_INCLUDE_MAIN */
typedef int cint_main_c_not_empty; /* Make ISO compilers happy. */
#endif
