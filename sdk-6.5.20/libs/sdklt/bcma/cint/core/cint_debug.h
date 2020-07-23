/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_debug.h
 * Purpose:     CINT debug interfaces
 */

#ifndef __CINT_DEBUG_H__
#define __CINT_DEBUG_H__

#include "cint_config.h"

#include "cint_ast.h"

/*
 * Debug execution trace
 */

#if CINT_CONFIG_INCLUDE_DTRACE == 1
extern void cint_dtrace(const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 1, 2)));
#define CINT_DTRACE(x) cint_dtrace x
#else
#define CINT_DTRACE(x)
#endif

#if CINT_CONFIG_INCLUDE_DEBUG == 1
extern void cint_debug(const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 1, 2)));
#define CINT_DEBUG(x) cint_debug x
#else
#define CINT_DEBUG(x)
#endif

extern void cint_trace(const char* pfx, const char* fmt, ...)
     COMPILER_ATTRIBUTE((format (printf, 2, 3)));
extern void cint_ftrace(const char* name, int enter);
extern void cint_atrace(cint_ast_t* ast);

#endif /* __CINT_DEBUG_H__ */
