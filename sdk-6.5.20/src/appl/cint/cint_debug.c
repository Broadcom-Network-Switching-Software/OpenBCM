/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_debug.c
 * Purpose:     CINT debug functions
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_debug.h"
#include "cint_interpreter.h"
#include "cint_ast.h"

void
__cint_trace(const char* pfx, const char* fmt, va_list args)
     COMPILER_ATTRIBUTE((format (printf, 2, 0))); 


void
__cint_trace(const char* pfx, const char* fmt, va_list args)
{
    CINT_PRINTF("{%s: ", pfx); 
    CINT_VPRINTF(fmt, args); 
    CINT_PRINTF("}\n"); 
}

void 
cint_dtrace(const char* fmt, ...)
{
    va_list args;     
    if(interp.debug.dtrace) {
        va_start(args, fmt); 
        __cint_trace("DTRACE", fmt, args); 
        va_end(args); 
    }   
}

void 
cint_debug(const char* fmt, ...)
{
    va_list args;     
    va_start(args, fmt); 
    __cint_trace("DEBUG", fmt, args); 
    va_end(args); 
}


void 
cint_trace(const char* pfx, const char* fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 
    __cint_trace(pfx, fmt, args); 
    va_end(args); 
}

void
cint_ftrace(const char* name, int enter)
{
    if(interp.debug.ftrace) {
        cint_trace("FTRACE", "'%s' %s", 
                   name, enter ? "Enter" : "Exit"); 
    }
}

void
cint_atrace(cint_ast_t* ast)
{
    if(interp.debug.atrace) {
        cint_trace("ATRACE", "%s", ""); 
        cint_ast_dump_single(ast, 8); 
    }
}
