/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_error.c
 * Purpose:     CINT error handling functions
 */

#include "cint_config.h"
#include "cint_porting.h"
#include "cint_error.h"

const char* 
cint_error_name(cint_error_t err)
{
    switch(err) {                       
#define CINT_ERROR_LIST_ENTRY(_entry) case CINT_E_##_entry : return #_entry ; 
#include "cint_error_entry.h"

        default: return "<UNKNOWN>";
    }
}       


void 
cint_internal_error(const char* f, int l, const char* fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 
    
    CINT_PRINTF("*** internal error (%s:%d): ", f, l); 
    CINT_VPRINTF(fmt, args); 
    CINT_PRINTF("\n"); 
    va_end(args); 
    cint_errno = CINT_E_INTERNAL; 
}       

static void 
__cint_vmsg(const char* f, int l, const char* type, const char* fmt, va_list args)
     COMPILER_ATTRIBUTE((format (printf, 4, 0)));

static void 
__cint_vmsg(const char* f, int l, const char* type, const char* fmt, va_list args)
{
    if(f) {
        CINT_PRINTF("** %s:%d: %s: ", f, l, type); 
    }
    else if(l != 0) {
        CINT_PRINTF("** %d: %s: ", l, type); 
    }
    else {
        CINT_PRINTF("** %s: ", type); 
    }
    CINT_VPRINTF(fmt, args); 
    CINT_PRINTF("\n"); 
}

void
cint_error(const char* f, int l, cint_error_t e, const char* fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 
    __cint_vmsg(f, l, "error", fmt, args); 
    va_end(args); 
    cint_errno = e; 
}

int
cint_ast_error(const cint_ast_t* ast, cint_error_t e, const char* fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 
    __cint_vmsg(ast ? ast->file : NULL, 
                ast ? ast->line : 0, 
                "error", fmt, args); 
    va_end(args);
    cint_errno = e;

    return e; 
}       
    
int 
cint_ast_warn(const cint_ast_t* ast, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt); 
    __cint_vmsg(ast ? ast->file : NULL, 
                ast ? ast->line : 0,
                "warning", fmt, args); 
    va_end(args); 
    return 0; 
}
   
void 
cint_warn(const char* f, int l, const char* fmt, ...)
{
    va_list args; 
    va_start(args, fmt); 
    __cint_vmsg(f, l, "warning", fmt, args); 
    va_end(args); 
}
