/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_config.h
 * Purpose:     CINT configuration
 */

#ifndef __CINT_CONFIG_H__
#define __CINT_CONFIG_H__

/*******************************************************************************
 *
 * Interpreter Configuration Settings
 */

#ifdef CINT_INCLUDE_CUSTOM_CONFIG
#include <cint_custom_config.h>
#endif

/*
 * Include or exclude the grammar parser.
 *
 * When the parser is included, you can call cint_interpreter_parser()
 * and allow it to parse and evaluate the input directly.
 *
 * When the parser is NOT included you must construct the cint_ast_t* syntax tree yourself
 * and pass it to cint_interpreter_evaluate().
 */
#ifndef CINT_CONFIG_INCLUDE_PARSER
#define CINT_CONFIG_INCLUDE_PARSER 1
#endif

/*
 *
 * Defines the number of dimensions supported for arrays.  This should be the
 * maximum expected sum of pointer depth and array dimensions.
 *
 * int a[5] would be a single dimension array.  int* a[5] would be considered
 * two dimensions [5][]. An int** would be represented internally as a two
 * dimension array with infinite dimension length [][]. int *[5] would be an
 * array of pointers with length five for the first dimensions and infinite for
 * the second [5][].  For a type X defined to be a six byte character array,
 * X **[5] would be represented internally as a four dimension type [5][][][6].
 */
#ifndef CINT_CONFIG_ARRAY_DIMENSION_LIMIT
#define CINT_CONFIG_ARRAY_DIMENSION_LIMIT 6
#endif

/*
 *
 * Pointers are represented as arrays of infinite dimension.
 */
#ifndef CINT_CONFIG_POINTER_INFINITE_DIMENSION
#define CINT_CONFIG_POINTER_INFINITE_DIMENSION (int)((unsigned int)(-1) >> 1)
#endif

/*
 *
 * Used for character buffers when appending text for printing purposes.
 */
#ifndef CINT_CONFIG_MAX_STACK_PRINT_BUFFER
#define CINT_CONFIG_MAX_STACK_PRINT_BUFFER 512
#endif

/*
 * Use readline() functionality for interactive input.
 * Currently requires CINT_READLINE() macro portability.
 */
#ifndef CINT_CONFIG_INCLUDE_PARSER_READLINE
#define CINT_CONFIG_INCLUDE_PARSER_READLINE 1
#endif

/*
 * Enable command line history() functionality for interactive input.
 * Currently requires CINT_ADD_HISTORY() macro portability.
 */
#ifndef CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY
#define CINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY 1
#endif

/*
 * If the parser is included, this setting assumes the input can be read from stdin
 * using the default C stdio environment.
 */
#ifndef CINT_CONFIG_PARSER_STDIN
#define CINT_CONFIG_PARSER_STDIN 1
#endif


/*
 * Include support for doubles. This support is optional as not all runtime environments
 * support them.
 *
 * Note -- this allows declaration of both 'float' and 'double' types in the interpreter.
 * However, all 'float' types are actually doubles internally.
 */
#ifndef CINT_CONFIG_INCLUDE_DOUBLES
#define CINT_CONFIG_INCLUDE_DOUBLES 1
#endif

/*
 * Include support for long longs.
 */
#ifndef CINT_CONFIG_INCLUDE_LONGLONGS
#ifdef __PEDANTIC__
#define CINT_CONFIG_INCLUDE_LONGLONGS 0
#else
#define CINT_CONFIG_INCLUDE_LONGLONGS 1
#endif
#endif


#ifndef CINT_CONFIG_INCLUDE_FLOATS
#define CINT_CONFIG_INCLUDE_FLOATS 0
#endif


/*
 * Include a default main() application in the library.
 */
#ifndef CINT_CONFIG_INCLUDE_MAIN
#define CINT_CONFIG_INCLUDE_MAIN 0
#endif


/*
 * Maximum allowable length of a variable name in characters.
 */
#ifndef CINT_CONFIG_MAX_VARIABLE_NAME
#define CINT_CONFIG_MAX_VARIABLE_NAME 64
#endif


/*
 * The maximum number of function parameters.
 */
#ifndef CINT_CONFIG_MAX_FPARAMS
#define CINT_CONFIG_MAX_FPARAMS 16
#endif


/*
 * The maximum length of an unconstrained char* that sprintf will print into
 */
#ifndef CINT_CONFIG_SPRINTF_BUFFER_LENGTH
#define CINT_CONFIG_SPRINTF_BUFFER_LENGTH 4096
#endif


/*
 * Include or exclude support for dynamic loading and registration of interpreter data
 * using shared libraries:
 *
 *      #> cint load <lib.so>
 *
 * Currently requires a 'dlopen()' style interface.
 * Disabled by default as it is non-portable.
 */

#ifndef CINT_CONFIG_INCLUDE_CINT_LOAD
#define CINT_CONFIG_INCLUDE_CINT_LOAD 1
#endif



/*
 * Include or exclude support for loading and evaluating source files into the
 * interpreter:
 *
 *      #> cint source "file.c"
 *
 * Disabled by default as it is non-portable.
 */
#ifndef CINT_CONFIG_INCLUDE_CINT_SOURCE
#define CINT_CONFIG_INCLUDE_CINT_SOURCE 0
#endif


/*
 * Include support for interpreter execution debug tracing.
 *
 * When compiled in, this is enabled or disabled through
 * interp.debug.dtrace
 *
 * When compiled out, the setting has no effect.
 */
#ifndef CINT_CONFIG_INCLUDE_DTRACE
#define CINT_CONFIG_INCLUDE_DTRACE 1
#endif


/*
 * Include support for generate interpreter debug messages.
 */
#ifndef CINT_CONFIG_INCLUDE_DEBUG
#define CINT_CONFIG_INCLUDE_DEBUG 1
#endif


/*
 * Support FILE IO
 */
#ifndef CINT_CONFIG_FILE_IO
#define CINT_CONFIG_FILE_IO 1
#endif

#ifndef CINT_CONFIG_INCLUDE_FILE_IO
#define CINT_CONFIG_INCLUDE_FILE_IO 0
#endif


/*
 * Support the #include directive.
 * Requires CINT_CONFIG_FILE_IO=1
 */
#ifndef CINT_CONFIG_INCLUDE_XINCLUDE
#define CINT_CONFIG_INCLUDE_XINCLUDE 1
#endif

/* check for config consistency */
#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
#if CINT_CONFIG_FILE_IO == 0
#error CINT_CONFIG_INCLUDE_XINCLUDE requires CINT_CONFIG_FILE_IO=1
#endif
#endif

/*
 * Maximum include depth when #includes are supported
 */
#ifndef CINT_CONFIG_XINCLUDE_DEPTH_MAX
#define CINT_CONFIG_XINCLUDE_DEPTH_MAX 128
#endif

/*
 * Assume and use stdlib functionality for all portability macros
 * that are otherwise undefined.
 */
#ifndef CINT_CONFIG_INCLUDE_STDLIB
#define CINT_CONFIG_INCLUDE_STDLIB 0
#endif

/*
 * Include test interface data
 */
#ifndef CINT_CONFIG_INCLUDE_TEST_DATA
#define CINT_CONFIG_INCLUDE_TEST_DATA 0
#endif

/*
 * Include POSIX timer interface
 */
#ifndef CINT_CONFIG_INCLUDE_POSIX_TIMER
#define CINT_CONFIG_INCLUDE_POSIX_TIMER 1
#endif

/*
 * Assume SDK SAL functionality for all portability macros
 * that are otherwise undefined.
 */
#ifndef CINT_CONFIG_INCLUDE_SDK_SAL
#define CINT_CONFIG_INCLUDE_SDK_SAL 0
#endif

#ifndef CINT_CONFIG_INCLUDE_SDKLT
#define CINT_CONFIG_INCLUDE_SDKLT 0
#endif

#ifndef CINT_CONFIG_INCLUDE_STUBS
#define CINT_CONFIG_INCLUDE_STUBS 0
#endif

/* check for config consistency */
#if CINT_CONFIG_INCLUDE_STDLIB == 1
 #if CINT_CONFIG_INCLUDE_SDK_SAL != 0
  #error Cannot set CINT_CONFIG_INCLUDE_STDLIB and CINT_CONFIG_INCLUDE_SDK_SAL
 #endif
 #if CINT_CONFIG_INCLUDE_STUBS != 0
  #error Cannot set CINT_CONFIG_INCLUDE_STDLIB and CINT_CONFIG_INCLUDE_STUBS
 #endif
 #if CINT_CONFIG_INCLUDE_SDKLT != 0
  #error Cannot set CINT_CONFIG_INCLUDE_STDLIB and CINT_CONFIG_INCLUDE_SDKLT
 #endif
#elif CINT_CONFIG_INCLUDE_SDK_SAL == 1
 #if CINT_CONFIG_INCLUDE_STUBS != 0
  #error Cannot set CINT_CONFIG_INCLUDE_SDK_SAL and CINT_CONFIG_INCLUDE_STUBS
 #endif
 #if CINT_CONFIG_INCLUDE_SDKLT != 0
  #error Cannot set CINT_CONFIG_INCLUDE_SDK_SAL and CINT_CONFIG_INCLUDE_SDKLT
 #endif
#elif CINT_CONFIG_INCLUDE_SDKLT == 1
 #if CINT_CONFIG_INCLUDE_STUBS != 0
  #error Cannot set CINT_CONFIG_INCLUDE_SDKLT and CINT_CONFIG_INCLUDE_STUBS
 #endif
#elif CINT_CONFIG_INCLUDE_STUBS == 1
/* conflicts would have been caught earlier */
#else
#error Must define one of CINT_CONFIG_INCLUDE_{STDLIB,SDK_SAL,STUBS,SDKLT}
#endif

/*
 * Include YAML API interface
 *
 * Adds cint command "yapi" to print a YAML formatted dump of the CINT
 * datatype database. Primarily used for SDK API analysis.
 */
#ifndef CINT_CONFIG_YAPI
#define CINT_CONFIG_YAPI 0
#endif


/*
 * CINT generally requires requires POSIX extensions, which aren't
 * usually available if __STRICT_ANSI__ is defined.
 */
#ifdef __STRICT_ANSI__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#endif /* __CINT_CONFIG_H__ */




