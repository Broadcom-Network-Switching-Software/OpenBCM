/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_yy.h
 * Purpose:     CINT parser interface
 */

#ifndef   _CINT_YY_H_
#define   _CINT_YY_H_

/* STATIC and CONST are used by both the SDK build system, and the C
   scanner, and interacts badly. The build system definitions are not
   needed here. */
#ifdef STATIC
#undef STATIC
#endif

#ifdef CONST
#undef CONST
#endif

#include "cint_ast.h"
#define YYSTYPE cint_ast_t*

/* FLEX interfaces */
#define YY_FATAL_ERROR(msg) cint_cparser_fatal_error(msg)
#define cint_c_alloc(size,scanner) cint_cparser_alloc(size)
#define cint_c_free(ptr,scanner) cint_cparser_free(ptr)
#define cint_c_realloc(ptr,size,scanner) cint_cparser_realloc(ptr,size)
#define ECHO (void) cint_cparser_message(yytext, yyleng)


/* BISON interfaces */
#define YYMALLOC cint_cparser_alloc
#define YYFREE cint_cparser_free
#define YYCOPY cint_cparser_memcpy

#endif /* _CINT_YY_H_ */
